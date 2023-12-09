cmake_minimum_required(VERSION 3.26)

project(lab_3_soundprocessor LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

find_package(GTest REQUIRED)
find_package(fmt REQUIRED)

add_library(common
        src/SoundProcessorApp.cpp
        src/SoundProcessorApp.h
        src/utils/CommandLineArguments.cpp
        src/utils/Logger.cpp
        src/ConfigurationFile.cpp
        src/ConfigurationFile.h)

target_include_directories(common PRIVATE src)
target_compile_options(common PRIVATE -Wall -Wpedantic)
set_target_properties(common PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(common PUBLIC GTest::gtest_main fmt)

add_executable(lab_3_soundprocessor
        src/main.cpp
        ${COMMON_SRC}
)
target_link_libraries(lab_3_soundprocessor common)


add_executable(lab_3_soundprocessor_tests
        test/ConfigurationFileTests.cpp
        ${COMMON_SRC}
)
target_link_libraries(lab_3_soundprocessor_tests common)

enable_testing()
gtest_discover_tests(lab_3_soundprocessor_tests WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/test/)
include(GoogleTest)
