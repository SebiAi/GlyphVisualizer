#ifndef GLYPH_H
#define GLYPH_H

#include <QSvgRenderer>
#include <QRectF>
#include <QPointF>
#include <QPainter>

class Glyph : private QSvgRenderer
{
public:
    /**
     * @brief Which side is referenced.
     */
    enum class Reference
    {
        TopLeft,
        TopRight,
        BottomLeft,
        BottomRight,
        Centered,
        CenteredHBottom,
        CenteredHTop,
        CenteredVLeft,
        CenteredVRight
    };

    /**
     * @brief Constructor for a Glyph object.
     * @param filename The filename of the svg file.
     * @param minOpacityValue The minimum opacity value which sould be used when rendering. Between 1 to 0.
     * @param reference The reference mode. Is used in the calculate function.
     * @param referenceOffset The offset from the reference. Is used in the calculate function.
     * @param id The id of a path. If specified renders only the path with this id if available. Will render the whole svg if the path id is not available.
     */
    explicit Glyph(const QString &filename, const qreal& minOpacityValue, const Glyph::Reference& reference, const QPointF& referenceOffset, const QString& id = QString());

    /**
     * @brief Renders the Glyph with the given painter and with the paintRect as bounds. Call calculate first.
     * @param painter The painter to draw with. WARNING: The opacity of the painter will be changed!!
     * @param opacity At what opacity the Glyph should be rendered. The minimum is always minOpacityValue. Must be between 0 and 1.
     */
    void render(QPainter *painter, const qreal& opacity);

    /**
     * @brief Calculates the paint rectangle for the Glyph.
     * @param drawArea The current area to reference.
     * @param scaleFactor The offset from the reference.
     */
    void calculate(const QRect& drawArea, const qreal& scaleFactor);

    /**
     * @brief paintRect Holds the position and size of the Glyph. Is used in the render function.
     */
    QRectF paintRect;

private:
    /**
     * @brief The minimal opacity value the Glyph should have.
     */
    const qreal minOpacityValue;

    /**
     * @brief The reference when calculating the paintRect.
     */
    const Glyph::Reference reference;

    /**
     * @brief The reference offset when calculating the paintRect.
     */
    const QPointF referenceOffset;

    /**
     * @brief Holds the specific path id if provided, else an empty string.
     */
    const QString id;
};

#endif // GLYPH_H
