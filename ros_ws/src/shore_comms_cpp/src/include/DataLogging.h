# pragma once

#include "ILoggableData.h"
#include "rclcpp/node.hpp"
#include <nlohmann/json.hpp>
class DataLogging {
public:
    explicit DataLogging(rclcpp::Node::SharedPtr node);
    template<typename T, typename L>
    void logData();


private:
    std::vector<ILoggableData> dataLoggers;  
    rclcpp::Node::SharedPtr node;
    std::function<void(nlohmann::json&)> add_data;
};