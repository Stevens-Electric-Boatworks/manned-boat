import random

import rclpy
from rclpy.node import Node
from rclpy.executors import ExternalShutdownException

from boat_common_libs.smooth_random import SmoothRandom
from boat_data_interfaces.msg import CANMotorData, CANBusStatus, BMSCellVoltage, BMSPackSummary, BMSSOCSummary, \
    BMSMcuSummary, BMSThermistor, CANThermistor


class CANMotorTestingDataNode(Node):
    def __init__(self):
        super().__init__("motor_node_testing_data")
        self.motorA = {
            "voltage": SmoothRandom(47.5, 2.0, 0, 57),  # int8,
            "throttle_mv": SmoothRandom(0, 20, 0, 5000),  # int16, mV
            "throttle_percentage": SmoothRandom(0, 3.0, 0, 100),  # int8, %
            "rpm": SmoothRandom(0, 400, -1200, 1800),  # int16, up to ~12k RPM
            "torque": SmoothRandom(0, 2.0, 0, 500),  # int16, Nm (scaled higher)
            "motor_temp": SmoothRandom(40, 0.5, 20, 150),  # int8, °C (idle warm to overheated)
            "current": SmoothRandom(0, 5.0, 0, 300),  # int8, A
            "power": SmoothRandom(0, 60, 0, 6000),  # int16, up to ~6 kW
            "enabled": True,
            "current_limited": False,
            "current_limit_reason": 16
        }
        self.motorB = {
            "voltage": SmoothRandom(47.5, 1.0, 0, 200),  # int8, ~200 V system
            "throttle_mv": SmoothRandom(0, 20, 0, 5000),  # int16, mV
            "throttle_percentage": SmoothRandom(0, 3.0, 0, 100),  # int8, %
            "rpm": SmoothRandom(0, 400, -1200, 1800),  # int16, up to ~12k RPM
            "torque": SmoothRandom(0, 2.0, 0, 500),  # int16, Nm (scaled higher)
            "motor_temp": SmoothRandom(40, 0.5, 20, 150),  # int8, °C (idle warm to overheated)
            "current": SmoothRandom(0, 5.0, 0, 300),  # int8, A
            "power": SmoothRandom(0, 60, 0, 6000),  # int16, up to ~6 kW
            "enabled": True,
            "current_limited": False,
            "current_limit_reason": 16
        }
        self.cooling_temp = SmoothRandom(19.5, 0.5, 0, 50)
        self.can_motor_a_pub = self.create_publisher(CANMotorData, '/motors/motorA', 10)
        self.can_motor_b_pub = self.create_publisher(CANMotorData, '/motors/motorB', 10)
        self.can_bus_status_publisher = self.create_publisher(CANBusStatus, '/motors/can_bus_state', 10)
        self.cooling_temp_pub = self.create_publisher(CANThermistor, "/can/cooling_temp", 10)
        self.bms_booster_thermistor_pub = self.create_publisher(CANThermistor, "/can/bms_thermistor", 10)

        # raw = mV / 40  →  3300mV=82, 3700mV=92, 4200mV=105  (fits int8)
        self._cell_voltage = {
            "low": SmoothRandom(90, 1, 82, 105),  # int8, raw = mV/40
            "mean": SmoothRandom(92, 1, 82, 105),
            "high": SmoothRandom(94, 1, 82, 105),
        }

        # --- Pack Summary ---
        # pack_voltage_raw: raw = V directly  →  46–59V fits int8
        # pack_current_raw: raw = A / 3       →  0–300A maps to 0–100  (fits int8)
        self._pack_summary = {
            "pack_voltage_raw": SmoothRandom(52, 1, 46, 59),  # int8, raw = V
            "pack_current_raw": SmoothRandom(20, 5, 0, 100),  # int8, raw = A/3
        }

        # --- SOC Summary ---
        # 52V pack at ~300A peak → typical capacity ~1–3 kWh; using 1500 Wh here
        self._soc_summary = {
            "soc_percent": SmoothRandom(75.0, 0.5, 0.0, 100.0),  # float32, %
            "pack_kwhr": SmoothRandom(1125, 10, 0, 1500),  # int32, Wh (75% of 1500)
            "max_kwhr": 1500,  # int32, fixed nominal capacity (Wh)
        }

        # --- MCU Summary (int8 charge_state, plug_state, alerts) ---
        self._mcu_summary = {
            "charge_state": SmoothRandom(1, 0.05, 0, 3),  # int8, enum-like state
            "plug_state": SmoothRandom(1, 0.02, 0, 1),  # int8, 0=unplugged 1=plugged
            "alerts": SmoothRandom(0, 0.01, 0, 15),  # int8, bitmask
        }

        # --- Thermistor ---
        # 52V/300A system runs warmer under load; cells idle ~25°C, hot ~55°C
        self._thermistor = {
            "min": SmoothRandom(30, 0.5, 15, 55),  # int8, °C
            "max": SmoothRandom(38, 0.5, 15, 65),  # int8, °C
        }

        # Publishers
        self._cell_voltage_pub = self.create_publisher(BMSCellVoltage, "/bms/cell_voltage", 10)
        self._pack_summary_pub = self.create_publisher(BMSPackSummary, "/bms/pack_summary", 10)
        self._soc_summary_pub = self.create_publisher(BMSSOCSummary, "/bms/soc_summary", 10)
        self._mcu_summary_pub = self.create_publisher(BMSMcuSummary, "/bms/mcu_summary", 10)
        self._thermistor_pub = self.create_publisher(BMSThermistor, "/bms/thermistor", 10)

        # Timers at different intervals
        self.create_timer(1.0, self.publish_can_thermistor)
        self.create_timer(0.1, self._publish_cell_voltage)
        self.create_timer(0.1, self._publish_pack_summary)
        self.create_timer(0.5, self._publish_soc_summary)
        self.create_timer(0.3, self._publish_mcu_summary)
        self.create_timer(1.0, self._publish_thermistor)

        self.create_timer(0.02, self.publish_test_data)
        self.create_timer(1, self.publish_bus_state)

    def publish_can_thermistor(self):
        self.cooling_temp_pub.publish(CANThermistor(temp=float(self.cooling_temp.next())))
        self.bms_booster_thermistor_pub.publish(CANThermistor(temp=float(self.cooling_temp.next())))

    def publish_test_data(self):
        motor_a_msg = CANMotorData()
        motor_a_msg.voltage = float(self.motorA["voltage"].next())
        motor_a_msg.throttle_mv = int(self.motorA["throttle_mv"].next())
        motor_a_msg.throttle_percentage = int(self.motorA["throttle_percentage"].next())
        motor_a_msg.rpm = int(self.motorA["rpm"].next())
        motor_a_msg.torque = float(self.motorA["torque"].next())
        motor_a_msg.motor_temp = float(self.motorA["motor_temp"].next())
        motor_a_msg.current = float(self.motorA["current"].next())
        motor_a_msg.power = float(self.motorA["power"].next())
        motor_a_msg.enabled = True
        motor_a_msg.current_limited = True
        motor_a_msg.current_limit_reason = 10

        motor_b_msg = CANMotorData()
        motor_b_msg.voltage = float(self.motorB["voltage"].next())
        motor_b_msg.throttle_mv = int(self.motorB["throttle_mv"].next())
        motor_b_msg.throttle_percentage = int(self.motorB["throttle_percentage"].next())
        motor_b_msg.rpm = int(self.motorB["rpm"].next())
        motor_b_msg.torque = float(self.motorB["torque"].next())
        motor_b_msg.motor_temp = float(self.motorB["motor_temp"].next())
        motor_b_msg.current = float(self.motorB["current"].next())
        motor_b_msg.power = float(self.motorB["power"].next())
        motor_b_msg.enabled = True
        motor_b_msg.current_limited = False
        motor_b_msg.current_limit_reason = 12


        self.can_motor_a_pub.publish(motor_a_msg)
        self.can_motor_b_pub.publish(motor_b_msg)

    def publish_bus_state(self):
        msg = CANBusStatus()
        msg.bus_state = CANBusStatus.TESTING
        self.can_bus_status_publisher.publish(msg)

    def _publish_cell_voltage(self):
        msg = BMSCellVoltage()
        msg.low = int(self._cell_voltage["low"].next())
        msg.mean = int(self._cell_voltage["mean"].next())
        msg.high = int(self._cell_voltage["high"].next())
        self._cell_voltage_pub.publish(msg)

    def _publish_pack_summary(self):
        msg = BMSPackSummary()
        msg.pack_voltage_raw = float(self._pack_summary["pack_voltage_raw"].next())
        msg.pack_current_raw = float(self._pack_summary["pack_current_raw"].next())
        self._pack_summary_pub.publish(msg)

    def _publish_soc_summary(self):
        msg = BMSSOCSummary()
        msg.soc_percent = float(self._soc_summary["soc_percent"].next())
        msg.pack_kwhr = int(self._soc_summary["pack_kwhr"].next())
        msg.max_kwhr = int(self._soc_summary["max_kwhr"])
        self._soc_summary_pub.publish(msg)

    def _publish_mcu_summary(self):
        msg = BMSMcuSummary()
        msg.charge_state = int(self._mcu_summary["charge_state"].next()) & 0x7F
        msg.plug_state = int(self._mcu_summary["plug_state"].next()) & 0x7F
        msg.alerts = int(self._mcu_summary["alerts"].next()) & 0x7F
        self._mcu_summary_pub.publish(msg)

    def _publish_thermistor(self):
        msg = BMSThermistor()
        msg.min = int(self._thermistor["min"].next())
        msg.max = int(self._thermistor["max"].next())
        self._thermistor_pub.publish(msg)


def main(args=None):
    try:
        with rclpy.init(args=args):
            rclpy.spin(CANMotorTestingDataNode())
    except (KeyboardInterrupt, ExternalShutdownException):
        pass


if __name__ == '__main__':
    main()
