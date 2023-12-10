cmake_minimum_required(VERSION 3.26)

project(lab_3_soundprocessor LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

find_package(GTest REQUIRED)
find_package(fmt REQUIRED)
find_package(Boost COMPONENTS program_options REQUIRED)

add_library(soundprocessor_common
        src/SoundProcessorApp.cpp
        src/SoundProcessorApp.h
        src/utils/Logger.cpp
        src/ConfigurationFile.cpp
        src/ConfigurationFile.h
        src/utils/CommandLineOptions.cpp
        src/utils/CommandLineOptions.h)

target_include_directories(soundprocessor_common PUBLIC src ${Boost_INCLUDE_DIR})
target_compile_options(soundprocessor_common PUBLIC -Wall -Wpedantic)
set_target_properties(soundprocessor_common PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(soundprocessor_common PUBLIC GTest::gtest_main fmt Boost::program_options)

add_executable(lab_3_soundprocessor
        src/main.cpp
        ${COMMON_SRC}
)
target_link_libraries(lab_3_soundprocessor soundprocessor_common)


add_executable(lab_3_soundprocessor_tests
        test/ConfigurationFileTests.cpp
        ${COMMON_SRC}
)
target_link_libraries(lab_3_soundprocessor_tests soundprocessor_common)

enable_testing()
gtest_discover_tests(lab_3_soundprocessor_tests WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/test/)
include(GoogleTest)
