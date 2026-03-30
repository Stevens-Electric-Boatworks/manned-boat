import psutil
import rclpy
from rclpy.executors import ExternalShutdownException
from rclpy.node import Node

from boat_data_interfaces.msg import SysUtilData


# pip install psutil
class SysUtilNode(Node):

    def __init__(self):
        super().__init__('sys_util_node')
        self.publisher_ = self.create_publisher(SysUtilData, '/sys_utilization', 10)
        self.timer = self.create_timer(0.5, self.send_sys_util)

    def send_sys_util(self):
        cpu_percent = psutil.cpu_percent(interval=0.5)
        cpu_freq = psutil.cpu_freq().current
        cpu_temp = psutil.sensors_temperatures()
        self._logger.info(str(cpu_temp))
        total_mem = psutil.virtual_memory().total
        current_mem = psutil.virtual_memory().used
        percent_mem = psutil.virtual_memory().percent
        disk_total = psutil.disk_usage("/").total
        disk_used = psutil.disk_usage("/").used
        self.publisher_.publish(
            SysUtilData(cpu_percent=cpu_percent, cpu_freq=cpu_freq, total_mem=total_mem, current_mem=current_mem,
                        percent_mem=percent_mem, disk_total=disk_total, disk_used=disk_used))


def main(args=None):
    try:
        with rclpy.init(args=args):
            rclpy.spin(SysUtilNode())
    except (KeyboardInterrupt, ExternalShutdownException):
        pass


if __name__ == '__main__':
    main()
