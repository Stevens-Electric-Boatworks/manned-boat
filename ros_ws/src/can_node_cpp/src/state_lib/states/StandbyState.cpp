//
// Created by Ishaan Sayal.
// Copyright (c) 2026 Stevens Electric Boatworks.

#include "can_node_cpp/state_lib/states/StandbyState.h"

#include "can_node_cpp/can/CANBusService.h"

eboat::StandbyState::~StandbyState() = default;

bool eboat::StandbyState::isValid() const {
    return true;
}

void eboat::StandbyState::onSwitch() const {
}

void eboat::StandbyState::periodic() const {
}

void eboat::StandbyState::cleanup() const {
}
