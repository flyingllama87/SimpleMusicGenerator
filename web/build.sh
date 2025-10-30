#!/bin/zsh

# export EMCC_DEBUG=1
mkdir -p build
cp shell-minimal.html ./build
cd build
cmake --version
emcmake cmake .. || exit 1
cmake --build ./ || exit 1
cd ../