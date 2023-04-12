cmake_minimum_required(VERSION 3.21)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

project(Nightfire-Open LANGUAGES C CXX)

include(project_definitions)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED YES)

add_subdirectory(xash3d_engine)
