#ifndef GLYPHWIDGET_H
#define GLYPHWIDGET_H

#include <QWidget>
#include <QPainter>
#include <QResizeEvent>
#include "Glyph.h"

class GlyphWidget : public QWidget
{
    Q_OBJECT
public:
    explicit GlyphWidget(QWidget *parent = nullptr);
    ~GlyphWidget();

    /**
     * @brief Enum definition for the visual modes.
     */
    enum class Visual { Phone1, Phone2 };

    /**
     * @brief Tries to set the visual depending on if the currently loaded composition allows it. If it is not possible nothing happens.
     * @param v Which visual to set.
     */
    void setVisual(Visual v)
    {
        // TODO: Check if the Visual is compatible with the currently loaded composition.
        this->currentVisual = v;
        this->resizeEvent(NULL);
        this->update();
    }

    /**
     * @brief Get the current visual mode.
     * @return The current visual mode.
     */
    const Visual& getVisual()
    {
        return this->currentVisual;
    }

private:

    /**
     * @brief Holds the current visual mode.
     */
    Visual currentVisual = Visual::Phone1;

    /**
     * @brief Holds the sizeHint of the widget - pulled from the Glyph Composer Android app.
     */
    const QSize GlyphWidgetSizeHint = QSize(182, 382);

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
