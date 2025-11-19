//
// Created by ishaan on 11/12/25.
//

#pragma once
#include <nlohmann/json.hpp>
#include <rclcpp/node.hpp>

struct LogData {
    double_t timestamp;
    std::string msg;
    std::string filename;
    std::string function;
    uint32_t line;
    uint8_t level;
};

struct Alarm {
    int16_t id;
    double_t timestamp;

    friend bool operator==(const Alarm &lhs, const Alarm &rhs) {
        return lhs.id == rhs.id
               && lhs.timestamp == rhs.timestamp;
    }
};
class IDataTransmitter {
public:
    virtual ~IDataTransmitter() = default;
    virtual void send_data(const nlohmann::json& json) = 0;
    virtual void send_log(const LogData& log_data) = 0;
    virtual void send_can_bus_state(const uint8_t& can_state) = 0;
    virtual void send_alarm(const Alarm& alarm) = 0;
};
