#!/bin/bash
set -e
colcon build --cmake-args -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_BUILD_TYPE=Debug --packages-skip shore_comms_cpp
