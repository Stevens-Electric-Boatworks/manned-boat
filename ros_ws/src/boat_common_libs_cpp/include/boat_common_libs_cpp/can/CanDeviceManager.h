//
// Created by Ishaan Sayal.
// Copyright (c) 2026 Stevens Electric Boatworks.
#pragma once
#include "boat_common_libs_cpp/can/CanIo.h"
#include "boat_common_libs_cpp/can/CanDevice.h"

namespace eboat::can
{
    class CanDeviceManager
    {
    public:
        explicit CanDeviceManager(CanIo& can_io)
            : can_io_(can_io)
        {
            can_io.register_msg_callback([this](const sockcanpp::CanMessage& can_message)
            {
                if (const auto it = devices.find(*can_message.getCanId()); it != devices.end())
                {
                    it->second->on_can_data(can_message);
                }
            });
        }

        void register_device(std::unique_ptr<CanDevice> device);

    private:
        std::unordered_map<uint8_t, std::unique_ptr<CanDevice>> devices = {};
        const CanIo& can_io_;
    };
}
