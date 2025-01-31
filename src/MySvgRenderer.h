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

#ifndef GV_MYSVGRENDERER_H
#define GV_MYSVGRENDERER_H

#include <QPainter>
#include <QPointF>
#include <QRect>
#include <QRectF>
#include <QString>
#include <QSvgRenderer>

class MySvgRenderer : private QSvgRenderer
{
    Q_OBJECT
public:
    enum class Reference {
        TOP_LEFT = 0,
        TOP_RIGHT = 1,
        BOTTOM_LEFT = 2,
        BOTTOM_RIGHT = 3,
        CENTERED = 4,
        CENTERED_H_BOTTOM = 5,
        CENTERED_H_TOP = 6,
        CENTERED_V_LEFT = 7,
        CENTERED_V_RIGHT = 8
    };
    Q_ENUM(Reference)

    explicit MySvgRenderer(const QString& filename, const Reference& reference, const QPointF& referenceOffset, const QString& id = QString());
    MySvgRenderer(const MySvgRenderer& g);

    QRectF getScaledAlignedBounds() const { return this->scaledAlignedBounds; }

    void render(QPainter* painter);
    virtual void calcBounds(const QRect& drawingArea, qreal scale);

protected:
    const QString filename;
    Reference reference;
    QPointF referenceOffset;
    QString id;
    QRectF bounds;
    QRectF scaledAlignedBounds;
};

#endif // GV_MYSVGRENDERER_H
