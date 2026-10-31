//
// Created by Ishaan Sayal.
// Copyright (c) 2026 Stevens Electric Boatworks.

#include "boat_common_libs_cpp/can/devices/eboat/ExampleCanNode.h"
#include "boat_common_libs_cpp/can/byte_helpers.h"

void eboat::can::device::ExampleCanNode::handle_custom_can_data(const sockcanpp::CanMessage& msg)
{
    switch (get_header_byte(msg))
    {
    case 0x10: parse_cooling_temp_senors_vals(msg);
        break;
    default:
        break;
    }
    std::printf("Temp A: %f, %f, %f\n", *temp_a, *temp_b, *temp_c);
}

const std::optional<double>& eboat::can::device::ExampleCanNode::get_temp_a() const
{
    return temp_a;
}

const std::optional<double>& eboat::can::device::ExampleCanNode::get_temp_b() const
{
    return temp_b;
}

const std::optional<double>& eboat::can::device::ExampleCanNode::get_temp_c() const
{
    return temp_c;
}

void eboat::can::device::ExampleCanNode::parse_cooling_temp_senors_vals(const sockcanpp::CanMessage& msg)
{
    const auto d = msg.getFrameData();
    temp_a = readU16(d, 1) * 0.01;
    temp_b = readU16(d, 3) * 0.01;
    temp_c = readU16(d, 5) * 0.01;
}
