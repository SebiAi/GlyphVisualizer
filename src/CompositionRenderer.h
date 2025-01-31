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
#include <QCoreApplication>
#include <QFileInfo>
#include <QImage>
#include <QMutex>
#include <QMutexLocker>
#include <QObject>
#include <QProcess>
#include <QSize>
#include <QString>
#include <QStringLiteral>
#include <QThread>

#include <taglib/fileref.h>
#include <taglib/audioproperties.h>

#include "CompositionManager.h"
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
    explicit CompositionRenderer(QObject* parent = nullptr);
    ~CompositionRenderer();

    void render(const QString& audioPath, IConfiguration* config, const QString& outputPath, const QSize& resolution, const QColor& backgroundColor = Qt::GlobalColor::black, const QString& ffmpegPath = QString{});

public slots:
    void abort();
    bool stop();
    void kill();

signals:
    void progressChanged(qint8 progress);
    void ffmpegErrorOccurred(QString error);
    void renderingAborted();
    void renderingFinished();
    void unexpectedErrorOccurred(QString error);

protected:
    virtual void run() override;

private:
    // Global vars
    QMutex abortMutex;
    bool abortFlag;
    bool ffmpegErrorFlag;
    bool unexpectedErrorFlag;
    qint8 progress;
    QString ffmpegPath;
    QColor backgroundColor;
    GlyphWidget* glyphWidget;

    // Per render vars
    QString audioPath;
    IConfiguration* config;
    QString outputPath;
    qsizetype frameCount;

    void setFFmpeg(const QString& ffmpegPath);
private slots:
    void onRenderingFinished();
};

#endif // GV_COMPOSITIONRENDERER_H
