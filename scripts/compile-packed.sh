#!/bin/sh

if [ $# -eq 0 ]; then
        echo "[WebLI] Usage: scripts/compile-packed.sh [test.cpp...]";
        exit 1;
fi

echo "[WebLI] compiling...";
c++ -O2 -ggdb -std=c++20 "$@" src/* -I include/ -I dep/json/include/ -lssl -lcrypto -o webli-packed.out;
echo "[WebLI] done!";
exit 0;
