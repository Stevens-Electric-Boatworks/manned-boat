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
        self.motorA = {
            "voltage": SmoothRandom(180, 1.0, 100, 200),  # int8, ~200 V system
            "throttle_mv": SmoothRandom(0, 20, 0, 5000),  # int16, mV
            "throttle_percentage": SmoothRandom(0, 3.0, 0, 100),  # int8, %
            "rpm": SmoothRandom(0, 400, -1200, 1800),  # int16, up to ~12k RPM
            "torque": SmoothRandom(0, 2.0, 0, 500),  # int16, Nm (scaled higher)
            "motor_temp": SmoothRandom(40, 0.5, 20, 150),  # int8, °C (idle warm to overheated)
            "current": SmoothRandom(0, 5.0, 0, 300),  # int8, A
            "power": SmoothRandom(0, 60, 0, 6000),  # int16, up to ~6 kW
        }
        self.motorB = {
            "voltage": SmoothRandom(180, 1.0, 100, 200),  # int8, ~200 V system
            "throttle_mv": SmoothRandom(0, 20, 0, 5000),  # int16, mV
            "throttle_percentage": SmoothRandom(0, 3.0, 0, 100),  # int8, %
            "rpm": SmoothRandom(0, 400, -1200, 1800),  # int16, up to ~12k RPM
            "torque": SmoothRandom(0, 2.0, 0, 500),  # int16, Nm (scaled higher)
            "motor_temp": SmoothRandom(40, 0.5, 20, 150),  # int8, °C (idle warm to overheated)
            "current": SmoothRandom(0, 5.0, 0, 300),  # int8, A
            "power": SmoothRandom(0, 60, 0, 6000),  # int16, up to ~6 kW
        }
        self.can_motor_a_pub = self.create_publisher(CANMotorData, '/motors/motorA', 10)
        self.can_motor_b_pub = self.create_publisher(CANMotorData, '/motors/motorB', 10)
        self.can_bus_status_publisher = self.create_publisher(CANBusStatus, '/motors/can_bus_state', 10)
        self.create_timer(0.3, self.publish_test_data)
        self.create_timer(1, self.publish_bus_state)

    def publish_test_data(self):
        motor_a_msg = CANMotorData()
        motor_a_msg.voltage = int(self.motorA["voltage"].next())
        motor_a_msg.throttle_mv = int(self.motorA["throttle_mv"].next())
        motor_a_msg.throttle_percentage = int(self.motorA["throttle_percentage"].next())
        motor_a_msg.rpm = int(self.motorA["rpm"].next())
        motor_a_msg.torque = int(self.motorA["torque"].next())
        motor_a_msg.motor_temp = int(self.motorA["motor_temp"].next())
        motor_a_msg.current = int(self.motorA["current"].next())
        motor_a_msg.power = int(self.motorA["power"].next())
        
        motor_b_msg = CANMotorData()
        motor_b_msg.voltage = int(self.motorA["voltage"].next())
        motor_b_msg.throttle_mv = int(self.motorA["throttle_mv"].next())
        motor_b_msg.throttle_percentage = int(self.motorA["throttle_percentage"].next())
        motor_b_msg.rpm = int(self.motorA["rpm"].next())
        motor_b_msg.torque = int(self.motorA["torque"].next())
        motor_b_msg.motor_temp = int(self.motorA["motor_temp"].next())
        motor_b_msg.current = int(self.motorA["current"].next())
        motor_b_msg.power = int(self.motorA["power"].next())

        self.can_motor_a_pub.publish(motor_a_msg)
        self.can_motor_b_pub.publish(motor_b_msg)

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