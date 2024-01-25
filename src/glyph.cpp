#include "glyph.h"

const QRegularExpression Glyph::svgFillColorRegex = QRegularExpression("(?<=fill=\")[^\"]+(?=\")", QRegularExpression::PatternOption::MultilineOption);

Glyph::Glyph(const QString &filename, const Glyph::Reference& reference, const QPointF& referenceOffset, const QString& id)
    : QSvgRenderer{filename}, filename{filename}, reference{reference}, referenceOffset{referenceOffset}, id{id}
{

}

/*
 * ==================================
 *             Functions
 * ==================================
 */

void Glyph::render(QPainter* const painter, const QColor& color)
{
    // Set color
    QFile f(this->filename);
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text))
        return;
    QString content(f.readAll());
    content.replace(this->svgFillColorRegex, color.name(QColor::NameFormat::HexRgb));
    this->QSvgRenderer::load(content.toUtf8());

    // Render svg
    if (this->QSvgRenderer::elementExists(this->id))
        this->QSvgRenderer::render(painter, this->id, paintRect);
    else
        this->QSvgRenderer::render(painter, paintRect);
}

void Glyph::calculate(const QRect& drawArea, const qreal& scaleFactor)
{
    // Get the size of the svg
    this->paintRect = this->viewBoxF();

    // Scale size
    this->paintRect.setSize(this->paintRect.size() * scaleFactor);

    // Move to reference
    switch (this->reference)
    {
    case Reference::TopLeft:
        this->paintRect.moveTopLeft(drawArea.topLeft());
        break;
    case Reference::TopRight:
        this->paintRect.moveTopRight(drawArea.topRight());
        break;
    case Reference::BottomLeft:
        this->paintRect.moveBottomLeft(drawArea.bottomLeft());
        break;
    case Reference::BottomRight:
        this->paintRect.moveBottomRight(drawArea.bottomRight());
        break;
    case Reference::Centered:
        this->paintRect.moveCenter(drawArea.center());
        break;
    case Reference::CenteredHBottom:
        this->paintRect.moveCenter(drawArea.center());
        this->paintRect.moveBottom(drawArea.bottom());
        break;
    case Reference::CenteredHTop:
        this->paintRect.moveCenter(drawArea.center());
        this->paintRect.moveTop(drawArea.top());
        break;
    case Reference::CenteredVLeft:
        this->paintRect.moveCenter(drawArea.center());
        this->paintRect.moveLeft(drawArea.left());
        break;
    case Reference::CenteredVRight:
        this->paintRect.moveCenter(drawArea.center());
        this->paintRect.moveRight(drawArea.right());
        break;
    default:
        // This should never happen
        throw std::logic_error(std::string("[Development Error] switch in function '").append(__FUNCTION__).append("' not updated!"));
        return;
    }

    // Apply offset with scaling
    this->paintRect.translate(this->referenceOffset * scaleFactor);

    // Handle path id
    if (this->elementExists(this->id))
    {
        // Get rectangle with element transform
        QRectF pathRect = this->transformForElement(id).mapRect(this->boundsOnElement(id));
        // Set size with scaling
        this->paintRect.setSize(pathRect.size() * scaleFactor);
        // Apply offset with scaling
        this->paintRect.translate(pathRect.topLeft() * scaleFactor);
    }
}
