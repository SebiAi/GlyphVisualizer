@echo off

REM This file is part of the GlyphVisualizer project, a Glyph composition
REM player that plays Glyph compositions from Nothing phones.
REM Copyright (C) 2025  Sebastian Aigner (aka. SebiAi)
REM 
REM This program is free software: you can redistribute it and/or modify
REM it under the terms of the GNU General Public License as published by
REM the Free Software Foundation, either version 3 of the License, or
REM (at your option) any later version.
REM 
REM This program is distributed in the hope that it will be useful,
REM but WITHOUT ANY WARRANTY; without even the implied warranty of
REM MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
REM GNU General Public License for more details.
REM 
REM You should have received a copy of the GNU General Public License
REM along with this program.  If not, see <https://www.gnu.org/licenses/>.

setlocal

REM Determine script directory and project directory
set "SCRIPT_DIR=%~dp0"
set "PROJECT_DIR=%SCRIPT_DIR%.."
set "CMAKE_SOURCE_DIR=%PROJECT_DIR%"
set "BUILD_DIR=%PROJECT_DIR%\build"

cd /d "%CMAKE_SOURCE_DIR%"

REM Check if GlyphVisualizer executable exists
if not exist "%BUILD_DIR%\GlyphVisualizer.exe" (
    echo "GlyphVisualizer executable does not exist. Run clean-build.bat first."
    exit /b 1
)

setlocal enabledelayedexpansion
set "version="
for /f "tokens=3 delims= " %%A in ('findstr /r /c:"project(.* VERSION [0-9]*\.[0-9]*\.[0-9]*" CMakeLists.txt') do (
    set "version=v%%A"
)
endlocal & set "version=%version%"

REM Create portable directory
set "PORTABLE_DIR=%BUILD_DIR%\GlyphVisualizer-%version%_windows-x64-portable"
if exist "%PORTABLE_DIR%" rmdir /s /q "%PORTABLE_DIR%"
mkdir "%PORTABLE_DIR%"

REM Copy GlyphVisualizer executable
copy "%BUILD_DIR%\GlyphVisualizer.exe" "%PORTABLE_DIR%"

REM Run windeployqt
windeployqt --release "%PORTABLE_DIR%\GlyphVisualizer.exe"

REM Copy additional files
REM   - zlib1.dll - might not exist if taglib was built with ZLIB=OFF
if exist "%BUILD_DIR%\zlib1.dll" copy "%BUILD_DIR%\zlib1.dll" "%PORTABLE_DIR%"

REM zip portable directory
set "PORTABLE_ZIP=%BUILD_DIR%\GlyphVisualizer-%version%_windows-x64-portable.zip"
powershell Compress-Archive -Path "%PORTABLE_DIR%" -DestinationPath "%PORTABLE_ZIP%"

REM Calculate SHA256 hash
powershell -Command "& {$(certutil -hashfile "%PORTABLE_ZIP%" SHA256)[1] | tee '%PORTABLE_ZIP%.sha256.txt'}"
