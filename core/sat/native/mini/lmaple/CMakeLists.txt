#add_custom_target(
  #NAME lmaple-lib-static-file ALL
  #COMMAND make libr
  #BYPRODUCTS ${CMAKE_CURRENT_BINARY_DIR}/lib_release.a
  #COMMENT "Building lmaple"
#)

#include_directories(sources)

#add_library(lmaple-lib-static INTERFACE)
#add_dependencies(lmaple-lib-static lmaple-lib-static-file)
#target_link_libraries(lmaple-lib-static INTERFACE ${CMAKE_CURRENT_BINARY_DIR}/lib_release.a)

# =================================

#cmake_minimum_required(VERSION 3.14 FATAL_ERROR)
#
#project(lmaple)
#set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
#set(CMAKE_CXX_STANDARD 17)

#--------------------------------------------------------------------------------------------------
# Configurable options:

#option(STATIC_BINARIES "Link binaries statically." ON)
#option(USE_SORELEASE   "Use SORELEASE in shared library filename." ON)

include_directories(..)
include_directories(.)
include_directories(lmaple)

#--------------------------------------------------------------------------------------------------
# Library version:

set(lmaple_SOMAJOR 2)
set(lmaple_SOMINOR 1)
set(lmaple_SORELEASE 0)

# Compute VERSION and SOVERSION:
if (USE_SORELEASE)
    set(lmaple_VERSION ${lmaple_SOMAJOR}.${lmaple_SOMINOR}.${lmaple_SORELEASE})
else ()
    set(lmaple_VERSION ${lmaple_SOMAJOR}.${lmaple_SOMINOR})
endif ()
set(lmaple_SOVERSION ${lmaple_SOMAJOR})

#--------------------------------------------------------------------------------------------------
# Dependencies:

find_package(ZLIB)
include_directories(${ZLIB_INCLUDE_DIR})
include_directories(${lmaple_SOURCE_DIR})

#--------------------------------------------------------------------------------------------------
# Compile flags:

add_definitions(-D__STDC_FORMAT_MACROS -D__STDC_LIMIT_MACROS)

#--------------------------------------------------------------------------------------------------
# Build Targets:

set(lmaple_LIB_SOURCES
        lmaple/core/Solver.cc
        lmaple/simp/SimpSolver.cc
        lmaple/utils/ccnr.cc)

add_library(lmaple-lib-static STATIC ${lmaple_LIB_SOURCES})
target_link_libraries(lmaple-lib-static ${ZLIB_LIBRARY} mini-lib-static)

#add_executable(lmaple_core lmaple/core/Main.cc)
#add_executable(lmaple_simp lmaple/simp/Main.cc)

if (STATIC_BINARIES)
#    target_link_libraries(lmaple_core lmaple-lib-static)
#    target_link_libraries(lmaple_simp lmaple-lib-static)
endif ()

set_target_properties(lmaple-lib-static PROPERTIES OUTPUT_NAME "lmaple")
#set_target_properties(lmaple_simp PROPERTIES OUTPUT_NAME "lmaple")

#--------------------------------------------------------------------------------------------------
# Installation targets:

#install(TARGETS lmaple-lib-static lmaple_core lmaple_simp
#        RUNTIME DESTINATION bin
#        LIBRARY DESTINATION lib
#        ARCHIVE DESTINATION lib)

install(DIRECTORY lmaple/mtl lmaple/utils lmaple/core lmaple/simp
        DESTINATION include/lmaple
        FILES_MATCHING PATTERN "*.h")
