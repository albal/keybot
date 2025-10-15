# ESP32 MacroPad - Root CMakeLists.txt for ESP-IDF
# Minimum version required: ESP-IDF v5.0 or later

cmake_minimum_required(VERSION 3.16)

# Set the components to include (optional, ESP-IDF will auto-detect if not set)
set(EXTRA_COMPONENT_DIRS components)

# Include the ESP-IDF build system
include($ENV{IDF_PATH}/tools/cmake/project.cmake)

# Define the project
project(esp32_macropad)
