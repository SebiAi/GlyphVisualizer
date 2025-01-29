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

#ifndef GV_GLYPHWIDGET_H
#define GV_GLYPHWIDGET_H

#include <QColor>
#include <QPainter>
#include <QPaintEvent>
#include <QRect>
#include <QResizeEvent>
#include <QSize>
#include <QSizePolicy>
#include <QStringLiteral>
#include <QWidget>

#include "../configurations/IConfiguration.h"

// Logging
#include <QLoggingCategory>
Q_DECLARE_LOGGING_CATEGORY(glyphWidget)
Q_DECLARE_LOGGING_CATEGORY(glyphWidgetVerbose)

class GlyphWidget : public QWidget
{
    Q_OBJECT
public:
    explicit GlyphWidget(IConfiguration* configuration, QWidget *parent = nullptr);

    DeviceBuild getConfigurationDeviceBuild() { return this->configuration->build; }
signals:

public slots:
    void setConfiguration(IConfiguration* configuration);
    void render(qsizetype colorIndex);

protected:
    virtual void resizeEvent(QResizeEvent* event) override;
    virtual QSize sizeHint() const override;
    virtual void paintEvent(QPaintEvent* event) override;

private:
    // Holds the size and the position of the painting area - is calculated in the resizeEvent and used in the paintEvent.
    QRect paintRect;

    // Holds the ratio between the height of the widget size and the height of the drawingRectangle height
    // (always bigger than 1) - is calculated in the resizeEvent.
    qreal sizeRatio;

    // Holds the current configuration. The configuration object is owned by the ConfigurationManager.
    IConfiguration* configuration;

    static const QColor phoneBackgroundColor;

    qsizetype index;

    void paintPhone(QPainter& painter);
};

#endif // GV_GLYPHWIDGET_H
