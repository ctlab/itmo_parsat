#cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

#project(mapleCOMSPS)
#set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
#set(CMAKE_CXX_STANDARD 17)

#add_compile_options(-O3 -flto)
#add_compile_definitions(NDEBUG)

include_directories(.)

#--------------------------------------------------------------------------------------------------
# Configurable options:

option(STATIC_BINARIES "Link binaries statically." ON)

#--------------------------------------------------------------------------------------------------
# Library version:

set(mapleCOMSPS_SOMAJOR 2)
set(mapleCOMSPS_SOMINOR 1)
set(mapleCOMSPS_SORELEASE 0)

# Compute VERSION and SOVERSION:
if (USE_SORELEASE)
    set(mapleCOMSPS_VERSION ${mapleCOMSPS_SOMAJOR}.${mapleCOMSPS_SOMINOR}.${mapleCOMSPS_SORELEASE})
else ()
    set(mapleCOMSPS_VERSION ${mapleCOMSPS_SOMAJOR}.${mapleCOMSPS_SOMINOR})
endif ()
set(mapleCOMSPS_SOVERSION ${mapleCOMSPS_SOMAJOR})

#--------------------------------------------------------------------------------------------------
# Build m4ri:
set(M4RI_LIBS ${CMAKE_BINARY_DIR}/core/sat/native/mini/mapleCOMSPS/m4ri-20140914/.libs/libm4ri.a)

add_custom_target(m4ri-build)
add_custom_command(
        TARGET m4ri-build
        BYPRODUCTS ${M4RI_LIBS}
        COMMAND core/sat/native/mini/mapleCOMSPS/build.sh ${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR}
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
)

include_directories(m4ri-20140914)

#--------------------------------------------------------------------------------------------------
# Dependencies:

find_package(ZLIB)
include_directories(${ZLIB_INCLUDE_DIR})
include_directories(${mapleCOMSPS_SOURCE_DIR})

#--------------------------------------------------------------------------------------------------
# Compile flags:

add_definitions(-D__STDC_FORMAT_MACROS -D__STDC_LIMIT_MACROS)

#--------------------------------------------------------------------------------------------------
# Build Targets:

set(mapleCOMSPS_LIB_SOURCES
        mapleCOMSPS/core/Solver.cc
        mapleCOMSPS/simp/SimpSolver.cc)

add_library(mapleCOMSPS-lib-static STATIC ${mapleCOMSPS_LIB_SOURCES})
add_dependencies(mapleCOMSPS-lib-static m4ri-build)
target_link_libraries(mapleCOMSPS-lib-static ${ZLIB_LIBRARY} ${M4RI_LIBS} mini-lib-static)

add_executable(mapleCOMSPS_core mapleCOMSPS/core/Main.cc)
add_dependencies(mapleCOMSPS_core m4ri-build)
add_executable(mapleCOMSPS_simp mapleCOMSPS/simp/Main.cc)
add_dependencies(mapleCOMSPS_simp m4ri-build)

if (STATIC_BINARIES)
    target_link_libraries(mapleCOMSPS_core mapleCOMSPS-lib-static)
    target_link_libraries(mapleCOMSPS_simp mapleCOMSPS-lib-static)
endif ()

set_target_properties(mapleCOMSPS-lib-static PROPERTIES OUTPUT_NAME "mapleCOMSPS")
set_target_properties(mapleCOMSPS_simp PROPERTIES OUTPUT_NAME "mapleCOMSPS")

#--------------------------------------------------------------------------------------------------
# Installation targets:

install(TARGETS mapleCOMSPS-lib-static mapleCOMSPS_core mapleCOMSPS_simp
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)

install(DIRECTORY mapleCOMSPS/mtl mapleCOMSPS/utils mapleCOMSPS/core mapleCOMSPS/simp
        DESTINATION include/mapleCOMSPS
        FILES_MATCHING PATTERN "*.h")
