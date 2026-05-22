cmake_minimum_required(VERSION 3.22.1)

project(gravisat)

add_library(
        gravisat_core
        SHARED
        gravisat_core.cpp
        dimacs_parser.cpp
        solver.cpp)

find_library(
        log-lib
        log)

target_link_libraries(
        gravisat_core
        ${log-lib})
