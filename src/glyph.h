#ifndef GLYPH_H
#define GLYPH_H

#include <QSvgRenderer>
#include <QRectF>
#include <QPainter>
#include "helper.h"

class Glyph : private QSvgRenderer
{
public:
    explicit Glyph(const QString &filename);

    /**
     * @brief Renders the Glyph with the given painter and with the paintRect as bounds.
     * @param painter The painter to draw with. WARNING: The opacity of the painter will be changed!!
     * @param opacity At what opacity the Glyph should be rendered. The default is MINIMUM_GLYPH_OPACITY_VALUE.
     */
    void render(QPainter *painter, const qreal& opacity);
    /**
     * @brief resetPaintRect Sets the paintRect to the viewbox of the Glyph.
     * @param factor What factor to scale the viewbox size with.
     * @return A reference to the reset paintRect.
     */
    QRectF& resetPaintRectAndScale(const qreal& factor);

    /**
     * @brief paintRect Holds the position and size of the Glyph. Is used in the render function.
     */
    QRectF paintRect;
};

#endif // GLYPH_H
