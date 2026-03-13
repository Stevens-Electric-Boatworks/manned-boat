import threading
import time
from typing import Callable

import serial
from rclpy.node import Node
from serial.serialutil import SerialException

from boat_common_libs.alarm_lib.alarm_helper import AlarmPublisher
from boat_common_libs.alarm_lib.alarms import Alarm


class SerialData:
    def __init__(self, data:bytes):
        self.data = data

    def to_utf_8(self) -> str:
        return self.data.decode('utf-8').strip()

    def to_bytes(self) -> bytes:
        return self.data



class SerialDevice:
    """
    Represents a serial device that is intended to work within the ROS ecosystem.
    """

    def __init__(self, node:Node, serial_port:str, on_msg_rec:Callable[[SerialData], None], alarm_manager: AlarmPublisher, baudrate:int = 115200, timeout:float = 1, polling_duration=0.1):
        """

        param: node - The ROS2 node that is using the serial device
        param: serial_port - The serial port string (eg. "/dev/tty0USB")
        param: on_msg_rec - The callback function to call whenever a message is received from the port. Must accept a singular parameter with the SerialData class

        """

        self.alarm_manager = alarm_manager
        self.baudrate = baudrate
        self.on_msg_rec = on_msg_rec
        self.serial_port = serial_port
        self.node = node
        self.logger = node.get_logger()
        self.timeout = timeout
        self.valid = False
        threading.Thread(target=self._device_connect_thread, daemon=True).start()
        self.node.create_timer(polling_duration, self._poll_serial_device)


    def _device_connect_thread(self):
        while True:
            try:
                self.device = serial.Serial(self.serial_port, self.baudrate, timeout=self.timeout)
                self.device.flush()
                self.valid = True
                self._unlatch_all()
                self.logger.info(f"Connected to serial device at '{self.serial_port}' with baudrate '{self.baudrate}!'")
                break
            except SerialException as e:
                self.valid = False
                if e.errno == 2:
                    self.alarm_manager.publish_alarm(Alarm.SERIAL_DEVICE_DOES_NOT_EXIST)
                elif e.errno == 16:
                    self.alarm_manager.publish_alarm(Alarm.SERIAL_DEVICE_IN_USE)
                else:
                    self.alarm_manager.publish_alarm(Alarm.GENERIC_SERIAL_DEVICE_ERROR)

                self.logger.warning(f"[SERIAL LIB] Unable to connect to '{self.serial_port}'\nAttempting to reconnect to the device after 3 seconds...")
                time.sleep(3)

    def _unlatch_all(self):
        self.alarm_manager.delatch_alarm(Alarm.SERIAL_DEVICE_IN_USE)
        self.alarm_manager.delatch_alarm(Alarm.GENERIC_SERIAL_DEVICE_ERROR)
        self.alarm_manager.delatch_alarm(Alarm.SERIAL_DEVICE_DOES_NOT_EXIST)

    def _poll_serial_device(self):
        if not self.valid:
            return
        try:
            while self.device.in_waiting > 0:
                data = SerialData(self.device.readline())
                self.on_msg_rec(data)
        except OSError as e:
            if e.errno == 5:
                self.logger.error(f"There was a serial IO error for '{self.serial_port}'. Sleeping for 0.5s, then reconnecting")
                self.alarm_manager.publish_alarm(Alarm.SERIAL_IO_ERROR)
                time.sleep(0.5)
                self.valid = False
                self.device.close()
                self.device = None
                threading.Thread(target=self._device_connect_thread, daemon=True).start()

    def send_string(self, data: str):
        if not self.valid:
            return
        self.logger.info(f"Sending {data}...")
        self.device.write(data.encode())


