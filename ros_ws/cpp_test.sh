#!/bin/bash
set -e
./cpp_build.sh
source install/setup.bash
colcon test --ctest-args tests --packages-select shore_comms_cpp
colcon test-result --test-result-base build/shore_comms_cpp --verbose --all
