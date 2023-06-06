cmake_minimum_required(VERSION 3.5.1)
project(pool-allocator C)

set(SRC src/main.c src/pool_allocator.c)

add_executable(${PROJECT_NAME} ${SRC})
