#!/bin/sh

echo "[WebLI] INIT: Clone Submodules...";
git submdule --quit update --init || exit 1

echo "[WebLI] INIT: Done!";

exit 0;
