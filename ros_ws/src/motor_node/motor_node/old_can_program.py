from threading import Thread

import canopen

import math
import time

from can import CanError
from rclpy.impl.rcutils_logger import RcutilsLogger

from boat_common_libs.alarm_lib.alarms import Alarm
from boat_data_interfaces.msg import CANMotorData, CANBusStatus

import traceback
import subprocess


# GatherData.py


# NEED to install pip package "canlib"
# sudo pip install canlib --break-system-packages
# https://pycanlib.readthedocs.io/en/v1.31.107/canlib.html#installation
# https://kvaser.com/canlib-webhelp/section_install_linux.htm

# Instructions coming soon AFTER we run them on the Pi to verify what we have done

# This program is for reading serial data from the arduino
# Any new sensor or data being read, must be added to the dictionaries below.
# All you must do is make sure the order in which it is getting printed corresponsd to the loop below


# They are using serial?
# == Ishaan

# ser = serial.Serial('/dev/ttyACM0', 115200, timeout=1)


# def read_serial():
#     try:
#         line = ser.readline().decode('utf-8').strip()  # Read a line from the serial port
#         values = line.split(",")  # Split the line by commas
#
#         # Ensure we have the expected number of values before proceeding
#         if len(values) != 7:
#             return {
#                 'yaw': 0,
#                 'pitch': 0,
#                 'roll': 0,
#                 'ax': 0,
#                 'ay': 0,
#                 'az': 0,
#                 'heading': 0
#             }
#
#         # loop mentioned in desctiption
#         yaw, pitch, roll, ax, ay, az, heading = [float(v) for v in values]
#         #         print("good data:", values)
#         return {
#             'yaw': yaw,
#             'pitch': pitch,
#             'roll': roll,
#             'ax': ax,
#             'ay': ay,
#             'az': az,
#             'heading': heading
#         }
#     except UnicodeDecodeError:
#         print("Received invalid byte sequence. Skipping...")
#         return {'yaw': 0,
#                 'pitch': 0,
#                 'roll': 0,
#                 'ax': 0,
#                 'ay': 0,
#                 'az': 0,
#                 'heading': 0
#                 }
#     except ValueError:
#         print("Error in converting values. Skipping...")
#         return {'yaw': 0,
#                 'pitch': 0,
#                 'roll': 0,
#                 'ax': 0,
#                 'ay': 0,
#                 'az': 0,
#                 'heading': 0
#                 }

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


class OldCanProgram:
    def __init__(self, logger: RcutilsLogger, dummy_efp, publisher, is_node_ok, declare_alarm, shutdown_node, unlatch_all_alarms):
        self.sdo = None
        self.start_time = None
        self.can_thread = None
        self.node = None
        self.publisher = None
        self.logger = logger
        self.dummy_efp = dummy_efp
        self.publisher = publisher
        self.is_node_ok = is_node_ok
        self.declare_alarm = declare_alarm
        self.shutdown_node = shutdown_node
        self.can_bus_state = CANBusStatus.OFFLINE
        self.unlatch_all_alarms = unlatch_all_alarms

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
            self.network.connect(channel='can0', bustype='socketcan')
        except CanError:
            self.logger.error(
                f"""Unable to connect to the CAN bus because of the following error: {traceback.format_exc()}""")
            self.declare_alarm(Alarm.INVALID_CAN_PACKET_READ)
            self.can_bus_state = CANBusStatus.OFFLINE
            return

        self.logger.info("Connected to SocketCAN")
        # Subscribe to messages
        self.network.subscribe(0, self.on_msg_receive)
        # You can create a node with a known node-ID
        node_id = 6  # Replace with your node ID
        self.logger.info("Using a dummy EDS file at \"" + self.dummy_efp + "\".")
        self.node = canopen.BaseNode402(node_id, canopen.import_od(self.dummy_efp))  # Use a dummy EDS here
        self.network.add_node(self.node)

        self.can_thread = Thread(
            target=self.read_can_messages, args=[self.publisher], daemon=True)
        self.can_thread.start()

    def read_can_messages(self, publisher):
        while True:
            try:
                if not self.is_node_ok:
                    self.logger.info("Safely shutting down the CAN Motor reader thread")
                    return

                self.publish_sdo_data(publisher)
                time.sleep(0.3)
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
    def read_and_log_sdo(self, index, subindex):
        try:
            value = self.node.sdo[index][subindex].raw
            self.unlatch_all_alarms()
            self.can_bus_state = CANBusStatus.ONLINE
            return value
        except RuntimeError as e:
            self.logger.error(f"Error reading SDO [{hex(index)}:{subindex}]: {e}")
            self.declare_alarm(Alarm.ERROR_READING_CAN_SDO)
            self.can_bus_state = CANBusStatus.OFFLINE
            return 0

    # These are SDOs retrieved from the controller via CANbus using above function
    # There is a wide list of sensor data that can be read, but these are the useful ones.
    # Feel free to browse the parameter list which is in testing/parameters.csv
    def publish_sdo_data(self, publisher):
        voltage = self.read_and_log_sdo(0x2030, 2) * 0.01  # Volts
        throttle_mv = -1  # self.read_and_log_sdo( 0x2013, 1)  # mV
        rpm = self.read_and_log_sdo(0x2001, 2)  # rpm
        current = self.read_and_log_sdo(0x2073, 1)  # Arms
        temperature = self.read_and_log_sdo( 0x2040, 2)  # deg C

        throttle_percent = throttle_mv / 2800  # %

        # this torque must be converted to lb*ft, because it is preferred
        torque = current * 0.15  # Nm

        power = (torque * rpm) * math.pi / 30000  # kW

        msg = CANMotorData()
        msg.voltage = int(voltage)
        msg.throttle_mv = int(throttle_mv)
        msg.throttle_percentage = int(throttle_percent)
        msg.rpm = int(rpm)
        msg.torque = int(torque)
        msg.motor_temp = int(temperature)
        msg.current = int(current)
        msg.power = int(power)
        
        
        if self.can_bus_state == CANBusStatus.ONLINE:
            publisher.publish(msg)


    def get_bus_state(self):
        return self.can_bus_state

    def testabc(self):
        pass