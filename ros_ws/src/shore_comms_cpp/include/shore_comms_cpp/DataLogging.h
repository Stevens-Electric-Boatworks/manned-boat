# pragma once

#include "ILoggableData.h"
#include "rclcpp/node.hpp"
#include <nlohmann/json.hpp>
class DataLogging {
public:
    explicit DataLogging(const rclcpp::Node::SharedPtr& node) {
        this->node = node;
        this->add_data = [this](nlohmann::json& data) {
            RCLCPP_INFO(this->node->get_logger(), "Added data %s", data.dump().c_str());
        };
    }
    template<typename T, typename L>
    void logData() {
        auto logger = std::make_shared<L>(this->node, this->add_data);
        this->dataLoggers.push_back(logger);
    }


private:
    std::vector<ILoggableData> dataLoggers;  
    rclcpp::Node::SharedPtr node;
    std::function<void(nlohmann::json&)> add_data;
};