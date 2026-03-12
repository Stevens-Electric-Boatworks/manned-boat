import rclpy
from rclpy.executors import ExternalShutdownException
from rclpy.node import Node

from std_msgs.msg import String

from boat_common_libs.alarm_lib.alarm_helper import AlarmPublisher
from boat_common_libs.serial_lib.devices.cell_serial_device import CellSerialDevice, CellData


class CellNode(Node):

    def __init__(self):
        super().__init__('cell_node')
                                                #TODO: change message type!
        self.publisher_ = self.create_publisher(String, '/cell', 10)
        self.alarm_pub = AlarmPublisher(self)
        self.cell = CellSerialDevice(self, self.alarm_pub, self._on_cell_data_rec)
        self.timer = self.create_timer(5, self.timer_callback)


    def _on_cell_data_rec(self, data: CellData):

        # publish
        self.publisher_.publish("f")

    def timer_callback(self):
        self.cell.update_cell_data()



def main(args=None):
    try:
        with rclpy.init(args=args):
            minimal_publisher = CellNode()

            rclpy.spin(minimal_publisher)
    except (KeyboardInterrupt, ExternalShutdownException):
        pass


if __name__ == '__main__':
    main()