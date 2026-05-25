cmake_minimum_required(VERSION 3.22.1)

project("gravisat")

add_library(
    gravisat
    SHARED

    gravisat_core.cpp
    propagation.cpp
    conflict.cpp
    heuristics.cpp
    restart.cpp
    jni_bridge.cpp
)

find_library(

        log-lib

        log
)

target_link_libraries(

        gravisat

        ${log-lib}
)
