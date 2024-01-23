#include "glyph_widget.h"

GlyphWidget::GlyphWidget(QWidget *parent)
    : QWidget{parent}
{
    // Set size policy to constrain minimum window size + expand
    this->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding));

    // Load Phone (1) Glyphs from resources.qrc
    glyphsPhone1[(int)GlyphsPhone1Enum::Camera] = new Glyph(":/glyphs/phone1/led_1", Glyph::Reference::TopLeft, QPointF(9.35, 10));
    glyphsPhone1[(int)GlyphsPhone1Enum::Diagonal] = new Glyph(":/glyphs/phone1/led_2", Glyph::Reference::TopRight, QPointF(-21.7, 21.56));
    glyphsPhone1[(int)GlyphsPhone1Enum::Battery] = new Glyph(":/glyphs/phone1/led_3", Glyph::Reference::Centered, QPointF(0, 0));
    glyphsPhone1[(int)GlyphsPhone1Enum::USBLine] = new Glyph(":/glyphs/phone1/led_4", Glyph::Reference::CenteredHBottom, QPointF(0, -7.33));
    glyphsPhone1[(int)GlyphsPhone1Enum::USBDot] = new Glyph(":/glyphs/phone1/led_5", Glyph::Reference::CenteredHBottom, QPointF(0, -10));

    // Load Phone (2) Glyphs from resources.qrc
    glyphsPhone2[(int)GlyphsPhone2Enum::CameraTop] = new Glyph(":/glyphs/phone2/led_a1", Glyph::Reference::TopLeft, QPointF(13.88, 10 + 3.5));
    glyphsPhone2[(int)GlyphsPhone2Enum::CameraBottom] = new Glyph(":/glyphs/phone2/led_a2", Glyph::Reference::TopLeft, QPointF(22.75, 51.56 + 3.5));
    glyphsPhone2[(int)GlyphsPhone2Enum::Diagonal] = new Glyph(":/glyphs/phone2/led_b", Glyph::Reference::TopRight, QPointF(-22.51, 20.07 + 3.5));
    glyphsPhone2[(int)GlyphsPhone2Enum::BatteryTopRight_Zone0] = new Glyph(":/glyphs/phone2/led_c1_zones", Glyph::Reference::TopRight, QPointF(-11.77, 92.49 + 3.5), "path_0");
    glyphsPhone2[(int)GlyphsPhone2Enum::BatteryTopRight_Zone1] = new Glyph(":/glyphs/phone2/led_c1_zones", Glyph::Reference::TopRight, QPointF(-11.77, 92.49 + 3.5), "path_1");
    glyphsPhone2[(int)GlyphsPhone2Enum::BatteryTopRight_Zone2] = new Glyph(":/glyphs/phone2/led_c1_zones", Glyph::Reference::TopRight, QPointF(-11.77, 92.49 + 3.5), "path_2");
    glyphsPhone2[(int)GlyphsPhone2Enum::BatteryTopRight_Zone3] = new Glyph(":/glyphs/phone2/led_c1_zones", Glyph::Reference::TopRight, QPointF(-11.77, 92.49 + 3.5), "path_3");
    glyphsPhone2[(int)GlyphsPhone2Enum::BatteryTopRight_Zone4] = new Glyph(":/glyphs/phone2/led_c1_zones", Glyph::Reference::TopRight, QPointF(-11.77, 92.49 + 3.5), "path_4");
    glyphsPhone2[(int)GlyphsPhone2Enum::BatteryTopRight_Zone5] = new Glyph(":/glyphs/phone2/led_c1_zones", Glyph::Reference::TopRight, QPointF(-11.77, 92.49 + 3.5), "path_5");
    glyphsPhone2[(int)GlyphsPhone2Enum::BatteryTopRight_Zone6] = new Glyph(":/glyphs/phone2/led_c1_zones", Glyph::Reference::TopRight, QPointF(-11.77, 92.49 + 3.5), "path_6");
    glyphsPhone2[(int)GlyphsPhone2Enum::BatteryTopRight_Zone7] = new Glyph(":/glyphs/phone2/led_c1_zones", Glyph::Reference::TopRight, QPointF(-11.77, 92.49 + 3.5), "path_7");
    glyphsPhone2[(int)GlyphsPhone2Enum::BatteryTopRight_Zone8] = new Glyph(":/glyphs/phone2/led_c1_zones", Glyph::Reference::TopRight, QPointF(-11.77, 92.49 + 3.5), "path_8");
    glyphsPhone2[(int)GlyphsPhone2Enum::BatteryTopRight_Zone9] = new Glyph(":/glyphs/phone2/led_c1_zones", Glyph::Reference::TopRight, QPointF(-11.77, 92.49 + 3.5), "path_9");
    glyphsPhone2[(int)GlyphsPhone2Enum::BatteryTopRight_Zone10] = new Glyph(":/glyphs/phone2/led_c1_zones", Glyph::Reference::TopRight, QPointF(-11.77, 92.49 + 3.5), "path_10");
    glyphsPhone2[(int)GlyphsPhone2Enum::BatteryTopRight_Zone11] = new Glyph(":/glyphs/phone2/led_c1_zones", Glyph::Reference::TopRight, QPointF(-11.77, 92.49 + 3.5), "path_11");
    glyphsPhone2[(int)GlyphsPhone2Enum::BatteryTopRight_Zone12] = new Glyph(":/glyphs/phone2/led_c1_zones", Glyph::Reference::TopRight, QPointF(-11.77, 92.49 + 3.5), "path_12");
    glyphsPhone2[(int)GlyphsPhone2Enum::BatteryTopRight_Zone13] = new Glyph(":/glyphs/phone2/led_c1_zones", Glyph::Reference::TopRight, QPointF(-11.77, 92.49 + 3.5), "path_13");
    glyphsPhone2[(int)GlyphsPhone2Enum::BatteryTopRight_Zone14] = new Glyph(":/glyphs/phone2/led_c1_zones", Glyph::Reference::TopRight, QPointF(-11.77, 92.49 + 3.5), "path_14");
    glyphsPhone2[(int)GlyphsPhone2Enum::BatteryTopRight_Zone15] = new Glyph(":/glyphs/phone2/led_c1_zones", Glyph::Reference::TopRight, QPointF(-11.77, 92.49 + 3.5), "path_15");
    glyphsPhone2[(int)GlyphsPhone2Enum::BatteryTopLeft] = new Glyph(":/glyphs/phone2/led_c2", Glyph::Reference::TopLeft, QPointF(13.7, 101.28 + 3.5));
    glyphsPhone2[(int)GlyphsPhone2Enum::BatteryTopVertical] = new Glyph(":/glyphs/phone2/led_c3", Glyph::Reference::TopLeft, QPointF(11.94, 146.3 + 3.5));
    glyphsPhone2[(int)GlyphsPhone2Enum::BatteryBottomLeft] = new Glyph(":/glyphs/phone2/led_c4", Glyph::Reference::BottomLeft, QPointF(13.7, -88.56 - 3.5));
    glyphsPhone2[(int)GlyphsPhone2Enum::BatteryBottomRight] = new Glyph(":/glyphs/phone2/led_c5", Glyph::Reference::BottomRight, QPointF(-11.78, -97.36 - 3.5));
    glyphsPhone2[(int)GlyphsPhone2Enum::BatteryBottomVertical] = new Glyph(":/glyphs/phone2/led_c6", Glyph::Reference::BottomRight, QPointF(-10, -151.83 - 3.5));
    glyphsPhone2[(int)GlyphsPhone2Enum::USBDot] = new Glyph(":/glyphs/phone2/led_e", Glyph::Reference::CenteredHBottom, QPointF(0, -10 - 3.5));
    glyphsPhone2[(int)GlyphsPhone2Enum::USBLine_Zone0] = new Glyph(":/glyphs/phone2/led_d_zones", Glyph::Reference::CenteredHBottom, QPointF(0, -24.38 - 3.5), "path_0"); // Append the GLYPH_USB_LINE at the end because the light data is structured that way - ask Nothing why
    glyphsPhone2[(int)GlyphsPhone2Enum::USBLine_Zone1] = new Glyph(":/glyphs/phone2/led_d_zones", Glyph::Reference::CenteredHBottom, QPointF(0, -24.38 - 3.5), "path_1");
    glyphsPhone2[(int)GlyphsPhone2Enum::USBLine_Zone2] = new Glyph(":/glyphs/phone2/led_d_zones", Glyph::Reference::CenteredHBottom, QPointF(0, -24.38 - 3.5), "path_2");
    glyphsPhone2[(int)GlyphsPhone2Enum::USBLine_Zone3] = new Glyph(":/glyphs/phone2/led_d_zones", Glyph::Reference::CenteredHBottom, QPointF(0, -24.38 - 3.5), "path_3");
    glyphsPhone2[(int)GlyphsPhone2Enum::USBLine_Zone4] = new Glyph(":/glyphs/phone2/led_d_zones", Glyph::Reference::CenteredHBottom, QPointF(0, -24.38 - 3.5), "path_4");
    glyphsPhone2[(int)GlyphsPhone2Enum::USBLine_Zone5] = new Glyph(":/glyphs/phone2/led_d_zones", Glyph::Reference::CenteredHBottom, QPointF(0, -24.38 - 3.5), "path_5");
    glyphsPhone2[(int)GlyphsPhone2Enum::USBLine_Zone6] = new Glyph(":/glyphs/phone2/led_d_zones", Glyph::Reference::CenteredHBottom, QPointF(0, -24.38 - 3.5), "path_6");
    glyphsPhone2[(int)GlyphsPhone2Enum::USBLine_Zone7] = new Glyph(":/glyphs/phone2/led_d_zones", Glyph::Reference::CenteredHBottom, QPointF(0, -24.38 - 3.5), "path_7");

    // Init
    this->compositionManager = new CompositionManager(this->glyphOnColor, glyphOffValue);
    connect(this->compositionManager->player, SIGNAL(positionChanged(qint64)), this, SLOT(compositionManager_onPositionChanged(qint64)));
}

/*
 * ==================================
 *           Getter/Setter
 * ==================================
 */

void GlyphWidget::setVisual(CompositionManager::PhoneModel v)
{
    // Check if the Visual is compatible with the currently loaded composition.
    if (this->compositionManager->getGlyphMode() == CompositionManager::GlyphMode::Phone2 && v == CompositionManager::PhoneModel::Phone1)
        throw std::invalid_argument("Can't switch to the Phone (1) visualisation because the currently loaded Composition only supports Phone (2)");

    this->currentVisual = v;
    this->glyphWidgetSizeHint = glyphWidgetSizes[(int)v];
    this->resizeEvent(NULL);
    this->update();
}

/*
 * ==================================
 *             Functions
 * ==================================
 */

void GlyphWidget::calcPhone1Glyphs()
{
    // Calculate Glyph 1
    glyphsPhone1[(int)GlyphsPhone1Enum::Camera]->calculate(paintRectangle, currentSizeRatio);

    // Calculate Glyph 2
    glyphsPhone1[(int)GlyphsPhone1Enum::Diagonal]->calculate(paintRectangle, currentSizeRatio);

    // Calculate Glyph 3
    glyphsPhone1[(int)GlyphsPhone1Enum::Battery]->calculate(paintRectangle, currentSizeRatio);

    // Calculate Glyph 5
    glyphsPhone1[(int)GlyphsPhone1Enum::USBDot]->calculate(paintRectangle, currentSizeRatio);

    // Calculate Glyph 4
    QRect tmp = QRect(paintRectangle);
    tmp.setBottom(glyphsPhone1[(int)GlyphsPhone1Enum::USBDot]->paintRect.top()); // Modify the paintRectangle so the glyph gets aligned to the top of the Glyph 5
    glyphsPhone1[(int)GlyphsPhone1Enum::USBLine]->calculate(tmp, currentSizeRatio);
}

void GlyphWidget::calcPhone2Glyphs()
{
    // Calc all Glyphs
    for (const auto& glyph : glyphsPhone2)
    {
        glyph->calculate(paintRectangle, currentSizeRatio);
    }
}

void GlyphWidget::resizeEvent(QResizeEvent* event)
{
    // Scale sizeHint while keeping the aspect ratio
    paintRectangle.setSize(this->sizeHint().scaled(this->size(), Qt::KeepAspectRatio));

    // Center the drawing area
    this->paintRectangle.moveCenter(this->rect().center());

    // Calculate the new size ratio
    currentSizeRatio = paintRectangle.height() / (qreal)this->sizeHint().height();

    switch (this->currentVisual)
    {
    case CompositionManager::PhoneModel::Phone1:
        calcPhone1Glyphs();
        break;
    case CompositionManager::PhoneModel::Phone2:
        calcPhone2Glyphs();
        break;
    case CompositionManager::PhoneModel::None:
        // This should never happen
        throw std::logic_error(std::string("[Development Error] Phone model None in function '").append(__FUNCTION__).append("' - can't be None!"));
    default:
        // This should never happen
        throw std::logic_error(std::string("[Development Error] switch in function '").append(__FUNCTION__).append("' not updated!"));
        return;
    }

}

QSize GlyphWidget::sizeHint() const
{
    return glyphWidgetSizeHint;
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
    switch (this->currentVisual)
    {
    case CompositionManager::PhoneModel::Phone1:
        glyphs = &glyphsPhone1;
        break;
    case CompositionManager::PhoneModel::Phone2:
        glyphs = &glyphsPhone2;
        break;
    case CompositionManager::PhoneModel::None:
        // This should never happen
        throw std::logic_error(std::string("[Development Error] Phone model None in function '").append(__FUNCTION__).append("' - can't be None!"));
    default:
        // This should never happen
        throw std::logic_error(std::string("[Development Error] switch in function '").append(__FUNCTION__).append("' not updated!"));
        return;
    }

    // Paint Glyphs
    const QList<QColor> *const colorValues = this->compositionManager->getPhoneColorValues(this->compositionManager->player->position(), this->currentVisual);
    for (qsizetype i = 0; i < glyphs->length(); i++)
    {
        // Render this Glyph with the appropriate color
        switch (this->currentVisual)
        {
        case CompositionManager::PhoneModel::Phone1:
        case CompositionManager::PhoneModel::Phone2:
            glyphs->at(i)->render(&painter, colorValues->at(i));
            break;
        case CompositionManager::PhoneModel::None:
            // This should never happen
            throw std::logic_error(std::string("[Development Error] Phone model None in function '").append(__FUNCTION__).append("' - can't be None!"));
        default:
            // This should never happen
            throw std::logic_error(std::string("[Development Error] switch in function '").append(__FUNCTION__).append("' not updated!"));
            return;
        }
    }
}

/*
 * ==================================
 *               Slots
 * ==================================
 */

void GlyphWidget::compositionManager_onPositionChanged(qint64 position)
{
    // Rerender the widget - this function will only be called every 40-60ms so this is fine.
    this->update();
}

/*
 * ==================================
 *           Deconstructor
 * ==================================
 */

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
