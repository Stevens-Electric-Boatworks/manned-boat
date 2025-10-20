from rclpy.node import Node

from boat_common_libs.alarm_lib.alarm_helper import AlarmPublisher
from serial_lib.serial_device import SerialDevice, SerialData


class GPSDevice(SerialDevice):
    def __init__(self, node:Node, alarm_pub:AlarmPublisher):
        super().__init__(node, "/dev/ttyUSB1", self._on_gps_msg_rec, alarm_pub)


    def _on_gps_msg_rec(self, data:SerialData):
        gps_str = data.to_utf_8()
