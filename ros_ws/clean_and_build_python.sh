#!/bin/bash

rm -rf build install log 
colcon build --symlink-install --packages-skip shore_comms_cpp boat_common_libs_cpp ixwebsocket
source install/setup.bash
