#! /bin/bash
source /home/rpi/.bashrc

echo "Starting up the Webserver for the Boat Display"
eval "$(fnm env --use-on-cd --shell bash)"
    


echo "Starting up ROS2"

# For Development
#USER_NAME="ishaan"
#ROS_WS="eboat_src/ros_ws"

# Relative to ros_ws/launch
LAUNCH_FILE="eboat_real_cpp.yaml"

# Relative to home
ROS_BAG_LOG_DIR="ros_bag_logs"


# For the RPI
USER_NAME="rpi"
ROS_WS="/home/$USER_NAME/eboat_src/ros_ws"
cd $ROS_WS


#echo "Starting up the CAN bus"
#sudo ip link set can0 type can bitrate 500000 
#sudo ip link set up can0

# Source the install.bash
echo "Sourcing install.bash"
source install/setup.bash

echo "Starting ROSBridge"
ros2 launch rosbridge_server rosbridge_websocket_launch.xml &

# Start recording everything through ROSBag
mkdir -p /home/$USER_NAME/$ROS_BAG_LOG_DIR
cd /home/$USER_NAME/$ROS_BAG_LOG_DIR || exit

echo "Attempting to record through ROSBag"
ros2 bag record -a &
echo "Started recording through ROSBag"

# Go into the launch directory, and launch the nodes in the background

echo "Launching launch/$LAUNCH_FILE"

ros2 launch $ROS_WS/launch/$LAUNCH_FILE &

echo "Finshed running startup script"

