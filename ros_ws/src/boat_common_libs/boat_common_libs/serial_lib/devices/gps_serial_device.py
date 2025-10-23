from typing import Callable
from types import NoneType

import pynmea2
from rclpy.node import Node

from boat_common_libs.alarm_lib.alarm_helper import AlarmPublisher
from boat_common_libs.serial_lib.serial_device import SerialDevice, SerialData


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

class GPVTGResult:
    def __init__(self, speed_knots, true_track):
        self.speed_knots = speed_knots
        self.true_track = true_track


class GPSDevice(SerialDevice):
    def __init__(self, node:Node, alarm_pub:AlarmPublisher, on_gpgga_result:Callable[[GPGGAResult], None], on_gpvtg_result:Callable[[GPVTGResult], None]):
        super().__init__(node, "/dev/ttyUSB1", self._on_gps_msg_rec, alarm_pub)
        self._gga_callback = on_gpgga_result
        self._vtg_callback = on_gpvtg_result
        self.node = node

    def _on_gps_msg_rec(self, data:SerialData):
        if data.to_utf_8().startswith("$GPGGA"):
            gps_str = pynmea2.parse(data.to_utf_8())
            if gps_str.lat != '':
                lat = convert_to_degrees(gps_str.lat, gps_str.lat_dir)
                lon = convert_to_degrees(gps_str.lon, gps_str.lon_dir)
                self._gga_callback(GPGGAResult(lat, lon))

        elif data.to_utf_8().startswith("$GPVTG"):
            gps_str = pynmea2.parse(data.to_utf_8())
            if not type(gps_str.spd_over_grnd_kts) == NoneType and not type(gps_str.track):
                self._vtg_callback(GPVTGResult(float(gps_str.spd_over_grnd_kts), float(gps_str.true_track)))