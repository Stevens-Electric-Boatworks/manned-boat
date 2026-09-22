//
// Created by Ishaan Sayal.
// Copyright (c) 2026 Stevens Electric Boatworks.

#include "can_node_cpp/monitor/MonitorLoop.h"
void eboat::MonitorLoop::initialize() {

}
void eboat::MonitorLoop::tick() {
  proccessQueue();
}
void eboat::MonitorLoop::proccessQueue() {
  std::queue<MotorSDOParam> queue = can_bus_service_.shared_store->get_queued_reads();
   while (!queue.empty()) {
     MotorSDOParam param = queue.front();
     // std::cout << "Reading parameter from queue" << std::endl;
     auto value = this->can_bus_service_.motorA->read(*can_bus_service_.shared_store, param);
     this->can_bus_service_.shared_store->storeSDO(param, CANData{
       .value = value,
       .timestamp = std::chrono::system_clock::now()
     });
   }

}