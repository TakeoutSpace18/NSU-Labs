cmake_minimum_required(VERSION 3.16)
project(WaveEquation LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(COMMON_FLAGS 
    "-fno-omit-frame-pointer"
    "-march=native"
    "-O2"
    "-ffast-math"
)

add_compile_options(${COMMON_FLAGS})

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

find_package(OpenMP REQUIRED)
if(OpenMP_CXX_FOUND)
    message(STATUS "OpenMP found")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

set(SOURCES
    src/main.cpp
    src/WaveEquation.cpp
    src/Application.cpp
)

set(HEADERS
    src/WaveEquation.h
    src/Application.h
    src/Utils.h
)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

target_link_libraries(${PROJECT_NAME}
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    OpenMP::OpenMP_CXX
)
