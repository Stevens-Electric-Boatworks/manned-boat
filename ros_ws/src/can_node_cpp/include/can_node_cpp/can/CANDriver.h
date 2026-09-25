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

protected:
  void OnBoot(lely::canopen::NmtState st, char es,
              const std::string &what) noexcept override {
    std::cout << "OnBoot fired for node " << static_cast<int>(id()) << ", es=" << es << "\n";
  }

public:
  template <typename T>
  void queueReadSDO(SharedStore &shared_store, MotorSDOParam& sdo_param) {
    Post([this, shared_store, sdo_param] () {
      auto future = AsyncRead<int16_t>(sdo_param.index, sdo_param.subindex);
      auto value = Wait<T>(future);
      auto canData = CANData {
        value,
        std::chrono::system_clock::now()
      };
      shared_store.storeSDO(sdo_param, canData);
    });
  }
};
}