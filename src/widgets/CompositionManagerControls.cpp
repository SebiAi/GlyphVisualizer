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

#include "CompositionManagerControls.h"

// Logging
Q_LOGGING_CATEGORY(compositionManagerControls, "CompositionManagerControls")
Q_LOGGING_CATEGORY(compositionManagerControlsVerbose, "CompositionManagerControls.Verbose")

CompositionManagerControls::CompositionManagerControls(CompositionManager* cm, QWidget *parent)
	: QWidget{parent}, cm{cm}, playPauseButton{new PlayPauseButton()}, currentTimeLabel{new TimeLabel()}, seekBar{new SeekBar()}, totalTimeLabel{new TimeLabel()}
{
	// Create horizontal layout
	QHBoxLayout* layout = new QHBoxLayout(this);
	layout->setSpacing(8);
	layout->setContentsMargins(11, 0, 22, 0);
	setLayout(layout);

	// Add UI elements
	layout->addWidget(this->playPauseButton);
	layout->addWidget(this->currentTimeLabel);
	layout->addWidget(this->seekBar);
	layout->addWidget(this->totalTimeLabel);

	// Connect signals
	connect(this->playPauseButton, &PlayPauseButton::clicked, this, &CompositionManagerControls::onPlayPauseButtonClicked);
	connect(this->seekBar, &SeekBar::seek, this, &CompositionManagerControls::onSeekBarSeek);
	connect(this->seekBar, &SeekBar::percentageSeek, this, &CompositionManagerControls::onSeekBarPercentageSeek);
	connect(this->cm, &CompositionManager::playbackStateChanged, this, &CompositionManagerControls::onCompositionManagerPlaybackStateChanged);
	connect(this->cm, &CompositionManager::durationChanged, this, &CompositionManagerControls::onCompositionManagerDurationChanged);
	connect(this->cm, &CompositionManager::positionChanged, this, &CompositionManagerControls::onCompositionManagerPositionChanged);
}

void CompositionManagerControls::resetControls() {
	qCInfo(compositionManagerControlsVerbose) << "Clearing controls...";

    this->currentTimeLabel->clearTimestamp();
    this->totalTimeLabel->clearTimestamp();
    this->seekBar->updatePosition(0);
}

void CompositionManagerControls::onPlayPauseButtonClicked(){
	qCInfo(compositionManagerControlsVerbose) << "Play/Pause button clicked";

	if (this->cm->isPlaying())
		this->cm->pause();
	else
		this->cm->play();

	emit playbackChanged(this->cm->isPlaying());
}

void CompositionManagerControls::onSeekBarSeek() {
	qCInfo(compositionManagerControlsVerbose) << "Seeking to position" << this->seekBar->sliderPosition() << "ms";

	this->cm->seek(this->seekBar->sliderPosition());
}

void CompositionManagerControls::onSeekBarPercentageSeek(qreal percent) {
	qCInfo(compositionManagerControlsVerbose).nospace() << "Seeking to percentage " << percent * 100 << "% (" << this->seekBar->maximum() * percent << " ms)";

	this->cm->seek(this->seekBar->maximum() * percent);
}

void CompositionManagerControls::onCompositionManagerPlaybackStateChanged(QMediaPlayer::PlaybackState newState) {
	qCInfo(compositionManagerControlsVerbose) << "Playback state changed to:" << newState;

	switch (newState) {
	case QMediaPlayer::PlaybackState::PausedState:
	case QMediaPlayer::PlaybackState::StoppedState:
		this->playPauseButton->setPlayingState(false);
		break;
	case QMediaPlayer::PlaybackState::PlayingState:
		this->playPauseButton->setPlayingState(true);
		break;
	}
}
void CompositionManagerControls::onCompositionManagerDurationChanged(qint64 duration) {
	qCInfo(compositionManagerControlsVerbose) << "Duration changed:" << duration << "ms";

	// Configure SeekBar
	this->seekBar->setMaximum((int)duration);

	// Configure total time label
	this->totalTimeLabel->setMS(duration);
}
void CompositionManagerControls::onCompositionManagerPositionChanged(qint64 position) {
	// Update SeekBar
	this->seekBar->updatePosition((int)position);
	// Update current time label
	this->currentTimeLabel->setMS(position);
}
