#!/bin/bash

target_release() {
    cd release
    cmake -DCMAKE_BUILD_TYPE=Release ../..
    make
    echo "Built target in build/release/"
    cd ../..
}

target_debug() {
    cd debug 
    cmake -DCMAKE_BUILD_TYPE=Debug ../..
    make
    echo "Built target in build/debug/"
    cd ../..
}

# Create folder for distribution
if [ "$1" = "release" ]
then
    if [ -d "$compute_shaders" ]
    then
        rm -rf -d compute_shaders
    fi

    mkdir -p compute_shaders
fi

# Creates the folder for the buildaries
mkdir -p compute_shaders 
mkdir -p compute_shaders/assets
mkdir -p build
mkdir -p build/release
mkdir -p build/debug
cd build

# Builds target
if [ "$1" = "release" ]
then
    target_release
    cp build/release/compute_shaders compute_shaders/compute_shaders
else
    target_debug
fi

cp -R assets compute_shaders/
