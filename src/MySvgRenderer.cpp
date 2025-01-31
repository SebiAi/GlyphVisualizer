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

#include "MySvgRenderer.h"

MySvgRenderer::MySvgRenderer(const QString& filename, const Reference& reference, const QPointF& referenceOffset, const QString& id)
    : QSvgRenderer{}, filename{filename}, reference{reference}, referenceOffset{referenceOffset}, id{id}, bounds{QRectF()}, scaledAlignedBounds{QRectF()}
{
    if (!load(filename)) {
        throw std::logic_error("Could not load glyph file '" + filename.toStdString() + "'");
    }

    this->bounds = viewBoxF();

    if (!this->id.isEmpty() && !elementExists(this->id))
        throw std::logic_error("Path " + this->id.toStdString() + " does not exist in svg " + filename.toStdString() + "!");
}
MySvgRenderer::MySvgRenderer(const MySvgRenderer& g)
    : MySvgRenderer{g.filename, g.reference, g.referenceOffset, g.id}
{
    // Copy all other values
    this->bounds = g.bounds;
    this->scaledAlignedBounds = g.scaledAlignedBounds;
}

void MySvgRenderer::render(QPainter* painter) {
    painter->save();
    painter->setRenderHints(QPainter::RenderHint::Antialiasing | QPainter::RenderHint::SmoothPixmapTransform);
    if (!this->id.isEmpty())
    {
        QSvgRenderer::render(painter, this->id, this->scaledAlignedBounds);
    } else {
        QSvgRenderer::render(painter, this->scaledAlignedBounds);
    }
    painter->restore();
}

void MySvgRenderer::calcBounds(const QRect& drawingArea, qreal scale) {
    // Scale the bounds
    this->scaledAlignedBounds = this->bounds;
    this->scaledAlignedBounds.setSize(this->scaledAlignedBounds.size() * scale);

    // Align the svg to the reference point
    switch (this->reference) {
    case MySvgRenderer::Reference::TOP_LEFT:
        this->scaledAlignedBounds.moveTopLeft(drawingArea.topLeft());
        break;
    case MySvgRenderer::Reference::TOP_RIGHT:
        this->scaledAlignedBounds.moveTopRight(drawingArea.topRight());
        break;
    case MySvgRenderer::Reference::BOTTOM_LEFT:
        this->scaledAlignedBounds.moveBottomLeft(drawingArea.bottomLeft());
        break;
    case MySvgRenderer::Reference::BOTTOM_RIGHT:
        this->scaledAlignedBounds.moveBottomRight(drawingArea.bottomRight());
        break;
    case MySvgRenderer::Reference::CENTERED:
        this->scaledAlignedBounds.moveCenter(drawingArea.center());
        break;
    case MySvgRenderer::Reference::CENTERED_H_BOTTOM:
        this->scaledAlignedBounds.moveCenter(drawingArea.center());
        this->scaledAlignedBounds.moveBottom(drawingArea.bottom());
        break;
    case MySvgRenderer::Reference::CENTERED_H_TOP:
        this->scaledAlignedBounds.moveCenter(drawingArea.center());
        this->scaledAlignedBounds.moveTop(drawingArea.top());
        break;
    case MySvgRenderer::Reference::CENTERED_V_LEFT:
        this->scaledAlignedBounds.moveCenter(drawingArea.center());
        this->scaledAlignedBounds.moveLeft(drawingArea.left());
        break;
    case MySvgRenderer::Reference::CENTERED_V_RIGHT:
        this->scaledAlignedBounds.moveCenter(drawingArea.center());
        this->scaledAlignedBounds.moveRight(drawingArea.right());
        break;
    default:
        throw std::logic_error("Missing case in Glyph::calcBounds");
    }

    // Translate with the offset
    this->scaledAlignedBounds.translate(this->referenceOffset * scale);

    // Handle paths
    if (!this->id.isEmpty()) {
        // Get rectangle with element transform
        QRectF pathRect = transformForElement(this->id).mapRect(boundsOnElement(this->id));
        // Scale and translate
        this->scaledAlignedBounds.setSize(pathRect.size() * scale);
        this->scaledAlignedBounds.translate(pathRect.topLeft() * scale);
    }
}
