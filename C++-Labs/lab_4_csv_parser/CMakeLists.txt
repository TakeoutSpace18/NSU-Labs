cmake_minimum_required(VERSION 3.26)
project(lab_4_csv_parser)

# GoogleTest requires at least C++14
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/af5ddbde4d57b0331e1d0ed3659ca2e39b37580d.zip
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)
enable_testing()
include(GoogleTest)

include_directories(src)

add_executable(lab_4_csv_parser
        src/CSVParser.h
        src/PrintTuple.h
        test/PrintTupleTests.cpp
        test/CSVParserTests.cpp
)

target_compile_options(lab_4_csv_parser PRIVATE -Wall -Wpedantic)
set_target_properties(lab_4_csv_parser PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(lab_4_csv_parser GTest::gtest_main)
gtest_discover_tests(lab_4_csv_parser)

find_package(fmt)
target_link_libraries(lab_4_csv_parser fmt)
