# DEPRECATION NOTICE

Please use the `rosbag-to-csv` project in this org to process data.

# Data Extraction Scripts
These scripts contain some tools to take a ROS bag recording, and return a `.csv` of specific topics/data points.

# Dependency Installations

Run the following commands to install the dependencies needed for the project (you do NOT need to be “sourced” with ROS)
```bash
$ pip install --break-system-packages pandas mcap-ros2-support
```

# Getting Started

1) Obtain a ROS bag recording using `$ ros bag record`
	1) If you are using the startup script on the Pi provided by this repo, this will be run automatically on startup, and will be stored in `~/ros_bag_logs`
	2) Copy these files into `input_rosbag`
		1) This file is added to the .gitignore, so you don’t commit ROSbag files to the repo by accident
2) Decide on what kind of data you are trying to output
	1) For example, if you are trying to extract temp data, and know that the topic `/electrical/temp_sensors/out` has data that you want, you will use the following string to denote the specific data type: `/electrical/temp_sensors/out.outlet_temp`
	2) This is because you want to use the `outlet_temp` variable
		1) Reminder that you can use `$ ros2 topic echo {topic}` to find that topic, and see what data it is outputting
	3) This data be a comma-separated list
3) Then, with your specific data, run the following command
	1) `$ ./extract-generic.sh {topic_comma_seperated_list} {.mcap file in rosbag}`
	2) Example: 
		1) `$ ./extract-generic.sh /electrical/temp_sensors/out.outlet_temp,/electrical/temp_sensors/in.inlet_temp input_rosbag/rosbag2_2025_10_21-19_41_03/rosbag2_2025_10_21-19_41_03_0.mcap`
4) You will now have a `.csv` file in `output_csv`

# Helpful Scripts

The script run above in the tutorial is a generic script that will run for anything and any topic, but we have a bunch of helper scripts that extract common information needed.

* `extract-coolant-temps.sh {ROS_BAG_FILE}`
	* Takes in a ROS Bag file and looks for the `/electrical/temp_sensors/*.*` temperature sensor loggings (currently inlet and outlet)


