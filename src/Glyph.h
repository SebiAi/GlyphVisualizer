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

#ifndef GV_GLYPH_H
#define GV_GLYPH_H

#include <QColor>
#include <QImage>
#include <QPainter>
#include <QPointF>
#include <QRect>
#include <QRectF>
#include <QSize>
#include <QString>

#include "MySvgRenderer.h"

class Glyph : public MySvgRenderer
{
public:

	explicit Glyph(const QString& filename, const Reference& reference, const QPointF& referenceOffset, const QString& id = QString());
    Glyph(const Glyph& g);

	void renderColored(QPainter* painter, const QColor& color);
	virtual void calcBounds(const QRect& drawingArea, qreal scale) override;

private:
    QImage svgImage;
};

#endif // GV_GLYPH_H
