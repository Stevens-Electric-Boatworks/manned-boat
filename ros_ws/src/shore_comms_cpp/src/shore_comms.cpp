//
// Created by ishaan on 10/23/25.
//

#include <memory>

#include "ixwebsocket/IXWebSocket.h"
#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"
#include "boat_data_interfaces/msg/inlet_coolant_data.hpp"

// Needs sudo apt install libwebsocketpp

class ShoreCommsNode : public rclcpp::Node {
public:
    ShoreCommsNode()
        : Node("minimal_subscriber") {

        auto timer_callback =
              [this]() -> void {
                  this->send_websocket_data();
        };
        auto timer_ = this->create_wall_timer(std::chrono::milliseconds(500), timer_callback);
        this->timers_.push_back(timer_);
        this->configureWebsocket();

        // this->log_topic<boat_data_interfaces::msg::InletCoolantData>("/electrical/temp_sensors/in",
        //                                                              &ShoreCommsNode::electrical_data_collector);
    }

    //
    // void electrical_data_collector(const boat_data_interfaces::msg::InletCoolantData::SharedPtr msg) {
    //     RCLCPP_INFO(this->get_logger(), "Sending %f to websocket", msg->inlet_temp);
    // }

private:
    std::vector<std::shared_ptr<rclcpp::TimerBase>> timers_;
    std::vector<std::shared_ptr<rclcpp::SubscriptionBase>> subscriptions_;
    ix::WebSocket websocket;
    bool connectionOpened = false;

    void configureWebsocket() {
        const std::string url("wss://shore.stevenseboat.org/api");
        websocket.setUrl(url);
        RCLCPP_INFO(this->get_logger(), "Connection to URL '%s'", url.c_str());
        websocket.setOnMessageCallback([this](const ix::WebSocketMessagePtr &msg) {
                if (msg->type == ix::WebSocketMessageType::Message) {
                    RCLCPP_INFO(this->get_logger(), "received message: %s", msg->str.c_str());
                } else if (msg->type == ix::WebSocketMessageType::Open) {
                    this->connectionOpened = true;
                    RCLCPP_INFO(this->get_logger(), "Connection established");
                } else if (msg->type == ix::WebSocketMessageType::Error) {
                    // Maybe SSL is not configured properly
                    RCLCPP_ERROR(this->get_logger(), "Connection error: %s", msg->errorInfo.reason.c_str());
                }
            }
        );
        websocket.start();
    }

    void send_websocket_data() {
        if (this->connectionOpened) {
            websocket.send("{\"type\":\"data\",\"payload\":{\"speed\":7,\"boat_time\":71727162638.2}, \"replay\": true}");
        }
        else {
            RCLCPP_INFO(this->get_logger(), "Websocket is not opened yet.");
        }
    }

    template<typename T, typename M>
    void log_topic(std::string topic_name, M callback) {
        auto bound = std::bind(callback, this, std::placeholders::_1);
        auto a = this->create_subscription<T>(topic_name, 10, bound);
        subscriptions_.push_back(a);
        RCLCPP_INFO(this->get_logger(), "Listening on '%s'", topic_name.c_str());
    }
};

int main(int argc, char *argv[]) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<ShoreCommsNode>());
    rclcpp::shutdown();
    return 0;
}
