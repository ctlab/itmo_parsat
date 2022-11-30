#!/usr/bin/env bash
set -eux

root=$(pwd)

# Create build directory and prepare cmake env
mkdir -p build &> /dev/null || true
cd build
cmake .. -DCMAKE_BUILD_TYPE=DEV_FAST
cd ../

ln -sf "$root/scripts/docker/ips.Dockerfile" Dockerfile
ln -sf "$root/scripts/docker/Singularity.def" Singularity.def
ln -sf "$root/scripts/run.sh" ips
ln -sf "$root/scripts/d.sh" d
ln -sf "$root/build/compile_commands.json"
