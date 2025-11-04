from datetime import datetime
from zoneinfo import ZoneInfo

import rclpy
from builtin_interfaces.msg import Time
from rclpy.executors import ExternalShutdownException
from rclpy.node import Node
class TimeNode(Node):
    def __init__(self):
        super().__init__("time_node")
        self.create_timer(0.1, self.send_time)
        self.pub = self.create_publisher(Time, "/boat_time", 10)
        ts_str = datetime.fromtimestamp(self._clock.now().seconds_nanoseconds()[0], tz=ZoneInfo("America/New_York")).strftime(
            "%m/%d/%y | %I:%M:%S %p")

        self._logger.info(f"The current time is {ts_str}")


    def send_time(self):
        self.pub.publish(self._clock.now().to_msg())


def main(args=None):
    try:
        with rclpy.init(args=args):
            rclpy.spin(TimeNode())
    except (KeyboardInterrupt, ExternalShutdownException):
        pass


if __name__ == '__main__':
    main()
