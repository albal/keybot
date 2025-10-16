# Keybot - Root CMakeLists.txt for ESP-IDF
# Minimum version required: ESP-IDF v5.0 or later

cmake_minimum_required(VERSION 3.16)

# Generate version information from git
execute_process(
    COMMAND git describe --tags --always --dirty
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_VERSION
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_QUIET
)

# If git describe fails, try to get just the short commit SHA
if(NOT GIT_VERSION)
    execute_process(
        COMMAND git rev-parse --short HEAD
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_VERSION
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
    )
endif()

# Fallback to "unknown" if git is not available
if(NOT GIT_VERSION)
    set(GIT_VERSION "unknown")
endif()

message(STATUS "Keybot version: ${GIT_VERSION}")

# Generate version header file
configure_file(
    ${CMAKE_SOURCE_DIR}/main/version.h.in
    ${CMAKE_BINARY_DIR}/generated/version.h
    @ONLY
)

# Include the ESP-IDF build system
include($ENV{IDF_PATH}/tools/cmake/project.cmake)

# Define the project
project(keybot)
