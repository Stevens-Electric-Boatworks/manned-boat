from typing import Callable
from types import NoneType

import pynmea2
from rclpy.node import Node

from boat_data_interfaces.msg import Satellite

from boat_common_libs.alarm_lib.alarm_helper import AlarmPublisher
from boat_common_libs.serial_lib.serial_device import SerialDevice, SerialData


def convert_to_degrees(raw_value, direction):
    raw_value = float(raw_value)
    degrees = int(raw_value // 100)
    minutes = raw_value - (degrees * 100)
    decimal = degrees + minutes / 60.0

    if direction in ['S', 'W']:
        decimal = -decimal
    return decimal


class GPGGAResult:
    def __init__(self, lat, lon):
        self.lat = lat
        self.lon = lon
class GPGSVResult:
    def __init__(self, sats):
        self.sats = sats

class GPGSAResult:
    def __init__(self, op_mode, mode, prn, pdop, hdop, vdop, system_id):
        self.op_mode = op_mode
        self.mode = mode
        self.prn = prn
        self.pdop = pdop
        self.hdop = hdop
        self.vdop = vdop
        self.system_id = system_id

class GPVTGResult:
    def __init__(self, speed_knots, true_track):
        self.speed_knots = speed_knots
        self.true_track = true_track


class GPSDevice(SerialDevice):    
    def __init__(self, node:Node, alarm_pub:AlarmPublisher, on_gpgga_result:Callable[[GPGGAResult], None], on_gpvtg_result:Callable[[GPVTGResult], None], on_gpsv_result:Callable[[GPGSVResult], None], on_gpgsa_result:Callable[[GPGSAResult], None]):
        super().__init__(node, "/tmp/ttyUSB1_fake", self._on_gps_msg_rec, alarm_pub)
        # super().__init__(node, "/dev/ttyUSB1", self._on_gps_msg_rec, alarm_pub)
        self._gga_callback = on_gpgga_result
        self._vtg_callback = on_gpvtg_result
        self._sv_callback = on_gpsv_result
        self._gsa_callback = on_gpgsa_result

        self.sats = []
        self.sv_state = {
            "current_msg": 0,
            "total_msgs": 0
        }

        self.node = node

    def _on_gps_msg_rec(self, data:SerialData):
        if data.to_utf_8().startswith("$GPGGA"):
            gps_str = pynmea2.parse(data.to_utf_8())
            if gps_str.lat != '':
                lat = convert_to_degrees(gps_str.lat, gps_str.lat_dir)
                lon = convert_to_degrees(gps_str.lon, gps_str.lon_dir)
                self._gga_callback(GPGGAResult(lat, lon))

        elif data.to_utf_8().startswith("$GPVTG"):
            gps_str = pynmea2.parse(data.to_utf_8())
            if not type(gps_str.spd_over_grnd_kts) == NoneType and hasattr(gps_str, "track") and not type(gps_str.track):
                self._vtg_callback(GPVTGResult(float(gps_str.spd_over_grnd_kts), float(gps_str.true_track)))

        elif data.to_utf_8().startswith("$GPGSA"):
            gps_str = data.to_utf_8().split(",")

            op_mode = gps_str[1]
            mode = int(gps_str[2])
            prns = []
            for i in range(12):
                try:
                    prns.append(int(gps_str[3 + i]))
                except ValueError:
                    break
            pdop = float(gps_str[15])
            hdop = float(gps_str[16])
            vdop = float(gps_str[17].split("*")[0])

            self._gsa_callback(GPGSAResult(
                op_mode=op_mode,
                mode=mode,
                prn=prns,
                pdop=pdop,
                hdop=hdop,
                vdop=vdop,
                system_id=1
            ))

        elif data.to_utf_8().startswith("$GPGSV"):
            gps_str = data.to_utf_8().split(",")
            if gps_str[3] == '': # No sats
                return 
            
            self.sv_state["current_msg"] = int(gps_str[2])
            if self.sv_state["current_msg"] <= self.sv_state["total_msgs"]:
                for i in range(4):
                    try:
                        prn = int(gps_str[4 + (i * 4) + 0])
                    except (ValueError, IndexError):
                        prn = 0xff
                    try:
                        elev = int(gps_str[4 + (i * 4) + 1])
                    except (ValueError, IndexError):
                        elev = 0xff
                    try:
                        azim = int(gps_str[4 + (i * 4) + 2])
                    except (ValueError, IndexError):
                        azim = 0xff
                    try:
                        snr = int(gps_str[4 + (i * 4) + 3])
                    except (ValueError, IndexError):
                        snr = 0xff

                    self.sats.append(Satellite(
                        prn=prn,
                        elev=elev,
                        azimuth=azim,
                        snr=snr
                    ))
            if gps_str[2] == "1":
                self.sv_state["total_msgs"] = int(gps_str[1])
            elif int(gps_str[2]) == self.sv_state["total_msgs"]:
                self._sv_callback(GPGSVResult(self.sats))
                self.sats = []
