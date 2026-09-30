//
// Created by Ishaan Sayal.
// Copyright (c) 2026 Stevens Electric Boatworks.

#include "can_node_cpp/can/CANMotor.h"

#include <iostream>

void eboat::CANMotor::read(
    const MotorODParam &sdo_param) const {
    if (this->canDriver == nullptr) {
        std::cerr << "[CANMotor::read] ERROR: canDriver is null!\n";
        return;
    }
    canDriver->enqueueReadSDO(sdo_param);
}

std::vector<eboat::MotorFault> eboat::CANMotor::readMotorFaults() {
    //TODO: Implement Motor Fault Reading
    return {};
}
