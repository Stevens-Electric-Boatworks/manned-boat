//
// Created by ishaan on 11/12/25.
//

#include "shore_comms_cpp/data_transmitter/WebsocketDataTransmitter.hpp"
#include "shore_comms_cpp/IDataTransmitter.h"
#include <chrono>

WebsocketDataTransmitter::WebsocketDataTransmitter(const rclcpp::Node::SharedPtr &node, int data_send_rate,
                                                   bool replay_mode) {
    this->node_ = node;
    this->data_send_ = data_send_rate;
    this->alarm_pub_ = std::make_shared<AlarmPublisher>(this->node_);
    this->replay_mode_ = replay_mode;
    const std::string url("wss://shore.stevenseboat.org/api");
    websocket_.setUrl(url);
    websocket_.setMaxWaitBetweenReconnectionRetries(5);
    websocket_.setOnMessageCallback([this](const std::unique_ptr<ix::WebSocketMessage> &msg) {
        if (msg->type == ix::WebSocketMessageType::Open) {
            this->connection_opened = true;
            this->opened_initally = true;
            RCLCPP_INFO(this->node_->get_logger(), "WebSocket connection established.");

            nlohmann::json j = {{"type", "ident"}, {"message", "boat"}};
            this->websocket_.send(j.dump());

            alarm_pub_->delatchAlarm(Faults::WEBSOCKET_CONNECTION_CLOSED);
            alarm_pub_->delatchAlarm(Faults::WEBSOCKET_INITIAL_CONNECTION_FAILURE);
            alarm_pub_->delatchAlarm(Faults::WEBSOCKET_NOT_OPENED);
            alarm_pub_->delatchAlarm(Faults::WEBSOCKET_IS_NOT_INITIALLY_OPENED_YET);
        } else if (msg->type == ix::WebSocketMessageType::Error) {
            this->connection_opened = false;
            RCLCPP_ERROR(this->node_->get_logger(), "WebSocket error: %s",
                         msg->errorInfo.reason.c_str());
            if (this->opened_initally) {
                alarm_pub_->publishAlarm(Faults::WEBSOCKET_INITIAL_CONNECTION_FAILURE);
            } else {
                alarm_pub_->publishAlarm(Faults::WEBSOCKET_CONNECTION_CLOSED);
            }
        } else {
            RCLCPP_WARN(this->node_->get_logger(), "Something happened? %s",
                        msg->errorInfo.reason.c_str());
        }
    });
    websocket_.start();

    const auto data_timer = node->create_wall_timer(std::chrono::milliseconds(data_send_rate), [this] {
        this->publish_data();
        this->publish_can_bus_state();
        this->publish_logs();
        this->publish_alarms();
    });
    this->timers_.push_back(data_timer);
}

void WebsocketDataTransmitter::send_data(const nlohmann::json &json) {
    this->data_.merge_patch(json);
}

void WebsocketDataTransmitter::send_log(const LogData &log_data) {
    this->logs_.push_back(log_data);
}

void WebsocketDataTransmitter::send_alarm(const Alarm &alarm) {
    this->alarms_.push_back(alarm);
}

void WebsocketDataTransmitter::send_can_bus_state(const uint8_t &can_state) {
    this->can_bus_state_ = can_state;
}

void WebsocketDataTransmitter::publish_data() {
    if (!this->data_.empty()) {
        nlohmann::json j = {
            {"type", "data"},
            {"payload", this->data_}
        };
        if (replay_mode_) {
            j.merge_patch({
                {"replay", true}
            });
        }
        websocket_.send(j.dump());
        this->data_.clear();
    }
}

void WebsocketDataTransmitter::publish_can_bus_state() {
    const nlohmann::json j = {
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
    for (auto [timestamp, msg, filename, function, line, level]: logs_) {
        logs.push_back({
            {"timestamp", timestamp},
            {"msg", msg},
            {"file", filename},
            {"function", function},
            {"line", line},
            {"level", level},
        });
    }
    j["payload"] = logs;
    this->websocket_.send(j.dump());

    if (this->connection_opened) {
        this->logs_.clear();
    }
}

void WebsocketDataTransmitter::publish_alarms() {
    if (this->alarms_.size() == 0)
        return;
    std::vector<nlohmann::json> alarms;

    for (const auto [id, timestamp, message]: alarms_) {
        nlohmann::json j = {{"type", "alarm"}, {"action", "set"}};
        const nlohmann::json payload = {
            {"id", id},
            {"timestamp", timestamp},
            {"message", message},
            {"type", "error"}
        };
        j["payload"] = payload;
        this->websocket_.send(j.dump());
    }
    if (this->connection_opened) {
        this->alarms_.clear();
    }
}





