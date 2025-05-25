cmake_minimum_required(VERSION 3.16)
project(ConstructionFirmFrontend VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt6 COMPONENTS Core Widgets Sql REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(SOURCES
    main.cpp
    mainwindow.cpp
    permissioncontroller.cpp
    loginwindow.cpp
    views/baseview.cpp
    views/departmentview.cpp
    views/siteview.cpp
    views/customerview.cpp
    views/objectview.cpp
    views/employeeview.cpp
    views/brigadesview.cpp
    views/workview.cpp
    views/materialsview.cpp
    views/machinesview.cpp
)

set(HEADERS
    mainwindow.h
    permissioncontroller.h
    loginwindow.h
    views/baseview.h
    views/departmentview.h
    views/siteview.h
    views/customerview.h
    views/objectview.h
    views/employeeview.h
    views/brigadesview.h
    views/workview.h
    views/materialsview.h
    views/machinesview.h
)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Core Qt6::Widgets Qt6::Sql)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

install(TARGETS ${PROJECT_NAME} DESTINATION bin)
