cmake_minimum_required(VERSION 3.26)
project(lab_2_game_of_life)

# GoogleTest requires at least C++14
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(src)

# Dear ImGui
set(IMGUI_DIR ./third-party/imgui)
include_directories(${IMGUI_DIR} ${IMGUI_DIR}/backends ..)

find_package(glfw3 3.3 REQUIRED)
find_package(Vulkan REQUIRED)

set(IMGUI_SRC
        ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
        ${IMGUI_DIR}/backends/imgui_impl_vulkan.cpp
        ${IMGUI_DIR}/imgui.cpp
        ${IMGUI_DIR}/imgui_draw.cpp
        ${IMGUI_DIR}/imgui_demo.cpp
        ${IMGUI_DIR}/imgui_tables.cpp
        ${IMGUI_DIR}/imgui_widgets.cpp)

include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/af5ddbde4d57b0331e1d0ed3659ca2e39b37580d.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

set(COMMON_SRC
        src/core/Universe.cpp
        src/core/Field.cpp
        src/Application.cpp
        src/serialize/UniverseSerializerBase.cpp
        src/frontend/UIRenderer.cpp
        src/frontend/UIRenderer.h
        src/utils/CommandLineArguments.cpp
        src/utils/CommandLineArguments.h
        src/utils/Logger.cpp
        src/utils/Logger.h
)

add_executable(lab_2_game_of_life
        ${COMMON_SRC}
        ${IMGUI_SRC}
        src/main.cpp
)

target_link_libraries(lab_2_game_of_life PRIVATE
        common
        glfw
        Vulkan::Vulkan
)

enable_testing()
add_executable(
        lab_2_game_of_life_tests
        ${COMMON_SRC}
        test/FieldTests.cpp
        test/UniverseTests.cpp
        test/CommandlineArgumentsTests.cpp
)
target_link_libraries(
        lab_2_game_of_life_tests PRIVATE
        common
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(lab_2_game_of_life_tests)

add_library(common INTERFACE
        src/frontend/vulkan_initialization.cpp)

target_compile_options(common INTERFACE
        -Wall -Wextra -Wshadow -Wnon-virtual-dtor
        -Wold-style-cast
        -Woverloaded-virtual -Wzero-as-null-pointer-constant
        -pedantic
        -D_GLIBCXX_ASSERTIONS
)

