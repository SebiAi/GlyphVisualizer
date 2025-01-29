/*
This file is part of the GlyphVisualizer project, a Glyph composition
player that plays Glyph compositions from Nothing phones.
Copyright (C) 2025  Sebastian Aigner (aka. SebiAi)

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "UpdateChecker.h"

// Logging
Q_LOGGING_CATEGORY(updateChecker, "UpdateChecker")
Q_LOGGING_CATEGORY(updateCheckerVerbose, "UpdateChecker.Verbose")

const QNetworkRequest UpdateChecker::request{QUrl(BUILDINFO_GITHUB_LATEST_RELEASE_API_URL)};
const QRegularExpression UpdateChecker::versionRegex{QStringLiteral(R"(v?(\d+)\.(\d+)\.(\d+))")};

UpdateChecker::UpdateChecker(
	QObject *parent)
	: QObject{parent}
{}

void UpdateChecker::checkForUpdate(const QString &currentVersion, bool silent) {
	if (silent) qCInfo(updateChecker) << "Checking for update (silent)...";
	else qCInfo(updateChecker) << "Checking for update...";

	QNetworkReply* reply{NetworkManager::getInstance().get(UpdateChecker::request)};
	if (!reply->isFinished())
		connect(reply, &QNetworkReply::finished, this, [=](){ onReplyFinished(reply, currentVersion, silent); });
	else
		onReplyFinished(reply, currentVersion, silent);
}

bool UpdateChecker::versionAbiggerThanVersionB(QString versionA, QString versionB) const {
	// If it does not match this regex it is an invalid version and false is returned!
	QRegularExpressionMatch matchA{UpdateChecker::versionRegex.match(versionA)};
	QRegularExpressionMatch matchB{UpdateChecker::versionRegex.match(versionB)};

	// Check if the version is valid, if not return false
	if (!matchA.hasMatch() || !matchB.hasMatch()) {
		if (!matchA.hasMatch()) qCWarning(updateCheckerVerbose) << "Invalid new version string";
		if (!matchB.hasMatch()) qCWarning(updateCheckerVerbose) << "Invalid current version string";
		return false;
	}

	// Loop through the groups
	for (quint8 i = 1; i <= 3; i++)	{
		// Convert the group to int.
		// No need to check the result of toInt() because we have a regex match.
		if (matchA.captured(i).toInt() > matchB.captured(i).toInt()) return true;
		if (matchA.captured(i).toInt() < matchB.captured(i).toInt()) return false;
	}

	return false;
}

void UpdateChecker::onReplyFinished(QNetworkReply* reply, const QString &currentVersion, bool silent) const {
	if (reply->error()) {
		qCWarning(updateChecker) << "Update check failed - Network response not ok";
		qCWarning(updateCheckerVerbose) << reply->errorString();
		if (!silent) emit updateCheckFailed("Network response not ok");
		return;
	}

	QByteArray rawResponse{reply->readAll()};
	QJsonDocument jsonResponse{QJsonDocument::fromJson(rawResponse)};
	if (jsonResponse.isNull()) {
		qCWarning(updateChecker) << "Update check failed - Json parsing error";
		qCInfo(updateCheckerVerbose) << "Raw request response:" << QString(rawResponse);
		if (!silent) emit updateCheckFailed("Json parsing error");
		return;
	}

	// Get the tag
	QJsonValue tagName{jsonResponse["tag_name"]};
	if (tagName.isUndefined()) {
		// Value does not exist
		qCWarning(updateChecker) << "Update check failed - New version string not found in response (tag_name)";
		qCInfo(updateCheckerVerbose) << "Json:" << jsonResponse.toJson(QJsonDocument::JsonFormat::Compact);
		if (!silent) emit updateCheckFailed("New version not found in response");
		return;
	}
	if (!tagName.isString()) {
		// Is not a string
		qCWarning(updateChecker) << "Update check failed - New version is not a string";
		qCInfo(updateCheckerVerbose) << "Json:" << jsonResponse.toJson(QJsonDocument::JsonFormat::Compact);
		if (!silent) emit updateCheckFailed("New version is not a string");
		return;
	}
	QString newVersion = tagName.toString();

	// Get bigger version number
	if (versionAbiggerThanVersionB(newVersion, currentVersion))
	{
		qCInfo(updateChecker) << "New version found:" << newVersion;
		emit updateAvailable(newVersion);
		return;
	}

	qCInfo(updateChecker) << "No new version found";
	qCInfo(updateCheckerVerbose).nospace() << "Current version: " << currentVersion << ", New version: " << newVersion;
	if (!silent) emit noUpdateAvailable();

	// Delete the reply object
	reply->deleteLater();
}
