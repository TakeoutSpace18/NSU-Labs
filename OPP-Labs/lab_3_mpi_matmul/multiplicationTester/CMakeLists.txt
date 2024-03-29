add_executable(multiplicationTester src/main.cpp src/MultiplicationTester.cpp)

target_include_directories(multiplicationTester PRIVATE ../utility)
target_include_directories(multiplicationTester PRIVATE src/main.cpp src/MultiplicationTester.cpp)
target_compile_options(multiplicationTester PRIVATE -Wall -Wextra -Wpedantic)

target_link_libraries(multiplicationTester)