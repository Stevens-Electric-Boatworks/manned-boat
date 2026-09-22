//
// Created by Ishaan Sayal.
// Copyright (c) 2026 Stevens Electric Boatworks.

#pragma once


#include "MotorSDOParam.h"
#include "SharedStore.h"

#include <iostream>
#include <lely/coapp/fiber_driver.hpp>


namespace eboat {

class CANDriver : public lely::canopen::FiberDriver {
public:
  using FiberDriver::FiberDriver;

  template <typename T>
  void queueReadSDO(SharedStore &shared_store, MotorSDOParam& sdo_param) {
    Post([this, shared_store, sdo_param] () {
      std::cout << "In post, running read" << "\n";
      auto future = AsyncRead<T>(sdo_param.index, sdo_param.subindex);
      auto value = Wait<T>(future);
      shared_store.storeSDO(value, sdo_param);
    });
  }
};
}