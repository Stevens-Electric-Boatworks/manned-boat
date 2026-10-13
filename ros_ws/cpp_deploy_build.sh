#!/bin/bash

# This script is used for when deploying to the MiniPC
set -e
colcon build --cmake-args -DCMAKE_BUILD_TYPE=Release
