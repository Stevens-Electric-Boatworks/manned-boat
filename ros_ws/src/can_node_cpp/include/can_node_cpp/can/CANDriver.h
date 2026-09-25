//
// Created by Ishaan Sayal.
// Copyright (c) 2026 Stevens Electric Boatworks.

#pragma once

#include "MotorSDOParam.h"
#include "SharedStore.h"

#include <iostream>
#include <lely/coapp/fiber_driver.hpp>

namespace eboat {

// CANDriver.h
class CANDriver : public lely::canopen::FiberDriver {
public:
  using FiberDriver::FiberDriver;

  void enqueueReadSDO(SharedStore &shared_store, MotorSDOParam sdo_param) {
    bool need_start = false;
    {
      std::lock_guard<std::mutex> lock(queue_mutex_);
      pending_.push(sdo_param);
      if (!worker_running_) {
        worker_running_ = true;
        need_start = true;
      }
    }
    if (need_start) {
      Post([this, &shared_store] { runWorker(shared_store); });
    }
  }

  void runWorker(SharedStore &shared_store) {
    for (;;) {
      MotorSDOParam param;
      {
        std::lock_guard<std::mutex> lock(queue_mutex_);
        if (pending_.empty()) {
          worker_running_ = false;
          return;
        }
        param = pending_.front();
        pending_.pop();
      }
      try {
        auto value = Wait(AsyncRead<int16_t>(param.index, param.subindex));
        shared_store.storeSDO(param, CANData{value, std::chrono::system_clock::now()});
      } catch (const lely::canopen::SdoError &e) {
        std::cerr << "SDO read failed for " << std::hex << param.index
                   << ":" << +param.subindex << " — " << e.what() << "\n";
      }
    }
  }

  void ensureWorkerRunning(SharedStore &shared_store) {
    if (worker_running_) return;
    worker_running_ = true;

    Post([this, &shared_store] { runWorker(shared_store); });
  }

private:
  std::mutex queue_mutex_;
  std::queue<MotorSDOParam> pending_;
  bool worker_running_ = false;
};
} // namespace eboat