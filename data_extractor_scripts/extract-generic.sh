#!/bin/bash

echo "Extracting $1"
echo "Taking it from ROS Bag file at $2"

mkdir -p output_csv
python3 data_extractor.py "$1" "$2" "output_csv/data_filtered_$(eval date +%d_%b_%Y-%H_%M_%S).csv"
