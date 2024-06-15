#!/usr/bin/env bash

# Run this script to generate a compile_commands.json for autocompletion.

BUILD_PATH="build"

cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -S . -B "$BUILD_PATH"
ln -sf "$BUILD_PATH/compile_commands.json" compile_commands.json

# https://github.com/MaskRay/ccls/issues/931
sed -ri 's/"[^"]*-(gcc|g\+\+) /"\1 /' "$BUILD_PATH/compile_commands.json"
