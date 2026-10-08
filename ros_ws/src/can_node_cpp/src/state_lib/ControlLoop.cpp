//
// Created by Ishaan Sayal.
// Copyright (c) 2026 Stevens Electric Boatworks.

#include "can_node_cpp/state_lib/ControlLoop.h"

#include "can_node_cpp/state_lib/states/InitializationState.h"
#include "can_node_cpp/state_lib/states/StandbyState.h"

void eboat::ControlLoop::initialize(rclcpp::Node *node) {
    if (this->canBus == nullptr) {
        this->canBus = std::make_shared<CANBusService>(node);
        this->canBus->initBus();
    }
    if (currentState == nullptr) {
        currentState = std::make_unique<InitializationState>(*canBus, [this](const States s) {
            switchTo(s);
        });
        currentState->onSwitch();
    }
}

#include <rclcpp/logging.hpp>

void eboat::ControlLoop::tickPeriodic() {
    loop_start = std::chrono::steady_clock::now();
    if (this->currentState == nullptr) {
        RCLCPP_ERROR(this->canBus->shared_store->logger, "Current state is null!");
        return;
    }
    this->currentState->periodic();

    auto diff_microsecs = std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::steady_clock::now() - loop_start);
    auto msg = std_msgs::msg::UInt32();
    msg.data = diff_microsecs.count();
    this->canBus->shared_store->pubs->controlLoopTime->publish(msg);
}

void eboat::ControlLoop::switchTo(const States state) {
    if (state == States::STANDBY) {
        auto newState  = std::make_unique<StandbyState>(*canBus, [this](const States s) {
            switchTo(s);
        });
        if (!newState->isValid()) {
            RCLCPP_INFO(this->canBus->shared_store->logger, "Attempted to switch to STANDBY State but invalid.");
            newState = nullptr; // release
            return;
        }
        currentState->cleanup();
        currentState = std::move(newState);
        currentState->onSwitch();
        RCLCPP_INFO(this->canBus->shared_store->logger, "Switched to STANDBY State");
    }
}
