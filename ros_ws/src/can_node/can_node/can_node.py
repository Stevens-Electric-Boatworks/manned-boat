import os

import rclpy
from rcl_interfaces.msg import ParameterDescriptor
from rclpy.executors import ExternalShutdownException
from rclpy.node import Node
from std_srvs.srv import Empty

from boat_common_libs.alarm_lib import alarm_helper
from boat_common_libs.alarm_lib.alarms import Alarm
from boat_data_interfaces.msg import CANMotorData, CANBusStatus, BMSMcuSummary, BMSPackSummary, BMSCellVoltage, \
    BMSSOCSummary, BMSThermistor, CANThermistor
from can_node.canbus import CANBus  # type: ignore


class MotorNode(Node):
    def __init__(self):
        super().__init__('can_node')
        self._logger.warn(
            "This node is using the code that was developed for the 2023 Boat. We should migrate to using ros2_canopen...")
        self.motorA_pub = self.create_publisher(CANMotorData, '/motors/motorA', 10)
        self.motorB_pub = self.create_publisher(CANMotorData, '/motors/motorB', 10)
        self._alarm_publisher = alarm_helper.create_alarm_publisher(self)
        self.can_bus_status_publisher = self.create_publisher(CANBusStatus, '/motors/can_bus_state', 10)
        description = ParameterDescriptor(description='Defines where to find the exact file the dummy epf data is')
        self.declare_parameter('dummy_epf', '~/eboat_src/data/motors.epf', description)
        # bms stuff
        self.bms_mcu_sum_pub = self.create_publisher(BMSMcuSummary, '/bms/mcu_summary', 10)
        self.bms_pack_sum_pub = self.create_publisher(BMSPackSummary, '/bms/pack_summary', 10)
        self.bms_cell_volt_pub = self.create_publisher(BMSCellVoltage, '/bms/cell_voltage', 10)
        self.bms_soc_sum_pub = self.create_publisher(BMSSOCSummary, '/bms/soc_summary', 10)
        self.bms_thermistor_pub = self.create_publisher(BMSThermistor, '/bms/thermistor', 10)

        self.can_thermistor_pub = self.create_publisher(CANThermistor, "/can/cooling_temp", 10)
        self.bms_booster_thermistor_pub = self.create_publisher(CANThermistor, "/can/bms_thermistor", 10)

        file_path = self.get_parameter('dummy_epf').get_parameter_value().string_value
        self.can = CANBus(self._logger, os.path.expanduser(file_path), self.motorA_pub, self.motorB_pub,
                          self.context.ok, self.declare_alarm, self.declare_motor_alarm, rclpy.shutdown,
                          self.unlatch_all_motor_alarms, self.unlatch_motor_alarm, self.unlatch_alarm,
                          bms_pack_sum_pub=self.bms_pack_sum_pub, bms_thermistor_pub=self.bms_thermistor_pub,
                          bms_mcu_sum_pub=self.bms_mcu_sum_pub, bms_soc_sum_pub=self.bms_soc_sum_pub,
                          bms_cell_volt_pub=self.bms_cell_volt_pub, can_thermistor_pub=self.can_thermistor_pub,
                          bms_booster_thermistor_pub=self.bms_booster_thermistor_pub)

        self.create_service(Empty, "/can/restart_bus", self.restart_bus)
        self.create_service(Empty, "/can/flush_bus", self.flush_bus)

        self.alarms = []
        self.unlatchedAlarms = []
        self.motorAlarms = []
        self.unlatchedMotorAlarms = []
        self.create_timer(1, self.publish_bus_state)
        self.create_timer(1, self._publish_alarms)
        self.can.setup_can()

    # noinspection PyUnusedLocal
    def restart_bus(self, req: Empty, res: Empty):
        self._logger.info("Restarting bus via a service call")
        self.can.restart_bus()
        return res

    def flush_bus(self, req: Empty, res: Empty):
        self._logger.info("Flushing bus via a service call")
        self.can.flush_bus()
        return res

    def _publish_alarms(self):
        for alarm in self.alarms:
            self._alarm_publisher.publish_alarm(alarm)
        self.alarms.clear()
        for motorAlarm in self.motorAlarms:
            self._alarm_publisher.publish_motor_alarm(motorAlarm[0], motorAlarm[1])
        self.motorAlarms.clear()

        for alarm in self.unlatchedAlarms:
            self._alarm_publisher.delatch_alarm(alarm)

        self.unlatchedAlarms.clear()

        for motorAlarm in self.unlatchedMotorAlarms:
            self._alarm_publisher.unlatch_motor_alarm(motorAlarm[0], motorAlarm[1])

        self.unlatchedMotorAlarms.clear()

    def publish_bus_state(self):
        msg = CANBusStatus()
        msg.bus_state = self.can.get_bus_state()
        self.can_bus_status_publisher.publish(msg)
        if self.can.get_bus_state == CANBusStatus.ONLINE:
            self.unlatch_all_motor_alarms()

    def declare_motor_alarm(self, is_motor_a, eventId):
        self.motorAlarms.append((is_motor_a, eventId))

    def unlatch_motor_alarm(self, is_motor_a, eventId):
        self.unlatchedMotorAlarms.append((is_motor_a, eventId))

    def unlatch_alarm(self, alarm: Alarm):
        self.unlatchedAlarms.append(alarm)

    def declare_alarm(self, alarm: Alarm):
        self.alarms.append(alarm)

    def unlatch_all_motor_alarms(self):
        self.unlatch_alarm(Alarm.CAN0_INTERFACE_NOT_UP)
        self.unlatch_alarm(Alarm.CAN0_INTERFACE_NOT_UP)
        self.unlatch_alarm(Alarm.ERROR_READING_CAN_SDO)
        self.unlatch_alarm(Alarm.FAILED_CAN_NETWORK_INIT)
        self.unlatch_alarm(Alarm.INVALID_CAN_PACKET_READ)


def main(args=None):
    try:
        with rclpy.init(args=args):
            rclpy.spin(MotorNode())
    except (KeyboardInterrupt, ExternalShutdownException):
        pass


if __name__ == '__main__':
    main()
