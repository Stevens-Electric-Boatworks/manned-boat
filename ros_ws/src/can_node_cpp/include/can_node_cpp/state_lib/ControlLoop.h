//
// Created by Ishaan Sayal.
// Copyright (c) 2026 Stevens Electric Boatworks.

#pragma once
#include "IState.h"
#include "states/States.h"
#include <can_node_cpp/can/CANBusService.h>

namespace eboat {
    class ControlLoop {
        std::unique_ptr<IState> currentState = nullptr;

    public:
        std::shared_ptr<CANBusService> canBus;

        void initialize(rclcpp::Node *node);

        /**
         * Runs the state machine, and must be called periodically
         */
        void tickPeriodic();

        void switchTo(States state);

    private:
        std::chrono::time_point<std::chrono::steady_clock> loop_start;
    };
}
