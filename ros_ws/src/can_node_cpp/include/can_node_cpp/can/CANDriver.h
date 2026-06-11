//
// Created by Ishaan Sayal.
// Copyright (c) 2026 Stevens Electric Boatworks.

#pragma once

#include "CANDriver.h"
#include "CANMotor.h"

#include <iostream>
#include <lely/coapp/fiber_driver.hpp>

namespace eboat {

/**
 * Defines a CAN SDO Read that can be used on the motor
 */
struct MotorSDOParam {
  uint16_t index;
  int8_t subindex;
};

class CANDriver : public lely::canopen::FiberDriver {
public:
  using FiberDriver::FiberDriver;

  template <typename T>
  T readSDO(MotorSDOParam sdo_param) {
    Post([this] () {
      std::cout << "In post, running read" << "\n";
      auto future = AsyncRead<T>(0x2030, 2);
      auto value = Wait<T>(future);
      std::cout << value << "\n";
    });
    return -1;
  }
};
}