#!/bin/sh

if [ $# -eq 0 ]; then
        echo "[WebLI] Usage: scripts/compile-linked.sh [test.cpp...]";
        exit 1;
fi

cmake -S . -B ./build -DWEBLI_CLIENT=ON
cmake --build ./build

echo "[WebLI] compiling...";
c++ -O2 -ggdb -std=c++20 "$@" -I include/ -I dep/json/include/ -I dep/base64/include ./build/libwebli.so -l crypto -o webli-linked.out;
echo "[WebLI] done!";
exit 0;
