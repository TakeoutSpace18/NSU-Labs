find_package(fmt REQUIRED)
find_package(Eigen3 3.3 REQUIRED NO_MODULE)

add_executable(MultiplicationTester src/main.cpp src/MultiplicationTester.cpp)

target_include_directories(MultiplicationTester PRIVATE ../Utility)
target_include_directories(MultiplicationTester PRIVATE src/main.cpp src/MultiplicationTester.cpp)
target_compile_options(MultiplicationTester PRIVATE -Wall -Wextra -Wpedantic)

target_link_libraries(MultiplicationTester fmt::fmt Eigen3::Eigen)