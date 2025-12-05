#!/bin/bash
./rust_build.sh
ros2 run examples_rclrs_minimal_pub_sub minimal_subscriber
