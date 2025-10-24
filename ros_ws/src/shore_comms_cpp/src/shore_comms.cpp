//
// Created by ishaan on 10/23/25.
//

#include <memory>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"
#include "boat_data_interfaces/msg/inlet_coolant_data.hpp"

class MinimalSubscriber : public rclcpp::Node
{
public:
    MinimalSubscriber()
    : Node("minimal_subscriber")
    {
        RCLCPP_INFO(this->get_logger(), "Listening on 'electrical/temp_sensors/in'");
        this->log_topic<boat_data_interfaces::msg::InletCoolantData>("/electrical/temp_sensors/in", &MinimalSubscriber::electrical_data_collector);
    }

    void electrical_data_collector(const boat_data_interfaces::msg::InletCoolantData::SharedPtr msg) const {
           RCLCPP_INFO(this->get_logger(), "Saw %f", msg->inlet_temp);
    }

private:
    std::vector<std::shared_ptr<rclcpp::SubscriptionBase>> subscriptions_;

template <typename T, typename M>
    void log_topic(std::string topic_name, M callback) {
        auto bound = std::bind(callback, this, std::placeholders::_1);
        auto a = this->create_subscription<T>(topic_name, 10, bound);
        subscriptions_.push_back(a);
    }

};

int main(int argc, char * argv[])
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<MinimalSubscriber>());
    rclcpp::shutdown();
    return 0;
}