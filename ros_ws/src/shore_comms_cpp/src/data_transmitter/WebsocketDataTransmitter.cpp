//
// Created by ishaan on 11/12/25.
//

#include "shore_comms_cpp/data_transmitter/WebsocketDataTransmitter.hpp"
#include "shore_comms_cpp/IDataTransmitter.h"
#include <chrono>

WebsocketDataTransmitter::WebsocketDataTransmitter(const rclcpp::Node::SharedPtr &node, int data_send){
    this->node_ = node;
    this->data_send = data_send;
    this->alarm_pub = std::make_shared<AlarmPublisher>(this->node_);
    const std::string url("wss://shore.stevenseboat.org/api");
    websocket_.setUrl(url);
    websocket_.setMaxWaitBetweenReconnectionRetries(5);
    websocket_.setOnMessageCallback([this](const std::unique_ptr<ix::WebSocketMessage>& msg) {
        if (msg->type == ix::WebSocketMessageType::Open) {
          this->connection_opened = true;
          this->opened_initally = true;
          RCLCPP_INFO(this->node_->get_logger(), "WebSocket connection established.");

          nlohmann::json j = {{"type", "ident"}, {"message", "boat"}};
          this->websocket_.send(j.dump());

          alarm_pub->delatchAlarm(Faults::WEBSOCKET_CONNECTION_CLOSED);
          alarm_pub->delatchAlarm(Faults::WEBSOCKET_INITIAL_CONNECTION_FAILURE);
          alarm_pub->delatchAlarm(Faults::WEBSOCKET_NOT_OPENED);
          alarm_pub->delatchAlarm(Faults::WEBSOCKET_IS_NOT_INITIALLY_OPENED_YET);
        } else if (msg->type == ix::WebSocketMessageType::Error) {
          this->connection_opened = false;
          RCLCPP_ERROR(this->node_->get_logger(), "WebSocket error: %s",
                       msg->errorInfo.reason.c_str());
          if (this->opened_initally) {
            alarm_pub->publishAlarm(Faults::WEBSOCKET_INITIAL_CONNECTION_FAILURE);
          } else {
            alarm_pub->publishAlarm(Faults::WEBSOCKET_CONNECTION_CLOSED);
          }
        } else {
          RCLCPP_WARN(this->node_->get_logger(), "Something happened? %s",
                      msg->errorInfo.reason.c_str());
        }
    });
    websocket_.start();

    auto data_timer = node->create_wall_timer(std::chrono::milliseconds(100), [this]() {
        this->publish_data();
        this->publish_can_bus_state();
        this->publish_logs();
    });
    this->timers_.push_back(data_timer);
}
void WebsocketDataTransmitter::send_data(const nlohmann::json &json) {
    this->data_.merge_patch(json);
}

void WebsocketDataTransmitter::send_log(const LogData& log) {
    this->logs_.push_back(log);
}

void WebsocketDataTransmitter::send_can_bus_state(const uint8_t& can_state) {
    // nlohmann::json j = {
    //     {"type", "can_bus"},
    //     {"state", can_state}
    // };
    // RCLCPP_INFO(this->node->get_logger(), "Sending: %s", j.dump().c_str());
    // websocket.send(j.dump());
    this->can_bus_state_ = can_state;
}

void WebsocketDataTransmitter::publish_data() {
    if (!this->data_.empty()) {
        nlohmann::json j = {
            {"type", "data"},
            {"payload", this->data_}
        };
        websocket_.send(j.dump());
        this->data_.clear();
    }
}

void WebsocketDataTransmitter::publish_can_bus_state() {
    nlohmann::json j = {
        {"type", "can_bus"},
        {"state", this->can_bus_state_}
    };
    this->websocket_.send(j.dump());
}

void WebsocketDataTransmitter::publish_logs() {
    if (this->logs_.size() == 0)
      return;
    std::vector<nlohmann::json> logs;
    nlohmann::json j = {{"type", "log"}};
    for (LogData data : logs_) {
      logs.push_back({
          {"timestamp", data.timestamp},
          {"msg", data.msg},
          {"file", data.filename},
          {"function", data.function},
          {"line", data.line},
          {"level", data.level},
      });
    }
    j["payload"] = logs;
    this->websocket_.send(j.dump());

    if(this->connection_opened) {
        this->logs_.clear();
    }
}

void WebsocketDataTransmitter::publish_alarms() {

}





