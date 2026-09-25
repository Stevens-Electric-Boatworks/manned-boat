//
// Created by Ishaan Sayal.
// Copyright (c) 2026 Stevens Electric Boatworks.

#pragma once
#include "can_node_cpp/can/CANBusService.h"

#include <memory>
namespace eboat {
class MonitorLoop {
  eboat::CANBusService& can_bus_service_;

public:
  explicit MonitorLoop(eboat::CANBusService &can_bus_service)
      : can_bus_service_(can_bus_service) {}

  void initialize();

  void tick();
private:
  void proccessQueue();
};
}
