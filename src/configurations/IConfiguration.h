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

#ifndef GV_ICONFIGURATION_H
#define GV_ICONFIGURATION_H

#include <QColor>
#include <QList>
#include <QPainter>
#include <QRect>
#include <QSize>

#include "DeviceBuild.h"
#include "../Glyph.h"
#include "../MySvgRenderer.h"

using namespace DeviceBuildNS;

class IConfiguration {
public:
    const QColor fallbackColor;
    QList<Glyph> glyphs;
    QList<QList<QColor>> parsedColors;
    const DeviceBuild build;
    QList<qsizetype> supportedZones;
    const QSize sizeHint;
	QList<MySvgRenderer> decorations;

	IConfiguration(const QColor& fallbackColor, const QList<Glyph>& glyphs, const DeviceBuild& build, const QList<qsizetype>& supportedZones, const QSize& sizeHint, const QList<MySvgRenderer>& decorations = QList<MySvgRenderer>())
		: fallbackColor{fallbackColor}, glyphs{glyphs}, parsedColors{}, build{build}, supportedZones{supportedZones}, sizeHint{sizeHint}, decorations{decorations}
    {
        if (!this->fallbackColor.isValid())
            throw std::logic_error("fallbackColor must be valid!");

        if (this->supportedZones.isEmpty())
            throw std::logic_error("supportedZones must not be empty!");
    }
    virtual ~IConfiguration() {}

    virtual void calcBounds(const QRect& drawingArea, qreal scale) {
		for (MySvgRenderer& s: this->decorations)
			s.calcBounds(drawingArea, scale);

		for (Glyph& g: this->glyphs)
            g.calcBounds(drawingArea, scale);
    }

    void render(QPainter& painter, qsizetype colorIndex) {
        // Get the color from the parsedColors or the fallbackColor
        QList<QColor> colors = this->parsedColors.value(colorIndex, QList<QColor>(this->supportedZones[0], fallbackColor));

        renderPrivate(painter, colors);
    }

protected:
    virtual void renderPrivate(QPainter& painter, const QList<QColor>& colors) {
		if (this->glyphs.size() != colors.size())
            throw std::logic_error("The default implementation expects equal size of glyphs and colors!");

		// Render the decorations first because we always want to see the glyphs
		for (MySvgRenderer& s: this->decorations)
			s.render(&painter);

		// Render the glyphs second to render over any decorations that render to the same spot
        for (qsizetype i = 0; i < colors.size(); ++i)
            this->glyphs[i].renderColored(&painter, colors.at(i));
    }
};

#endif // GV_ICONFIGURATION_H
