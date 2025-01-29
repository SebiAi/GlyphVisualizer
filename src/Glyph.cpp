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

#include "Glyph.h"

Glyph::Glyph(const QString& filename, const Reference& reference, const QPointF& referenceOffset, const QString& id)
	: MySvgRenderer{filename, reference, referenceOffset, id}
{
	// Use the ARGB32_Premultiplied format because it is best optimized for rendering with QPainter
    this->svgImage = QImage(QSize(1, 1), QImage::Format::Format_ARGB32_Premultiplied);
}
Glyph::Glyph(const Glyph& g)
	: MySvgRenderer{g}, svgImage{g.svgImage}
{}

void Glyph::renderColored(QPainter* painter, const QColor& color) {

    // Render the svg to a transparent image and then color it
    // We don't know if the painter passed in draws on a transparent surface so we need to take this detour

    this->svgImage.fill(Qt::GlobalColor::transparent); // Make sure the image is transparent or the coloring won't work properly

    // Draw the svg onto the image
	QRectF savedAlignedBounds{this->scaledAlignedBounds};
	this->scaledAlignedBounds = this->svgImage.rect().toRectF();
    QPainter svgPainter(&this->svgImage);
	MySvgRenderer::render(&svgPainter);
	this->scaledAlignedBounds = savedAlignedBounds;

    // Color the svg with the requested color
	svgPainter.setCompositionMode(QPainter::CompositionMode::CompositionMode_SourceAtop);
	svgPainter.fillRect(this->svgImage.rect(), color);

    // Stop painting on the image and draw the image to the original painter
    svgPainter.end();
    painter->save();
    painter->setRenderHints(QPainter::RenderHint::Antialiasing | QPainter::RenderHint::SmoothPixmapTransform);
	painter->drawImage(this->scaledAlignedBounds, svgImage);
    painter->restore();
}

void Glyph::calcBounds(const QRect& drawingArea, qreal scale) {
	MySvgRenderer::calcBounds(drawingArea, scale);

    // Adjust the size of the intermediate image
    // Use the ARGB32_Premultiplied format because it is best optimized for rendering with QPainter
    // Also make sure that we have an valid image of at least size 1x1. We get a QPainter error spam otherwise
    // We double the scale to make the rendered SVG look good after rendering the image to the passed in painter
	this->svgImage = QImage(this->scaledAlignedBounds.size().toSize().expandedTo(QSize(1, 1)) * 2, QImage::Format::Format_ARGB32_Premultiplied);
}
