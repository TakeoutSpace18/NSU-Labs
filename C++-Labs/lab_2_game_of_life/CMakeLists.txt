cmake_minimum_required(VERSION 3.26)
project(lab_2_game_of_life)

# GoogleTest requires at least C++14
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(src)

include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/af5ddbde4d57b0331e1d0ed3659ca2e39b37580d.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(lab_2_game_of_life
        src/core/Universe.cpp
        src/core/Universe.h
        src/core/Field.cpp
        src/core/Field.h
        src/Application.cpp
        src/Application.h
        src/main.cpp
        src/serialize/UniverseSerializerBase.cpp
        src/serialize/UniverseSerializerBase.h
        )

target_link_libraries(lab_2_game_of_life PRIVATE
        common
)

enable_testing()
add_executable(
        lab_2_game_of_life_tests
        src/core/Universe.cpp
        src/core/Universe.h
        src/core/Field.cpp
        src/core/Field.h
        src/Application.cpp
        src/Application.h
        src/serialize/UniverseSerializerBase.cpp
        src/serialize/UniverseSerializerBase.h
        test/FieldTests.cpp
        test/UniverseTests.cpp
)
target_link_libraries(
        lab_2_game_of_life_tests PRIVATE
        common
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(lab_2_game_of_life_tests)

add_library(common INTERFACE)

target_compile_options(common INTERFACE
        -Wall -Wextra -Wshadow -Wnon-virtual-dtor
        -Wold-style-cast
        -Woverloaded-virtual -Wzero-as-null-pointer-constant
        -pedantic
        -D_GLIBCXX_ASSERTIONS
)
