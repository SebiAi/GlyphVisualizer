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

#ifndef GV_PHONE1CONFIGURATION_H
#define GV_PHONE1CONFIGURATION_H

#include <QStringLiteral>

#include "IConfiguration.h"
#include "../Utils.h"

class Phone1Configuration : public IConfiguration {
public:
    static constexpr DeviceBuild staticBuild = DeviceBuild::Spacewar;

    Phone1Configuration(const QColor& fallbackColor): IConfiguration{
        fallbackColor,
        QList<Glyph>{
            Glyph{QStringLiteral(":/glyphs/phone1/led_1"), Glyph::Reference::TOP_LEFT, QPointF{9.35, 10}}, // Camera
            Glyph{QStringLiteral(":/glyphs/phone1/led_2"), Glyph::Reference::TOP_RIGHT, QPointF{-21.7, 21.56}}, // Diagonal
			Glyph{QStringLiteral(":/glyphs/phone1/led_3_zones"), Glyph::Reference::CENTERED, QPointF{0, 0}, QStringLiteral("path_0")}, // BatteryBottomLeft
			Glyph{QStringLiteral(":/glyphs/phone1/led_3_zones"), Glyph::Reference::CENTERED, QPointF{0, 0}, QStringLiteral("path_1")}, // BatteryBottomRight
			Glyph{QStringLiteral(":/glyphs/phone1/led_3_zones"), Glyph::Reference::CENTERED, QPointF{0, 0}, QStringLiteral("path_2")}, // BatteryTopRight
			Glyph{QStringLiteral(":/glyphs/phone1/led_3_zones"), Glyph::Reference::CENTERED, QPointF{0, 0}, QStringLiteral("path_3")}, // BatteryTopLeft
            Glyph{QStringLiteral(":/glyphs/phone1/led_5"), Glyph::Reference::CENTERED_H_BOTTOM, QPointF{0, -10}}, // USBDot
			Glyph{QStringLiteral(":/glyphs/phone1/led_4_zones"), Glyph::Reference::CENTERED_H_BOTTOM, QPointF{0, -7.33}, QStringLiteral("path_0")}, // USBLine_Zone0
			Glyph{QStringLiteral(":/glyphs/phone1/led_4_zones"), Glyph::Reference::CENTERED_H_BOTTOM, QPointF{0, -7.33}, QStringLiteral("path_1")}, // USBLine_Zone1
			Glyph{QStringLiteral(":/glyphs/phone1/led_4_zones"), Glyph::Reference::CENTERED_H_BOTTOM, QPointF{0, -7.33}, QStringLiteral("path_2")}, // USBLine_Zone2
			Glyph{QStringLiteral(":/glyphs/phone1/led_4_zones"), Glyph::Reference::CENTERED_H_BOTTOM, QPointF{0, -7.33}, QStringLiteral("path_3")}, // USBLine_Zone3
			Glyph{QStringLiteral(":/glyphs/phone1/led_4_zones"), Glyph::Reference::CENTERED_H_BOTTOM, QPointF{0, -7.33}, QStringLiteral("path_4")}, // USBLine_Zone4
			Glyph{QStringLiteral(":/glyphs/phone1/led_4_zones"), Glyph::Reference::CENTERED_H_BOTTOM, QPointF{0, -7.33}, QStringLiteral("path_5")}, // USBLine_Zone5
			Glyph{QStringLiteral(":/glyphs/phone1/led_4_zones"), Glyph::Reference::CENTERED_H_BOTTOM, QPointF{0, -7.33}, QStringLiteral("path_6")}, // USBLine_Zone6
			Glyph{QStringLiteral(":/glyphs/phone1/led_4_zones"), Glyph::Reference::CENTERED_H_BOTTOM, QPointF{0, -7.33}, QStringLiteral("path_7")}, // USBLine_Zone7
        },
        Phone1Configuration::staticBuild,
        QList<qsizetype>({15, 5}),
        QSize{182, 382}
    } {}

    virtual void calcBounds(const QRect& drawingArea, qreal scale) override {
        for (qsizetype i = 0; i <= 5; ++i) // Camera, Diagonal, BatteryTopRight, BatteryTopLeft, BatteryBottomRight, BatteryBottomLeft
            this->glyphs[i].calcBounds(drawingArea, scale);

        this->glyphs[6].calcBounds(drawingArea, scale);

        // Calculate the bounds of USBLine last because it depends on the bounds of USBDot
        QRect tmpDrawingArea{drawingArea};
        // Modify the drawing_area so the glyph gets aligned to the top of the USBDot glyph
        tmpDrawingArea.setBottom(this->glyphs[6].getScaledAlignedBounds().top());
        for (qsizetype i = 7; i <= 14; ++i) // USBLine
            this->glyphs[i].calcBounds(tmpDrawingArea, scale);
    }

protected:
    virtual void renderPrivate(QPainter& painter, const QList<QColor>& colors) override {
        switch (colors.size()) {
        case 15:
            IConfiguration::renderPrivate(painter, colors);
            break;
        case 5:
            for (qsizetype i = 0; i < 5; ++i) {
                switch (i) {
                case 0:
                    this->glyphs[0].renderColored(&painter, colors.at(i));
                    break;
                case 1:
                    this->glyphs[1].renderColored(&painter, colors.at(i));
                    break;
                case 2:
                    for (qsizetype j = 2; j <= 5; ++j)
                        this->glyphs[j].renderColored(&painter, colors.at(i));
                    break;
                case 3:
                    for (qsizetype j = 7; j <= 14; ++j)
                        this->glyphs[j].renderColored(&painter, colors.at(i));
                    break;
                case 4:
                    this->glyphs[6].renderColored(&painter, colors.at(i));
                    break;
                }
            }
            break;
        default:
            throw std::logic_error("Invalid colors length! Got: " + std::to_string(colors.size()) + ", Expected: " + listToString(this->supportedZones).toStdString());
        }
    }
};

#endif // GV_PHONE1CONFIGURATION_H
