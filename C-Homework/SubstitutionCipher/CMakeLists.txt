cmake_minimum_required(VERSION 3.5.1)
project(SubstitutionCipher C)

set(SRC src/main.c src/error_handling.c src/substitution_cipher.c src/monogram_analysis.c src/trigram_analysis.c)

add_executable(${PROJECT_NAME} ${SRC})

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/../modules")
include(common_lab)
