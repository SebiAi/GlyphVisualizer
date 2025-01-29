# Oldest LTS release of Ubuntu
FROM ubuntu:22.04

ENV VERSION_QT=6.8.1

RUN apt-get update && apt-get install -y \
    locales \
    wget \
    file \
# python3.13 for aqtinstall
    python3 \
    python3-pip \
    python3-venv \
# Build dependencies
    build-essential \
    cmake \
    git \
    ninja-build \
## Qt build dependencies (https://doc.qt.io/qt-6/linux-requirements.html)
    libfontconfig1-dev \
    libfreetype-dev \
    libx11-dev \
    libx11-xcb-dev \
    libxcb-cursor-dev \
    libxcb-glx0-dev \
    libxcb-icccm4-dev \
    libxcb-image0-dev \
    libxcb-keysyms1-dev \
    libxcb-randr0-dev \
    libxcb-render-util0-dev \
    libxcb-shape0-dev \
    libxcb-shm0-dev \
    libxcb-sync-dev \
    libxcb-util-dev \
    libxcb-xfixes0-dev \
    libxcb-xinerama0-dev \
    libxcb-xkb-dev \
    libxcb1-dev \
    libxext-dev \
    libxfixes-dev \
    libxi-dev \
    libxkbcommon-dev \
    libxkbcommon-x11-dev \
    libxrender-dev \
## TagLib build dependencies
    libutfcpp-dev \
# Qt dependencies - does not work without them
## qmake error with aqtinstall
    libglib2.0-dev \
## Qt does not configure without (OpenGL)
    libgl1-mesa-dev \
## Optional WrapVulkanHeaders
    libvulkan-dev \
## Missing pulse dependency while building the project
    libpulse-dev \
## AppImage packaging of multimedia module fails
    libxrandr2 \
# linuxdeploy dependencies (appstreamcli)
    appstream

# Clean
RUN apt-get clean && rm -rf /var/lib/apt/lists/*

# Set locale to en_US.UTF-8 - needed for Qt to generate the translation files properly in utf-8
ENV LANG=en_US.UTF-8
ENV LANGUAGE=en_US:en
RUN locale-gen en_US.UTF-8

# Install TagLib v2.0.2 (static) (https://github.com/taglib/taglib/blob/master/INSTALL.md)
RUN git clone -b v2.0.2 --depth=1 https://github.com/taglib/taglib.git taglib && \
    cmake -B taglib/build -DCMAKE_INSTALL_PREFIX=/usr/local -DBUILD_SHARED_LIBS=OFF -DVISIBILITY_HIDDEN=ON -DCMAKE_BUILD_TYPE=Release taglib && \
    cmake --build taglib/build -j$(nproc) && \
    cmake --install taglib/build --strip && \
    rm -rf taglib

# Install aqtinstall and dependencies + install Qt (https://ddalcino.github.io/aqt-list-server/)
RUN python3 -m venv venv && \
    . venv/bin/activate && \
    pip install aqtinstall requests semantic-version patch py7zr texttable bs4 defusedxml && \
    python3 -m aqt install-qt --outputdir /opt/Qt linux desktop ${VERSION_QT} linux_gcc_64 -m qtmultimedia && \
    rm aqtinstall.log && \
    deactivate && \
    rm -rf venv

# Set evnironment variables for Qt
ENV QT_ROOT_DIR=/opt/Qt/${VERSION_QT}/gcc_64
ENV QT_PLUGIN_PATH=${QT_ROOT_DIR}/plugins
ENV QML2_IMPORT_PATH=${QT_ROOT_DIR}/qml
ENV LD_LIBRARY_PATH=${QT_ROOT_DIR}/lib:$LD_LIBRARY_PATH
ENV PATH=${QT_ROOT_DIR}/bin:$PATH

# Set environment variables for AppImages - libfuse does not work per default in docker
ENV APPIMAGE_EXTRACT_AND_RUN=1

# No linuxdeploy for appimage creation here
# because we always want to use the latest version.
# See appimage-with-docker.sh