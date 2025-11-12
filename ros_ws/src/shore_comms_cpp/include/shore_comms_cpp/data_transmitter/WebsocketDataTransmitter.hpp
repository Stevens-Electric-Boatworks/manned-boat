//
// Created by ishaan on 11/12/25.
//

#pragma once
#include "shore_comms_cpp/IDataTransmitter.h"
#include "ixwebsocket/IXWebSocket.h"

class WebsocketDataTransmitter : public IDataTransmitter {
public:
    explicit WebsocketDataTransmitter(const rclcpp::Node::SharedPtr &node);
    void send_data(const nlohmann::json& json) override;

private:
    ix::WebSocket websocket;
    rclcpp::Node::SharedPtr node;
};
