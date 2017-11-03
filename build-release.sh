#!/usr/bin/env bash
BUILD_DIR="build-release"
if [ ! -e $BUILD_DIR ]; then
    mkdir $BUILD_DIR
fi
cd $BUILD_DIR
cmake ..
make

