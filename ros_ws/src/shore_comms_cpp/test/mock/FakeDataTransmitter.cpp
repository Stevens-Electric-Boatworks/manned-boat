//
// Created by ishaan on 11/17/25.
//

#include "shore_comms_cpp_test/mock/FakeDataTransmitter.hpp"

#include "gtest/gtest.h"

void FakeDataTransmitter::send_alarm(const Alarm &alarm) {
    this->alarms_.push_back(alarm);
}

void FakeDataTransmitter::send_can_bus_state(const uint8_t &can_state) {
    this->can_bus_state_ = can_state;
}

void FakeDataTransmitter::send_data(const nlohmann::json &json) {
    this->data.merge_patch(json);
}

void FakeDataTransmitter::send_log(const LogData &log_data) {
    this->logs_.push_back(log_data);
}

void FakeDataTransmitter::assert_has_can_status(const uint8_t can_state) const {
    ASSERT_EQ(this->can_bus_state_, can_state);
}

void FakeDataTransmitter::assert_has_alarm(const Alarm &alarm, const int amount) {
    ASSERT_EQ(this->alarms_.size(), amount);
    ASSERT_TRUE(std::find(alarms_.begin(), alarms_.end(), alarm) != alarms_.end());
}

void FakeDataTransmitter::assert_has_log(const LogData &log_data, const int amount) {
    ASSERT_EQ(this->logs_.size(), amount);
    ASSERT_TRUE(std::find(logs_.begin(), logs_.end(), log_data) != logs_.end());
}

void FakeDataTransmitter::assert_has_data(const nlohmann::json &json) const {
    ASSERT_EQ(this->data, json) << "Data: " << this->data.dump();
}


