#!/bin/bash

if [ "$1" = "release" ]
then
    ./build/release/compute_shaders
else
    ./build/debug/compute_shaders
fi
