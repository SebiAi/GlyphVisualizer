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

REM Remove old build directory
if exist "%BUILD_DIR%" rd /s /q "%BUILD_DIR%"

REM Configure and build with Ninja
cmake -G Ninja -DCMAKE_BUILD_TYPE=Release -B "%BUILD_DIR%" "%CMAKE_SOURCE_DIR%"
cmake --build "%BUILD_DIR%" --config Release -j %NUMBER_OF_PROCESSORS%