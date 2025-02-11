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

#ifndef GV_UTILS_H
#define GV_UTILS_H

#include <QDir>
#include <QFileInfo>
#include <QIcon>
#include <QList>
#include <QStandardPaths>
#include <QStringList>
#include <QString>
#include <QStringLiteral>

class InvalidLightDataException: public std::runtime_error {
public:
    explicit InvalidLightDataException(const std::string& msg): std::runtime_error{msg} {}
};
class SourceFileException: public std::runtime_error {
public:
    explicit SourceFileException(const std::string& msg): std::runtime_error{msg} {}
};

QIcon getApplicationIcon();

QString listToString(const QList<qsizetype>& list, const QString& separator = QStringLiteral(", "));
void createPathIfNeeded(const QDir& dir);
QDir getAppConfigLocation();
QString getValidPath(const QString& path);
QString getExecutableFromPath(const QString& executableName);

#endif // GV_UTILS_H
