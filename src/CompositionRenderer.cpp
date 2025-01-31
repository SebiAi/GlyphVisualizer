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

#include "CompositionRenderer.h"

// Logging
Q_LOGGING_CATEGORY(compositionRenderer, "CompositionRenderer")
Q_LOGGING_CATEGORY(compositionRendererVerbose, "CompositionRenderer.Verbose")

CompositionRenderer::CompositionRenderer(QObject* parent, const QColor& backgroundColor, const QString& ffmpegPath)
    : QThread(parent), abortMutex{}, abortFlag{false}, ffmpegErrorFlag{false}, progress{0}, ffmpegPath{},
    backgroundColor{backgroundColor}, glyphWidget{nullptr},
    audioPath{}, config{nullptr}, outputPath{}, frameCount{0}
{
    // Set the ffmpeg path
    setFFmpeg(ffmpegPath);

    // Set up signals
    connect(this, &QThread::finished, this, &CompositionRenderer::onRenderingFinished);
}
CompositionRenderer::~CompositionRenderer() {
    // Stop the thread
    if (!stop()) {
        terminate();
        wait(10000);
    }

    delete this->glyphWidget;
    delete this->config;
}

void CompositionRenderer::setFFmpeg(const QString& ffmpegPath) {
    if (isRunning())
        throw std::logic_error("Can't set ffmpeg. The thread is already running!");

    // Resolve the ffmpeg path
    if (!ffmpegPath.isEmpty()) {
        // Not null or empty
        QFileInfo fileInfo{ffmpegPath};
        if (!fileInfo.isFile() || !fileInfo.isExecutable()) {
            throw SourceFileException("The provided FFmpeg file path is invalid or the file can not be executed.");
        }
        this->ffmpegPath = ffmpegPath;
    } else {
        QString newFFmpegPath{QStandardPaths::findExecutable(QStringLiteral("ffmpeg"))};
        if (newFFmpegPath.isEmpty())
            throw SourceFileException("FFmpeg could not be found in PATH! Please install ffmpeg and make sure that it can be found in PATH or specify the path manually.");
        this->ffmpegPath = newFFmpegPath;
    }
}
void CompositionRenderer::setBackgroundColor(const QColor& backgroundColor) {
    if (isRunning())
        throw std::logic_error("Can't set background color. The thread is already running!");

    this->backgroundColor = backgroundColor;
}

void CompositionRenderer::render(const QString& audioPath, IConfiguration* config, const QString& outputPath, const QSize& resolution, qsizetype frameCount) {
    if (isRunning())
        throw std::logic_error("Can't render. The thread is already running!");
    if (resolution.width() % 2 || resolution.height() % 2)
        throw std::logic_error("Resolution must be even!");

    // Save the parameters
    this->audioPath = audioPath;
    this->outputPath = outputPath;
    this->frameCount = frameCount;

    // Init the GlyphWidget
    delete this->glyphWidget;
    delete this->config;
    this->config = new IConfiguration{*config}; // Copy the config so we do not have a segfault on destruction
    this->glyphWidget = new GlyphWidget{this->config};
    this->glyphWidget->resize(resolution);
    this->glyphWidget->callResizeEvent(); // We need to call the resize event manually because the widget is not visible/shown

    // Reset flags
    this->abortMutex.lock();
    this->abortFlag = false;
    this->abortMutex.unlock();
    this->ffmpegErrorFlag = false;

    // Start the thread
    qCInfo(compositionRenderer) << "Starting rendering";
    qCInfo(compositionRendererVerbose) << "Rendering build" << config->build << "with resolution" << resolution << "and" << frameCount << "frames";
    start();
}

bool CompositionRenderer::stop() {
    qCInfo(compositionRenderer) << "Requesting stop";
    this->abortMutex.lock();
    this->abortFlag = true;
    this->abortMutex.unlock();

    bool success = wait(30000);
    if (!success) qCInfo(compositionRendererVerbose) << "Timeout reached - thread still running";
    else qCInfo(compositionRendererVerbose) << "Thread stopped or was already stopped";
    return success;
}
void CompositionRenderer::abort() {
    if (!isRunning()) {
        qCInfo(compositionRendererVerbose) << "Requested abort but thread is not running";
        return;
    }

    qCInfo(compositionRenderer) << "Requesting abort";
    QMutexLocker locker{&this->abortMutex};
    this->abortFlag = true;
}

// FIXME: The memory usage is huge. Either there is a leak somewhere (valgrind is silent) or a higher compression (png?) of the images is needed.
void CompositionRenderer::run() {
    qCInfo(compositionRenderer) << "Started rendering";
    // Create ffmpeg process
    QProcess ffmpegProcess{};
    ffmpegProcess.setReadChannel(QProcess::ProcessChannel::StandardError);
    /*
     * We use bgra in the FFmpeg argument because of three reasons:
     * 1. QPainter is optimized to render to the QImage::Format::Format_RGB32 and we don't want a format conversion
     * 2. Somehow the raw data we get from the QImage in RGB32 format is saved as brga (0xBBGGRRff) - complete opposite as described in
     * the QImage::Format table.
     * 3. Formats like QImage::Format::Format_RGB888 are saved weirdly in memory resulting in a scrolling like video in both axies
     * or a pipe break when using the rgb24 format as FFmpeg argument. But only for certain resolutions like QSize{898,1920}.
     * QSize{1080,1920} works fine and returns the expected 1080*1920*3=6,220,800 bytes in image size where as the first mentioned
     * resolution returns 5176320 bytes instead of the 898*1920*3=5,172,480 bytes - 2 bytes more per scanline. (Something something alignment?)
     */
    ffmpegProcess.start(
        this->ffmpegPath,
        {
            QStringLiteral("-y"),
            QStringLiteral("-hide_banner"),

            QStringLiteral("-f"), QStringLiteral("rawvideo"),
            QStringLiteral("-pix_fmt"), QStringLiteral("bgra"),
            QStringLiteral("-s"), QStringLiteral("%1x%2").arg(this->glyphWidget->width()).arg(this->glyphWidget->height()),
            QStringLiteral("-r"), QStringLiteral("60"),
            QStringLiteral("-i"), QStringLiteral("-"),
            QStringLiteral("-i"), this->audioPath,

            QStringLiteral("-c:v"), QStringLiteral("libx264"),
            QStringLiteral("-c:a"), QStringLiteral("aac"),
            QStringLiteral("-pix_fmt"), QStringLiteral("yuv420p"),
            this->outputPath
        }
    );
    if (!ffmpegProcess.waitForStarted()) {
        this->ffmpegErrorFlag = true;
        emit ffmpegErrorOccurred(QStringLiteral("FFmpeg took to long to start."));
        ffmpegProcess.kill();
        ffmpegProcess.waitForFinished();
        return;
    }

    // Render the frames
    for (qsizetype i{0}; i < this->frameCount; ++i) {
        // Check for abort
        {
            QMutexLocker locker{&this->abortMutex};
            if (this->abortFlag) {
                qCInfo(compositionRenderer) << "Abort received";
                break;
            }
        }

        // Render frame
        qCInfo(compositionRendererVerbose).nospace() << "Rendering frame " << i+1 << "/" << this->frameCount;
        QImage image{this->glyphWidget->renderRGB32Image(i, this->backgroundColor)};
        // image.save(QStringLiteral("/home/sebiai/Downloads/GlyphVisualizer/images/frame_%1.bmp").arg(i));

        // Send frame to ffmpeg
        if (ffmpegProcess.state() != QProcess::ProcessState::Running) {
            qCWarning(compositionRenderer) << "FFmpeg terminated prematurely";
            this->ffmpegErrorFlag = true;
            emit ffmpegErrorOccurred(QStringLiteral("FFmpeg terminated prematurely:\n%1").arg(ffmpegProcess.readAllStandardError()));
            ffmpegProcess.kill();
            ffmpegProcess.waitForFinished();
            return;
        }
        QByteArray bytes{(const char*)image.constBits(), image.sizeInBytes()};
        ffmpegProcess.write(bytes);
        if (!ffmpegProcess.isWritable() || !ffmpegProcess.waitForBytesWritten()) {
            qCWarning(compositionRenderer) << "Could not write to FFmpeg";
            this->ffmpegErrorFlag = true;
            emit ffmpegErrorOccurred(QStringLiteral("Could not write to FFmpeg:\n%1").arg(ffmpegProcess.readAllStandardError()));
            ffmpegProcess.kill();
            ffmpegProcess.waitForFinished();
            return;
        }

        // Update the progress
        qint8 progress{(qint8)qRound((i + 1.) / this->frameCount * 100.)};
        if (progress != this->progress) {
            this->progress = progress;
            emit progressChanged(progress);
        }
    }

    // Close the ffmpeg process
    qCInfo(compositionRenderer) << "Shutting down FFmpeg";
    ffmpegProcess.closeWriteChannel();
    if (!ffmpegProcess.waitForFinished()) {
        qCWarning(compositionRenderer) << "FFmpeg took to long to finish";
        this->ffmpegErrorFlag = true;
        emit ffmpegErrorOccurred(QStringLiteral("FFmpeg took to long to finish."));
        ffmpegProcess.kill();
        ffmpegProcess.waitForFinished();
        return;
    }
    if (ffmpegProcess.exitStatus() != QProcess::ExitStatus::NormalExit || ffmpegProcess.exitCode() != 0) {
        QMutexLocker locker{&this->abortMutex};
        if (!this->abortFlag) {
            qCWarning(compositionRenderer) << "FFmpeg terminated abnormally";
            this->ffmpegErrorFlag = true;
            emit ffmpegErrorOccurred(QStringLiteral("FFmpeg terminated abnormally:\n%1").arg(ffmpegProcess.readAllStandardError()));
        }
    }
}

void CompositionRenderer::onRenderingFinished() {
    qCInfo(compositionRenderer) << "Thread finished";
    QMutexLocker locker{&this->abortMutex};

    if (this->abortFlag) {
        // Abort the render
        qCInfo(compositionRendererVerbose) << "Render aborted";
        emit renderingAborted();
    } else if (!this->ffmpegErrorFlag) {
        // Finished successfully
        // FFmpeg error gets emitted inside run function
        qCInfo(compositionRendererVerbose) << "Rendering finished successfully";
        emit progressChanged(100);
        emit renderingFinished();
    }
    // Reset flags
    this->abortFlag = false;
    this->ffmpegErrorFlag = false;
}
