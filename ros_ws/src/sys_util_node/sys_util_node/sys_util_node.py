import time

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
        self.timer = self.create_timer(1, self.send_sys_util)
        self._cache = {}
        self._last_time = None

    def send_sys_util(self):
        cpu_percent = psutil.cpu_percent(interval=0.5)
        cpu_freq = psutil.cpu_freq().current
        cpu_temp = psutil.sensors_temperatures()
        total_mem = psutil.virtual_memory().total
        current_mem = psutil.virtual_memory().used
        percent_mem = psutil.virtual_memory().percent
        disk_total = psutil.disk_usage("/").total
        disk_used = psutil.disk_usage("/").used
        disk_percent = psutil.disk_usage("/").percent
        cpu_temp = psutil.sensors_temperatures()
        if "cpu_thermal" in cpu_temp.keys():
            cpu_temp = float(cpu_temp["cpu_thermal"][0].current)
        else:
            cpu_temp = float(-1)

        net_tx, net_rx = self._get_network_stats()

        self.publisher_.publish(
            SysUtilData(cpu_percent=cpu_percent, cpu_freq=cpu_freq, total_mem=total_mem, current_mem=current_mem,
                        percent_mem=percent_mem, disk_total=disk_total, disk_used=disk_used, rx_mb=net_rx, tx_mb=net_tx, disk_percent=disk_percent, cpu_temp=cpu_temp))

    def _get_network_stats(self) -> tuple:
        total_tx = 0
        total_rx = 0

        now = time.monotonic()
        counters = psutil.net_io_counters(pernic=True)

        cache = self._cache
        last_time = self._last_time

        elapsed = now - last_time if last_time else 1.0

        for iface, stats in counters.items():
            if iface == "lo" or iface.startswith("Loopback"):
                continue
            prev = cache.get(iface)
            if prev and elapsed > 0:
                rx_rate = max(0, (stats.bytes_recv - prev.bytes_recv) / elapsed)
                tx_rate = max(0, (stats.bytes_sent - prev.bytes_sent) / elapsed)
            else:
                rx_rate = tx_rate = 0.0

            total_tx += tx_rate
            total_rx += rx_rate

        self._cache = counters
        self._last_time = now
        return round(total_tx / 1e6, 2), round(total_rx/1e6, 2)


def main(args=None):
    try:
        with rclpy.init(args=args):
            rclpy.spin(SysUtilNode())
    except (KeyboardInterrupt, ExternalShutdownException):
        pass


if __name__ == '__main__':
    main()
