#!/usr/bin/env bash
BUILD_DIR="build-debug"

rm -r $BUILD_DIR
mkdir $BUILD_DIR; cd $BUILD_DIR
cmake -D CMAKE_BUILD_TYPE=Debug ..
make
