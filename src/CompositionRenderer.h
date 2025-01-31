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

#ifndef GV_COMPOSITIONRENDERER_H
#define GV_COMPOSITIONRENDERER_H

#include <QColor>
#include <QFileInfo>
#include <QImage>
#include <QMutex>
#include <QMutexLocker>
#include <QObject>
#include <QProcess>
#include <QSize>
#include <QStandardPaths>
#include <QString>
#include <QStringLiteral>
#include <QThread>

#include "configurations/IConfiguration.h"
#include "Utils.h"
#include "widgets/GlyphWidget.h"

// Logging
#include <QLoggingCategory>
Q_DECLARE_LOGGING_CATEGORY(compositionRenderer)
Q_DECLARE_LOGGING_CATEGORY(compositionRendererVerbose)

class CompositionRenderer : public QThread
{
    Q_OBJECT
public:
    explicit CompositionRenderer(QObject* parent = nullptr, const QColor& backgroundColor = Qt::GlobalColor::black, const QString& ffmpegPath = QString{});
    ~CompositionRenderer();

    void setFFmpeg(const QString& ffmpegPath);
    void setBackgroundColor(const QColor& backgroundColor);

    void render(const QString& audioPath, IConfiguration* config, const QString& outputPath, const QSize& resolution, qsizetype frameCount);

public slots:
    bool stop();
    void abort();

signals:
    void progressChanged(qint8 progress);
    void ffmpegErrorOccurred(QString);
    void renderingAborted();
    void renderingFinished();

protected:
    virtual void run() override;

private:
    // Global vars
    QMutex abortMutex;
    bool abortFlag;
    bool ffmpegErrorFlag;
    qint8 progress;
    QString ffmpegPath;
    QColor backgroundColor;
    GlyphWidget* glyphWidget;

    // Per render vars
    QString audioPath;
    IConfiguration* config;
    QString outputPath;
    qsizetype frameCount;

private slots:
    void onRenderingFinished();
};

#endif // GV_COMPOSITIONRENDERER_H
