//
// Created by Ishaan Sayal.
// Copyright (c) 2026 Stevens Electric Boatworks.

#pragma once
#include "CANMotor.h"

#include <lely/coapp/fiber_driver.hpp>

namespace eboat {

class CANDriver : public lely::canopen::FiberDriver {
 public:
  using FiberDriver::FiberDriver;
};

class CANBusService {
public:
  CANMotor& motorA;
  CANMotor& motorB;

  static void initBus();

  static bool initialized();
};
}