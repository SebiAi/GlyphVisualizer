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

#include "PlayPauseButton.h"

PlayPauseButton::PlayPauseButton(QWidget *parent)
	: QToolButton{parent}
{
	setIcon(PlayPauseButton::getIconPlay());
	setIconSize(QSize{40, 40});
	setAutoRaise(true);

	this->spaceShortcut = new QShortcut{Qt::Key::Key_Space, this};
	this->kShortcut = new QShortcut{Qt::Key::Key_K, this};
	connect(this->spaceShortcut, &QShortcut::activated, this, &QToolButton::click);
	connect(this->kShortcut, &QShortcut::activated, this, &QToolButton::click);
}

void PlayPauseButton::setPlayingState(bool isPlay) {
	setIcon(isPlay ? PlayPauseButton::getIconPause() : PlayPauseButton::getIconPlay());
}
