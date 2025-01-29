#!/usr/bin/env bash

# This file is part of the GlyphVisualizer project, a Glyph composition
# player that plays Glyph compositions from Nothing phones.
# Copyright (C) 2025  Sebastian Aigner (aka. SebiAi)

# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

# Exit on error
set -euo pipefail

# Variables
SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
PROJECT_DIR=${SCRIPT_DIR}/..
CMAKE_SOURCE_DIR=${PROJECT_DIR}
BUILD_DIR=${PROJECT_DIR}/build

cd "${CMAKE_SOURCE_DIR}"
rm -rf "${BUILD_DIR}"
cmake -G Ninja -DCMAKE_BUILD_TYPE=Release -B "${BUILD_DIR}" "${CMAKE_SOURCE_DIR}" && cmake --build "${BUILD_DIR}" -j$(nproc)