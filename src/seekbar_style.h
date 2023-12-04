#ifndef SEEKBAR_STYLE_H
#define SEEKBAR_STYLE_H

#include <QProxyStyle>

/**
 * @brief This style is needed so the slider value jumps to our mouse cursor if we click on the slider bar.
 * @example seekbar.setStyle(new SeekbarStyle(seekbar->style()));
 */
class SeekbarStyle : public QProxyStyle
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

#endif // SEEKBAR_STYLE_H
