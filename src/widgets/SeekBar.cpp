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

#include "SeekBar.h"

SeekBar::SeekBar(QWidget* parent)
    : QSlider{parent}, seekBarStyle{new SeekBarStyle()}, border{}, groove{}, subPage{}
{
    setStyle(this->seekBarStyle);
    setOrientation(Qt::Orientation::Horizontal);
    setRange(0, 10000);
    setSingleStep(1000);
    setPageStep(5000);
    setStyleSheet(
        QStringLiteral("QSlider { height: 23px; background: #ffffff }"
                       "QSlider::sub-page:horizontal { background: #d71921; border-radius: 5px; margin: 0px 2px }"
                       "QSlider::groove:horizontal { border: 1px solid #494b50; height: 12px; margin: -2px 0px; border-radius: 5px }"
                       "QSlider::handle:horizontal { background: transparent }"));

    // Update the widget when the value changes
    connect(this, &QSlider::valueChanged, this, &SeekBar::calculateSubPage);

    connect(this, &QSlider::sliderReleased, this, &SeekBar::onSliderReleased);
    connect(this, &QAbstractSlider::actionTriggered, this, &SeekBar::onActionTriggered);

    this->zeroPShortcut = new QShortcut{Qt::Key::Key_0, this};
    this->onePShortcut = new QShortcut{Qt::Key::Key_1, this};
    this->twoPShortcut = new QShortcut{Qt::Key::Key_2, this};
    this->threePShortcut = new QShortcut{Qt::Key::Key_3, this};
    this->fourPShortcut = new QShortcut{Qt::Key::Key_4, this};
    this->fivePShortcut = new QShortcut{Qt::Key::Key_5, this};
    this->sixPShortcut = new QShortcut{Qt::Key::Key_6, this};
    this->sevenPShortcut = new QShortcut{Qt::Key::Key_7, this};
    this->eightPShortcut = new QShortcut{Qt::Key::Key_8, this};
    this->ninePShortcut = new QShortcut{Qt::Key::Key_9, this};

    connect(this->zeroPShortcut, &QShortcut::activated, [=](){ emit percentageSeek(0.0); });
    connect(this->onePShortcut, &QShortcut::activated, [=](){ emit percentageSeek(0.1); });
    connect(this->twoPShortcut, &QShortcut::activated, [=](){ emit percentageSeek(0.2); });
    connect(this->threePShortcut, &QShortcut::activated, [=](){ emit percentageSeek(0.3); });
    connect(this->fourPShortcut, &QShortcut::activated, [=](){ emit percentageSeek(0.4); });
    connect(this->fivePShortcut, &QShortcut::activated, [=](){ emit percentageSeek(0.5); });
    connect(this->sixPShortcut, &QShortcut::activated, [=](){ emit percentageSeek(0.6); });
    connect(this->sevenPShortcut, &QShortcut::activated, [=](){ emit percentageSeek(0.7); });
    connect(this->eightPShortcut, &QShortcut::activated, [=](){ emit percentageSeek(0.8); });
    connect(this->ninePShortcut, &QShortcut::activated, [=](){ emit percentageSeek(0.9); });
}
SeekBar::~SeekBar() {
    delete this->seekBarStyle;
}

void SeekBar::updatePosition(int position) {
    if (!this->isSliderDown())
        this->setValue(position);
}

void SeekBar::resizeEvent(QResizeEvent* event) {
    // Update border rectangle
    this->border = QRect{0, 0, this->width(), 14};
    this->border.moveCenter(rect().center());

    // Update the groove rectangle
    this->groove = this->border.adjusted(1, 1, -1, -1);

    // Update the sub-page rectangle
    calculateSubPage();
}

void SeekBar::paintEvent(QPaintEvent* event) {
    QPainter painter{this};
    painter.setRenderHint(QPainter::RenderHint::Antialiasing);
    painter.setPen(Qt::PenStyle::NoPen);

    // Border of groove
    painter.setBrush(QColor{QStringLiteral("#494b50")});
    painter.drawRoundedRect(this->border, SeekBar::borderRadius, SeekBar::borderRadius, Qt::SizeMode::AbsoluteSize);

    // Background of groove
    painter.setBrush(QColor{QStringLiteral("#2f3033")});
    painter.drawRoundedRect(this->groove, SeekBar::borderRadius, SeekBar::borderRadius, Qt::SizeMode::AbsoluteSize);

    // Background of groove
    painter.setBrush(QColor{QStringLiteral("#d71921")});
    painter.drawRoundedRect(this->subPage, SeekBar::borderRadius, SeekBar::borderRadius, Qt::SizeMode::AbsoluteSize);

    painter.end();
}

void SeekBar::calculateSubPage() {
    this->subPage = this->groove.adjusted(1, 1, -1, -1);
    this->subPage.setWidth(this->subPage.width() * this->value() / (qreal)this->maximum());
}

void SeekBar::onSliderReleased() {
    emit seek(this->value());
}

void SeekBar::onActionTriggered(int action) {
    switch (action) {
    case QAbstractSlider::SliderAction::SliderSingleStepAdd:
    case QAbstractSlider::SliderAction::SliderSingleStepSub:
    case QAbstractSlider::SliderAction::SliderPageStepAdd:
    case QAbstractSlider::SliderAction::SliderPageStepSub:
        // Emit seek if a page/single step was performed
        // This is possible because sliderPosition was already adjusted
        // but value has not been changed - which is controlled externally.
        emit seek(this->sliderPosition());
        break;
    default:
        break;
    }
}
