import rclpy
from pynmea2 import VTG
from rclpy.executors import ExternalShutdownException
from rclpy.node import Node

from boat_common_libs.alarm_lib.alarm_helper import AlarmPublisher
from boat_common_libs.serial_lib.devices.gps_serial_device import GPGGAResult, GPSDevice, GPVTGResult, GPGSVResult, GPGSAResult
from boat_data_interfaces.msg import GPSData, GPSVTGData, GPSSVData, GPGSAData

#pip install --break-system-packages pynmea2

class MotionNode(Node):
    def __init__(self):
        super().__init__('motion_node')
        self.declare_parameter("gnss_serial_fd", "/dev/ttyUSB1")
        self._gps_pub = self.create_publisher(GPSData, '/motion/gps', 10)
        self._speed_pub = self.create_publisher(GPSVTGData, '/motion/vtg', 10)
        self._sats_pub = self.create_publisher(GPSSVData, '/motion/sv', 10)
        self._mode_pub = self.create_publisher(GPGSAData, '/motion/gsa', 10)
        self._logger.info("Attempting to read REAL GPS Data")
        self.alarm_pub = AlarmPublisher(self)
        self.dev = GPSDevice(self, self.alarm_pub, self._gpa_callback, self._vtg_callback, self._sv_callback, self._gsa_callback)
        # self.dev = serial.Serial(
        #     port='/dev/ttyUSB1',
        #     baudrate=115200,
        #     parity=serial.PARITY_NONE,
        #     stopbits=serial.STOPBITS_ONE,
        #     bytesize=serial.EIGHTBITS,
        #     timeout=1
        # )

    def _gpa_callback(self, data:GPGGAResult):
        self._gps_pub.publish(GPSData(lat=data.lat, lon=data.lon))

    def _vtg_callback(self, data:GPVTGResult):
        self._speed_pub.publish(GPSVTGData(speed=data.speed_knots, true_track=data.true_track))

    def _sv_callback(self, data:GPGSVResult):
        self._sats_pub.publish(GPSSVData(sats=data.sats))

    def _gsa_callback(self, data:GPGSAResult):
        self._mode_pub.publish(GPGSAData(
            op_mode=data.op_mode,
            mode=data.mode,
            prn=data.prn,
            pdop=data.pdop,
            hdop=data.hdop,
            vdop=data.vdop,
            system_id=data.system_id
        ))


def main(args=None):
    try:
        with rclpy.init(args=args):
            rclpy.spin(MotionNode())
    except (KeyboardInterrupt, ExternalShutdownException):
        pass


if __name__ == '__main__':
    main()
