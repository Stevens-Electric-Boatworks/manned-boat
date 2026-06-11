//
// Created by Ishaan Sayal.
// Copyright (c) 2026 Stevens Electric Boatworks.

#include "can_node_cpp/can/CANMotor.h"

#include <iostream>
eboat::CANMotor::MotorValue eboat::CANMotor::read(const MotorSDOParam sdo_param) const {
        std::cout<< "Going to perform AsyncRead" << "\n";

  auto x = 0;

  if (this->canDriver == nullptr) {
    std::cerr << "[CANMotor::read] ERROR: canDriver is null!\n";
    return std::nullopt;
  }

  auto result = canDriver->readSDO<int16_t>(sdo_param);
  return result;
  // std::printf("There was an error!\n");
  // return std::nullopt;
}
std::vector<eboat::MotorFault> eboat::CANMotor::readMotorFaults() {
  //TODO: Implement Motor Fault Reading
  return {};
}
