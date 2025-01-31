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

#include "TimeLabel.h"

const QString TimeLabel::invalidTimestamp{"--:--"};

TimeLabel::TimeLabel(QWidget *parent)
    : QLabel{parent}
{
    setText(TimeLabel::invalidTimestamp);
}

void TimeLabel::setMS(qint64 ms) {
    setText(QString::asprintf("%02lld:%02lld", ms / 60000, (ms / 1000) % 60));
}

void TimeLabel::clearTimestamp() {
    setText(TimeLabel::invalidTimestamp);
}
