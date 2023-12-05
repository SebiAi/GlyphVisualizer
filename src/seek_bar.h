#ifndef SEEK_BAR_H
#define SEEK_BAR_H

#include <QWidget>
#include <QSlider>
#include <QProxyStyle>

class SeekBar : public QSlider
{
    Q_OBJECT
public:
    explicit SeekBar(QWidget *parent = nullptr);

    /**
     * @brief Contains the minimum value of the seek bar so it does not turn into a rectangle. See MAGIC_NUMBER.
     */
    inline uint getMinValue() const { return this->minValue; };

    /**
     * @brief Returns the value considering minValue.
     * @return The actual value (value - minValue);
     */
    inline uint getActualValue() const { return this->value() - this->minValue; };

    /**
     * @brief Sets the value considering minValue.
     * @param value The value to set.
     */
    inline void setActualValue(int value) { this->setValue(value + this->minValue); };

    /**
     * @brief Sets the maximum considering minValue.
     * @param maximum The maximum to set.
     */
    inline void setActualMaximum(int maximum) { this->setMaximum(maximum + this->minValue); };

signals:
    /**
     * @brief Signal for when minValue changes.
     * @param minValueNew The new minValue.
     * @param minValueOld The old minValue.
     */
    void minValueChanged(uint minValueNew, uint minValueOld);

private:
    /**
     * @brief The rounded seek bar goes back to a rectangle if seekBar->width() / (qreal)(mediaDuration + 1) * seekBar->value()
     * @brief is smaller than this value. This magic number is dependant on the groove height and the groove border-radius.
     * @brief We can use this fact to calculate the minimum value of the seekBar where it does not go back to a rectangle.
     * @brief This minimum value will need to be recalculated everytime the seekBar is horizontally resized.
     */
    static constexpr int MAGIC_NUMBER = 12;

    /**
     * @brief Contains the minimum value of the seek bar so it does not turn into a rectangle. See MAGIC_NUMBER.
     */
    uint minValue = 0;

    /**
     * @brief This style is needed so the slider value jumps to our mouse cursor if we click on the slider bar.
     * @example seekBar.setStyle(new SeekBarStyle(seekBar->style()));
     */
    class SeekBarStyle : public QProxyStyle
    {
    public:
        using QProxyStyle::QProxyStyle;

        int styleHint(QStyle::StyleHint hint, const QStyleOption* option = 0, const QWidget* widget = 0, QStyleHintReturn* returnData = 0) const
        {
            if (hint == QStyle::SH_Slider_AbsoluteSetButtons)
                return (Qt::LeftButton | Qt::MiddleButton | Qt::RightButton);
            return QProxyStyle::styleHint(hint, option, widget, returnData);
        }
    };

    void resizeEvent(QResizeEvent *event) override;
};

#endif // SEEK_BAR_H
