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

#ifndef GV_PHONE3ACONFIGURATION_H
#define GV_PHONE3ACONFIGURATION_H

#include "IConfiguration.h"

class Phone3aConfiguration : public IConfiguration {
public:
    static constexpr DeviceBuild staticBuild = DeviceBuild::Asteroids;

    Phone3aConfiguration(const QColor& fallbackColor): IConfiguration{
        fallbackColor,
        QList<Glyph>{
            Glyph{QStringLiteral(":/glyphs/phone3a/led_a_zones"), Glyph::Reference::TOP_LEFT, QPointF{0 + 15, 0 + 15}, QStringLiteral("path_0")}, // TopLeft_Zone0
            Glyph{QStringLiteral(":/glyphs/phone3a/led_a_zones"), Glyph::Reference::TOP_LEFT, QPointF{0 + 15, 0 + 15}, QStringLiteral("path_1")}, // TopLeft_Zone1
            Glyph{QStringLiteral(":/glyphs/phone3a/led_a_zones"), Glyph::Reference::TOP_LEFT, QPointF{0 + 15, 0 + 15}, QStringLiteral("path_2")}, // TopLeft_Zone2
            Glyph{QStringLiteral(":/glyphs/phone3a/led_a_zones"), Glyph::Reference::TOP_LEFT, QPointF{0 + 15, 0 + 15}, QStringLiteral("path_3")}, // TopLeft_Zone3
            Glyph{QStringLiteral(":/glyphs/phone3a/led_a_zones"), Glyph::Reference::TOP_LEFT, QPointF{0 + 15, 0 + 15}, QStringLiteral("path_4")}, // TopLeft_Zone4
            Glyph{QStringLiteral(":/glyphs/phone3a/led_a_zones"), Glyph::Reference::TOP_LEFT, QPointF{0 + 15, 0 + 15}, QStringLiteral("path_5")}, // TopLeft_Zone5
            Glyph{QStringLiteral(":/glyphs/phone3a/led_a_zones"), Glyph::Reference::TOP_LEFT, QPointF{0 + 15, 0 + 15}, QStringLiteral("path_6")}, // TopLeft_Zone6
            Glyph{QStringLiteral(":/glyphs/phone3a/led_a_zones"), Glyph::Reference::TOP_LEFT, QPointF{0 + 15, 0 + 15}, QStringLiteral("path_7")}, // TopLeft_Zone7
            Glyph{QStringLiteral(":/glyphs/phone3a/led_a_zones"), Glyph::Reference::TOP_LEFT, QPointF{0 + 15, 0 + 15}, QStringLiteral("path_8")}, // TopLeft_Zone8
            Glyph{QStringLiteral(":/glyphs/phone3a/led_a_zones"), Glyph::Reference::TOP_LEFT, QPointF{0 + 15, 0 + 15}, QStringLiteral("path_9")}, // TopLeft_Zone9
            Glyph{QStringLiteral(":/glyphs/phone3a/led_a_zones"), Glyph::Reference::TOP_LEFT, QPointF{0 + 15, 0 + 15}, QStringLiteral("path_10")}, // TopLeft_Zone10
            Glyph{QStringLiteral(":/glyphs/phone3a/led_a_zones"), Glyph::Reference::TOP_LEFT, QPointF{0 + 15, 0 + 15}, QStringLiteral("path_11")}, // TopLeft_Zone11
            Glyph{QStringLiteral(":/glyphs/phone3a/led_a_zones"), Glyph::Reference::TOP_LEFT, QPointF{0 + 15, 0 + 15}, QStringLiteral("path_12")}, // TopLeft_Zone12
            Glyph{QStringLiteral(":/glyphs/phone3a/led_a_zones"), Glyph::Reference::TOP_LEFT, QPointF{0 + 15, 0 + 15}, QStringLiteral("path_13")}, // TopLeft_Zone13
            Glyph{QStringLiteral(":/glyphs/phone3a/led_a_zones"), Glyph::Reference::TOP_LEFT, QPointF{0 + 15, 0 + 15}, QStringLiteral("path_14")}, // TopLeft_Zone14
            Glyph{QStringLiteral(":/glyphs/phone3a/led_a_zones"), Glyph::Reference::TOP_LEFT, QPointF{0 + 15, 0 + 15}, QStringLiteral("path_15")}, // TopLeft_Zone15
            Glyph{QStringLiteral(":/glyphs/phone3a/led_a_zones"), Glyph::Reference::TOP_LEFT, QPointF{0 + 15, 0 + 15}, QStringLiteral("path_16")}, // TopLeft_Zone16
            Glyph{QStringLiteral(":/glyphs/phone3a/led_a_zones"), Glyph::Reference::TOP_LEFT, QPointF{0 + 15, 0 + 15}, QStringLiteral("path_17")}, // TopLeft_Zone17
            Glyph{QStringLiteral(":/glyphs/phone3a/led_a_zones"), Glyph::Reference::TOP_LEFT, QPointF{0 + 15, 0 + 15}, QStringLiteral("path_18")}, // TopLeft_Zone18
            Glyph{QStringLiteral(":/glyphs/phone3a/led_a_zones"), Glyph::Reference::TOP_LEFT, QPointF{0 + 15, 0 + 15}, QStringLiteral("path_19")}, // TopLeft_Zone19
            Glyph{QStringLiteral(":/glyphs/phone3a/led_b_zones"), Glyph::Reference::TOP_RIGHT, QPointF{0 - 15, 86.6 + 15}, QStringLiteral("path_0")}, // MiddleRight_Zone0
            Glyph{QStringLiteral(":/glyphs/phone3a/led_b_zones"), Glyph::Reference::TOP_RIGHT, QPointF{0 - 15, 86.6 + 15}, QStringLiteral("path_1")}, // MiddleRight_Zone1
            Glyph{QStringLiteral(":/glyphs/phone3a/led_b_zones"), Glyph::Reference::TOP_RIGHT, QPointF{0 - 15, 86.6 + 15}, QStringLiteral("path_2")}, // MiddleRight_Zone2
            Glyph{QStringLiteral(":/glyphs/phone3a/led_b_zones"), Glyph::Reference::TOP_RIGHT, QPointF{0 - 15, 86.6 + 15}, QStringLiteral("path_3")}, // MiddleRight_Zone3
            Glyph{QStringLiteral(":/glyphs/phone3a/led_b_zones"), Glyph::Reference::TOP_RIGHT, QPointF{0 - 15, 86.6 + 15}, QStringLiteral("path_4")}, // MiddleRight_Zone4
            Glyph{QStringLiteral(":/glyphs/phone3a/led_b_zones"), Glyph::Reference::TOP_RIGHT, QPointF{0 - 15, 86.6 + 15}, QStringLiteral("path_5")}, // MiddleRight_Zone5
            Glyph{QStringLiteral(":/glyphs/phone3a/led_b_zones"), Glyph::Reference::TOP_RIGHT, QPointF{0 - 15, 86.6 + 15}, QStringLiteral("path_6")}, // MiddleRight_Zone6
            Glyph{QStringLiteral(":/glyphs/phone3a/led_b_zones"), Glyph::Reference::TOP_RIGHT, QPointF{0 - 15, 86.6 + 15}, QStringLiteral("path_7")}, // MiddleRight_Zone7
            Glyph{QStringLiteral(":/glyphs/phone3a/led_b_zones"), Glyph::Reference::TOP_RIGHT, QPointF{0 - 15, 86.6 + 15}, QStringLiteral("path_8")}, // MiddleRight_Zone8
            Glyph{QStringLiteral(":/glyphs/phone3a/led_b_zones"), Glyph::Reference::TOP_RIGHT, QPointF{0 - 15, 86.6 + 15}, QStringLiteral("path_9")}, // MiddleRight_Zone9
            Glyph{QStringLiteral(":/glyphs/phone3a/led_b_zones"), Glyph::Reference::TOP_RIGHT, QPointF{0 - 15, 86.6 + 15}, QStringLiteral("path_10")}, // MiddleRight_Zone10
            Glyph{QStringLiteral(":/glyphs/phone3a/led_c_zones"), Glyph::Reference::BOTTOM_LEFT, QPointF{8.7 + 15, -22.1 - 15}, QStringLiteral("path_0")}, // BottomLeft_Zone0
            Glyph{QStringLiteral(":/glyphs/phone3a/led_c_zones"), Glyph::Reference::BOTTOM_LEFT, QPointF{8.7 + 15, -22.1 - 15}, QStringLiteral("path_1")}, // BottomLeft_Zone1
            Glyph{QStringLiteral(":/glyphs/phone3a/led_c_zones"), Glyph::Reference::BOTTOM_LEFT, QPointF{8.7 + 15, -22.1 - 15}, QStringLiteral("path_2")}, // BottomLeft_Zone2
            Glyph{QStringLiteral(":/glyphs/phone3a/led_c_zones"), Glyph::Reference::BOTTOM_LEFT, QPointF{8.7 + 15, -22.1 - 15}, QStringLiteral("path_3")}, // BottomLeft_Zone3
            Glyph{QStringLiteral(":/glyphs/phone3a/led_c_zones"), Glyph::Reference::BOTTOM_LEFT, QPointF{8.7 + 15, -22.1 - 15}, QStringLiteral("path_4")}, // BottomLeft_Zone4
        },
        Phone3aConfiguration::staticBuild,
        QList<qsizetype>({36}),
        QSize{314, 283},
        QList<MySvgRenderer>{
            Glyph{QStringLiteral(":/glyphs/phone3a/center_part"), Glyph::Reference::BOTTOM_LEFT, QPointF{14 + 15, 0 - 15}}, // CenterPart
        }
    } {}

    virtual IConfiguration* clone() const override {
        return new Phone3aConfiguration{*this};
    }
};

#endif // GV_PHONE3ACONFIGURATION_H
