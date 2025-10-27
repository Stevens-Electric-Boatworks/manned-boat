//
// Created by ishaan on 10/23/25.
//

#include <memory>
#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/client.hpp>
#include <memory>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"
#include "boat_data_interfaces/msg/inlet_coolant_data.hpp"

// Needs sudo apt install libwebsocketpp

typedef websocketpp::client<websocketpp::config::asio_client> client;
class ShoreCommsNode : public rclcpp::Node
{
public:
    ShoreCommsNode()
    : Node("minimal_subscriber")
    {
        this->configureWebsocket();

        this->log_topic<boat_data_interfaces::msg::InletCoolantData>("/electrical/temp_sensors/in", &ShoreCommsNode::electrical_data_collector);
    }


    void electrical_data_collector(const boat_data_interfaces::msg::InletCoolantData::SharedPtr msg) const {
           RCLCPP_INFO(this->get_logger(), "Saw %f", msg->inlet_temp);
    }

private:
    client websocket;
    websocketpp::lib::shared_ptr<websocketpp::lib::thread> m_thread;
    std::vector<std::shared_ptr<rclcpp::SubscriptionBase>> subscriptions_;

    void configureWebsocket() {
        websocket.clear_access_channels(websocketpp::log::alevel::all);
        websocket.clear_error_channels(websocketpp::log::elevel::all);

        websocket.init_asio();
        websocket.start_perpetual();

        m_thread = std::make_shared<websocketpp::lib::thread>(&client::run, &websocket);
    }

template <typename T, typename M>
    void log_topic(std::string topic_name, M callback) {
        auto bound = std::bind(callback, this, std::placeholders::_1);
        auto a = this->create_subscription<T>(topic_name, 10, bound);
        subscriptions_.push_back(a);
        RCLCPP_INFO(this->get_logger(), "Listening on '%s'", topic_name.c_str());
    }

};

int main(int argc, char * argv[])
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<ShoreCommsNode>());
    rclcpp::shutdown();
    return 0;
}