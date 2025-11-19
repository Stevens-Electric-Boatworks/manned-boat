//
// Created by ishaan on 11/17/25.
//

#pragma once
#include "shore_comms_cpp/IDataTransmitter.h"
#include "shore_comms_cpp/data_loggers/CANBusStateLogger.hpp"


class FakeDataTransmitter : public IDataTransmitter {
public:
    void send_alarm(const Alarm &alarm) override;
    void send_can_bus_state(const uint8_t &can_state) override;
    void send_data(const nlohmann::json &json) override;
    void send_log(const LogData &log_data) override;

    void assert_has_data(const nlohmann::json &json) const;

    void assert_has_can_status(uint8_t can_state) const;

    void assert_has_alarm(const Alarm &alarm, int amount);

    void assert_has_log(const LogData& log_data, int amount);

private:
    nlohmann::json data;
    uint8_t can_bus_state_ = 0;
    std::vector<Alarm> alarms_;
    std::vector<LogData> logs_;
};


