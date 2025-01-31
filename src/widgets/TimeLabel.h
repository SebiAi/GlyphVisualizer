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

#ifndef GV_TIMELABEL_H
#define GV_TIMELABEL_H

#include <QLabel>
#include <QString>
#include <QWidget>

class TimeLabel : public QLabel
{
    Q_OBJECT
public:
    explicit TimeLabel(QWidget *parent = nullptr);

public slots:
    void setMS(qint64 ms);
    void clearTimestamp();

private:
    static const QString invalidTimestamp;
};

#endif // GV_TIMELABEL_H
