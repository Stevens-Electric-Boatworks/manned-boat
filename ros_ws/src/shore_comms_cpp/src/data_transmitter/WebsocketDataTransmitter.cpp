//
// Created by ishaan on 11/12/25.
//

#include "shore_comms_cpp/data_transmitter/WebsocketDataTransmitter.hpp"



WebsocketDataTransmitter::WebsocketDataTransmitter(const rclcpp::Node::SharedPtr &node){
    this->node = node;
    const std::string url("wss://shore.stevenseboat.org/api");
    websocket.setUrl(url);
    websocket.setMaxWaitBetweenReconnectionRetries(5);
    websocket.start();
}

void WebsocketDataTransmitter::send_data(const nlohmann::json &json) {
    websocket.send(json.dump());
}





