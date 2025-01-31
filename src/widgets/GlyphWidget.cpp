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

#include "GlyphWidget.h"

// Logging
Q_LOGGING_CATEGORY(glyphWidget, "GlyphWidget")
Q_LOGGING_CATEGORY(glyphWidgetVerbose, "GlyphWidget.Verbose")

const QColor GlyphWidget::phoneBackgroundColor{QStringLiteral("#2f3033")};

GlyphWidget::GlyphWidget(IConfiguration* configuration, QWidget *parent)
    : QWidget{parent}, paintRect{}, sizeRatio{1.0}, index{0}
{
    // Set size policy to constrain minimum window size + expand
    setSizePolicy(QSizePolicy(QSizePolicy::Policy::MinimumExpanding, QSizePolicy::Policy::MinimumExpanding));

    setConfiguration(configuration);
}

void GlyphWidget::setConfiguration(IConfiguration* configuration) {
    if (configuration == nullptr)
        throw std::logic_error("Tried to set a nullptr as IConfiguration");

    qCInfo(glyphWidget) << "Setting configuration to" << configuration->build;

    this->configuration = configuration;
    // Resize and update the widget
    QResizeEvent rEvent{size(), size()};
    resizeEvent(&rEvent);
    update();
}

void GlyphWidget::render(qsizetype colorIndex) {
    this->index = colorIndex;
    update();
}

void GlyphWidget::resizeEvent(QResizeEvent* event) {
    Q_UNUSED(event);

    // Scale sizeHint while keeping the aspect ratio
    this->paintRect.setSize(sizeHint().scaled(size(), Qt::AspectRatioMode::KeepAspectRatio));

    // Center the painting rectangle in the drawing area
    this->paintRect.moveCenter(rect().center());

    // Calculate the size ratio
    this->sizeRatio = (qreal)this->paintRect.height() / sizeHint().height();

    // Calculate bounds for configuration
    this->configuration->calcBounds(this->paintRect, this->sizeRatio);
}

QSize GlyphWidget::sizeHint() const { return this->configuration->sizeHint; }

void GlyphWidget::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event);

    QPainter painter{this};
    painter.setRenderHint(QPainter::RenderHint::Antialiasing);

    paintPhone(painter);

    painter.end();
}

void GlyphWidget::paintPhone(QPainter& painter) {
    // Render the background
    painter.setPen(Qt::PenStyle::NoPen);
    painter.setBrush(GlyphWidget::phoneBackgroundColor);
    painter.drawRoundedRect(this->paintRect, 22 * this->sizeRatio, 22 * this->sizeRatio);

    // Rerender all glyphs
    this->configuration->render(painter, this->index);
}
