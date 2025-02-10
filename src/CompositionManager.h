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

#ifndef GV_COMPOSITIONMANAGER_H
#define GV_COMPOSITIONMANAGER_H

#include <QAudioOutput>
#include <QDeadlineTimer>
#include <QElapsedTimer>
#include <QFileInfo>
#include <QMediaPlayer>
#include <QObject>
#include <QString>
#include <QTimer>
#include <QUrl>

#include "Utils.h"

// Logging
#include <QLoggingCategory>
Q_DECLARE_LOGGING_CATEGORY(compositionManager)
Q_DECLARE_LOGGING_CATEGORY(compositionManagerVerbose)

class CompositionManager : public QObject
{
    Q_OBJECT
public:
    static constexpr qreal tickIntervalMS{16.666};
    static constexpr int tickIntervalMSINT{(int)tickIntervalMS};
    static constexpr qint64 tickIntervalNS{(qint64)(tickIntervalMS * 1e6)};

    explicit CompositionManager(QObject *parent = nullptr);
    ~CompositionManager();

    bool isPlaying() const { return this->player->isPlaying(); }
    QString audioPath() const { return this->player->source().toLocalFile(); };

signals:
    void compositionTick(qint64 position);
    void mediaStatusChanged(QMediaPlayer::MediaStatus status);
    void playbackStateChanged(QMediaPlayer::PlaybackState newState);
    void durationChanged(qint64 duration);
    void positionChanged(qint64 position);

public slots:
    void seek(qint64 position);
    void loadAudio(const QString& audioPath);
    void play() { this->player->play(); };
    void pause() { this->player->pause(); };
    void stop() { this->player->stop(); }

private:
    QMediaPlayer* player;
    QAudioOutput* audioOutput;

    QTimer* tickTimer;
    QElapsedTimer* elapsedTimer;
    QElapsedTimer* audioTimer;
    QDeadlineTimer* waitTimer;

    qint64 audioResumeTimeMS;

private slots:
    void onTick();
    void onPlaybackStateChanged(QMediaPlayer::PlaybackState state);
};

#endif // GV_COMPOSITIONMANAGER_H
