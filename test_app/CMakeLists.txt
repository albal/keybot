# Test Application for ESP32 MacroPad
# This is a separate ESP-IDF application for running unit tests

cmake_minimum_required(VERSION 3.16)

# Include the ESP-IDF build system
include($ENV{IDF_PATH}/tools/cmake/project.cmake)

# Define the test project
project(keybot-test)
