from enum import Enum

class Alarm(Enum):
    """Enumeration of system alarms and faults."""
    UNKNOWN_FAULT = -1
    """FAULT: Unknown Fault — Only if there is an unknown fault code that is published."""
    UNKNOWN_WARNING = 0
    """WARN: Unknown Warning — Only if there is an unknown warning code that is published."""
    WEBSOCKET_INITIAL_CONNECTION_FAILURE = 1
    """FAULT: Websocket Initial Connection Failure — Unable to open a connection to the shore server."""
    WEBSOCKET_CONNECTION_CLOSED = 2
    """FAULT: Websocket Connection Closed — The websocket connection unexpectedly closed."""
    WEBSOCKET_IS_NOT_INITIALLY_OPENED_YET = 3
    """WARN: Websocket Is Not Initially Opened Yet — Websocket is still waiting to be opened."""
    WEBSOCKET_NOT_OPENED = 4
    """FAULT: Websocket Not Opened — Websocket is reported as no longer open."""
    CAN_MOTOR_NODE_SHUTDOWN = 10
    """FAULT: CAN Motor Node Shutdown — The motor node had to shut down due to a fatal error."""
    CAN0_INTERFACE_NOT_UP = 11
    """FAULT: 'can0' Interface Not Up — If `ip link status can0` does not have `status UP`."""
    INVALID_CAN_PACKET_READ = 12
    """WARN: Invalid CAN Packet Read — A `canerror` was thrown by the underlying `canopen` library."""
    ERROR_READING_CAN_SDO = 13
    """WARN: Error Reading CAN SDO — Caused by an invalid CAN SDO."""
    FAILED_CAN_NETWORK_INIT = 14
    """FAULT: Failed CAN Network Init — Failed to initialize `canopen.Network()`."""
    SERIAL_DEVICE_DOES_NOT_EXIST = 15
    """FAULT: Serial Device Does Not Exist — Failed to open a serial device because it does not exist."""
    GENERIC_SERIAL_DEVICE_ERROR = 16
    """FAULT: Generic Serial Device Error — There is some kind of generic serial device error."""
    SERIAL_DEVICE_IN_USE = 17
    """FAULT: Serial Device In-Use — The serial device is in use and is locked."""
    SERIAL_IO_ERROR = 18
    """FAULT: Serial Device IO Error — There was an IO error thrown when trying to read from the serial device."""
    DRIVE_MOTOR_FAULT = 19
    """FAULT: Drive Motor Fault - There was a fault raised by the inmotion controllers"""
    CELL_SIGNAL_QUALITY_POOR = 30
    """WARN: Cellular Signal Quality POOR WARNING — Cellular signal quality is poor."""
    CELL_AUTH_MODE_NOT_READY = 31
    """FAULT: Cellular Authentication Mode NOT READY — AT+CPIN? is not READY."""
    BMS_CCENUS_FAULT = 40
    """WARN: BMS Ccenus Fault — BMS Reports a CCENUS Fault."""
    BMS_TCENCUS_FAULT = 41
    """WARN: BMS Tcencus Fault — BMS Reports a Tcencus Fault."""
    BMS_HIGH_VOLTAGE_FAULT = 42
    """FAULT: BMS High Voltage Fault — BMS Reports a High Voltage Fault."""
    BMS_LOW_VOLTAGE_FAULT = 43
    """FAULT: BMS Low Voltage Fault — BMS Reports a Low Voltage Fault."""
    BMS_HIGH_TEMP_FAULT = 44
    """FAULT: BMS High Temp Fault — BMS Reports a Temperature too High."""
    BMS_LOW_TEMP_FAULT = 45
    """FAULT: BMS Low Temp Fault — BMS Reports a Temperature too Low."""
    BMS_HARDWARE_FAULT = 46
    """FAULT: BMS Hardware Fault — BMS Reports a Hardware Fault."""