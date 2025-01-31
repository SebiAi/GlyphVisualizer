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

#include "NetworkManager.h"

NetworkManager& NetworkManager::getInstance() {
    static NetworkManager instance; // This is thread safe (C++11)
    return instance;
}

QNetworkReply* NetworkManager::get(const QNetworkRequest& request) {
    QMutexLocker locker(&this->mutex);
    return manager->get(request);
}

NetworkManager::NetworkManager(QObject *parent)
    : QObject{parent}, manager{new QNetworkAccessManager{this}}
{}

