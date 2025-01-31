/*
This file is part of the GlyphVisualizer project, a Glyph composition
player that plays Glyph compositions from Nothing phones.
Copyright (C) 2025  Sebastian Aigner (aka. SebiAi)

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef GV_SEEKBAR_H
#define GV_SEEKBAR_H

#include <QColor>
#include <QPainter>
#include <QProxyStyle>
#include <QRect>
#include <QShortcut>
#include <QSlider>
#include <QStringLiteral>
#include <QStyle>
#include <QStyleHintReturn>
#include <QStyleOption>
#include <QWidget>

class SeekBar : public QSlider
{
    Q_OBJECT
public:
    explicit SeekBar(QWidget* parent = nullptr);
    ~SeekBar();

signals:
    void seek(int position);
    void percentageSeek(qreal percent);

public slots:
    void updatePosition(int position);

protected:
    virtual void resizeEvent(QResizeEvent* event) override;
    virtual void paintEvent(QPaintEvent* event) override;

private:
    class SeekBarStyle : public QProxyStyle {
        int styleHint(QStyle::StyleHint hint, const QStyleOption* option = nullptr, const QWidget* widget = nullptr, QStyleHintReturn* returnData = nullptr) const override {
            if (hint == QStyle::StyleHint::SH_Slider_AbsoluteSetButtons)
                return Qt::MouseButton::LeftButton;
            return QProxyStyle::styleHint(hint, option, widget, returnData);
        }
    };

    static constexpr int borderRadius{5};

    SeekBarStyle* seekBarStyle;
    QRect border;
    QRect groove;
    QRect subPage;

    QShortcut* zeroPShortcut;
    QShortcut* onePShortcut;
    QShortcut* twoPShortcut;
    QShortcut* threePShortcut;
    QShortcut* fourPShortcut;
    QShortcut* fivePShortcut;
    QShortcut* sixPShortcut;
    QShortcut* sevenPShortcut;
    QShortcut* eightPShortcut;
    QShortcut* ninePShortcut;

private slots:
    void calculateSubPage();
    void onSliderReleased();
    void onActionTriggered(int action);
};

#endif // GV_SEEKBAR_H
