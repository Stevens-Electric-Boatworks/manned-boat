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
        explicit CanDeviceManager(const CanIo& can_io)
            : can_io_(can_io)
        {
        }

        void register_device(std::unique_ptr<CanDevice> device);

    private:
        std::vector<std::unique_ptr<CanDevice>> devices = {};
        const CanIo& can_io_;
    };
}
