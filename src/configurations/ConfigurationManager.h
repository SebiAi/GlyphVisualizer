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

#ifndef GV_CONFIGURATIONMANAGER_H
#define GV_CONFIGURATIONMANAGER_H

#include <QByteArray>
#include <QColor>
#include <QCoreApplication>
#include <QFile>
#include <QFileInfo>
#include <QList>
#include <QMap>
#include <QMetaEnum>
#include <QList>
#include <QObject>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QSharedPointer>
#include <QString>
#include <QStringLiteral>

#include <taglib/fileref.h>
#include <taglib/tpropertymap.h>
#include <taglib/opusfile.h>

#include "DeviceBuild.h"
#include "IConfiguration.h"
#include "Phone1Configuration.h"
#include "Phone2aConfiguration.h"
#include "Phone2Configuration.h"
#include "../Utils.h"

// Logging
#include <QLoggingCategory>
Q_DECLARE_LOGGING_CATEGORY(configurationManager)
Q_DECLARE_LOGGING_CATEGORY(configurationManagerVerbose)

using namespace DeviceBuildNS;

class ConfigurationManager: private QObject {
    Q_OBJECT
public:
    static constexpr int maxLightValue = 4095;
    static const QColor glyphOnColor;
    static constexpr qreal glyphOffHSVValue = 0.285;

    ConfigurationManager();

    IConfiguration* getConfiguration(DeviceBuild device);

    DeviceBuild loadCompositionFromAudio(const QString& audioPath);
    DeviceBuild loadCompositionFromNglyph(const QString& nglyphPath);

    static QColor brightnessToGlyphColor(int value);
private:
    QMap<DeviceBuild, QSharedPointer<IConfiguration>> configurations;
    static const QRegularExpression composerExpression;

    static QList<QList<int>> parseLightData(const QString& lightData);
};

#endif // GV_CONFIGURATIONMANAGER_H
