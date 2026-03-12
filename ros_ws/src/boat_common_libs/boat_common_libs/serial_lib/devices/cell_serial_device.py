from typing import Callable

from rclpy import Node

from boat_common_libs.alarm_lib.alarm_helper import AlarmPublisher
from boat_common_libs.serial_lib.serial_device import SerialDevice, SerialData


class CellData:
    def __init__(self, data1, data2):
        self.data1 = data1
        self.data2 = data2


class CellSerialDevice(SerialDevice):
    def __init__(self, node: Node, alarm_pub: AlarmPublisher, on_cell_data: Callable[[CellData], None]):
        super().__init__(node, "/dev/ttyUSB2", self._on_cell_data_rec, alarm_pub)
        self.node = node
        self.on_cell_data = on_cell_data
        self.configure()


    def configure(self):
        # send data
        self.send_string("...")
        pass

    def update_cell_data(self):
        # send command to get data
        self.send_string("...")
        pass

    def _on_cell_data_rec(self, data: SerialData):
        # whenever we receive cell data
        data_str = data.to_utf_8()

        #process cell data

        self.on_cell_data(CellData(1, 2))

