#include "glyph.h"

Glyph::Glyph(const QString &filename, const qreal& minOpacityValue)
    : QSvgRenderer{filename}, minOpacityValue{minOpacityValue}
{
    Q_ASSERT(this->minOpacityValue >= 0 && this->minOpacityValue <= 1.0);
}

/*
 * ==================================
 *             Functions
 * ==================================
 */

void Glyph::render(QPainter *painter, const qreal& opacity)
{
    painter->setOpacity(opacity < this->minOpacityValue ? this->minOpacityValue : opacity);
    this->QSvgRenderer::render(painter, paintRect);
}

QRectF& Glyph::resetPaintRectAndScale(const qreal& factor)
{
    this->paintRect = this->viewBoxF();
    this->paintRect.setSize(this->paintRect.size() * factor);
    return this->paintRect;
}
