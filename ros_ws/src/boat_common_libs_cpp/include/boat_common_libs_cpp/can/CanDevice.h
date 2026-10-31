//
// Created by Ishaan Sayal.
// Copyright (c) 2026 Stevens Electric Boatworks.

#pragma once
#include "sockcanpp/CanMessage.hpp"

namespace eboat::can
{
    /**
     * Interface to define a CAN Device on the bus
     */
    class CanDevice
    {
    public:
        virtual ~CanDevice() = default;
        virtual void on_can_error(const sockcanpp::CanMessage& msg) = 0;
        virtual void on_can_data(const sockcanpp::CanMessage& msg) = 0;
        virtual uint8_t get_can_id() = 0;
    };
}
