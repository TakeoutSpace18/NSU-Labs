cmake_minimum_required(VERSION 3.26)

project(lab_3_soundprocessor LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

find_package(GTest REQUIRED)
find_package(fmt REQUIRED)
find_package(Boost COMPONENTS program_options REQUIRED)

add_library(soundprocessor_common
        src/app/SoundProcessorApp.cpp
        src/app/SoundProcessorApp.h
        src/utils/Logger.cpp
        src/app/ConfigurationFile.cpp
        src/app/ConfigurationFile.h
        src/app/CommandLineOptions.cpp
        src/app/CommandLineOptions.h
        src/audio/AudioStream.h
        src/convert/Converter.h
        src/audio/AudioFile.h
        src/audio/Pcm_s16le_AudioFile.cpp
        src/audio/Pcm_s16le_AudioFile.h
        src/audio/AudioFile.cpp
        src/audio/AudioStream.cpp
        src/app/InputFilesLoader.cpp
        src/app/InputFilesLoader.h
)

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