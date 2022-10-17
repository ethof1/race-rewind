cmake_minimum_required(VERSION 3.8)

project("race-rewind")

# Find the correct Qt version
# TODO: Remove absolute path
list (APPEND CMAKE_PREFIX_PATH "C:/Qt/5.15.2/msvc2019_64")

# Set directory for custom find scripts
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

# Include Qt deployment helper function
include(windeployqt)

# Add source directory
add_subdirectory("src")
