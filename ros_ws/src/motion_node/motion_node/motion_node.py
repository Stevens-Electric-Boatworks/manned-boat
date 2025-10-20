import pynmea2
import rclpy
from rclpy.executors import ExternalShutdownException
from rclpy.node import Node

from boat_common_libs.alarm_lib.alarm_helper import AlarmPublisher
from boat_common_libs.serial_lib.serial_device import SerialDevice, SerialData
from boat_data_interfaces.msg import MotionData, BoatAlarm, GPSData

import random
import json
import serial

#pip install --break-system-packages pynmea2

class MotionNode(Node):
    def __init__(self):
        super().__init__('motion_node')
        self.publisher_ = self.create_publisher(GPSData, '/motion/gps', 10)
        self._logger.info("Attempting to read REAL GPS Data")
        self.alarm_pub = AlarmPublisher(self)
        self.dev = SerialDevice(self, "/dev/ttyUSB1", self._dev_callback, self.alarm_pub, polling_duration=0.3)
        # self.dev = serial.Serial(
        #     port='/dev/ttyUSB1',
        #     baudrate=115200,
        #     parity=serial.PARITY_NONE,
        #     stopbits=serial.STOPBITS_ONE,
        #     bytesize=serial.EIGHTBITS,
        #     timeout=1
        # )

    def _dev_callback(self, data:SerialData):
        if data.to_utf_8().startswith("$GPGGA"):
            gps_str = pynmea2.parse(data.to_utf_8())
            if gps_str.lat != '':
                if gps_str.lat_dir == 'N':
                    lat = float(gps_str.lat)
                else:
                    lat = -float(gps_str.lat)

                if gps_str.lon_dir == 'W':
                    lon = -float(gps_str.lon)
                else:
                    lon = float(gps_str.lon)

                msg = GPSData()
                msg.lat = lat
                msg.lon = lon
                self.publisher_.publish(msg)
            else:
                msg = GPSData()
                msg.lat = float(0)
                msg.lon = float(0)
                self.publisher_.publish(msg)


def main(args=None):
    try:
        with rclpy.init(args=args):
            rclpy.spin(MotionNode())
    except (KeyboardInterrupt, ExternalShutdownException):
        pass


if __name__ == '__main__':
    main()
