//
// Created by ishaan on 11/12/25.
//

#pragma once
#include "boat_data_interfaces/msg/can_bus_status.hpp"
#include "shore_comms_cpp/IDataTransmitter.h"
#include "ixwebsocket/IXWebSocket.h"
#include <boat_common_libs_cpp/AlarmPublisher.hpp>
#include <cstdint>

class WebsocketDataTransmitter : public IDataTransmitter {
public:
    explicit WebsocketDataTransmitter(const rclcpp::Node::SharedPtr &node, int data_send);
    void send_data(const nlohmann::json& json) override;
    void send_can_bus_state(const uint8_t &can_state) override;
    void send_log(const LogData& log_data) override;
    void send_alarm(const Alarm &alarm) override;

private:
    void publish_data();
    void publish_can_bus_state();
    void publish_alarms();
    void publish_logs();
    
    std::shared_ptr<AlarmPublisher> alarm_pub;
    ix::WebSocket websocket_;
    rclcpp::Node::SharedPtr node_;
    std::vector<LogData> logs_;
    std::vector<Alarm> alarms_;
    nlohmann::json data_;
    bool replay_mode_ = false;
    uint8_t can_bus_state_ = boat_data_interfaces::msg::CANBusStatus::OFFLINE;
    std::vector<std::shared_ptr<rclcpp::TimerBase> > timers_;
    
    int data_send;

    bool connection_opened = false;
    bool opened_initally = false;
};
