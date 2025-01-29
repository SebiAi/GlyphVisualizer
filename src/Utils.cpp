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
