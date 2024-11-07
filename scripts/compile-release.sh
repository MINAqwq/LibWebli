#!/bin/sh

if [ -d "./build" ]; then
    echo "delete old build dir before compiling release"
    exit 1
fi

cmake -S . -B ./build -G Ninja  -DWEBLI_CLIENT=ON -DCMAKE_BUILD_TYPE=Release
cmake --build ./build
