from datetime import date

from boat_data_interfaces.msg._satellite import Satellite
import rclpy
from rclpy.executors import ExternalShutdownException
from rclpy.node import Node

from boat_common_libs.smooth_random import SmoothRandom
from boat_data_interfaces.msg import (MotionData, BoatAlarm, GPSData, GPSVTGData, GPSSVData, GPGSAData,)
                                      #GPSSpeed)  # type: ignore

import random
import json

class MotionNode(Node):
    def __init__(self):
        super().__init__('motion_node_test')
        self._gps_pub = self.create_publisher(GPSData, '/motion/gps', 10)
        self._speed_pub = self.create_publisher(GPSVTGData, '/motion/vtg', 10)
        self._sats_pub = self.create_publisher(GPSSVData, '/motion/sv', 10)
        self._sats_mode_pub = self.create_publisher(GPGSAData, '/motion/gsa', 10)
        timer_period = random.random() * 0.1
        self._logger.info("Sending test data at a period of " + str(timer_period))

        # GPS coordinates initialized near Hoboken, NJ for realistic test data.
        # The step is very small to simulate realistic movement.
        self.gps_lat = SmoothRandom(start=40.744, step=0.0001, low=40.73, high=40.76)
        self.gps_long = SmoothRandom(start=-74.032, step=0.0001, low=-74.06, high=-74.02)
        self.speed = SmoothRandom(start=5, step=0.5, low=-10, high=40)
        self.heading = SmoothRandom(start=55, step=1, low=0, high=360)
        self.sats = SmoothRandom(start=6, step=0.5, low=0, high=18)
        self.timer = self.create_timer(timer_period, self.timer_callback)

    def timer_callback(self):
        msg = GPSData()
        sats = GPSSVData()
        mode = GPGSAData()

        msg.lon = self.gps_long.next()
        msg.lat = self.gps_lat.next()
        self._gps_pub.publish(msg)
        self._speed_pub.publish(GPSVTGData(speed=float(self.speed.next()), true_track=float(self.heading.next())))
        sats.sats.append(Satellite(prn=24, elev=64, azimuth=80, snr=93))
        sats.sats.append(Satellite(prn=14, elev=41, azimuth=99, snr=62))
        sats.sats.append(Satellite(prn=5, elev=63, azimuth=12, snr=44))
        sats.sats.append(Satellite(prn=8, elev=85, azimuth=156, snr=87))
        sats.sats.append(Satellite(prn=16, elev=14, azimuth=277, snr=55))
        sats.sats.append(Satellite(prn=19, elev=89, azimuth=190, snr=53))
        self._sats_pub.publish(sats)
        mode.mode = 3
        mode.op_mode = "A"
        mode.hdop = 0.01
        mode.vdop = 0.02
        mode.pdop = 0.01
        mode.system_id=1
        mode.prn = [ 5, 14, 24 ]
        self._sats_mode_pub.publish(mode)


def main(args=None):
    try:
        with rclpy.init(args=args):
            rclpy.spin(MotionNode())
    except (KeyboardInterrupt, ExternalShutdownException):
        pass


if __name__ == '__main__':
    main()
