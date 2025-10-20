from typing import Callable

import pynmea2
from rclpy.node import Node

from boat_common_libs.alarm_lib.alarm_helper import AlarmPublisher
from boat_data_interfaces.msg import GPSData
from serial_lib.serial_device import SerialDevice, SerialData


def convert_to_degrees(raw_value, direction):
    raw_value = float(raw_value)
    degrees = int(raw_value // 100)
    minutes = raw_value - (degrees * 100)
    decimal = degrees + minutes / 60.0

    if direction in ['S', 'W']:
        decimal = -decimal
    return decimal


class GPGGAResult:
    def __init__(self, lat, lon):
        self.lat = lat
        self.lon = lon


class GPSDevice(SerialDevice):
    def __init__(self, node:Node, alarm_pub:AlarmPublisher, on_gpgga_result:Callable[[GPGGAResult], None]):
        super().__init__(node, "/dev/ttyUSB1", self._on_gps_msg_rec, alarm_pub)
        self.callback = on_gpgga_result

    def _on_gps_msg_rec(self, data:SerialData):
        if data.to_utf_8().startswith("$GPGGA"):
            gps_str = pynmea2.parse(data.to_utf_8())
            if gps_str.lat != '':
                lat = convert_to_degrees(gps_str.lat, gps_str.lat_dir)
                lon = convert_to_degrees(gps_str.lon, gps_str.lon_dir)

                self.callback(GPGGAResult(lat, lon))