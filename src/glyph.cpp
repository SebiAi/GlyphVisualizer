#include "glyph.h"

Glyph::Glyph(const QString &filename)
    : QSvgRenderer{filename}
{

}

/*
 * ==================================
 *             Functions
 * ==================================
 */

void Glyph::render(QPainter *painter, const qreal& opacity)
{
    Q_ASSERT(opacity >= MINIMUM_GLYPH_OPACITY_VALUE);
    painter->setOpacity(opacity);
    this->QSvgRenderer::render(painter, paintRect);
}

QRectF& Glyph::resetPaintRectAndScale(const qreal& factor)
{
    this->paintRect = this->viewBoxF();
    this->paintRect.setSize(this->paintRect.size() * factor);
    return this->paintRect;
}
