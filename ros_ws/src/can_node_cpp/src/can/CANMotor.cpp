//
// Created by Ishaan Sayal.
// Copyright (c) 2026 Stevens Electric Boatworks.

#include "can_node_cpp/can/CANMotor.h"

#include <iostream>
std::optional<std::any> eboat::CANMotor::read(SharedStore& shared_store, const MotorSDOParam sdo_param) const {
        std::cout<< "Going to perform AsyncRead" << "\n";

  auto x = 0;

  if (this->canDriver == nullptr) {
    std::cerr << "[CANMotor::read] ERROR: canDriver is null!\n";
    return std::nullopt;
  }

  // auto result = canDriver->queueReadSDO<int16_t>(shared_store, sdo_param);
  return 8;
}
std::vector<eboat::MotorFault> eboat::CANMotor::readMotorFaults() {
  //TODO: Implement Motor Fault Reading
  return {};
}
