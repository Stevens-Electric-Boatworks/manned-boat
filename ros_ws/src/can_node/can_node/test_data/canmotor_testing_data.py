import random

import rclpy
from rclpy.node import Node
from rclpy.executors import ExternalShutdownException

from boat_common_libs.smooth_random import SmoothRandom
from boat_data_interfaces.msg import CANMotorData, CANBusStatus


class CANMotorTestingDataNode(Node):
    def __init__(self):
        super().__init__("motor_node_testing_data")
        self._logger.info("Sending test data for motors/can_motor_data")
        self.randoms = {
            "voltage": SmoothRandom(180, 1.0, 100, 200),  # int8, ~200 V system
            "throttle_mv": SmoothRandom(0, 20, 0, 5000),  # int16, mV
            "throttle_percentage": SmoothRandom(0, 3.0, 0, 100),  # int8, %
            "rpm": SmoothRandom(0, 400, -1200, 1800),  # int16, up to ~12k RPM
            "torque": SmoothRandom(0, 2.0, 0, 500),  # int16, Nm (scaled higher)
            "motor_temp": SmoothRandom(40, 0.5, 20, 150),  # int8, °C (idle warm to overheated)
            "current": SmoothRandom(0, 5.0, 0, 300),  # int8, A
            "power": SmoothRandom(0, 60, 0, 6000),  # int16, up to ~6 kW
        }
        self.can_motor_publisher_ = self.create_publisher(CANMotorData, '/motors/can_motor_data', 10)
        self.can_bus_status_publisher = self.create_publisher(CANBusStatus, '/motors/can_bus_state', 10)
        self.create_timer(0.3, self.publish_test_data)
        self.create_timer(1, self.publish_bus_state)

    def publish_test_data(self):
        msg = CANMotorData()
        msg.voltage = int(self.randoms["voltage"].next())
        msg.throttle_mv = int(self.randoms["throttle_mv"].next())
        msg.throttle_percentage = int(self.randoms["throttle_percentage"].next())
        msg.rpm = int(self.randoms["rpm"].next())
        msg.torque = int(self.randoms["torque"].next())
        msg.motor_temp = int(self.randoms["motor_temp"].next())
        msg.current = int(self.randoms["current"].next())
        msg.power = int(self.randoms["power"].next())

        self.can_motor_publisher_.publish(msg)

    def publish_bus_state(self):
        msg = CANBusStatus()
        msg.bus_state = CANBusStatus.TESTING
        self.can_bus_status_publisher.publish(msg)

def main(args=None):
    try:
        with rclpy.init(args=args):
            rclpy.spin(CANMotorTestingDataNode())
    except (KeyboardInterrupt, ExternalShutdownException):
        pass


if __name__ == '__main__':
    main()