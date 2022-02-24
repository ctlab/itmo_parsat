#!/bin/bash
set -eu

SOURCE_DIR="$1"
BUILD_DIR="$2"
M4RI_SUBPATH="/core/sat/native/mini/mapleCOMSPS/m4ri-20140914"
M4RI_BUILD_DIR="$BUILD_DIR/$M4RI_SUBPATH"
M4RI_SOURCE_DIR="$SOURCE_DIR/$M4RI_SUBPATH"

if ! [[ -f "${M4RI_BUILD_DIR}/.libs/libm4ri.a" ]]; then
    echo "Building m4ri"
    cp -r "${M4RI_SOURCE_DIR}" "${M4RI_BUILD_DIR}"
    cd "${M4RI_BUILD_DIR}"
    ./configure
    make -j 10
else
    echo "-- m4ri has already been built"
fi