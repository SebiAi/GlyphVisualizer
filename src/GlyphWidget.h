#ifndef GLYPHWIDGET_H
#define GLYPHWIDGET_H

#include <QWidget>
#include <QPainter>
#include <QResizeEvent>
#include <QMessageBox>
#include "Glyph.h"
#include "CompositionManager.h"

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
    void setVisual(Visual v)
    {
        // Check if the Visual is compatible with the currently loaded composition.
        if (this->compositionManager->getGlyphMode() == CompositionManager::GlyphMode::Phone2 && v == Visual::Phone1)
            throw std::invalid_argument("Can't switch to the Phone (1) visualisation because the currently loaded Composition only supports Phone (2)");

        this->currentVisual = v;
        this->glyphWidgetSizeHint = glyphWidgetSizes[(int)v];
        this->resizeEvent(NULL);
        this->update();
    }

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
     * @brief Defines a lookup table to convert the 33 Phone (2) Zones to the 11 Phone (2) Glyphs. Usage: glyphs[x] = zones[zone33To11GlyphsLookupTable[x]]
     */
    static constexpr quint8 zone33To11GlyphsLookupTable[11] = {
        0,  // 0
        1,  // 1
        2,  // 2
        3,  // 3
        19, // 4
        20, // 5
        21, // 6
        22, // 7
        23, // 8
        24, // 9
        25  // 10
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
    QList<Glyph*> glyphsPhone1;
    /**
     * @brief Holds the Phone (2) Glyphs.
     */
    QList<Glyph*> glyphsPhone2;

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
signals:

};

#endif // GLYPHWIDGET_H
