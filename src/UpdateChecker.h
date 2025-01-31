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

#ifndef GV_UPDATECHECKER_H
#define GV_UPDATECHECKER_H

#include <QByteArray>
#include <QJsonDocument>
#include <QJsonValue>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QObject>
#include <QRegularExpression>

#include "BuildInfo.h"
#include "NetworkManager.h"

// Logging
#include <QLoggingCategory>
Q_DECLARE_LOGGING_CATEGORY(updateChecker)
Q_DECLARE_LOGGING_CATEGORY(updateCheckerVerbose)

class UpdateChecker : public QObject
{
    Q_OBJECT
public:
    explicit UpdateChecker(QObject *parent = nullptr);

    void checkForUpdate(const QString &currentVersion, bool silent = false);

signals:
    void updateAvailable(const QString &newVersion) const;
    void noUpdateAvailable() const;
    void updateCheckFailed(const QString &errorMessage) const;

private:
    static const QNetworkRequest request;
    static const QRegularExpression versionRegex;

    bool versionAbiggerThanVersionB(QString versionA, QString versionB) const;

private slots:
    void onReplyFinished(QNetworkReply* reply, const QString &currentVersion, bool silent) const;
};

#endif // GV_UPDATECHECKER_H
