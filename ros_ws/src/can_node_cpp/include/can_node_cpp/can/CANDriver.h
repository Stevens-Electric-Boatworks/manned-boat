//
// Created by Ishaan Sayal.
// Copyright (c) 2026 Stevens Electric Boatworks.

#pragma once

#include "CANDriver.h"
#include "CANMotor.h"
#include "SharedStore.h"

#include <iostream>
#include <lely/coapp/fiber_driver.hpp>


namespace eboat {

/**
 * Defines a CAN SDO Read that can be used on the motor
 */
struct MotorSDOParam {
  uint16_t index;
  int8_t subindex;
  bool operator==(const MotorSDOParam& other) const {
    return index == other.index && subindex == other.subindex;
  }
};



class CANDriver : public lely::canopen::FiberDriver {
public:
  using FiberDriver::FiberDriver;

  template <typename T>
  T queueReadSDO(SharedStore& shared_store,  MotorSDOParam sdo_param) {
    Post([this, shared_store, sdo_param] () {
      std::cout << "In post, running read" << "\n";
      auto future = AsyncRead<T>(0x2030, 2);
      auto value = Wait<T>(future);
      shared_store.storeSDO(value, sdo_param);
    });
    return -1;
  }
};
}

template <> struct std::hash<eboat::MotorSDOParam> {
  std::size_t operator()(const eboat::MotorSDOParam &p) const noexcept {
    std::size_t h1 = std::hash<int>{}(p.index);
    std::size_t h2 = std::hash<int>{}(p.subindex);
    return h1 ^ (h2 << 1);
  }
};