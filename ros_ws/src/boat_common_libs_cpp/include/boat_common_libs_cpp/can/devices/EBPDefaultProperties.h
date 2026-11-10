//
// Created by Ishaan Sayal.
// Copyright (c) 2026 Stevens Electric Boatworks.

#pragma once
#include <rclcpp/exceptions/exceptions.hpp>

namespace eboat::can
{
    /**
     * EBoat Protocol Fault Codes
     */
    class EBPFaultCodes
    {
        static EBPFaultCodes fromBitField()
        {
            //TODO Return the fault codes from a bit field
            throw rclcpp::exceptions::UnimplementedError("This function is not implemented yet.");
        }
    };

    enum class DeviceState
    {
        INIT,
        OPERATIONAL,
        INACTIVE,
        UNKNOWN
    };

    enum class BoardType
    {
        COOLING,
        POWER,
        BATTERY,
        UNKNOWN
    };

    struct PowerRails
    {
        uint16_t input_voltage_mv;
        uint16_t input_current_ma;
    };

    inline DeviceState device_state_from_protocol(const uint8_t raw_value)
    {
        // todo use real values
        switch (raw_value)
        {
        case 0:
            return DeviceState::INIT;
        case 1:
            return DeviceState::OPERATIONAL;
        case 2:
            return DeviceState::INACTIVE;
        default:
            return DeviceState::UNKNOWN;
        }
    }

    inline BoardType board_type_from_protocol(const uint8_t raw_value)
    {
        //TODO use real values
        switch (raw_value)
        {
        case 0:
            return BoardType::POWER;
        case 1:
            return BoardType::COOLING;
        case 2:
            return BoardType::BATTERY;
        default:
            return BoardType::UNKNOWN;
        }
    }
}
