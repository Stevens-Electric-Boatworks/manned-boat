//
// Created by ishaan on 11/12/25.
//

#pragma once
#include <nlohmann/json.hpp>
#include <rclcpp/node.hpp>
#include <string>

struct LogData {
    double_t timestamp;
    std::string msg;
    std::string filename;
    std::string function;
    uint32_t line;
    uint8_t level;

    friend bool operator==(const LogData &lhs, const LogData &rhs) {
        return lhs.timestamp == rhs.timestamp
               && lhs.msg == rhs.msg
               && lhs.filename == rhs.filename
               && lhs.function == rhs.function
               && lhs.line == rhs.line
               && lhs.level == rhs.level;
    }
};

struct Alarm {
    int16_t id;
    double_t timestamp;
    std::string message;

    friend bool operator==(const Alarm &lhs, const Alarm &rhs) {
        return lhs.id == rhs.id
               && lhs.timestamp == rhs.timestamp
               && lhs.message == rhs.message;
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
