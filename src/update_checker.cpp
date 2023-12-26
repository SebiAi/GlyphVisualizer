#include "update_checker.h"

// Logging
Q_LOGGING_CATEGORY(updateChecker, "UpdateChecker")
Q_LOGGING_CATEGORY(updateCheckerVerbose, "UpdateChecker.Verbose")

UpdateChecker::UpdateChecker(QObject *parent) :
    QObject{parent}
{

}

/*
 * ==================================
 *             Functions
 * ==================================
 */

void UpdateChecker::checkForUpdate(const QString &version, bool silent)
{
    if (silent) qCInfo(updateChecker) << "Checking for update (silent)...";
    else qCInfo(updateChecker) << "Checking for update...";

    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    connect(manager, &QNetworkAccessManager::finished, this, [=](QNetworkReply *reply){
        if (reply->error())
        {
            // Network error
            qCWarning(updateChecker) << "Update check failed - Network response not ok";
            qCWarning(updateCheckerVerbose) << reply->errorString();
            if (!silent) emit updateCheckFailed("Network response not ok");
            return;
        }

        QByteArray raw_response = reply->readAll();

        QJsonDocument jsonResponse = QJsonDocument::fromJson(raw_response);
        if (jsonResponse.isNull())
        {
            // Json parsing error
            qCWarning(updateChecker) << "Update check failed - Json parsing error";
            qCInfo(updateCheckerVerbose) << "Raw request response:" << QString(raw_response);
            if (!silent) emit updateCheckFailed("Json parsing error");
            return;
        }

        // Get the tag
        QJsonValue tag_name = jsonResponse["tag_name"];
        if (tag_name.isUndefined())
        {
            // Value does not exist
            qCWarning(updateChecker) << "Update check failed - New version string not found in response (tag_name)";
            qCInfo(updateCheckerVerbose) << "Json:" << jsonResponse.toJson(QJsonDocument::JsonFormat::Compact);
            if (!silent) emit updateCheckFailed("New version not found in response");
            return;
        }
        if (!tag_name.isString())
        {
            // Is not a string
            qCWarning(updateChecker) << "Update check failed - New version is not a string";
            qCInfo(updateCheckerVerbose) << "Json:" << jsonResponse.toJson(QJsonDocument::JsonFormat::Compact);
            if (!silent) emit updateCheckFailed("New version is not a string");
            return;
        }

        QString newVersion = tag_name.toString();

        // Get bigger version number
        if (versionAbiggerThanVersionB(newVersion, version))
        {
            qCInfo(updateChecker) << "New version found:" << newVersion;
            if (!silent) emit updateAvailable(newVersion);
            return;
        }

        qCInfo(updateChecker) << "No new version found";
        qCInfo(updateCheckerVerbose).nospace() << "Current version: " << version << ", New version: " << newVersion;
        if (!silent) emit noUpdateAvailable();
    });
    connect(manager, &QNetworkAccessManager::finished, manager, &QObject::deleteLater);

    manager->get(this->request);
}

bool UpdateChecker::versionAbiggerThanVersionB(QString versionA, QString versionB)
{
    // If it does not match this regex it is an invalid version and false is returned!
    QRegularExpressionMatch matchA = this->versionRegex.match(versionA);
    QRegularExpressionMatch matchB = this->versionRegex.match(versionB);

    // Check if the version is valid, if not return false
    if (!matchA.hasMatch() || !matchB.hasMatch())
    {
        if (!matchA.hasMatch()) qCWarning(updateCheckerVerbose) << "Invalid new version string";
        if (!matchB.hasMatch()) qCWarning(updateCheckerVerbose) << "Invalid current version string";
        return false;
    }

    // Loop through the groups
    for (quint8 i = 1; i <= 3; i++)
    {
        // Convert the group to int.
        // No need to check the result of toInt() because we have a regex match.
        if (matchA.captured(i).toInt() > matchB.captured(i).toInt()) return true;
        if (matchA.captured(i).toInt() < matchB.captured(i).toInt()) return false;
    }

    return false;
}
