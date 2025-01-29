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
if [ ! -f "${BUILD_DIR}/GlyphVisualizer" ]; then
    echo "GlyphVisualizer executable does not exist. Run clean-build-with-docker.sh / clean-build.sh first."
    exit 1
fi

# Parse current version from CMakeLists.txt
VERSION="v$(grep -oP 'project\(GlyphVisualizer VERSION \K[0-9]+\.[0-9]+\.[0-9]+' CMakeLists.txt)"

# Pull the latest linuxdeploy + qt plugin + appimage plugin
LINUXDEPLOY_APPIMAGE_PATH="${BUILD_DIR}/linuxdeploy-x86_64.AppImage"
LINUXDEPLOY_QT_PLUGIN_APPIMAGE_PATH="${BUILD_DIR}/linuxdeploy-plugin-qt-x86_64.AppImage"
LINUXDEPLOY_APPIMAGE_PLUGIN_APPIMAGE_PATH="${BUILD_DIR}/linuxdeploy-plugin-appimage-x86_64.AppImage"
wget -O "${LINUXDEPLOY_APPIMAGE_PATH}" "https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage"
wget -O "${LINUXDEPLOY_QT_PLUGIN_APPIMAGE_PATH}" "https://github.com/linuxdeploy/linuxdeploy-plugin-qt/releases/download/continuous/linuxdeploy-plugin-qt-x86_64.AppImage"
wget -O "${LINUXDEPLOY_APPIMAGE_PLUGIN_APPIMAGE_PATH}" "https://github.com/linuxdeploy/linuxdeploy-plugin-appimage/releases/download/continuous/linuxdeploy-plugin-appimage-x86_64.AppImage"
chmod +x "${LINUXDEPLOY_APPIMAGE_PATH}" "${LINUXDEPLOY_QT_PLUGIN_APPIMAGE_PATH}" "${LINUXDEPLOY_APPIMAGE_PLUGIN_APPIMAGE_PATH}"

# Apply AppImage fix for docker container (https://github.com/AppImage/AppImageKit/issues/828)
# This docker check might not be reliable in the future (https://github.com/moby/moby/issues/18355#issuecomment-220484748)
if [ -f /.dockerenv ]; then
    echo "!!--- Detected docker environment. Applying AppImage fix. ---!!"
    dd if=/dev/zero bs=1 count=3 seek=8 conv=notrunc of="${LINUXDEPLOY_APPIMAGE_PATH}"
    dd if=/dev/zero bs=1 count=3 seek=8 conv=notrunc of="${LINUXDEPLOY_QT_PLUGIN_APPIMAGE_PATH}"
    dd if=/dev/zero bs=1 count=3 seek=8 conv=notrunc of="${LINUXDEPLOY_APPIMAGE_PLUGIN_APPIMAGE_PATH}"
fi

# Make AppDir with qt plugin
cd "${BUILD_DIR}" # The zsync file is created in the current working directory
GV_APPIMAGE_NAME="GlyphVisualizer-${VERSION}_linux-ubuntu-x64.AppImage"
export LINUXDEPLOY_OUTPUT_VERSION="${VERSION}"
export LDAI_OUTPUT="${BUILD_DIR}/${GV_APPIMAGE_NAME}"
export LDAI_UPDATE_INFORMATION="gh-releases-zsync|SebiAi|GlyphVisualizer|latest|GlyphVisualizer*_linux-ubuntu-x64.AppImage.zsync"
APPDIR_PATH="${BUILD_DIR}/AppDir"
if [ -d "${APPDIR_PATH}" ]; then
    rm -rf "${APPDIR_PATH}"
fi
"${LINUXDEPLOY_APPIMAGE_PATH}" --executable "${BUILD_DIR}/GlyphVisualizer" --desktop-file "${CMAKE_SOURCE_DIR}/deploy/linux/AppImage/com.sebiai.GlyphVisualizer.desktop" --icon-file "${CMAKE_SOURCE_DIR}/deploy/GlyphVisualizer.png" --plugin qt --appdir "${APPDIR_PATH}"

# Add additional files to AppDir if needed
echo ""
echo ""
echo "Copying additional files to AppDir..."
mkdir -p "${APPDIR_PATH}/usr/share/metainfo"
cp "${CMAKE_SOURCE_DIR}/deploy/linux/AppImage/com.sebiai.GlyphVisualizer.appdata.xml" "${APPDIR_PATH}/usr/share/metainfo"

# Create AppImage
echo ""
echo ""
"${LINUXDEPLOY_APPIMAGE_PATH}" --executable "${BUILD_DIR}/GlyphVisualizer" --desktop-file "${CMAKE_SOURCE_DIR}/deploy/linux/AppImage/com.sebiai.GlyphVisualizer.desktop" --icon-file "${CMAKE_SOURCE_DIR}/deploy/GlyphVisualizer.png" --appdir "${BUILD_DIR}/AppDir" --output appimage

# Create checksum file
echo ""
echo ""
echo "Creating checksum file..."
cd "${BUILD_DIR}" # The checksum file is created in the current working directory and we need to use the relative path to get the right output
sha256sum -b "${GV_APPIMAGE_NAME}" | tee "${LDAI_OUTPUT}.sha256"