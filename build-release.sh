#!/usr/bin/env bash
BUILD_DIR="build-release"

rm -r $BUILD_DIR
mkdir $BUILD_DIR; cd $BUILD_DIR
cmake ..
make
