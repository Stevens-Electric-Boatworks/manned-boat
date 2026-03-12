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
        # string - Operation Mode. Possible values: "A" or "M"
        # "A" - Automatic Mode
        # "M" - Manual Mode, forced to operate in 2D or 3D
        self.op_mode = op_mode
        # int - Mode. Possible values: 1, 2, 3
        # 1 - Fix not available
        # 2 - 2D
        # 3 - 3D
        self.mode = mode
        # int - PRN (Psuedorandom Noise)
        # Unique identifer for each satellite
        self.prn = prn
        # float - Position dilution of precision
        self.pdop = pdop
        # float - Horizontal dilution of precision
        self.hdop = hdop
        # float - Vertical dilution of precision
        self.vdop = vdop
        # int - System ID
        self.system_id = system_id


class GPVTGResult:
    def __init__(self, speed_knots, true_track):
        self.speed_knots = speed_knots
        self.true_track = true_track


class GPSDevice(SerialDevice):
    def __init__(self, node: Node, alarm_pub: AlarmPublisher, on_gpgga_result: Callable[[GPGGAResult], None],
                 on_gpvtg_result: Callable[[GPVTGResult], None], on_gpsv_result: Callable[[GPGSVResult], None],
                 on_gpgsa_result: Callable[[GPGSAResult], None]):
        super().__init__(node, node.get_parameter("gnss_serial_fd").get_parameter_value().string_value,
                         self._on_gps_msg_rec, alarm_pub)
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

    def _on_gps_msg_rec(self, data: SerialData):
        if data.to_utf_8().startswith("$GPGGA"):
            gps_str = pynmea2.parse(data.to_utf_8())
            if gps_str.lat != '':
                lat = convert_to_degrees(gps_str.lat, gps_str.lat_dir)
                lon = convert_to_degrees(gps_str.lon, gps_str.lon_dir)
                self._gga_callback(GPGGAResult(lat, lon))

        elif data.to_utf_8().startswith("$GPVTG"):
            gps_str = pynmea2.parse(data.to_utf_8())
            if not type(gps_str.spd_over_grnd_kts) == NoneType and hasattr(gps_str, "track") and not type(
                    gps_str.track):
                self._vtg_callback(GPVTGResult(float(gps_str.spd_over_grnd_kts), float(gps_str.true_track)))

        elif data.to_utf_8().startswith("$GPGSA"):
            gps_str = data.to_utf_8().split(",")

            try:
                mode = int(gps_str[2])
            except ValueError:
                self.node.get_logger().error("Failed to parse GPGSA message. Invalid mode.")
                return
            
            op_mode = gps_str[1]
            
            if mode == 1:
                prns = []
                pdop = 0xffffffff
                hdop = 0xffffffff
                vdop = 0xffffffff
            else:
                try:
                    for i in range(12):
                        try:
                            prns.append(int(gps_str[3 + i]))
                        except ValueError:
                            break

                    pdop = float(gps_str[15])
                    hdop = float(gps_str[16])
                    vdop = float(gps_str[17].split("*")[0])
                except ValueError:
                    self.node.get_logger().error("Failed to parse field in GPGSA sentence.")

            self._gsa_callback(GPGSAResult(
                    op_mode=op_mode,
                    mode=mode,
                    prn=prns,
                    pdop=pdop,
                    hdop=hdop,
                    vdop=vdop,
                    system_id=1 
                # Not all NMEA versions include this message, so for now I hard code
                # it to `1` (for USA GPS). 
            ))

        elif data.to_utf_8().startswith("$GPGSV"):
            gps_str = data.to_utf_8().split(",")
            if gps_str[3] == '':  # No sats
                return

            try:
                self.sv_state["current_msg"] = int(gps_str[2])
            except ValueError:
                self.node.get_logger().error(f"Unable to parse GPGSV string: \"${gps_str}\"")
                return
            if self.sv_state["current_msg"] <= self.sv_state["total_msgs"]:
                for i in range(4):
                    try:
                        prn = int(gps_str[4 + (i * 4) + 0])
                    except (ValueError, IndexError):
                        continue
                    try:
                        elev = int(gps_str[4 + (i * 4) + 1])
                    except (ValueError, IndexError):
                        elev = 0xffffffff
                    try:
                        azim = int(gps_str[4 + (i * 4) + 2])
                    except (ValueError, IndexError):
                        azim = 0xffffffff
                    try:
                        snr = int(gps_str[4 + (i * 4) + 3])
                    except (ValueError, IndexError):
                        snr = 0xffffffff

                    self.sats.append(Satellite(
                        prn=prn,
                        elev=elev,
                        azimuth=azim,
                        snr=snr
                    ))
            if gps_str[2] == "1":
                try:
                    self.sv_state["total_msgs"] = int(gps_str[1])
                except ValueError:
                    self.node.get_logger().error(f"Unable to parse GPGSV string: \"${gps_str}\"")
                    return
            elif int(gps_str[2]) == self.sv_state["total_msgs"]:
                self._sv_callback(GPGSVResult(self.sats))
                self.sats = []
