#!/bin/sh

echo "[WebLI] INIT: Clone Submodules...";
git submodule --quiet update --init

echo "[WebLI] INIT: Done!";

exit 0;
