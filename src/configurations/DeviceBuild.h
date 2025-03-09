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

#ifndef GV_DEVICEBUILD_H
#define GV_DEVICEBUILD_H

#include <QMap>
#include <QString>
#include <QStringLiteral>
namespace DeviceBuildNS {
    Q_NAMESPACE

    enum class DeviceBuild {
        Spacewar = 0,
        Pong = 1,
        Pacman = 2,
        PacmanPro = 3,
        Asteroids = 4
    };
    Q_ENUM_NS(DeviceBuild)

    const static QMap<QString, DeviceBuild> mapNglyphToDeviceBuild{
        {QStringLiteral("PHONE1"), DeviceBuild::Spacewar},
        {QStringLiteral("PHONE2"), DeviceBuild::Pong},
        {QStringLiteral("PHONE2A"), DeviceBuild::Pacman},
        {QStringLiteral("PHONE3A"), DeviceBuild::Asteroids}
    };
}

#endif // GV_DEVICEBUILD_H
