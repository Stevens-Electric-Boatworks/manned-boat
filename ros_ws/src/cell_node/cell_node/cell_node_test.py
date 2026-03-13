from boat_common_libs.smooth_random import SmoothRandom
from boat_data_interfaces.msg._cell_data import CellData
import rclpy
from rclpy.node import Node
from rclpy.executors import ExternalShutdownException

class CellNode(Node):
    def __init__(self):
        super().__init__('cell_node_test')
        self.publisher_ = self.create_publisher(CellData, "/cell", 10)
        self.timer = self.create_timer(5, self._timer_callback)

        self.network = "AT&T"
        self.tech = "LTE"
        self.rsrp = SmoothRandom(-110, 3, -140, -75)
        self.rsrq = SmoothRandom(-10, 1, -20, -8)
        self.reg_status = 1
        self.ip_addr = "192.168.1.1"
        self.apn = "super"
        self.pin_status = "READY"

        pass

    def _timer_callback(self):
        self.rsrp.next()
        self.rsrq.next()

        # Uses same algorithm as CellSerialDevice
        # to determine bars from the random data
        if self.rsrp.value < -130:
            bars = 0
        elif self.rsrp.value < -120:
            bars = 1
        elif self.rsrp.value < -105:
            bars = 2
        elif self.rsrp.value < -90:
            bars = 3
        elif self.rsrp.value + 140 == 255:
            bars = 255
        else:
            bars = 4

        if self.rsrq.value < -15:
            bars = max(0, bars - 1)

        self.publisher_.publish(CellData(network=self.network, technology=self.tech,
                                         bars=bars, rsrp=int(self.rsrp.value), rsrq=int(self.rsrq.value),
                                         reg_status=self.reg_status, ip_addr=self.ip_addr,
                                         apn=self.apn, pin_status=self.pin_status))

def main(args=None):
    try:
        with rclpy.init(args=None):
            rclpy.spin(CellNode())
    except (KeyboardInterrupt, ExternalShutdownException):
        pass

if __name__ == '__main__':
    main()