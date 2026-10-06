//
// Created by Ishaan Sayal.
// Copyright (c) 2026 Stevens Electric Boatworks.

#include <utility>

#include "can_node_cpp/monitor/MonitorLoop.h"

void eboat::MonitorLoop::initialize() const {
    this->can_bus_service.shared_store->onCANDataReceive = [this](const Motors motors,
                                                                  const MotorODParam motor_sdo_param,
                                                                  const CANData &can_data) {
        onCANDataReceive(motors, motor_sdo_param, can_data);
    };
}

void eboat::MonitorLoop::tick() const {
    proccessQueue();
}

void eboat::MonitorLoop::slowTick() const {
    publishQueueSize();
}

void eboat::MonitorLoop::publishQueueSize() const {
    const auto motorAQueue = this->can_bus_service.motorA->getQueueLength();
    const auto motorBQueue = this->can_bus_service.motorB->getQueueLength();

    auto msgA = std_msgs::msg::UInt16();
    msgA.data = motorAQueue;
    can_bus_service.shared_store->pubs->motorA->queue_size->publish(msgA);

    auto msgB = std_msgs::msg::UInt16();
    msgB.data = motorBQueue;
    can_bus_service.shared_store->pubs->motorB->queue_size->publish(msgB);
}

void eboat::MonitorLoop::onCANDataReceive(const Motors motorNum, const MotorODParam param, const CANData &data) const {
    const auto motorSub = motorNum == Motors::MOTOR_A
                              ? this->can_bus_service.shared_store->pubs->motorA.get()
                              : this->can_bus_service.shared_store->pubs->motorB.get();

    //voltage
    if (param.index == 0x2030 && param.subindex == 3) {
        auto msg = std_msgs::msg::Int16();
        msg.data = std::any_cast<int16_t>(data.value);
        motorSub->voltage->publish(msg);
    }
}

void eboat::MonitorLoop::proccessQueue() const {
    auto &queue = can_bus_service.shared_store->get_queued_reads();
    while (!queue.empty()) {
        MotorODParam param = queue.front();
        this->can_bus_service.motorA->read(param);
        this->can_bus_service.motorB->read(param);
        queue.pop();
    }
}

void eboat::MonitorLoop::addDefaultParameters() const {
    constexpr MotorODParam defaults[] = {
        {.index = 2030, .subindex = 2, .type = ODType::I16},
    };
    for (const auto param: defaults) {
        // we can safely discard the data, we are just trying to put onto the queue
        auto _ = this->can_bus_service.shared_store->getSDO(Motors::MOTOR_A, param);
        _ = this->can_bus_service.shared_store->getSDO(Motors::MOTOR_B, param);
    }
}
