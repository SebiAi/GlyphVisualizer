cmake_minimum_required(VERSION 3.5)

project(GlyphVisualizer VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Svg)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Multimedia)

set(PROJECT_SOURCES
        src/main.cpp
        src/mainwindow.cpp
        src/mainwindow.h
        src/mainwindow.ui
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(GlyphVisualizer
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        src/glyph_widget.h src/glyph_widget.cpp
        resources.qrc
        src/glyph.h src/glyph.cpp
        src/composition_manager.h src/composition_manager.cpp
        src/helper.h src/helper.cpp
        src/open_composition_dialog.h src/open_composition_dialog.cpp
        src/seekbar_style.h
        src/APPLICATION_INFO.h
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET GlyphVisualizer APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(GlyphVisualizer SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(GlyphVisualizer
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(GlyphVisualizer PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(GlyphVisualizer PRIVATE Qt${QT_VERSION_MAJOR}::Svg)
target_link_libraries(GlyphVisualizer PRIVATE Qt${QT_VERSION_MAJOR}::Multimedia)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
#if(${QT_VERSION} VERSION_LESS 6.1.0)
set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.sebiai.GlyphVisualizer)
#endif()
set_target_properties(GlyphVisualizer PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

if(APPLE)
    set_target_properties(GlyphVisualizer PROPERTIES MACOSX_BUNDLE TRUE)
endif()

include(GNUInstallDirs)
install(TARGETS GlyphVisualizer
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(GlyphVisualizer)
endif()
