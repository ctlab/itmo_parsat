cmake_minimum_required(VERSION 3.16)
project(itmo-parsat)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Prioritize static linkage
set(Boost_USE_STATIC_LIBS ON)
set(CMAKE_FIND_LIBRARY_SUFFIXES ".a" ".so")

# Options
set(ARCH "native" CACHE STRING "Architecture to optimize for (-march[=native])")
option(SANITIZE "Enable sanitizers" OFF)
option(GPROF "Enable -pg flag for gprof" OFF)
option(LTO "Enable LTO" ON)
option(PGO_GEN "Enable pgo generation" OFF)
option(PGO_USE "Enable pgo usage" OFF)

message(STATUS "The architecture is " ${ARCH})

# Global compiler options
add_compile_options(-march=${ARCH})
add_link_options(-march=${ARCH})

# Global linker options
link_libraries(tbb)

if (CMAKE_BUILD_TYPE STREQUAL "DEBUG")
    message(STATUS "DEBUG build mode (Og). All checks are enabled, log and tracer are enabled.")
    add_compile_options(-O0 -g)
    add_compile_definitions(DEBUG)
    set(LTO OFF)
elseif (CMAKE_BUILD_TYPE STREQUAL "DEV_FAST")
    message(STATUS "DEV_FAST build mode (O3). Internal checks disabled.")
    add_compile_options(-O3)
    add_compile_definitions(
            NDEBUG
            IPS_DISABLE_HEAVY_CHECKS
    )
elseif (CMAKE_BUILD_TYPE STREQUAL "RELEASE")
    message(STATUS "RELEASE build mode (O3). All checks, log and tracer disabled.")
    add_compile_options(-O3)
    add_compile_definitions(
            NDEBUG
            IPS_DISABLE_TRACE
            IPS_DISABLE_HEAVY_CHECKS
            IPS_DISABLE_VERIFY
            IPS_DISABLE_LOG
    )
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

if (GPROF)
    message(STATUS "GPROF options (-pg) enabled.")
    add_compile_options(-pg)
    add_link_options(-pg)
else ()
    message(STATUS "GPROF options (-pg) disabled.")
endif ()

if (LTO)
    message(STATUS "LTO enabled.")
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
else ()
    message(STATUS "LTO disabled.")
endif ()

if (PGO_GEN)
    message(STATUS "PGO generation enabled.")
    add_compile_options(-fprofile-generate -fprofile-correction)
    add_link_options(-fprofile-generate)
endif ()

if (PGO_USE)
    message(STATUS "PGO usage enabled.")
    add_compile_options(-fprofile-use -fprofile-correction -fprofile-partial-training)
    add_link_options(-fprofile-use -fprofile-partial-training)
endif ()

# Dependencies
find_package(GTest REQUIRED)
find_package(glog REQUIRED)
find_package(Boost 1.76 COMPONENTS program_options timer thread REQUIRED)
find_package(Protobuf REQUIRED)
find_package(ZLIB)

# Root-level include statements are default in this project
include_directories(.)

# Project root
add_compile_definitions(IPS_PROJECT_ROOT="${PROJECT_SOURCE_DIR}")

include_directories(${CMAKE_BINARY_DIR})
add_subdirectory(util)
add_subdirectory(core)
add_subdirectory(infra)
add_subdirectory(cli)