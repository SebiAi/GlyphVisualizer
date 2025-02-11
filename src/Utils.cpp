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

#include "Utils.h"

QIcon getApplicationIcon() {
    static bool initialized = false;
    static QIcon icon;
    if (!initialized) {
        initialized = true;
        icon.addFile(QStringLiteral(":/icons/appicon/512x512"), QSize{512, 512}, QIcon::Mode::Normal, QIcon::State::On);
        icon.addFile(QStringLiteral(":/icons/appicon/256x256"), QSize{256, 256}, QIcon::Mode::Normal, QIcon::State::On);
        icon.addFile(QStringLiteral(":/icons/appicon/48x48"), QSize{48, 48}, QIcon::Mode::Normal, QIcon::State::On);
        icon.addFile(QStringLiteral(":/icons/appicon/32x32"), QSize{32, 32}, QIcon::Mode::Normal, QIcon::State::On);
        icon.addFile(QStringLiteral(":/icons/appicon/16x16"), QSize{16, 16}, QIcon::Mode::Normal, QIcon::State::On);
    }

    return icon;
}

QString listToString(const QList<qsizetype>& list, const QString& separator) {
    QStringList strings;
    strings.reserve(list.size());

    std::transform(list.cbegin(), list.cend(), std::back_inserter(strings), [](const qsizetype& x){return QString::number(x);});

    return QStringLiteral("[%1]").arg(strings.join(separator));
}

void createPathIfNeeded(const QDir& dir)
{
    if (!dir.exists() && !dir.mkpath(dir.absolutePath()))
        throw std::runtime_error("Could not create directory structure '" + dir.absolutePath().toStdString() + "'");
}

QDir getAppConfigLocation() {
    // Get all app config locations
    QStringList appConfigLocations = QStandardPaths::standardLocations(QStandardPaths::AppConfigLocation);

    // We need at least one location
    if (appConfigLocations.isEmpty())
    {
        throw std::runtime_error("No app config location found!!");
    }

    // Pick the first location which also has the highest priority
    QDir appConfigDir = QDir(appConfigLocations.at(0));

    // Create the directory structure if the location does not exist
    createPathIfNeeded(appConfigDir);

    return appConfigDir;
}

QString getValidPath(const QString& path) {
    QFileInfo pathInfo{path};

    if (pathInfo.exists())
    {
        // Path exists (could be file or dir)
        return pathInfo.canonicalFilePath();
    }

    if (!pathInfo.suffix().isEmpty()) {
        // If path is a deleted directory with a dot in its name we also end up here
        // does not matter - how often does this happen, right?
        // Might be a file => see if the parent dir exists
        QDir parentDir{pathInfo.absoluteDir()};
        if (parentDir.exists())
            return parentDir.canonicalPath();
    }

    // Parent dir of file or dir non existent
    return QDir::homePath();
}

QString getExecutableFromPath(const QString& executableName) {
    return QStandardPaths::findExecutable(executableName);
}
