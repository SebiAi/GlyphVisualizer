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

#ifndef GV_PHONE2CONFIGURATION_H
#define GV_PHONE2CONFIGURATION_H

#include "IConfiguration.h"

class Phone2Configuration : public IConfiguration {
public:
    static constexpr DeviceBuild staticBuild = DeviceBuild::Pong;

    Phone2Configuration(const QColor& fallbackColor): IConfiguration{
        fallbackColor,
        QList<Glyph>({
            Glyph{QStringLiteral(":/glyphs/phone2/led_a1"), Glyph::Reference::TOP_LEFT, QPointF{13.88, 10 + 3.5}}, // CameraTop
            Glyph{QStringLiteral(":/glyphs/phone2/led_a2"), Glyph::Reference::TOP_LEFT, QPointF{22.75, 51.56 + 3.5}}, // CameraBottom
            Glyph{QStringLiteral(":/glyphs/phone2/led_b"), Glyph::Reference::TOP_RIGHT, QPointF{-22.51, 20.07 + 3.5}}, // Diagonal
			Glyph{QStringLiteral(":/glyphs/phone2/led_c1_zones"), Glyph::Reference::TOP_RIGHT, QPointF{-11.77, 92.49 + 3.5}, QStringLiteral("path_0")}, // BatteryTopRight_Zone0
			Glyph{QStringLiteral(":/glyphs/phone2/led_c1_zones"), Glyph::Reference::TOP_RIGHT, QPointF{-11.77, 92.49 + 3.5}, QStringLiteral("path_1")}, // BatteryTopRight_Zone1
			Glyph{QStringLiteral(":/glyphs/phone2/led_c1_zones"), Glyph::Reference::TOP_RIGHT, QPointF{-11.77, 92.49 + 3.5}, QStringLiteral("path_2")}, // BatteryTopRight_Zone2
			Glyph{QStringLiteral(":/glyphs/phone2/led_c1_zones"), Glyph::Reference::TOP_RIGHT, QPointF{-11.77, 92.49 + 3.5}, QStringLiteral("path_3")}, // BatteryTopRight_Zone3
			Glyph{QStringLiteral(":/glyphs/phone2/led_c1_zones"), Glyph::Reference::TOP_RIGHT, QPointF{-11.77, 92.49 + 3.5}, QStringLiteral("path_4")}, // BatteryTopRight_Zone4
			Glyph{QStringLiteral(":/glyphs/phone2/led_c1_zones"), Glyph::Reference::TOP_RIGHT, QPointF{-11.77, 92.49 + 3.5}, QStringLiteral("path_5")}, // BatteryTopRight_Zone5
			Glyph{QStringLiteral(":/glyphs/phone2/led_c1_zones"), Glyph::Reference::TOP_RIGHT, QPointF{-11.77, 92.49 + 3.5}, QStringLiteral("path_6")}, // BatteryTopRight_Zone6
			Glyph{QStringLiteral(":/glyphs/phone2/led_c1_zones"), Glyph::Reference::TOP_RIGHT, QPointF{-11.77, 92.49 + 3.5}, QStringLiteral("path_7")}, // BatteryTopRight_Zone7
			Glyph{QStringLiteral(":/glyphs/phone2/led_c1_zones"), Glyph::Reference::TOP_RIGHT, QPointF{-11.77, 92.49 + 3.5}, QStringLiteral("path_8")}, // BatteryTopRight_Zone8
			Glyph{QStringLiteral(":/glyphs/phone2/led_c1_zones"), Glyph::Reference::TOP_RIGHT, QPointF{-11.77, 92.49 + 3.5}, QStringLiteral("path_9")}, // BatteryTopRight_Zone9
			Glyph{QStringLiteral(":/glyphs/phone2/led_c1_zones"), Glyph::Reference::TOP_RIGHT, QPointF{-11.77, 92.49 + 3.5}, QStringLiteral("path_10")}, // BatteryTopRight_Zone10
			Glyph{QStringLiteral(":/glyphs/phone2/led_c1_zones"), Glyph::Reference::TOP_RIGHT, QPointF{-11.77, 92.49 + 3.5}, QStringLiteral("path_11")}, // BatteryTopRight_Zone11
			Glyph{QStringLiteral(":/glyphs/phone2/led_c1_zones"), Glyph::Reference::TOP_RIGHT, QPointF{-11.77, 92.49 + 3.5}, QStringLiteral("path_12")}, // BatteryTopRight_Zone12
			Glyph{QStringLiteral(":/glyphs/phone2/led_c1_zones"), Glyph::Reference::TOP_RIGHT, QPointF{-11.77, 92.49 + 3.5}, QStringLiteral("path_13")}, // BatteryTopRight_Zone13
			Glyph{QStringLiteral(":/glyphs/phone2/led_c1_zones"), Glyph::Reference::TOP_RIGHT, QPointF{-11.77, 92.49 + 3.5}, QStringLiteral("path_14")}, // BatteryTopRight_Zone14
			Glyph{QStringLiteral(":/glyphs/phone2/led_c1_zones"), Glyph::Reference::TOP_RIGHT, QPointF{-11.77, 92.49 + 3.5}, QStringLiteral("path_15")}, // BatteryTopRight_Zone15
            Glyph{QStringLiteral(":/glyphs/phone2/led_c2"), Glyph::Reference::TOP_LEFT, QPointF{13.7, 101.28 + 3.5}}, // BatteryTopLeft
            Glyph{QStringLiteral(":/glyphs/phone2/led_c3"), Glyph::Reference::TOP_LEFT, QPointF{11.94, 146.3 + 3.5}}, // BatteryTopVertical
            Glyph{QStringLiteral(":/glyphs/phone2/led_c4"), Glyph::Reference::BOTTOM_LEFT, QPointF{13.7, -88.56 - 3.5}}, // BatteryBottomLeft
            Glyph{QStringLiteral(":/glyphs/phone2/led_c5"), Glyph::Reference::BOTTOM_RIGHT, QPointF{-11.78, -97.36 - 3.5}}, // BatteryBottomRight
            Glyph{QStringLiteral(":/glyphs/phone2/led_c6"), Glyph::Reference::BOTTOM_RIGHT, QPointF{-10, -151.83 - 3.5}}, // BatteryBottomVertical
            Glyph{QStringLiteral(":/glyphs/phone2/led_e"), Glyph::Reference::CENTERED_H_BOTTOM, QPointF{0, -10 - 3.5}}, // BatteryUSBDot
			Glyph{QStringLiteral(":/glyphs/phone2/led_d_zones"), Glyph::Reference::CENTERED_H_BOTTOM, QPointF{0, -24.38 - 3.5}, QStringLiteral("path_0")}, // BatteryUSBDot_Zone0 - add them last so we can directly apply the author data
			Glyph{QStringLiteral(":/glyphs/phone2/led_d_zones"), Glyph::Reference::CENTERED_H_BOTTOM, QPointF{0, -24.38 - 3.5}, QStringLiteral("path_1")}, // BatteryUSBDot_Zone1
			Glyph{QStringLiteral(":/glyphs/phone2/led_d_zones"), Glyph::Reference::CENTERED_H_BOTTOM, QPointF{0, -24.38 - 3.5}, QStringLiteral("path_2")}, // BatteryUSBDot_Zone2
			Glyph{QStringLiteral(":/glyphs/phone2/led_d_zones"), Glyph::Reference::CENTERED_H_BOTTOM, QPointF{0, -24.38 - 3.5}, QStringLiteral("path_3")}, // BatteryUSBDot_Zone3
			Glyph{QStringLiteral(":/glyphs/phone2/led_d_zones"), Glyph::Reference::CENTERED_H_BOTTOM, QPointF{0, -24.38 - 3.5}, QStringLiteral("path_4")}, // BatteryUSBDot_Zone4
			Glyph{QStringLiteral(":/glyphs/phone2/led_d_zones"), Glyph::Reference::CENTERED_H_BOTTOM, QPointF{0, -24.38 - 3.5}, QStringLiteral("path_5")}, // BatteryUSBDot_Zone5
			Glyph{QStringLiteral(":/glyphs/phone2/led_d_zones"), Glyph::Reference::CENTERED_H_BOTTOM, QPointF{0, -24.38 - 3.5}, QStringLiteral("path_6")}, // BatteryUSBDot_Zone6
			Glyph{QStringLiteral(":/glyphs/phone2/led_d_zones"), Glyph::Reference::CENTERED_H_BOTTOM, QPointF{0, -24.38 - 3.5}, QStringLiteral("path_7")}, // BatteryUSBDot_Zone7
        }),
        Phone2Configuration::staticBuild,
        QList<qsizetype>({33}),
        QSize{182, 389}
    } {}
};

#endif // GV_PHONE2CONFIGURATION_H
