//
// Created by Ishaan Sayal.
// Copyright (c) 2026 Stevens Electric Boatworks.

#include "boat_common_libs_cpp/can/devices/EBoatCANDevice.h"

#include "boat_common_libs_cpp/can/byte_helpers.h"

void eboat::can::EBoatCANDevice::on_can_error(const sockcanpp::CanMessage& msg)
{

    //TODO: implement error related stuff
}

void eboat::can::EBoatCANDevice::on_can_data(const sockcanpp::CanMessage& msg)
{
    parse_generic_headers(msg);
    handle_custom_can_data(msg);

}

uint8_t eboat::can::EBoatCANDevice::get_can_id()
{
    return can_id;
}

eboat::can::DeviceState eboat::can::EBoatCANDevice::get_device_state() const
{
    return device_state_;
}

eboat::can::BoardType eboat::can::EBoatCANDevice::get_board_type() const
{
    return board_type_;
}

const std::optional<eboat::can::PowerRails>& eboat::can::EBoatCANDevice::get_power_rails() const
{
    return rails_;
}

const std::optional<uint16_t>& eboat::can::EBoatCANDevice::get_seconds_since_boot() const
{
    return seconds_since_boot;
}

const std::optional<uint16_t>& eboat::can::EBoatCANDevice::get_ms_since_heartbeat() const
{
    return ms_since_heartbeat;
}

uint8_t eboat::can::EBoatCANDevice::get_header_byte(const sockcanpp::CanMessage& msg)
{
    return readU8(msg.getFrameData(), 0);
}


void eboat::can::EBoatCANDevice::parse_generic_headers(const sockcanpp::CanMessage& msg)
{
    switch (get_header_byte(msg)) {
    case 0x01: parse_device_status_frame(msg); break;
    case 0x02: /*TODO implement*/;   break;
    case 0x03: parse_power_rails_frame(msg); break;
    default:
        break;
    }}

void eboat::can::EBoatCANDevice::parse_device_status_frame(const sockcanpp::CanMessage& msg)
{
    const auto d = msg.getFrameData();
    device_state_ = device_state_from_protocol(readU8(d, 1));
    board_type_ = board_type_from_protocol(readU8(d, 2));
    seconds_since_boot = readU16(d, 3);
    ms_since_heartbeat =  readU16(d, 5);

    // bytes 6/7 are unused
}

void eboat::can::EBoatCANDevice::parse_power_rails_frame(const sockcanpp::CanMessage& msg)
{
    const auto d = msg.getFrameData();
    rails_ = PowerRails {
      readU16(d, 1), // input voltage mV
        readU16(d, 3) // input current mA
    };
    // bytes 5-7 are unused
}
