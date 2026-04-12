from enum import Enum

import rclpy
from builtin_interfaces.msg import Time
from rclpy.node import Node

from boat_common_libs.alarm_lib.alarms import Alarm
from boat_data_interfaces.msg import BoatAlarm
from boat_data_interfaces.srv import AlarmRaise, AlarmDelatch, MotorAlarmRaise


class AlarmRaiseResult(Enum):
    STICKY_NOT_RAISED_BEFORE = 0
    """
    Means that this alarm is a sticky alarm, but has not been raised yet. 
    As such, it will be published to the shore computer
    """
    STICKY_ALREADY_RAISED = 1
    """
    Means that this alarm is a sticky alarm, but HAS been raised before.
    As such, it will NOT be published to the shore computer
    """

    RAISED = 2
    """
    Means that this is a basic alarm that has been raised.
    As such, it will be published to the shore computer
    """

    UNKNOWN = -1
    """
    The alarm is not known, but is still logged as occuring.
    As such, it will NOT be published to the shore computer
    """


class AlarmDelatchResult(Enum):
    SUCCESS = True
    """
    Means that the alarm was successfully delatched (it was already there)
    """

    NOT_RAISED = False
    """
    Means that the alarm was not delatched because it was not in the list
    """


def _create_srv_client(node, type, name):
    client = node.create_client(type, name)
    connect_attempts = 3
    while not client.wait_for_service(timeout_sec=1.0) and connect_attempts > 0:
        node.get_logger().info(
            f'[ALARM LIB] Alarm Service at "{name}" is not available, waiting again ({connect_attempts} attempts remaining)...')
        connect_attempts -= 1

    return client


class AlarmPublisher:
    def __init__(self, node: Node):
        self._node = node
        self._alarm_motor_raise_pub = _create_srv_client(node, MotorAlarmRaise, "/alarm/raise/motor")
        self._alarm_raise_pub = _create_srv_client(node, AlarmRaise, "/alarm/raise")
        self._alarm_delatch_pub = _create_srv_client(node, AlarmDelatch, "/alarm/delatch")
        self._alarm_motor_delatch_pub = _create_srv_client(node, MotorAlarmRaise, "/alarm/delatch/motor")

    def publish_motor_alarm(self, isMotorA: bool, eventID: int):
        req = MotorAlarmRaise.Request()
        req.event_id = eventID
        req.is_motor_a = isMotorA
        req.timestamp = self._node.get_clock().now().to_msg()

        try:
            self._alarm_motor_raise_pub.call_async(req)
            return None
        except TypeError:
            self._node.get_logger().error(
                "[ALARM LIB] The alarm publisher is using the wrong type for the service! Investigate this issue at once!",
                once=True)
            return None

    def unlatch_motor_alarm(self, isMotorA: bool, eventID: int):
        req = MotorAlarmRaise.Request()
        req.event_id = eventID
        req.is_motor_a = isMotorA
        req.timestamp = self._node.get_clock().now().to_msg()

        try:
            self._alarm_motor_delatch_pub.call_async(req)
            return None
        except TypeError:
            self._node.get_logger().error(
                "[ALARM LIB] The alarm publisher is using the wrong type for the service! Investigate this issue at once!",
                once=True)
            return None

    def publish_alarm(self, alarm: Alarm | int, ignore_result: bool = True) -> AlarmRaiseResult | None:
        """
        Publishes an alarm with the specified Alarm. It will then return the result of the alarm if ignore_result = false
        If ignore_result == true, then the method will return None
        If ignore_result == false, then the method will return AlarmRaiseResult
        """

        # https://docs.ros.org/en/kilted/Tutorials/Beginner-Client-Libraries/Writing-A-Simple-Py-Service-And-Client.html
        req = AlarmRaise.Request()
        alarm_msg = BoatAlarm()
        alarm_msg.timestamp = self._node.get_clock().now().to_msg()
        if isinstance(alarm, Alarm):
            alarm_msg.error_code = alarm.value
        else:
            alarm_msg.error_code = alarm
        req.alarm = alarm_msg

        try:
            if ignore_result:
                self._alarm_raise_pub.call_async(req)
                return None
            else:
                future = self._alarm_raise_pub.call_async(req)
                rclpy.spin_until_future_complete(self._node, future)
                response: AlarmRaise.Response = future.result()
                return AlarmRaiseResult(response.result)
        except TypeError:
            self._node.get_logger().error(
                "[ALARM LIB] The alarm publisher is using the wrong type for the service! Investigate this issue at once!",
                once=True)
            return None

    def delatch_alarm(self, alarm: Alarm, ignore_result: bool = True) -> AlarmDelatchResult | None:
        """
        Delatches an alarm that may be active. Will not throw an error if an alarm is not raised
        """
        req = AlarmDelatch.Request()
        req.error_code = alarm.value

        try:
            if ignore_result:
                self._alarm_delatch_pub.call_async(req)
                return None
            else:
                future = self._alarm_delatch_pub.call_async(req)
                rclpy.spin_until_future_complete(self._node, future)
                response: AlarmDelatch.Response = future.result()
                return AlarmDelatchResult(response.success)
        except TypeError:
            self._node.get_logger().error(
                "[ALARM LIB] The alarm unlatcher is using the wrong type for the service! Investigate this issue at once!",
                once=True)
            return None


def create_alarm_publisher(node: Node) -> AlarmPublisher:
    return AlarmPublisher(node)
