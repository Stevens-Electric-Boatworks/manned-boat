//
// Created by Ishaan Sayal.
// Copyright (c) 2026 Stevens Electric Boatworks.

#include "boat_common_libs_cpp/can/CanDeviceManager.h"

void eboat::can::CanDeviceManager::register_device(std::unique_ptr<CanDevice> device)
{
    this->devices.emplace_back(std::move(device));
}
