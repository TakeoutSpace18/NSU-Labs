cmake_minimum_required(VERSION 3.5.1)
project(lab5 C)

set(SRC src/main.c src/BitStream.h src/BitStream.c src/Queue.h src/Queue.c src/Compress.h src/Compress.c src/Decompress.h src/Decompress.c src/HuffmanTree.h src/HuffmanTree.c)

add_executable(${PROJECT_NAME} ${SRC})

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/../modules")
include(common_lab)
