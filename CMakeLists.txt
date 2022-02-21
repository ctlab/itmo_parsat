cmake_minimum_required(VERSION 3.16)
project(itmo-parsat)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Enables LTO
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)

# Global compiler options
add_compile_options(-march=native)

# Global linker options
link_libraries(tbb)

# Options
option(SANITIZE "Enable sanitizers" OFF)

if (CMAKE_BUILD_TYPE STREQUAL "DEBUG")
    message(STATUS "DEBUG build mode (Og). All checks are enabled, log and tracer are enabled.")
    add_compile_options(-Og)
elseif (CMAKE_BUILD_TYPE STREQUAL "DEV_FAST")
    message(STATUS "DEV_FAST build mode (O3). Internal checks disabled.")
    add_compile_options(-O3)
    add_compile_definitions(IPS_DISABLE_HEAVY_CHECKS NDEBUG)
elseif (CMAKE_BUILD_TYPE STREQUAL "RELEASE")
    message(STATUS "RELEASE build mode (O3). All checks, log and tracer disabled.")
    add_compile_options(-O3)
    add_compile_definitions(IPS_DISABLE_TRACE IPS_DISABLE_HEAVY_CHECKS IPS_DISABLE_VERIFY IPS_DISABLE_LOG NDEBUG)
endif ()

if (SANITIZE)
    message(STATUS "Address sanitizer enabled.")
    add_compile_definitions(IPS_SANITIZE)
    add_link_options(
            -fsanitize=address
            -fsanitize=leak
            -fsanitize=undefined
    )
endif ()

# Dependencies
find_package(GTest REQUIRED)
find_package(glog REQUIRED)
find_package(Boost 1.76 COMPONENTS program_options timer thread REQUIRED)
find_package(Protobuf REQUIRED)
find_package(ZLIB)

# Root-level include statements are default in this project
include_directories(.)

add_subdirectory(util)
add_subdirectory(core)
add_subdirectory(infra)
add_subdirectory(cli)
