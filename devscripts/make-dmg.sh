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

# Check if GlyphVisualizer executable exist
if [ ! -f "${BUILD_DIR}/GlyphVisualizer.app/Contents/MacOS/GlyphVisualizer" ]; then
    echo "GlyphVisualizer executable does not exist. Run clean-build-with-docker.sh / clean-build.sh first."
    exit 1
fi

# Parse current version from CMakeLists.txt
VERSION="v$(grep -oE 'project\(GlyphVisualizer VERSION [0-9]+\.[0-9]+\.[0-9]+' CMakeLists.txt | grep -oE '[0-9]+\.[0-9]+\.[0-9]+')"

# Create basic dmg structure
mkdir -p "${BUILD_DIR}/GlyphVisualizer.app/Contents/Frameworks/"

# Copy additional files
## - libz is present in macOS

# Package with macdeployqt
cd "${BUILD_DIR}" # We need to change directory so the dmg volume name is correct
macdeployqt "GlyphVisualizer.app" -dmg

# Rename the dmg file
GV_DMG_NAME="GlyphVisualizer-${VERSION}_macOS-13-x64.dmg"
mv "GlyphVisualizer.dmg" "${GV_DMG_NAME}"

# Create checksum file
echo ""
echo ""
echo "Creating checksum file..."
shasum -a 256 -b "${GV_DMG_NAME}" | tee "${GV_DMG_NAME}.sha256"