from builtin_interfaces.msg import Time
from websockets.exceptions import ConnectionClosed, InvalidStatus
import rclpy
from rclpy.executors import ExternalShutdownException
import rclpy.logging
from rclpy.node import Node

from boat_common_libs.alarm_lib.alarms import Alarm
from boat_data_interfaces.msg import ElectricalData, MotionData, BoatAlarm, \
    CANMotorData, CANBusStatus, GPSData, OutletCoolantData, InletCoolantData, GPSVTGData, CellData, ShoreBoatAlarm, \
    SysUtilData, BMSCellVoltage, BMSPackSummary, BMSSOCSummary, BMSMcuSummary, GPSSVData, GPGSAData, CANThermistor
from rcl_interfaces.msg import Log, ParameterDescriptor, SetParametersResult
from boat_common_libs.alarm_lib import alarm_helper

# Websockets
import asyncio
from websockets.client import connect
from websockets.client import WebSocketClientProtocol
import json
import threading
from rosidl_runtime_py import message_to_ordereddict

SHORE_URI = "wss://shore.stevenseboat.org/api"


# SHORE_URI = "ws://localhost:5001/api"

def get_time_in_ms(time: Time):
    return time.sec * 1000 + (time.nanosec / 1e+6)


class ShoreDataCollector(Node):

    def __init__(self):
        super().__init__('shore_comms')
        self.alarms = []
        self.create_sub(ShoreBoatAlarm, "/alarm/shore/publish", self.alarms_collector)

        self.declare_parameter("data_send", 0.1, ParameterDescriptor(
            description='How often shore_comms should send data to the shore server.'))

        self.declare_parameter("replay_mode", False,
                               ParameterDescriptor(description='Is the shore node trying to replay data?'))

        self.websocket: WebSocketClientProtocol = None
        self.data = {}
        self.logs = []
        self.can_bus_state = CANBusStatus.OFFLINE

        self.alarm_publisher = alarm_helper.create_alarm_publisher(self)

        ros_out_topic = "/rosout"
        self.isReplay = self.get_parameter("replay_mode").get_parameter_value().bool_value
        if self.isReplay:
            self._logger.info("The SHORE node is in replay mode. Now replaying log files from /logout")
            ros_out_topic = "/logout"
            logged_data = {
                "timestamp": get_time_in_ms(self._clock.now().to_msg()),
                "msg": "The shore node is in REPLAY MODE",
                "file": "REPLAY MODE",
                "function": "REPLAY MODE",
                "line": 67,
                "level": 40,
                "name": "REPLAY MODE"
            }
            self.logs.append(logged_data)

        self.create_sub(Log, ros_out_topic, self.logs_collector)
        self.create_sub(InletCoolantData, "/electrical/temp_sensors/in", self.electrical_coolant_temp_collector_inlet)
        self.create_sub(OutletCoolantData, "/electrical/temp_sensors/out",
                        self.electrical_coolant_temp_collector_outlet)
        self.create_sub(GPSData, "/motion/gps", self.gps_location_collector)
        self.create_sub(GPSVTGData, "/motion/vtg", self.gps_speed_collector)
        self.create_sub(GPSSVData, "/motion/sv", self.gps_sats_collector)
        self.create_sub(GPGSAData, "/motion/gsa", self.gps_sat_mode_collector)
        self.create_sub(CellData, "/cell", self.cell_data_collector)
        self.create_sub(CANMotorData, "/motors/motorA", self.motorA_collector)
        self.create_sub(CANMotorData, "/motors/motorB", self.motorB_collector)
        self.create_sub(CANBusStatus, "/motors/can_bus_state", self.bus_state_collector)

        self.create_sub(BMSCellVoltage, "/bms/cell_voltage", self.bms_cell_voltage)
        self.create_sub(BMSPackSummary, "/bms/pack_summary", self.bms_pack_summary)
        self.create_sub(BMSSOCSummary, "/bms/soc_summary", self.bms_soc_summary)
        self.create_sub(BMSMcuSummary, "/bms/mcu_summary", self.bms_mcu_summary)

        self.create_sub(CANThermistor, "/can/cooling_temp", self.can_cooling_temp_collector)

        self.create_sub(Time, "/boat_time", self.time_collector)
        self.create_sub(SysUtilData, "/sys_utilization", self.sys_util_collector)
        self.wss_watchdog = self.create_timer(5, self.watchdog_callback)
        self.add_on_set_parameters_callback(self.on_param_change_callback)
        threading.Thread(target=self._run_asyncio_loop, daemon=True).start()

    def on_param_change_callback(self, param_list):
        self._logger.info("Data send rate was changed to " + str(param_list[0].value) + "s via a parameter callback")
        return SetParametersResult(successful=True)

    def create_sub(self, data_type, topic, callback):
        self._logger.info("Logging <" + topic + "> with custom msg of <" + data_type.__name__ + ">")
        self.create_subscription(data_type, topic, callback, 10)

    def _run_asyncio_loop(self):
        asyncio.run(self.start_background_shore_sender())

    def add_data(self, data_name, data):
        """
        Adds data to be sent to the shore server.
         param data_name - The name of the data as required by the ShoreAPI
         param data - The actual data to send
        """
        self.data[data_name] = data

    def add_alarm(self, error_code: int, timestamp, msg: str):
        """
        Queues an alarm to be sent to the shore server.
         param error_code - The error code based on the spreadsheet
         param timestamp - The timestamp of when the alarm was issued
        """
        self.alarms.append((error_code, timestamp, msg))

    def clear_all_websocket_alarms(self):
        self.alarm_publisher.delatch_alarm(Alarm.WEBSOCKET_CONNECTION_CLOSED)
        self.alarm_publisher.delatch_alarm(Alarm.WEBSOCKET_INITIAL_CONNECTION_FAILURE)
        self.alarm_publisher.delatch_alarm(Alarm.WEBSOCKET_IS_NOT_INITIALLY_OPENED_YET)
        self.alarm_publisher.delatch_alarm(Alarm.WEBSOCKET_NOT_OPENED)

    async def start_background_shore_sender(self):
        """
        Starts the background task to send the data to the shore server. Is automatically called every DATA_SEND ms
        """
        self._logger.info(f"Attempting to connect to the Shore Server via a Websocket at {SHORE_URI}")
        async for self.websocket in connect(SHORE_URI):
            try:
                if not self.websocket.open:
                    self._logger.error("Unable to open a connect to the shore server.")
                    self._logger.error(f"Attempted URI: {SHORE_URI}. SHUTTING DOWN...")
                    self.alarm_publisher.publish_alarm(Alarm.WEBSOCKET_INITIAL_CONNECTION_FAILURE)  # ALARM:
                    # Shore Comms Node Shutdown
                    self.destroy_node()
                    return
                await self.send_initial_ident()
                await self.send_data_to_shore(False)
                self._logger.info(f"Connected to the websocket at {SHORE_URI} ✅")
                self._logger.info(f"Data will be sent every {self.get_parameter("data_send").value}s")
                self.clear_all_websocket_alarms()
                while True:
                    await self.send_data_to_shore(True)
                    await self.send_alarms_to_shore(True)
                    await self.send_logs_to_shore()
                    await self.send_bus_state_to_shore()
                    await asyncio.sleep(self.get_parameter("data_send").value)

            except ConnectionClosed as e:
                # Will retry on some kind of failure
                self._logger.error(f"Websocket error: {e.reason}")
                self.alarm_publisher.publish_alarm(Alarm.WEBSOCKET_CONNECTION_CLOSED)
                continue

    def watchdog_callback(self):
        self._logger.debug("[Websocket Watchdog] running callback")
        if not hasattr(self, "websocket") or self.websocket is None:
            self._logger.warn("[Websocket Watchdog] Websocket is not opened yet...")
            self.alarm_publisher.publish_alarm(Alarm.WEBSOCKET_IS_NOT_INITIALLY_OPENED_YET)  # ALARM:
            # Shore Comms Websocket failure

        elif not self.websocket.open:
            self._logger.error("[Websocket Watchdog] The node is not connected to the shore server via the websocket.")
            self.alarm_publisher.publish_alarm(Alarm.WEBSOCKET_NOT_OPENED)  # ALARM: Shore Comms Websocket failure

    async def send_initial_ident(self):
        output_data = {
            "type": "ident",
            "message": "boat"
        }
        await self.websocket.send(json.dumps(output_data))

    async def send_data_to_shore(self, ignore_empty):
        if len(self.data) == 0 and ignore_empty:
            return
        output_data = {
            "type": "data",
            "payload": self.data
        }
        if self.isReplay:
            output_data["replay"] = True
        await self.websocket.send(json.dumps(output_data))
        self.data.clear()

    async def send_alarms_to_shore(self, ignore_empty):
        if len(self.alarms) == 0 and ignore_empty:
            return
        # go through all alarms in the queue

        for alarm in self.alarms:
            output_data = {
                "type": "alarm",
                "action": "set",
                "payload": {
                    "id": alarm[0],
                    "timestamp": alarm[1],
                    "message": alarm[2],
                    "type": "error"
                }
            }
            try:
                await self.websocket.send(json.dumps(output_data))
                await self.websocket.ensure_open()
                self.clear_all_websocket_alarms()
            except ConnectionClosed or InvalidStatus:
                self.alarm_publisher.publish_alarm(Alarm.WEBSOCKET_CONNECTION_CLOSED)
                # Keep alarms because data wasn’t sent
                return

        self.alarms.clear()

    async def send_logs_to_shore(self):
        if len(self.logs) == 0:
            return

        output_data = {
            "type": "log",
            "payload": self.logs
        }
        try:
            await self.websocket.send(json.dumps(output_data))
            await self.websocket.ensure_open()
            self.clear_all_websocket_alarms()
        except ConnectionClosed or InvalidStatus:
            self.alarm_publisher.publish_alarm(Alarm.WEBSOCKET_CONNECTION_CLOSED)
            # Keep logs because data wasn’t sent
            return

        self.logs.clear()

    async def send_bus_state_to_shore(self):
        output_data = {
            "type": "can_bus",
            "state": self.can_bus_state
        }
        await self.websocket.send(json.dumps(output_data))

        # IMPORTANT: Parameter name MUST be "msg"

    def electrical_coolant_temp_collector_inlet(self, msg: InletCoolantData):
        self.add_data("inlet_temp", msg.inlet_temp)

    def electrical_coolant_temp_collector_outlet(self, msg: OutletCoolantData):
        self.add_data("outlet_temp", msg.outlet_temp)

    def gps_location_collector(self, msg: GPSData):
        self.add_data("lat", msg.lat)
        self.add_data("long", msg.lon)

    def gps_speed_collector(self, msg: GPSVTGData):
        self.add_data("speed", msg.speed)
        self.add_data("heading", msg.true_track)

    def gps_sats_collector(self, msg: GPSSVData):
        self.add_data("sats", [message_to_ordereddict(sat) for sat in msg.sats])

    def gps_sat_mode_collector(self, msg: GPGSAData):
        self.add_data("sat_mode", message_to_ordereddict(msg))

    def cell_data_collector(self, msg: CellData):
        self.add_data("cell", message_to_ordereddict(msg))

    def motorA_collector(self, msg: CANMotorData):
        self.add_data("motor_a.voltage", msg.voltage)
        self.add_data("motors.throttle", msg.throttle_percentage)
        self.add_data("motor_a.rpm", msg.rpm)
        self.add_data("motors.rpm", msg.rpm)
        self.add_data("motor_a.torque", msg.torque)
        self.add_data("motor_a.temp", msg.motor_temp)
        self.add_data("motor_a.current", msg.current)
        self.add_data("motor_a.power", msg.power)
        self.add_data("motor_a.enabled", msg.enabled)

    def motorB_collector(self, msg: CANMotorData):
        self.add_data("motor_b.voltage", msg.voltage)
        self.add_data("motor_b.throttle_mv", msg.throttle_mv)
        self.add_data("motor_b.rpm", msg.rpm)
        self.add_data("motor_b.torque", msg.torque)
        self.add_data("motor_b.temp", msg.motor_temp)
        self.add_data("motor_b.current", msg.current)
        self.add_data("motor_b.power", msg.power)
        self.add_data("motor_b.enabled", msg.enabled)

    def bms_cell_voltage(self, msg: BMSCellVoltage):
        self.add_data("bms.cell_voltage_high", msg.high)
        self.add_data("bms.cell_voltage_low", msg.low)
        self.add_data("bms.cell_voltage_mean", msg.mean)

    def bms_pack_summary(self, msg: BMSPackSummary):
        self.add_data("bms.pack_voltage_raw", msg.pack_voltage_raw)
        self.add_data("bms.pack_current_raw", msg.pack_current_raw)

    def bms_soc_summary(self, msg: BMSSOCSummary):
        self.add_data("bms.soc_percent", msg.soc_percent)
        self.add_data("bms.pack_kwhr", msg.pack_kwhr)
        self.add_data("bms.max_kwhr", msg.max_kwhr)

    def bms_mcu_summary(self, msg: BMSMcuSummary):
        self.add_data("bms.charge_state", msg.charge_state)
        self.add_data("bms.alerts", msg.alerts)
        self.add_data("bms.plug_state", msg.plug_state)

    def can_cooling_temp_collector(self, msg: CANThermistor):
        self.add_data("cooling_temp", msg.temp)

    def bus_state_collector(self, msg: CANBusStatus):
        self.can_bus_state = msg.bus_state

    def time_collector(self, msg: Time):
        self.add_data("boat_time", get_time_in_ms(msg))

    def sys_util_collector(self, msg: SysUtilData):
        self.add_data("rpi.cpu.currentLoad", msg.cpu_percent)
        self.add_data("rpi.cpu.speed", msg.cpu_freq / 1000)
        self.add_data("rpi.memory.total", msg.total_mem * 1000000)
        self.add_data("rpi.memory.used", msg.current_mem * 1000000)
        self.add_data("rpi.memory.percent", msg.percent_mem)
        self.add_data("rpi.disk.total", msg.disk_total * 1000000)
        self.add_data("rpi.disk.used", msg.disk_used * 1000000)
        self.add_data("rpi.net.tx_mb", msg.tx_mb)
        self.add_data("rpi.net.rx_mb", msg.rx_mb)

    def alarms_collector(self, msg: ShoreBoatAlarm):
        self.add_alarm(msg.error_code, get_time_in_ms(msg.timestamp), msg.message)

    def logs_collector(self, msg: Log):
        logged_data = {
            "timestamp": get_time_in_ms(msg.stamp),
            "msg": msg.msg,
            "file": msg.file,
            "function": msg.function,
            "line": msg.line,
            "level": msg.level,
            "name": msg.name
        }
        self.logs.append(logged_data)


def main(args=None):
    try:
        with rclpy.init(args=args):
            minimal_subscriber = ShoreDataCollector()
            rclpy.spin(minimal_subscriber)
    except (KeyboardInterrupt, ExternalShutdownException):
        pass


if __name__ == '__main__':
    main()
