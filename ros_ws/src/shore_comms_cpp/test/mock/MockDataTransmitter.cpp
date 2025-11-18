//
// Created by ishaan on 11/17/25.
//

#include "shore_comms_cpp_test/mock/MockDataTransmitter.hpp"

#include "gtest/gtest.h"

void MockDataTransmitter::send_alarm(const Alarm &alarm) {
}

void MockDataTransmitter::send_can_bus_state(const uint8_t &can_state) {
    this->can_bus_state_ = can_state;
}

void MockDataTransmitter::send_data(const nlohmann::json &json) {
    this->data.merge_patch(json);
}

void MockDataTransmitter::send_log(const LogData &log_data) {
}

void MockDataTransmitter::assert_has_can_status(const uint8_t can_state) const {
    ASSERT_EQ(this->can_bus_state_, can_state);
}

void MockDataTransmitter::assert_has_data(const nlohmann::json &json) const {
    ASSERT_EQ(this->data, json);
}

