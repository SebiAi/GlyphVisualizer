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

#ifndef GV_NETWORKMANAGER_H
#define GV_NETWORKMANAGER_H

#include <QMutex>
#include <QMutexLocker>
#include <QNetworkAccessManager>
#include <QObject>

// Singleton implementation reference: https://stackoverflow.com/a/1008289

class NetworkManager : public QObject
{
    Q_OBJECT
public:
    // Delete copy constructor and copy operator to prevent copies
    NetworkManager(const NetworkManager&) = delete;
    void operator=(const NetworkManager&) = delete;

    static NetworkManager& getInstance();

public slots:
    QNetworkReply* get(const QNetworkRequest& request);

signals:

private:
    // QNetworkAccessManagers functions are only reentrant but not thread safe
    // => We need to protect the functions with a mutex
    QNetworkAccessManager* manager;
    QMutex mutex;

    // Do not allow outside instantiation
    explicit NetworkManager(QObject *parent = nullptr);
};

#endif // GV_NETWORKMANAGER_H
