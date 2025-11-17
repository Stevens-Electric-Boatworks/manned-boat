//
// Created by ishaan on 11/17/25.
//

#pragma once
#include "shore_comms_cpp/IDataTransmitter.h"


class MockDataTransmitter : public IDataTransmitter {
public:
    void send_alarm(const Alarm &alarm) override;
    void send_can_bus_state(const uint8_t &can_state) override;
    void send_data(const nlohmann::json &json) override;
    void send_log(const LogData &log_data) override;

    bool has(const nlohmann::json &json) const;

private:
    nlohmann::json data;
};


