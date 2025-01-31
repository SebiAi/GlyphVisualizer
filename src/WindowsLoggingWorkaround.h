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

#ifndef GV_WINDOWSLOGGINGWORKAROUND_H
#define GV_WINDOWSLOGGINGWORKAROUND_H

// Logging
#include <QLoggingCategory>
Q_DECLARE_LOGGING_CATEGORY(windowsLoggingWorkaround)
Q_DECLARE_LOGGING_CATEGORY(windowsLoggingWorkaroundVerbose)
Q_LOGGING_CATEGORY(windowsLoggingWorkaround, "WindowsLoggingWorkaround")
Q_LOGGING_CATEGORY(windowsLoggingWorkaroundVerbose, "WindowsLoggingWorkaround.Verbose")

#ifdef Q_OS_WIN
#include <Windows.h>

class WindowsLoggingWorkaround {
public:
    explicit WindowsLoggingWorkaround() {
        // Windows workarround to output logs to the console if started from one
        if (AttachConsole(ATTACH_PARENT_PROCESS)) {
            freopen("CONOUT$", "w", stdout);
            freopen("CONOUT$", "w", stderr);
        }
        qCInfo(windowsLoggingWorkaround) << "Windows detected - attached to console";
    }

    ~WindowsLoggingWorkaround() {
        qCInfo(windowsLoggingWorkaround) << "Windows detected - detaching from console";
        FreeConsole();
    }
};

#else

class WindowsLoggingWorkaround {
public:
    explicit WindowsLoggingWorkaround() {
        qCInfo(windowsLoggingWorkaround) << "Not Windows - nothing to do";
    }

    ~WindowsLoggingWorkaround() {
        qCInfo(windowsLoggingWorkaround) << "Not Windows - nothing to free";
    }
};

#endif

#endif // GV_WINDOWSLOGGINGWORKAROUND_H
