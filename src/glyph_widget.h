#ifndef GLYPHWIDGET_H
#define GLYPHWIDGET_H

#include <QWidget>
#include <QPainter>
#include <QResizeEvent>
#include <QMessageBox>
#include "glyph.h"
#include "composition_manager.h"

// The minimum percentage value the Glyphs should have. Must be between 0 - 1 (default: 0.12 => 12%)
#define MINIMUM_GLYPH_OPACITY_VALUE 0.12

class GlyphWidget : public QWidget
{
    Q_OBJECT
public:
    explicit GlyphWidget(QWidget *parent = nullptr);
    ~GlyphWidget();

    /**
     * @brief Enum definition for the visual modes.
     */
    enum class Visual { Phone1 = 0, Phone2 = 1 };

    /**
     * @brief Tries to set the visual depending on if the currently loaded composition allows it. If it is not possible an exception is thrown.
     * @param v Which visual to set.
     *
     * @throws std::invalid_argument When trying to set the visual mode to Phone (1) and the currently loaded composition is only compatible with Phone (2)
     */
    void setVisual(Visual v);

    /**
     * @brief Get the current visual mode.
     * @return The current visual mode.
     */
    const Visual& getVisualMode()
    {
        return this->currentVisual;
    }

    /**
     * @brief Loads the Compositions and plays the music.
     */
    CompositionManager *compositionManager;

public slots:
    /**
     * @brief Will be called by the void QMediaPlayer::positionChanged(qint64 position) signal which resides in the compositionManager.
     * @param position Position expressed in milliseconds.
     */
    void compositionManager_onPositionChanged(qint64 position);

private:
    /**
     * @brief Holds the current visual mode.
     */
    Visual currentVisual = Visual::Phone1;

    /**
     * @brief Size for the Phone (1) and Phone (2) - pulled from the Glyph Composer Android app and under consideration of the size difference in the ringtone settings graphics.
     */
    static constexpr QSize glyphWidgetSizes[2] = {
        QSize(182, 382),    // Phone (1)
        QSize(182, 389)     // Phone (2)
    };

    /**
     * @brief Holds the sizeHint of the widget - pulled from the Glyph Composer Android app.
     */
    QSize glyphWidgetSizeHint = glyphWidgetSizes[(int)currentVisual];

    /**
     * @brief Holds the size and the position of the painting area - is calculated in the resizeEvent and used in the paintEvent.
     */
    QRect paintRectangle;

    /**
     * @brief Holds the ratio between the height of the widget size and the height of the drawingRectangle height (always bigger than 1) - is calculated in the resizeEvent.
     */
    qreal currentSizeRatio = 1.0;

    /**
     * @brief Holds the Phone (1) Glyphs.
     */
    QList<Glyph*> glyphsPhone1 = QList<Glyph*>(5);
    /**
     * @brief Use this enum to better access glyphsPhone1.
     */
    enum class GlyphsPhone1Enum
    {
        Camera = 0,
        Diagonal = 1,
        Battery = 2,
        USBLine = 3,
        USBDot = 4
    };
    /**
     * @brief Holds the Phone (2) Glyphs.
     */
    QList<Glyph*> glyphsPhone2 = QList<Glyph*>(33);
    /**
     * @brief Use this enum to better access glyphsPhone2.
     */
    enum class GlyphsPhone2Enum
    {
        CameraTop = 0,
        CameraBottom = 1,
        Diagonal = 2,
        BatteryTopRight_Zone0 = 3,
        BatteryTopRight_Zone1 = 4,
        BatteryTopRight_Zone2 = 5,
        BatteryTopRight_Zone3 = 6,
        BatteryTopRight_Zone4 = 7,
        BatteryTopRight_Zone5 = 8,
        BatteryTopRight_Zone6 = 9,
        BatteryTopRight_Zone7 = 10,
        BatteryTopRight_Zone8 = 11,
        BatteryTopRight_Zone9 = 12,
        BatteryTopRight_Zone10 = 13,
        BatteryTopRight_Zone11 = 14,
        BatteryTopRight_Zone12 = 15,
        BatteryTopRight_Zone13 = 16,
        BatteryTopRight_Zone14 = 17,
        BatteryTopRight_Zone15 = 18,
        BatteryTopLeft = 19,
        BatteryTopVertical = 20,
        BatteryBottomLeft = 21,
        BatteryBottomRight = 22,
        BatteryBottomVertical = 23,
        USBDot = 24,
        USBLine_Zone0 = 25,
        USBLine_Zone1 = 26,
        USBLine_Zone2 = 27,
        USBLine_Zone3 = 28,
        USBLine_Zone4 = 29,
        USBLine_Zone5 = 30,
        USBLine_Zone6 = 31,
        USBLine_Zone7 = 32
    };

    /**
     * @brief Calculates the positions and sizes of the Phone (1) Glyphs.
     */
    void calcPhone1Glyphs();

    /**
     * @brief Calculates the positions and sizes of the Phone (2) Glyphs.
     */
    void calcPhone2Glyphs();

    // Event overrides
    void resizeEvent(QResizeEvent* event) override;
    QSize sizeHint() const override;
    void paintEvent(QPaintEvent *event) override;
};

#endif // GLYPHWIDGET_H
