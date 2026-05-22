cmake_minimum_required(VERSION 3.22.1)

project("gravisat_gpu_npu_shield")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -Wall -Wextra -std=c11 -ftree-vectorize")

add_library(
    gravisat_core
    SHARED
    gravisat_core.c
)

find_library(
    log-lib
    log
)

target_link_libraries(
    gravisat_core
    ${log-lib}
)
