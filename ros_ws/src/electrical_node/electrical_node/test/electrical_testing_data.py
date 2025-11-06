import random

import rclpy
from rclpy.node import Node
from rclpy.executors import ExternalShutdownException

from boat_common_libs.alarm_lib.alarm_helper import AlarmPublisher
from boat_common_libs.alarm_lib.alarms import Alarm
from boat_common_libs.smooth_random import SmoothRandom
from boat_data_interfaces.msg import OutletCoolantData, InletCoolantData


class ElectricalTestingDataNode(Node):
    def __init__(self):
        super().__init__("electrical_testing_data")
        self._logger.info("Sending test data for /electrical/temp_sensors")
        self.randoms = {
            "inlet_temp": SmoothRandom(20, 0.8, -30, 50),
            "outlet_temp": SmoothRandom(30, 0.5, -30, 50),
        }
        self._out_pub = self.create_publisher(OutletCoolantData, '/electrical/temp_sensors/out', 10)
        self._in_pub = self.create_publisher(InletCoolantData, '/electrical/temp_sensors/in', 10)
        self.alarm_pub = AlarmPublisher(self)
        self.create_timer(0.5, self.publish_test_data)

    def publish_test_data(self):
        msg_out = OutletCoolantData()
        msg_in = InletCoolantData()
        msg_in.inlet_temp = self.randoms["inlet_temp"].next()
        msg_out.outlet_temp = self.randoms["outlet_temp"].next()

        self._out_pub.publish(msg_out)
        self._in_pub.publish(msg_in)

        self.alarm_pub.publish_alarm(Alarm.ERROR_READING_CAN_SDO)



def main(args=None):
    try:
        with rclpy.init(args=args):
            rclpy.spin(ElectricalTestingDataNode())
    except (KeyboardInterrupt, ExternalShutdownException):
        pass


if __name__ == '__main__':
    main()