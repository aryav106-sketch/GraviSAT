cmake_minimum_required(VERSION 3.22.1)

project("gravisat")

# =========================
# GraviSAT Native Library
# =========================

add_library(

        gravisat

        SHARED

        gravisat_core.cpp
        solver.cpp
)

# =========================
# Android Log Library
# =========================

find_library(

        log-lib

        log
)

# =========================
# Link Libraries
# =========================

target_link_libraries(

        gravisat

        ${log-lib}
)
