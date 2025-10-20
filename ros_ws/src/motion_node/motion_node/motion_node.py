import rclpy
from rclpy.executors import ExternalShutdownException
from rclpy.node import Node

from boat_common_libs.alarm_lib.alarm_helper import AlarmPublisher
from boat_common_libs.serial_lib.devices.gps_serial_device import GPGGAResult, GPSDevice
from boat_data_interfaces.msg import GPSData


#pip install --break-system-packages pynmea2

class MotionNode(Node):
    def __init__(self):
        super().__init__('motion_node')
        self.publisher_ = self.create_publisher(GPSData, '/motion/gps', 10)
        self._logger.info("Attempting to read REAL GPS Data")
        self.alarm_pub = AlarmPublisher(self)
        self.dev = GPSDevice(self, self.alarm_pub, self._dev_callback)
        # self.dev = serial.Serial(
        #     port='/dev/ttyUSB1',
        #     baudrate=115200,
        #     parity=serial.PARITY_NONE,
        #     stopbits=serial.STOPBITS_ONE,
        #     bytesize=serial.EIGHTBITS,
        #     timeout=1
        # )

    def _dev_callback(self, data:GPGGAResult):
        self.publisher_.publish(GPSData(lat=data.lat, lon=data.lon))


def main(args=None):
    try:
        with rclpy.init(args=args):
            rclpy.spin(MotionNode())
    except (KeyboardInterrupt, ExternalShutdownException):
        pass


if __name__ == '__main__':
    main()
