import rclpy
from rclpy.executors import ExternalShutdownException
from rclpy.node import Node

from boat_common_libs.smooth_random import SmoothRandom
from boat_data_interfaces.msg import (MotionData, BoatAlarm, GPSData, GPSVTGData, )
                                      #GPSSpeed)  # type: ignore

import random
import json

class MotionNode(Node):
    def __init__(self):
        super().__init__('motion_node_test')
        self._gps_pub = self.create_publisher(GPSData, '/motion/gps', 10)
        self._speed_pub = self.create_publisher(GPSVTGData, '/motion/vtg', 10)
        timer_period = random.random() * 0.1
        self._logger.info("Sending test data at a period of " + str(timer_period))

        # GPS coordinates initialized near Hoboken, NJ for realistic test data.
        # The step is very small to simulate realistic movement.
        self.gps_lat = SmoothRandom(start=40.744, step=0.0001, low=40.73, high=40.76)
        self.gps_long = SmoothRandom(start=-74.032, step=0.0001, low=-74.06, high=-74.02)
        self.speed = SmoothRandom(start=5, step=0.5, low=-10, high=40)
        self.timer = self.create_timer(timer_period, self.timer_callback)

    def timer_callback(self):
        msg = GPSData()

        msg.lon = self.gps_long.next()
        msg.lat = self.gps_lat.next()
        self._gps_pub.publish(msg)
        self._speed_pub.publish(GPSVTGData(speed=float(self.speed.next()), true_track=float(14)))


def main(args=None):
    try:
        with rclpy.init(args=args):
            rclpy.spin(MotionNode())
    except (KeyboardInterrupt, ExternalShutdownException):
        pass


if __name__ == '__main__':
    main()
