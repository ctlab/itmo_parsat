#!/bin/bash
set -eux

git config --local pull.rebase true
root=$(pwd)
ln -sf "$root/scripts/hooks/pre-commit" "$root/.git/hooks/pre-commit"
ln -sf "$root/scripts/hooks/prepare-commit-msg" "$root/.git/hooks/prepare-commit-msg"
ln -sf "$root/scripts/docker/ips.Dockerfile" Dockerfile
ln -sf "$root/scripts/run.sh" ips
ln -sf "$root/scripts/d.sh" d
