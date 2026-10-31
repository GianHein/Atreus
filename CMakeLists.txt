cmake_minimum_required(VERSION 3.25)
project(Atreus LANGUAGES C CXX VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/Bin/$<CONFIG>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/Bin/$<CONFIG>")

add_subdirectory(Vendor/glad)


option(TRACY_ENABLE "" ON)
option(TRACY_ON_DEMAND "" OFF)
# My laptop's TSC is not synchronized across cores (kernel marks it unstable),
# so rdtsc timestamps jump backwards on core migration and break the profiler UI
option(TRACY_TIMER_FALLBACK "" ON)
option(TRACY_DISALLOW_HW_TIMER "" ON)
add_subdirectory(Vendor/tracy)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/CMake")

add_subdirectory(Engine)
add_subdirectory(Launcher)