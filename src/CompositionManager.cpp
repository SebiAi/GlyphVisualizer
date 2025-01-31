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

#include "CompositionManager.h"

// Logging
Q_LOGGING_CATEGORY(compositionManager, "CompositionManager")
Q_LOGGING_CATEGORY(compositionManagerVerbose, "CompositionManager.Verbose")

CompositionManager::CompositionManager(QObject *parent)
    : QObject{parent}, player{new QMediaPlayer{this}}, audioOutput{new QAudioOutput{this->player}}, tickTimer{new QTimer{this}},
    elapsedTimer{new QElapsedTimer{}}, audioTimer{new QElapsedTimer{}},waitTimer{new QDeadlineTimer{Qt::TimerType::PreciseTimer}},
    audioResumeTimeMS{0}
{
    this->audioOutput->setVolume(0.4);
    connect(this->player, &QMediaPlayer::playbackStateChanged, this, &CompositionManager::onPlaybackStateChanged);

    this->tickTimer->setTimerType(Qt::TimerType::PreciseTimer);
    this->tickTimer->setInterval(CompositionManager::tickIntervalMSINT);
    connect(this->tickTimer, &QTimer::timeout, this, &CompositionManager::onTick);

    // Forward all the signals
    connect(this->player, &QMediaPlayer::mediaStatusChanged, this, &CompositionManager::mediaStatusChanged);
    connect(this->player, &QMediaPlayer::playbackStateChanged, this, &CompositionManager::playbackStateChanged);
    connect(this->player, &QMediaPlayer::durationChanged, this, &CompositionManager::durationChanged);
    connect(this->player, &QMediaPlayer::positionChanged, this, &CompositionManager::positionChanged);
}

void CompositionManager::seek(qint64 position) {
    this->player->setPosition(position);
    onPlaybackStateChanged(QMediaPlayer::PlaybackState::PausedState);
    if (this->player->isPlaying())
        onPlaybackStateChanged(QMediaPlayer::PlaybackState::PlayingState);

    emit compositionTick(this->audioResumeTimeMS);
}

void CompositionManager::loadAudio(const QString& audioPath) {
    qCInfo(compositionManager) << "Loading audio" << audioPath;

    QFileInfo fileInfo{audioPath};
    if (!fileInfo.isFile())
        throw SourceFileException("The audio file '" + audioPath.toStdString() + "' could not be found!");

    this->player->stop();
    this->player->setAudioOutput(this->audioOutput);
    this->player->setSource(QUrl::fromLocalFile(audioPath));
}

void CompositionManager::onTick() {
    // Get the elapsed time and restart the timer
    qint64 elapsedTime{this->elapsedTimer->nsecsElapsed()};
    this->elapsedTimer->start(); // Restart the timer

    // Calculate the time we still have to wait
    qint64 waitTime{CompositionManager::tickIntervalNS - elapsedTime};

    // Wait if needed
    if (waitTime > 400000) { // 0.4ms
        this->waitTimer->setPreciseRemainingTime(0, waitTime, Qt::TimerType::PreciseTimer);
        while (!this->waitTimer->hasExpired()) {}
    }

    // Emit tick event
    emit compositionTick(this->audioResumeTimeMS + this->audioTimer->elapsed());
}

void CompositionManager::onPlaybackStateChanged(QMediaPlayer::PlaybackState state) {
    switch (state) {
    case QMediaPlayer::PlaybackState::StoppedState:
        // Stop the timers
        this->tickTimer->stop();
        this->elapsedTimer->invalidate();
        this->audioTimer->invalidate();
        this->audioResumeTimeMS = 0;
        break;
    case QMediaPlayer::PlaybackState::PlayingState:
        // Start the timers
        this->tickTimer->start();
        this->elapsedTimer->start();
        this->audioTimer->start();
        break;
    case QMediaPlayer::PlaybackState::PausedState:
        // Stop the timers and save the paused position
        this->tickTimer->stop();
        this->elapsedTimer->invalidate();
        this->audioTimer->invalidate();
        this->audioResumeTimeMS = this->player->position();
        break;
    }
}
