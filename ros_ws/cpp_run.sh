#!/bin/bash
./cpp_build.sh
source install/setup.bash
ros2 launch launch/test_eboat_allnodes.yaml
