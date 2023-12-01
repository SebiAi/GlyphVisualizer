#include "glyph.h"

Glyph::Glyph(const QString &filename)
    : QSvgRenderer{filename}
{

}

void Glyph::render(QPainter *painter, qreal opacity)
{
    Q_ASSERT(opacity >= MINIMUM_GLYPH_OPACITY_VALUE);
    painter->setOpacity(opacity);
    this->QSvgRenderer::render(painter, paintRect);
}

QRectF& Glyph::resetPaintRectAndScale(qreal factor)
{
    this->paintRect = this->viewBoxF();
    this->paintRect.setSize(this->paintRect.size() * factor);
    return this->paintRect;
}
