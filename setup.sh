#!/usr/bin/env bash
set -eux

root=$(pwd)

# Create build directory and prepare cmake env
rm -rf build
mkdir -p build &> /dev/null || true
cd build
cmake .. -DCMAKE_BUILD_TYPE=DEV_FAST
cd ../

if [[ -d ".git" ]]; then
  ln -sf "$root/scripts/hooks/pre-commit" "$root/.git/hooks/pre-commit"
  ln -sf "$root/scripts/hooks/prepare-commit-msg" "$root/.git/hooks/prepare-commit-msg"
  git config --local pull.rebase true
fi

ln -sf "$root/scripts/docker/ips.Dockerfile" Dockerfile
ln -sf "$root/scripts/docker/Singularity.def" Singularity.def
ln -sf "$root/scripts/run.sh" ips
ln -sf "$root/scripts/d.sh" d
ln -sf "$root/build/compile_commands.json"
