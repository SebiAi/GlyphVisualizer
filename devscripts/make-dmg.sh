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
APP_NAME="GlyphVisualizer.app"

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
cd "${BUILD_DIR}"
macdeployqt "${APP_NAME}"

# Rename the dmg file
MACHINE_ARCH=$(uname -m)
if [ "${MACHINE_ARCH}" = "x86_64" ]; then
    echo "Detected x86_64 architecture."
elif [ "${MACHINE_ARCH}" = "arm64" ]; then
    echo "Detected arm64 architecture."
else
    echo "Unknown architecture: ${MACHINE_ARCH}"
fi
GV_DMG_NAME="GlyphVisualizer-${VERSION}_macOS-14-${MACHINE_ARCH}.dmg"
RW_DMG="${BUILD_DIR}/GlyphVisualizer-layout.dmg"
STAGING_DIR="${BUILD_DIR}/GlyphVisualizer-dmg-staging"
VOLUME_NAME="GlyphVisualizer"
MOUNT_POINT="/Volumes/${VOLUME_NAME}"
WINX=200
WINY=120
WINW=660
WINH=400
WIN_RIGHT=$((WINX + WINW))
WIN_BOTTOM=$((WINY + WINH))
ICON_SIZE=128
TEXT_SIZE=16
APP_X=180
APP_Y=185
APPLICATIONS_X=480
APPLICATIONS_Y=185
SKIP_FINDER_LAYOUT="${SKIP_FINDER_LAYOUT:-}"

detach_dmg() {
    local detach_target="$1"
    local attempts=0
    local max_attempts=3

    until hdiutil detach "${detach_target}"; do
        attempts=$((attempts + 1))
        if [ "${attempts}" -ge "${max_attempts}" ]; then
            echo "Detaching ${detach_target} with force."
            hdiutil detach -force "${detach_target}" || {
                echo "Failed to detach ${detach_target} after ${max_attempts} attempts."
                return 1
            }
            return 0
        fi
        echo "Detach busy, retrying..."
        sleep $((2 ** attempts))
    done
}

if [ -f "${GV_DMG_NAME}" ]; then
    rm -f "${GV_DMG_NAME}"
fi

if [ -f "${RW_DMG}" ]; then
    rm -f "${RW_DMG}"
fi

rm -rf "${STAGING_DIR}"
mkdir -p "${STAGING_DIR}"
cp -R "${APP_NAME}" "${STAGING_DIR}/${APP_NAME}"
ln -s /Applications "${STAGING_DIR}/Applications"

hdiutil create \
    -volname "${VOLUME_NAME}" \
    -srcfolder "${STAGING_DIR}" \
    -ov \
    -format UDRW \
    -fs HFS+ \
    "${RW_DMG}"

ATTACH_OUTPUT=$(hdiutil attach "${RW_DMG}" -mountpoint "${MOUNT_POINT}" -noautoopen)
DEV_NAME=$(printf '%s\n' "${ATTACH_OUTPUT}" | sed -nE 's#^(/dev/disk[0-9]+).*#\1#p' | head -n 1)

if [ -z "${DEV_NAME}" ]; then
    echo "Failed to determine mounted DMG device name."
    exit 1
fi

if [ ! -e "${MOUNT_POINT}/Applications" ]; then
    ln -s /Applications "${MOUNT_POINT}/Applications"
fi

if [ "${SKIP_FINDER_LAYOUT}" != "1" ]; then
    # Configure a polished drag-and-drop installer window in Finder.
osascript <<EOF
tell application "Finder"
    tell disk "${VOLUME_NAME}"
        open
        set current view of container window to icon view
        set toolbar visible of container window to false
        set statusbar visible of container window to false
        set bounds of container window to {${WINX}, ${WINY}, ${WIN_RIGHT}, ${WIN_BOTTOM}}
        set theViewOptions to the icon view options of container window
        set arrangement of theViewOptions to not arranged
        set icon size of theViewOptions to ${ICON_SIZE}
        set text size of theViewOptions to ${TEXT_SIZE}
        set position of item "${APP_NAME}" of container window to {${APP_X}, ${APP_Y}}
        set position of item "Applications" of container window to {${APPLICATIONS_X}, ${APPLICATIONS_Y}}
        close
        open
        update without registering applications
        delay 2
    end tell
end tell
EOF
else
    echo "Skipping Finder layout customization."
fi

if [ "${MACHINE_ARCH}" = "arm64" ]; then
    bless --folder "${MOUNT_POINT}" || true
else
    bless --folder "${MOUNT_POINT}" --openfolder "${MOUNT_POINT}" || true
fi
sync
detach_dmg "${DEV_NAME}"

hdiutil convert "${RW_DMG}" -format UDZO -imagekey zlib-level=9 -o "${GV_DMG_NAME}"
rm -f "${RW_DMG}"
rm -rf "${STAGING_DIR}"

# Create checksum file
echo ""
echo ""
echo "Creating checksum file..."
shasum -a 256 -b "${GV_DMG_NAME}" | tee "${GV_DMG_NAME}.sha256"