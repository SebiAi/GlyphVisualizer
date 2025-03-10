cmake_minimum_required(VERSION 3.19)
project(GlyphVisualizer VERSION 2.2.1 LANGUAGES CXX)

set(GV_VERSION "v${PROJECT_VERSION}")
set(BUILDINFO_GIT_COMMIT_HASH "unknown")
set(BUILDINFO_GIT_COMMIT_HASH_SHORT "unknown")

# Get the git commit hash
find_package(Git QUIET)
if(GIT_FOUND)
    execute_process(
        COMMAND ${GIT_EXECUTABLE} rev-parse HEAD
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        RESULT_VARIABLE GIT_COMMIT_HASH_RESULT
        OUTPUT_VARIABLE BUILDINFO_GIT_COMMIT_HASH
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    if (NOT GIT_COMMIT_HASH_RESULT EQUAL 0)
        message(FATAL_ERROR "Failed to get the git commit hash")
    endif()
    execute_process(
        COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        RESULT_VARIABLE GIT_COMMIT_HASH_SHORT_RESULT
        OUTPUT_VARIABLE BUILDINFO_GIT_COMMIT_HASH_SHORT
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    if (NOT GIT_COMMIT_HASH_SHORT_RESULT EQUAL 0)
        message(FATAL_ERROR "Failed to get the git commit hash")
    endif()
else()
    message(WARNING "Git not found, commit hash will be unknown")
endif()

find_package(Qt6 6.6 REQUIRED COMPONENTS Core Widgets LinguistTools Svg Multimedia Network)

qt_standard_project_setup()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTORCC ON)

set(SOURCES
    src/main.cpp
    src/MainWindow.h src/MainWindow.cpp
    src/configurations/IConfiguration.h
    src/configurations/Phone1Configuration.h
    src/configurations/Phone2Configuration.h
    src/configurations/Phone2aConfiguration.h
    src/configurations/Phone3aConfiguration.h
    src/configurations/ConfigurationManager.h src/configurations/ConfigurationManager.cpp
    src/configurations/DeviceBuild.h
    src/MySvgRenderer.h src/MySvgRenderer.cpp
    src/Glyph.h src/Glyph.cpp
    resources.qrc
    src/Utils.h src/Utils.cpp
    src/CompositionManager.h src/CompositionManager.cpp
    src/widgets/GlyphWidget.h src/widgets/GlyphWidget.cpp
    src/widgets/SeekBar.h src/widgets/SeekBar.cpp
    src/widgets/PlayPauseButton.h src/widgets/PlayPauseButton.cpp
    src/widgets/TimeLabel.h src/widgets/TimeLabel.cpp
    src/widgets/CompositionManagerControls.h src/widgets/CompositionManagerControls.cpp
    src/OpenCompositionDialog.h src/OpenCompositionDialog.cpp
    src/Config.h src/Config.cpp
    src/NetworkManager.h src/NetworkManager.cpp
    src/UpdateChecker.h src/UpdateChecker.cpp
    src/WindowsLoggingWorkaround.h
    src/DonationDialog.h src/DonationDialog.cpp
    src/CompositionRenderer.h src/CompositionRenderer.cpp
    src/RenderingSettingsDialog.h src/RenderingSettingsDialog.cpp
)

if(WIN32)
    message(NOTICE "Windows detected - adding windows resource file (icon)")
    # The resource file contains infos like the icon and name/version of the application
    # Sources: https://doc.qt.io/qt-6/appicon.html, https://www-user.tu-chemnitz.de/~heha/hsn/chm/gorc.chm/ver.htm
    # The ico file can be generated with icotool from the icoutils package
    set(GV_ICON_PATH ${CMAKE_CURRENT_SOURCE_DIR}/GlyphVisualizer.ico)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/GlyphVisualizer.rc.in ${CMAKE_CURRENT_BINARY_DIR}/GlyphVisualizer.rc @ONLY)
    set(SOURCES ${SOURCES} ${CMAKE_CURRENT_BINARY_DIR}/GlyphVisualizer.rc)
elseif(APPLE)
    message(NOTICE "MacOS detected - adding icon")
    # Sources: https://doc.qt.io/qt-6/appicon.html
    # The icns file can be generated with iconutil - see https://github.com/dockur/macos
    # Here WITHOUT the full path
    set(MACOSX_BUNDLE_ICON_FILE GlyphVisualizer.icns)

    set(app_icon_macos ${CMAKE_CURRENT_SOURCE_DIR}/GlyphVisualizer.icns)
    set_source_files_properties(${app_icon_macos} PROPERTIES
           MACOSX_PACKAGE_LOCATION "Resources")
    set(SOURCES ${SOURCES} ${app_icon_macos})
endif()

qt_add_executable(GlyphVisualizer
    WIN32 MACOSX_BUNDLE
    ${SOURCES}
)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/src/BuildInfo.h.in ${CMAKE_CURRENT_BINARY_DIR}/src/BuildInfo.h @ONLY)
# Include the binary dir for the configured BuildInfo.h
target_include_directories(GlyphVisualizer
    PRIVATE
        "${CMAKE_CURRENT_BINARY_DIR}/src"
)

# qt_add_translations(
#     TARGETS GlyphVisualizer
#     TS_FILES GlyphVisualizer_en.ts
# )

# Could not get it to work with FetchContent
# See devscripts/Dockerfile on how to build and install TagLib
find_package(ZLIB)
find_package(TagLib 2.0.0 REQUIRED)

target_link_libraries(GlyphVisualizer
    PRIVATE
        Qt::Core
        Qt::Widgets
        Qt6::Svg
        Qt6::Multimedia
        Qt6::Network
        TagLib::tag
)

include(GNUInstallDirs)

install(TARGETS GlyphVisualizer
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET GlyphVisualizer
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
