cmake_minimum_required(VERSION 3.5.1)
project(lab11 C)

set(SRC src/main.c src/KnapsackProblem.c src/ErrorHandling.c)

add_executable(${PROJECT_NAME} ${SRC})

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/../modules")
include(common_lab)
