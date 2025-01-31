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

#ifndef GV_COMPOSITIONMANAGERCONTROLS_H
#define GV_COMPOSITIONMANAGERCONTROLS_H

#include <QHBoxLayout>
#include <QWidget>

#include "../CompositionManager.h"
#include "PlayPauseButton.h"
#include "SeekBar.h"
#include "TimeLabel.h"

// Logging
#include <QLoggingCategory>
Q_DECLARE_LOGGING_CATEGORY(compositionManagerControls)
Q_DECLARE_LOGGING_CATEGORY(compositionManagerControlsVerbose)

class CompositionManagerControls : public QWidget
{
    Q_OBJECT
public:
    explicit CompositionManagerControls(CompositionManager* cm, QWidget *parent = nullptr);

signals:
    void playbackChanged(bool playing);

public slots:
    void resetControls();

private:
    CompositionManager* cm;

    PlayPauseButton* playPauseButton;
    TimeLabel* currentTimeLabel;
    SeekBar* seekBar;
    TimeLabel* totalTimeLabel;

private slots:
    void onPlayPauseButtonClicked();
    void onSeekBarSeek();
    void onSeekBarPercentageSeek(qreal percent);
    void onCompositionManagerPlaybackStateChanged(QMediaPlayer::PlaybackState newState);
    void onCompositionManagerDurationChanged(qint64 duration);
    void onCompositionManagerPositionChanged(qint64 position);
};

#endif // GV_COMPOSITIONMANAGERCONTROLS_H
