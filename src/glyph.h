#ifndef GLYPH_H
#define GLYPH_H

#include <QSvgRenderer>
#include <QRectF>
#include <QPointF>
#include <QPainter>
#include <QFile>
#include <QRegularExpression>

class Glyph : private QSvgRenderer
{
    Q_OBJECT
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
    Q_ENUM(Reference)

    /**
     * @brief Constructor for a Glyph object.
     * @param filename The filename of the svg file.
     * @param reference The reference mode. Is used in the calculate function.
     * @param referenceOffset The offset from the reference. Is used in the calculate function.
     * @param id The id of a path. If specified renders only the path with this id if available. Will render the whole svg if the path id is not available.
     */
    explicit Glyph(const QString &filename, const Glyph::Reference& reference, const QPointF& referenceOffset, const QString& id = QString(), QObject *parent = nullptr);

    /**
     * @brief Renders the Glyph with the given painter and with the paintRect as bounds. Call calculate first.
     * @param painter The painter to draw with.
     * @param color Which color the Glyph should be.
     */
    void render(QPainter* const painter, const QColor& color);

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
     * @brief The filename of the svg file.
     */
    const QString filename;

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

    /**
     * @brief Holds the regex needed to change the color of an svg.
     */
    static const QRegularExpression svgFillColorRegex;
};

#endif // GLYPH_H
