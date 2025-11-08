#include "include/ILoggableData.h"
#include "include/LoggableData.h"
#include "include/DataLogging.h"
#include <any>
#include <memory>
#include <nlohmann/json.hpp>
#include <rclcpp/logging.hpp>
#include <rclcpp/node.hpp>
#include <rclcpp/serialized_message.hpp>
#include <string>
#include <vector>


DataLogging::DataLogging(rclcpp::Node::SharedPtr node){
    this->node = node;
    this->add_data = [this](nlohmann::json& data) {
        RCLCPP_INFO(this->node->get_logger(), "Added data %s", data.dump().c_str());
    };
}
template <typename T, typename L>
void DataLogging::logData(){
    auto logger = std::make_shared<T>(this->node, this->add_data);
    this->dataLoggers.push_back(logger);
}