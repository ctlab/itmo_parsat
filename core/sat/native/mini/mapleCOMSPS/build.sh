#!/bin/bash
#set -eu

SOURCE_DIR="$1"
BUILD_DIR="$2"
M4RI_SUBPATH="/core/sat/native/mini/mapleCOMSPS/m4ri-20140914"
M4RI_BUILD_DIR="$BUILD_DIR/$M4RI_SUBPATH"
M4RI_SOURCE_DIR="$SOURCE_DIR/$M4RI_SUBPATH"

if ! [[ -f "${M4RI_BUILD_DIR}/.libs/libm4ri.a" ]]; then
    echo "Building m4ri now..."
    cp -r "${M4RI_SOURCE_DIR}" "${M4RI_BUILD_DIR}"
    cwd=$(pwd)
    cd "${M4RI_BUILD_DIR}"
    echo "I am in $(pwd) and building"
    CC=gcc CXX=gcc ./configure 
    CC=gcc CXX=gcc gcc-10=gcc make 
    make install
    cd $cwd
else
    echo "-- m4ri has already been built"
fi
