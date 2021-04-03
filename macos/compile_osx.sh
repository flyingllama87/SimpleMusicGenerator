#!/bin/bash

set -eo pipefail

mkdir bin || true
clang++ ../src/*.cpp -I/usr/local/include/SDL2/ -L/usr/local/lib/ -l SDL2  -std=c++1y -v -o ./bin/SimpleMusicGen