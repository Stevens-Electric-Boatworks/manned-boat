#!/bin/bash

echo Taking ROS Bag file at "${1}"
# Reminder that the data is comma seperated, and is {topic name}.{data name (in the .msg file)}
DATA="/electrical/temp_sensors/out.outlet_temp,/electrical/temp_sensors/in.inlet_temp,/motors/can_motor_data.motor_temp,/motors/can_motor_data.rpm,/motors/can_motor_data.current,/motors/can_motor_data.voltage"
./extract-generic.sh $DATA "${1}"
