cmake_minimum_required(VERSION 3.16)

project(ICGFilter VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets SvgWidgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets SvgWidgets)
find_package(OpenMP REQUIRED)  # Find OpenMP package


set(PROJECT_SOURCES
        src/main.cpp

        src/ICGFilter.cpp
        src/ICGFilter.h
        src/ICGFilter.ui

        src/ImageViewport.cpp
        src/ImageViewport.h

        src/AboutDialog.h

        src/filter/Filter.h
        src/filter/Filter.cpp

        src/filter/FilterPanel.h
        src/filter/FilterPanel.cpp

        src/filter/BWFilter.h
        src/filter/BWFilter.cpp

        src/filter/InversionFilter.h
        src/filter/InversionFilter.cpp

        src/filter/BlurFilter.h
        src/filter/BlurFilter.cpp

        src/filter/SharpenFilter.h
        src/filter/SharpenFilter.cpp

        src/filter/EmbossFilter.h
        src/filter/EmbossFilter.cpp

        src/filter/GammaCorrectionFilter.h
        src/filter/GammaCorrectionFilter.cpp

        src/filter/EdgeDetectionFilter.h
        src/filter/EdgeDetectionFilter.cpp

        src/filter/DitheringFilter.h
        src/filter/DitheringFilter.cpp

        src/filter/RotationFilter.h
        src/filter/RotationFilter.cpp

        src/filter/ChromaticAberrationFilter.h
        src/filter/ChromaticAberrationFilter.cpp

        src/filter/WatercolorFilter.h
        src/filter/WatercolorFilter.cpp

        src/util/SliderWithInput.h
        src/util/SliderWithInput.cpp

        src/util/DoubleSliderWithInput.h
        src/util/DoubleSliderWithInput.cpp

        resources.qrc
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(ICGFilter
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET ICGFilter APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(ICGFilter SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(ICGFilter
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(ICGFilter PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::SvgWidgets
)

target_include_directories(ICGFilter PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

if(OpenMP_CXX_FOUND)
    message(STATUS "OpenMP found")
    target_link_libraries(ICGFilter PRIVATE OpenMP::OpenMP_CXX)
endif()

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.ICGFilter)
endif()
set_target_properties(ICGFilter PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS ICGFilter
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(ICGFilter)
endif()
