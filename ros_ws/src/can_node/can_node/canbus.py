import math
import subprocess
import time
import traceback
from sys import exec_prefix
from threading import Thread

import can
import canopen
from can import CanError
from canopen import BaseNode402, SdoCommunicationError, SdoAbortedError
from rclpy.impl.rcutils_logger import RcutilsLogger
from rclpy.publisher import Publisher

from boat_common_libs.alarm_lib.alarms import Alarm
from boat_data_interfaces.msg import CANMotorData, CANBusStatus, BMSSOCSummary, BMSThermistor, BMSCellVoltage, \
    BMSPackSummary, BMSMcuSummary


def is_can_interface_up(interface: str = "can0") -> bool:
    try:
        result = subprocess.run(
            ["ip", "link", "show", interface],
            capture_output=True,
            text=True,
            check=True
        )
        # Example output line: "3: can0: <NOARP,UP,LOWER_UP> mtu 16 ..."
        return "state UP" in result.stdout
    except subprocess.CalledProcessError:
        return False


class CANBus:
    def __init__(self, logger: RcutilsLogger, dummy_efp, motorA_pub, motorB_pub, is_node_ok, declare_alarm,
                 shutdown_node,
                 unlatch_all_alarms, bms_pack_sum_pub, bms_mcu_sum_pub, bms_cell_volt_pub, bms_thermistor_pub,
                 bms_soc_sum_pub):
        self.network = None
        self.sdo = None
        self.start_time = None
        self.can_thread = None
        self.motorA = None
        self.motorB = None
        self.logger = logger
        self.dummy_efp = dummy_efp
        self.motorA_pub = motorA_pub
        self.motorB_pub = motorB_pub
        self.is_node_ok = is_node_ok
        self.declare_alarm = declare_alarm
        self.shutdown_node = shutdown_node
        self.can_bus_state = CANBusStatus.OFFLINE
        self.unlatch_all_alarms = unlatch_all_alarms

        self.bms_pack_sum_pub: Publisher = bms_pack_sum_pub
        self.bms_mcu_sum_pub: Publisher = bms_mcu_sum_pub
        self.bms_cell_volt_pub: Publisher = bms_cell_volt_pub
        self.bms_soc_sum_pub: Publisher = bms_soc_sum_pub
        self.bms_thermistor_pub: Publisher = bms_thermistor_pub

    def setup_can(self):
        # This is to ensure that we can publish alarms
        time.sleep(1.5)
        self.logger.info("Setting up the old can...")
        self.logger.warning("The throttle values and motor temperature are not real.")

        # test for can0 open
        if not is_can_interface_up():
            self.logger.error("can0 is not up. Aborting startup!!")
            self.declare_alarm(Alarm.CAN0_INTERFACE_NOT_UP)
            self.can_bus_state = CANBusStatus.OFFLINE
            return

        # Start with creating a new network representing one CAN bus
        self.network = canopen.Network()

        # Connect to the CAN bus
        try:
            self.network.connect(channel='can0', bustype='socketcan', bitrate=500_000)
        except CanError:
            self.logger.error(
                f"""Unable to connect to the CAN bus because of the following error: {traceback.format_exc()}""")
            self.declare_alarm(Alarm.INVALID_CAN_PACKET_READ)
            self.can_bus_state = CANBusStatus.OFFLINE
            return

        self.logger.info("Connected to SocketCAN")
        # Subscribe to messages
        self.network.subscribe(1 , self.on_msg_receive)
        self.logger.info("Using a dummy EDS file at \"" + self.dummy_efp + "\".")
        self.motorA = canopen.BaseNode402(7, canopen.import_od(self.dummy_efp))  # Use a dummy EDS here
        self.motorB = canopen.BaseNode402(6, canopen.import_od(self.dummy_efp))  # Use a dummy EDS here
        self.network.add_node(self.motorA)
        self.network.add_node(self.motorB)

        self.can_thread = Thread(
            target=self.read_can_messages, args=[self.motorA_pub, self.motorB_pub], daemon=True)
        self.can_thread.start()

        self.bms_thread = Thread(
            target=self.bms_thread_callback, args=[], daemon=True)
        self.bms_thread.start()

    def request_bms_status_data(self):
        """
        Send PDO2 MOSI (SID 0x313) to ask the MCU to start
        sending PDO2 MISO status updates (SID 0x293).
        B0 selects which data: 0x01=MCU Summary, 0x02=Pack, 0x03=Cell V, etc.
        """

        def send(e):
            request = can.Message(
                arbitration_id=0x313,
                data=e,
                is_extended_id=False
            )
            self.network.bus.send(request)
            time.sleep(0.01)

        send([0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01])

    def query_bms(self):
        self.request_bms_status_data()
        msg = self.network.bus.recv(timeout=0.5)
        if msg is None:
            return
        sid = msg.arbitration_id
        data = msg.data

        if sid == 0x293:  # PDO2 MISO — Status Data Reply
            b0 = data[0]  # Message type identifier

            if b0 == 0x01:  # MCU Summary
                charge_state = data[4]
                plug_state = data[5]
                alerts = data[6]

                self.bms_mcu_sum_pub.publish(
                    BMSMcuSummary(charge_state=charge_state, plug_state=plug_state, alerts=alerts))
                print(f"MCU Summary — ChargeState={charge_state}, PlugState={plug_state}, Alerts={alerts}")

            elif b0 == 0x02:  # Pack Summary
                print(str(data))
                pack_voltage_raw = int.from_bytes(data[2:4], 'little', signed=False)
                pack_current_raw = int.from_bytes(data[4:6], 'little', signed=False)
                # Scale factors depend on your firmware config; check the doc for your version
                self.bms_pack_sum_pub.publish(
                    BMSPackSummary(pack_voltage_raw=pack_voltage_raw, pack_current_raw=pack_current_raw))
                print(f"Pack Summary — Voltage bytes={data[2:4].hex()}, Current bytes={data[4:6].hex()}")

            elif b0 == 0x03:  # Cell Voltage Summary
                cv_low = int.from_bytes(data[2:4], 'little')
                cv_mean = int.from_bytes(data[4:6], 'little')
                cv_hi = int.from_bytes(data[6:8], 'little')
                self.bms_cell_volt_pub.publish(BMSCellVoltage(low=cv_low, mean=cv_mean, high=cv_hi))
                print(f"Cell Voltage — Low={cv_low}, Mean={cv_mean}, High={cv_hi}")

            elif b0 == 0x04:  # Thermistor Summary
                th_min = data[1]
                th_max = data[2]
                self.bms_thermistor_pub.publish(BMSThermistor(min=th_min, max=th_max))
                print(f"Thermistor — Min={th_min}, Max={th_max}")

            elif b0 == 0x05:  # SOC Summary
                soc = data[1]
                pack_kwhr = int.from_bytes(data[2:4], 'little')
                pack_max_kwhr = int.from_bytes(data[6:8], 'little')
                self.bms_soc_sum_pub.publish(BMSSOCSummary(pack_kwhr=pack_kwhr, pack_max_kwhr=pack_max_kwhr))
                print(f"SOC Summary — SOC={soc}%, PackKWHr={pack_kwhr}, MaxKWHr={pack_max_kwhr}")

        elif sid == 0x193:  # PDO1 MISO — Configuration Data Reply
            print(f"Config reply: {data.hex()}")

        elif sid == 0xbe:
            number = int.from_bytes(data[1:2], 'little')
            print(f"Number: {number}")

        # if msg and msg.arbitration_id == 0x293:
        #     print(f"Status [{msg.data[0]:#04x}]: {msg.data.hex()}")

        # This read runs in the background to query data from the BMS

    def bms_thread_callback(self):
        while True:
            self.query_bms()

    def read_can_messages(self, motorA_pub, motorB_pub):
        while True:
            try:
                if not self.is_node_ok:
                    self.logger.info("Safely shutting down the CAN Motor reader thread")
                    return

                self.publish_sdo_data(self.motorA, motorA_pub)
                self.publish_sdo_data(self.motorB, motorB_pub)
                time.sleep(0.5)
            except Exception as e:
                time.sleep(0.8)
                self.logger.error(f"Error reading CAN message: {e}")
                self.network.clear()
                self.logger.error(str(traceback.format_exc()))
                self.declare_alarm(Alarm.INVALID_CAN_PACKET_READ)
                self.can_bus_state = CANBusStatus.OFFLINE

    def on_msg_receive(self, node_id: int, data: bytearray, subindex: float):
        self.logger.info(f"""The following message was received from the CAN Bus.
                    Node_ID: %s
                    Data: %s
                    SubIndex: %s
                    """.format(str(node_id), str(data), str(subindex)))

    # The SDO index (or address) is found in the parameters.csv file.
    def read_and_log_sdo(self, motor: BaseNode402, index, subindex, tries = 0):
        if tries >= 10:
            return -1
        try:
            value = motor.sdo[index][subindex].raw
            self.unlatch_all_alarms()
            self.can_bus_state = CANBusStatus.ONLINE
            return value
        except canopen.sdo.exceptions.SdoCommunicationError as e:
            return self.read_and_log_sdo(motor, index, subindex, tries + 1)
        except SdoAbortedError as e:
            self.logger.error(f"SDO communication error [{hex(index)}:{subindex}]: {e}")
            self.declare_alarm(Alarm.ERROR_READING_CAN_SDO)
            self.can_bus_state = CANBusStatus.OFFLINE
            self.network.add_node(self.motorB)
            self.network.add_node(self.motorA)
            return self.read_and_log_sdo(motor, index, subindex, tries + 1)
        except RuntimeError as e:
            self.logger.error(f"Error reading SDO [{hex(index)}:{subindex}]: {e}")
            self.declare_alarm(Alarm.ERROR_READING_CAN_SDO)
            self.can_bus_state = CANBusStatus.OFFLINE
            self.network.add_node(self.motorA)
            self.network.add_node(self.motorB)
            return -1

    # These are SDOs retrieved from the controller via CANbus using above function
    # There is a wide list of sensor data that can be read, but these are the useful ones.
    # Feel free to browse the parameter list which is in testing/parameters.csv
    def publish_sdo_data(self, motor, publisher):
        voltage = self.read_and_log_sdo(motor, 0x2030, 2) * 0.01  # Volts
        throttle_percent = self.read_and_log_sdo(motor, 0x2029, 6)  # mV
        rpm = self.read_and_log_sdo(motor, 0x2052, 1)  # rpm
        current = self.read_and_log_sdo(motor, 0x2073, 1)  # Arms
        if motor == self.motorB:
            temperature = self.read_and_log_sdo(motor, 0x2040, 2)  # deg C
        else:
            temperature = -1
        throttle_percent = throttle_percent / 10
        # this torque must be converted to lb*ft, because it is preferred
        torque = current * 0.15  # Nm

        power = voltage * current

        msg = CANMotorData()
        msg.voltage = int(voltage)
        msg.throttle_mv = int(throttle_percent)
        msg.throttle_percentage = int(throttle_percent)
        msg.rpm = int(rpm)
        msg.torque = int(torque)
        msg.motor_temp = int(temperature)
        msg.current = abs(int(current))
        msg.power = int(power)

        if self.can_bus_state == CANBusStatus.ONLINE:
            publisher.publish(msg)

    def get_bus_state(self):
        return self.can_bus_state
