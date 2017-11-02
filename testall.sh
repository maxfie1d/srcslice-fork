#!/usr/bin/env bash
BUILD_DIR="build-debug"
if [ ! -e $BUILD_DIR ]; then
    mkdir $BUILD_DIR
fi
cd $BUILD_DIR
cmake -D CMAKE_BUILD_TYPE=Debug ..
make srcslicetest
./bin/srcslicetest

