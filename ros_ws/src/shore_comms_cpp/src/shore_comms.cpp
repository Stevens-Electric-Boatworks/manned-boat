//
// Created by ishaan on 10/23/25.
//

#include <memory>
#include <websocketpp/config/asio_client.hpp>
#include <websocketpp/client.hpp>
#include <memory>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"
#include "boat_data_interfaces/msg/inlet_coolant_data.hpp"

// Needs sudo apt install libwebsocketpp

typedef websocketpp::client<websocketpp::config::asio_tls_client> client;

class ShoreCommsNode : public rclcpp::Node {
public:
    ShoreCommsNode()
        : Node("minimal_subscriber") {
        this->configureWebsocket();

        this->log_topic<boat_data_interfaces::msg::InletCoolantData>("/electrical/temp_sensors/in",
                                                                     &ShoreCommsNode::electrical_data_collector);
    }


    void electrical_data_collector(const boat_data_interfaces::msg::InletCoolantData::SharedPtr msg) {
        RCLCPP_INFO(this->get_logger(), "Sending %f to websocket", msg->inlet_temp);
        this->websocket.send(hdl, "TEST!!!", websocketpp::frame::opcode::text);
    }

private:
    ix::WebSocket ws;
    client websocket;
    std::vector<std::shared_ptr<rclcpp::SubscriptionBase> > subscriptions_;

    void configureWebsocket() {
        websocket.clear_access_channels(websocketpp::log::alevel::all);
        websocket.clear_error_channels(websocketpp::log::elevel::all);

        websocket.init_asio();
        websocket.start_perpetual();

        websocketpp::lib::error_code ec;
        const std::string uri = "wss://shore.stevenseboat.org/api";
        const client::connection_ptr ptr = websocket.get_connection(uri, ec);

        if (ec) {
            RCLCPP_ERROR(this->get_logger(), "Unable to open the Websocket because '%d'", ec.value());
            RCLCPP_ERROR(this->get_logger(), "Human Readable Error Message: '%s'", ec.message().c_str());

            return;
        }
        websocket.connect(ptr);

        websocket.set_message_handler([&](websocketpp::connection_hdl, client::message_ptr msg) {
            std::cout << "Received: " << msg->get_payload() << std::endl;
        });
        websocket.set_open_handler([&](websocketpp::connection_hdl hdl) {
            this->hdl = hdl;
        });
        websocket.set_tls_init_handler([](websocketpp::connection_hdl hdl) {
            auto ctx = std::make_shared<boost::asio::ssl::context>(boost::asio::ssl::context::sslv23);
            try {
                ctx->set_default_verify_paths(); // system CA
                ctx->set_verify_mode(boost::asio::ssl::verify_peer);
            } catch (const std::exception &e) {
                std::cerr << "TLS context setup failed: " << e.what() << std::endl;
            }
            return ctx;
        });
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
