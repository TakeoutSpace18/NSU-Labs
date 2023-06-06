cmake_minimum_required(VERSION 3.5.1)
project(HashMap C)

set(SRC src/hash_map_test.c src/hash_map.c src/avl_tree.c)

add_executable(${PROJECT_NAME} ${SRC})
