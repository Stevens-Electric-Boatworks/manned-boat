import threading
from time import sleep

import rclpy
from std_srvs.srv import Empty

from boat_common_libs.alarm_lib.alarm_helper import AlarmPublisher
from boat_common_libs.serial_lib.devices.cell_serial_device import (
    CellData,
    CellSerialDevice,
)
from rclpy.executors import ExternalShutdownException
from rclpy.node import Node

from boat_data_interfaces.msg import CellData as CellDataMsg


class CellNode(Node):
    def __init__(self):
        super().__init__("cell_node")
        self.publisher_ = self.create_publisher(CellDataMsg, "/cell", 10)
        self.alarm_pub = AlarmPublisher(self)
        self.cell = CellSerialDevice(self, self.alarm_pub, self._on_cell_data_rec)
        self.timer = threading.Thread(target=self.timer_callback).start()
        self.reconfig_srv = self.create_service(Empty, "/cell/reconfigure", self.on_reconfigure)

    def on_reconfigure(self, _, __):
        self._logger.info("Reconfiguring cell node from service call")
        self.cell.configure_then_signal()
        return __

    def _on_cell_data_rec(self, data: CellData):
        # publish
        self.publisher_.publish(
            CellDataMsg(
                network=data.network,
                technology=data.tech,
                bars=data.bars,
                rsrp=data.rsrp,
                rsrq=data.rsrq,
                reg_status=data.reg_status,
                ip_addr=data.ip_addr,
                apn=data.apn,
                pin_status=data.pin_status,
            )
        )

    def timer_callback(self):
        while True:
            self.cell.update_cell_data()
            sleep(5)


def main(args=None):
    try:
        with rclpy.init(args=args):
            minimal_publisher = CellNode()

            rclpy.spin(minimal_publisher)
    except (KeyboardInterrupt, ExternalShutdownException):
        pass


if __name__ == "__main__":
    main()
