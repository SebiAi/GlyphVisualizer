#include "GlyphWidget.h"

GlyphWidget::GlyphWidget(QWidget *parent)
    : QWidget{parent}
{
//    // Draw green background for now
//    QPalette palette = this->palette();
//    palette.setColor(QPalette::Window, Qt::green);
//    this->setAutoFillBackground(true);
//    this->setPalette(palette);

    // Set size policy to constrain minimum window size + expand
    this->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding));

    // Load Phone (1) Glyphs from resources.qrc
    glyphsPhone1.append(new Glyph(":/glyphs/phone1/led_1"));
    glyphsPhone1.append(new Glyph(":/glyphs/phone1/led_2"));
    glyphsPhone1.append(new Glyph(":/glyphs/phone1/led_3"));
    glyphsPhone1.append(new Glyph(":/glyphs/phone1/led_4"));
    glyphsPhone1.append(new Glyph(":/glyphs/phone1/led_5"));

    // Load Phone (2) Glyphs from resources.qrc
    glyphsPhone2.append(new Glyph(":/glyphs/phone2/led_a1"));
    glyphsPhone2.append(new Glyph(":/glyphs/phone2/led_a2"));
    glyphsPhone2.append(new Glyph(":/glyphs/phone2/led_b"));
    glyphsPhone2.append(new Glyph(":/glyphs/phone2/led_c1"));
    glyphsPhone2.append(new Glyph(":/glyphs/phone2/led_c2"));
    glyphsPhone2.append(new Glyph(":/glyphs/phone2/led_c3"));
    glyphsPhone2.append(new Glyph(":/glyphs/phone2/led_c4"));
    glyphsPhone2.append(new Glyph(":/glyphs/phone2/led_c5"));
    glyphsPhone2.append(new Glyph(":/glyphs/phone2/led_c6"));
    glyphsPhone2.append(new Glyph(":/glyphs/phone2/led_e"));
    glyphsPhone2.append(new Glyph(":/glyphs/phone2/led_d")); // Append the GLYPH_USB_LINE at the end because the light data is structured that way - ask Nothing why

    // Init
    this->compositionManager = new CompositionManager();
    connect(this->compositionManager->player, SIGNAL(positionChanged(qint64)), this, SLOT(compositionManager_onPositionChanged(qint64)));
}

void GlyphWidget::compositionManager_onPositionChanged(qint64 position)
{
    // Rerender the widget - this function will only be called every 40-60ms so this is fine.
    this->update();
}

QSize GlyphWidget::sizeHint() const
{
    return glyphWidgetSizeHint;
}

void GlyphWidget::calcPhone1Glyphs()
{
    // Calculate Glyph 1
    glyphsPhone1[0]->resetPaintRectAndScale(currentSizeRatio).moveTopLeft(paintRectangle.topLeft());
    glyphsPhone1[0]->paintRect.translate(QPointF(9.35, 10) * currentSizeRatio);

    // Calculate Glyph 2
    glyphsPhone1[1]->resetPaintRectAndScale(currentSizeRatio).moveTopRight(paintRectangle.topRight());
    glyphsPhone1[1]->paintRect.translate(QPointF(-21.7, 21.56) * currentSizeRatio);

    // Calculate Glyph 3
    glyphsPhone1[2]->resetPaintRectAndScale(currentSizeRatio).moveCenter(paintRectangle.center());

    // Calculate Glyph 5
    glyphsPhone1[4]->resetPaintRectAndScale(currentSizeRatio).moveCenter(paintRectangle.center());
    glyphsPhone1[4]->paintRect.moveBottom(paintRectangle.bottom());
    glyphsPhone1[4]->paintRect.translate(QPointF(0, -10) * currentSizeRatio);

    // Calculate Glyph 4
    glyphsPhone1[3]->resetPaintRectAndScale(currentSizeRatio).moveCenter(paintRectangle.center());
    glyphsPhone1[3]->paintRect.moveBottom(glyphsPhone1[4]->paintRect.top());
    glyphsPhone1[3]->paintRect.translate(QPointF(0, -7.33) * currentSizeRatio);
}

void GlyphWidget::calcPhone2Glyphs()
{
    // Calculate Glyph a1
    glyphsPhone2[0]->resetPaintRectAndScale(currentSizeRatio).moveTopLeft(paintRectangle.topLeft());
    glyphsPhone2[0]->paintRect.translate(QPointF(13.88, 10 + 3.5) * currentSizeRatio);

    // Calculate Glyph a2
    glyphsPhone2[1]->resetPaintRectAndScale(currentSizeRatio).moveTopLeft(paintRectangle.topLeft());
    glyphsPhone2[1]->paintRect.translate(QPointF(22.75, 51.56 + 3.5) * currentSizeRatio);

    // Calculate Glyph b
    glyphsPhone2[2]->resetPaintRectAndScale(currentSizeRatio).moveTopRight(paintRectangle.topRight());
    glyphsPhone2[2]->paintRect.translate(QPointF(-22.51, 20.07 + 3.5) * currentSizeRatio);

    // Calculate Glyph c1
    glyphsPhone2[3]->resetPaintRectAndScale(currentSizeRatio).moveTopRight(paintRectangle.topRight());
    glyphsPhone2[3]->paintRect.translate(QPointF(-11.77, 92.49 + 3.5) * currentSizeRatio);

    // Calculate Glyph c2
    glyphsPhone2[4]->resetPaintRectAndScale(currentSizeRatio).moveTopLeft(paintRectangle.topLeft());
    glyphsPhone2[4]->paintRect.translate(QPointF(13.7, 101.28 + 3.5) * currentSizeRatio);

    // Calculate Glyph c3
    glyphsPhone2[5]->resetPaintRectAndScale(currentSizeRatio).moveTopLeft(paintRectangle.topLeft());
    glyphsPhone2[5]->paintRect.translate(QPointF(11.94, 146.3 + 3.5) * currentSizeRatio);

    // Calculate Glyph c4
    glyphsPhone2[6]->resetPaintRectAndScale(currentSizeRatio).moveBottomLeft(paintRectangle.bottomLeft());
    glyphsPhone2[6]->paintRect.translate(QPointF(13.7, -88.56 - 3.5) * currentSizeRatio);

    // Calculate Glyph c5
    glyphsPhone2[7]->resetPaintRectAndScale(currentSizeRatio).moveBottomRight(paintRectangle.bottomRight());
    glyphsPhone2[7]->paintRect.translate(QPointF(-11.78, -97.36 - 3.5) * currentSizeRatio);

    // Calculate Glyph c6
    glyphsPhone2[8]->resetPaintRectAndScale(currentSizeRatio).moveBottomRight(paintRectangle.bottomRight());
    glyphsPhone2[8]->paintRect.translate(QPointF(-10, -151.83 - 3.5) * currentSizeRatio);

    // Calculate Glyph e
    glyphsPhone2[9]->resetPaintRectAndScale(currentSizeRatio).moveCenter(paintRectangle.center());
    glyphsPhone2[9]->paintRect.moveBottom(paintRectangle.bottom());
    glyphsPhone2[9]->paintRect.translate(QPointF(0, -10 - 3.5) * currentSizeRatio);

    // Calculate Glyph d
    glyphsPhone2[10]->resetPaintRectAndScale(currentSizeRatio).moveCenter(paintRectangle.center());
    glyphsPhone2[10]->paintRect.moveBottom(paintRectangle.bottom());
    glyphsPhone2[10]->paintRect.translate(QPointF(0, -24.38 - 3.5) * currentSizeRatio);
}

void GlyphWidget::resizeEvent(QResizeEvent* event)
{
    // Scale sizeHint while keeping the aspect ratio
    paintRectangle.setSize(this->sizeHint().scaled(this->size(), Qt::KeepAspectRatio));
//    qDebug().nospace() << "sizeHint: " << this->sizeHint() << ", size: " << this->size() << ", newSize: " << drawingRectangle.size();

//    // Center the drawing area
    this->paintRectangle.moveCenter(this->rect().center());

    // Calculate the new size ratio
    currentSizeRatio = paintRectangle.height() / (qreal)this->sizeHint().height();
//    qDebug().nospace() << "currentSizeRatio: " << currentSizeRatio;

    switch (this->currentVisual)
    {
    case Visual::Phone1:
        calcPhone1Glyphs();
        break;
    case Visual::Phone2:
        calcPhone2Glyphs();
        break;
    default:
        // This should never happen
        throw std::logic_error(std::string("[Development Error] switch in function '").append(__FUNCTION__).append("' not updated!"));
        return;
    }

}

void GlyphWidget::paintEvent(QPaintEvent *event)
{
    // Get the painter
    QPainter painter(this);

    // Render background
    painter.setPen(Qt::PenStyle::NoPen); // No border
    painter.setBrush(QBrush(QColor::fromRgb(47, 48, 51))); // Set color
    painter.drawRoundedRect(paintRectangle, 22 * currentSizeRatio, 22 * currentSizeRatio);

    // Get pointers to the Phone (1) or Phone (2) versions of each list/function
    QList<Glyph*> *glyphs = NULL;
    const QList<qreal>* const (CompositionManager::*opacityValuesFunction)(qint64) const;
    switch (this->currentVisual)
    {
    case Visual::Phone1:
        glyphs = &glyphsPhone1;
        opacityValuesFunction = &CompositionManager::getPhone1OpacityValues;
        break;
    case Visual::Phone2:
        glyphs = &glyphsPhone2;
        opacityValuesFunction = &CompositionManager::getPhone2OpacityValues;
        break;
    default:
        // This should never happen
        throw std::logic_error(std::string("[Development Error] switch in function '").append(__FUNCTION__).append("' not updated!"));
        return;
    }

    // Paint Glyphs
    const QList<qreal> *const opacityValues = (compositionManager->*opacityValuesFunction)(compositionManager->player->position());
    for (qsizetype i = 0; i < glyphs->length(); i++)
    {
        // Render this Glyph with the appropriate opacity
        switch (this->currentVisual)
        {
        case Visual::Phone1:
            glyphs->at(i)->render(&painter, opacityValues->at(i));
            break;
        case Visual::Phone2:
            switch (i)
            {
            case 3: // GLYPH_BATTERY
                // Calculate the average as the opacity value from the 16 Zones for now
                glyphs->at(i)->render(&painter, std::accumulate(opacityValues->begin() + 3, opacityValues->begin() + 19, (qreal)0) / 16.0);
                break;
            case 10: // GLYPH_USB_LINE
                // Calculate the average as the opacity value from the 8 Zones for now
                glyphs->at(i)->render(&painter, std::accumulate(opacityValues->begin() + 25, opacityValues->begin() + 33, (qreal)0) / 8.0);
                break;
            default:
                glyphs->at(i)->render(&painter, opacityValues->at(zone33To11GlyphsLookupTable[i]));
                break;
            }
            break;
        default:
            // This should never happen
            throw std::logic_error(std::string("[Development Error] switch in function '").append(__FUNCTION__).append("' not updated!"));
            return;
        }
    }
}

GlyphWidget::~GlyphWidget()
{
    for (auto x: glyphsPhone1) {
        delete x;
    }
    for (auto x: glyphsPhone2) {
        delete x;
    }

    delete this->compositionManager;
}
