cmake_minimum_required(VERSION 3.22.1)

project(GraviSAT)

add_library(
        gravisat
        SHARED
        solver.cpp
        gravisat_core.cpp
)

find_library(
        log-lib
        log
)

target_link_libraries(
        gravisat
        ${log-lib}
)
