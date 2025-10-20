import rclpy
from rclpy.executors import ExternalShutdownException
from rclpy.node import Node

# pip install pyserial (should be installed by default)
import serial

from boat_common_libs.alarm_lib.alarm_helper import AlarmPublisher
from boat_data_interfaces.msg import InletCoolantData, OutletCoolantData
from boat_common_libs.serial_lib.serial_device import SerialDevice, SerialData


class ElectricalNode(Node):
    def __init__(self):
        super().__init__('electrical_node')
        self._out_pub = self.create_publisher(OutletCoolantData, '/electrical/temp_sensors/out', 10)
        self._in_pub = self.create_publisher(InletCoolantData, '/electrical/temp_sensors/in', 10)

        self.alarm_pub = AlarmPublisher(self)
        # Verify serial directory with pi
        self.dev_a = SerialDevice(self, "/dev/ttyACM0", self._on_ser_read, self.alarm_pub)
        self.dev_a = SerialDevice(self, "/dev/ttyACM1", self._on_ser_read, self.alarm_pub)

    def _on_ser_read(self, data:SerialData):
        self._pub_data(data.to_utf_8())

    def _pub_data(self, data):
        if data[0] == 'I':
            self._in_pub.publish(InletCoolantData(inlet_temp=float(data[1:])))
        elif data[0] == 'O':
            self._out_pub.publish(OutletCoolantData(outlet_temp=float(data[1:])))

def main(args=None):
    try:
        with rclpy.init(args=args):
            rclpy.spin(ElectricalNode())
    except (KeyboardInterrupt, ExternalShutdownException):
        pass


if __name__ == '__main__':
    main()
