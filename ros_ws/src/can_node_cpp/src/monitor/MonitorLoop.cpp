//
// Created by Ishaan Sayal.
// Copyright (c) 2026 Stevens Electric Boatworks.

#include "can_node_cpp/monitor/MonitorLoop.h"
void eboat::MonitorLoop::initialize() {

}
void eboat::MonitorLoop::tick() {
  this->can_bus_service_.pergiodic();
  proccessQueue();
}
void eboat::MonitorLoop::proccessQueue() {
  auto &queue = can_bus_service_.shared_store->get_queued_reads();
  while (!queue.empty()) {
    MotorSDOParam param = queue.front();
    this->can_bus_service_.motorA->read(*can_bus_service_.shared_store, param);
    queue.pop();
  }
  this->can_bus_service_.motorA->canDriver->ensureWorkerRunning(*can_bus_service_.shared_store);
}
