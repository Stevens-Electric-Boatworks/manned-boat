from typing import Callable

from rclpy.node import Node

from boat_common_libs.alarm_lib.alarm_helper import AlarmPublisher
from boat_common_libs.serial_lib.serial_device import SerialDevice, SerialData

# NOTE: A lot of the info in this node is technical output from the
# Telit LE910C4-NF cell modem.
# See its manual for more information about specifics:
# https://sixfab.com/wp-content/uploads/2022/02/Telit_LE910Cx_AT_Commands_Reference_Guide_r12.pdf

class CellData:
    def __init__(self, network=None, tech=None, bars=None, rsrp=None, rsrq=None, reg_status=None, ip_addr=None, apn=None, pin_status=None):
        self.network = network
        self.tech = tech
        self.bars = bars
        self.rsrp = rsrp
        self.rsrq = rsrq
        self.reg_status = reg_status
        self.ip_addr = ip_addr
        self.apn = apn
        self.pin_status = pin_status
        


class CellSerialDevice(SerialDevice):
    def __init__(self, node: Node, alarm_pub: AlarmPublisher, on_cell_data: Callable[[CellData], None]):
        super().__init__(node, "/dev/ttyUSB2", self._on_cell_data_rec, alarm_pub)
        self.node = node
        self.on_cell_data = on_cell_data
        self.data = CellData()
        self.configure()


    def configure(self):
        # Commands to reset GPS receiver
        self.send_string("AT$GPSRST\n")
        self.send_string("AT$GPSNVRAM=15,0\r")
        self.send_string("AT$GPSACP\r")
        # Sets the data that will be output by GPS receiver - currently set to
        # unsolicited output on /dev/ttyUSB1 for all types of messages
        self.send_string("AT$GPSNMUN=2,1,1,1,1,1,1\r")
        # Turn on GPS receiver
        self.send_string("AT$GPSP=1\r")

    def update_cell_data(self):
        # Retrieve cell tech and network
        self.send_string("AT+COPS?\r")
        # Retrieve info about cell signal quality
        self.send_string("AT+CESQ\r")
        # Get registration status
        self.send_string("AT+CEREG?\r")
        # Get IP cell's address
        self.send_string("AT+CGPADDR=1\r")
        # Get information about context, most useful is APN
        self.send_string("AT+CGDCONT?\r")
        # Get PIN status
        self.send_string("AT+CPIN?\r")
        # Get GNSS power status
        self.send_string("AT$GPSP?\r")

    def _on_cell_data_rec(self, data: SerialData):
        # whenever we receive cell data
        data_str = data.to_utf_8()
        self.node.get_logger().info(f"{data_str}")

        # Process network & technology
        if data_str.startswith("+COPS:"):
            split = data_str.split(",")

            if split[3] == "7" or split[3] == "13":
                self.data.tech = "LTE"
            elif split[3] == "9":
                self.data.tech = "H+"
            elif split[3] == "4":
                self.data.tech = "H"
            elif split[3] == "0" or split[3] == "2":
                self.data.tech = "3G"

            self.data.network = split[2].split("\"")[1]

        elif data_str.startswith("+CESQ"):
            split = data_str.split(",")
            try:
                rsrp = int(split[5])
                self.data.rsrp = rsrp

                if rsrp < 15:
                    self.data.bars = 0
                elif rsrp < 22:
                    self.data.bars = 1
                elif rsrp < 29:
                    self.data.bars = 2
                elif rsrp < 37:
                    self.data.bars = 3
                elif rsrp == 255:
                    self.data.bars = 255
                else:
                    self.data.bars = 4
            except ValueError:
                self.node.get_logger().error(f"Failed to parse RSRP signal power: {split[5]}")

            try:
                rsrq = int(split[4])
                self.data.rsrq = rsrq
            except ValueError:
                self.node.get_logger().error(f"Failed to parse RSRQ signal quality: {split[4]}")
                

        elif data_str.startswith("+CEREG:"):
            split = data_str.split(",")
            try:
                reg_status = int(split[1])
                self.data.reg_status = reg_status
            except ValueError:
                self.node.get_logger().error(f"Failed to parse CEREG: {split[1]}")

        elif data_str.startswith("+CGPADDR:"):
            split = data_str.split(",")
            ip = split[1].replace("\"", "")
            self.data.ip_addr = ip

        elif data_str.startswith("+CGDCONT:"):
            split = data_str.split(",")
            # Don't consider the other contexts
            if split[0] != "+CGDCONT: 1":
                return
            apn = split[1].replace("\"", "")
            self.data.apn = apn

        elif data_str.startswith("+CPIN:"):
            split = data_str.split(",")
            pin_status = split[1].replace(" ", "")
            self.data.pin_status = pin_status
        elif data_str.startswith("$GPSP:"):
            # Eventually we will query the GNSS power but for now 
            # we use this to indicate when we can send new data
            if self.data is None:
                return
            
            self.on_cell_data(self.data)
            

        #process cell data

        

