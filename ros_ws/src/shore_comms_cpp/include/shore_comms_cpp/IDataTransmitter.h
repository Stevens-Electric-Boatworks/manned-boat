//
// Created by ishaan on 11/12/25.
//

#pragma once
#include <nlohmann/json.hpp>
#include <rclcpp/node.hpp>

class IDataTransmitter {
public:
    virtual ~IDataTransmitter() = default;
    virtual void send_data(const nlohmann::json& json) = 0;
};
