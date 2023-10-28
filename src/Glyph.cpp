#include "Glyph.h"

Glyph::Glyph(const QString &filename)
    : QSvgRenderer{filename}
{

}

void Glyph::render(QPainter *painter, qreal opacity)
{
    painter->setOpacity(opacity >= 0 ? opacity : 0.12);
    this->QSvgRenderer::render(painter, paintRect);
}

QRectF& Glyph::resetPaintRectAndScale(qreal factor)
{
    this->paintRect = this->QSvgRenderer::viewBoxF();
    this->paintRect.setSize(this->paintRect.size() * factor);
    return this->paintRect;
}
