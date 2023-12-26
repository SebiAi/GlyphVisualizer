#ifndef UPDATECHECKER_H
#define UPDATECHECKER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QString>
#include <QJsonDocument>
#include <QRegularExpression>
#include <QRegularExpressionMatch>

#include "APPLICATION_INFO.h"

// Logging
#include <QLoggingCategory>
Q_DECLARE_LOGGING_CATEGORY(updateChecker)
Q_DECLARE_LOGGING_CATEGORY(updateCheckerVerbose)

class UpdateChecker : public QObject
{
    Q_OBJECT
public:
    explicit UpdateChecker(QObject *parent = nullptr);

    /**
     * @brief Checks if an update is available.
     * @param version The version to check against.
     * @param silent true: Only emit updateAvailable; false:
     */
    void checkForUpdate(const QString &version, bool silent = false);

private:
    const QNetworkRequest request = QNetworkRequest(QUrl(APPLICATION_GITHUB_LATEST_RELEASE_API_URL));

    /**
     * @brief Every version should match this regex: "/^v?(\d+)\.(\d+)\.(\d+)$/g"
     */
    const QRegularExpression versionRegex = QRegularExpression("^v?(\\d+)\\.(\\d+)\\.(\\d+)$");

    /**
     * @brief Takes in two semantic versions and compares them.
     * @param a First version (e.g.: v1.0.1) 'v' is optional
     * @param b Secons version (e.g.: v1.0.0) 'v' is optional
     * @return If versionA is newer than versionB then return true.
     */
    bool versionAbiggerThanVersionB(QString versionA, QString versionB);

signals:
    /**
     * @brief This signal will emit after calling the checkForUpdate function if an update is available.
     * @param newVersion The new version number. E.g.: v1.2.3
     */
    void updateAvailable(const QString &newVersion);

    /**
     * @brief This signal will emit after calling the checkForUpdate function if no update is available.
     */
    void noUpdateAvailable();

    /**
     * @brief This signal will emit after calling the checkForUpdate function if the update check failed.
     * @param errorMessage The error message.
     */
    void updateCheckFailed(const QString &errorMessage);
};

#endif // UPDATECHECKER_H
