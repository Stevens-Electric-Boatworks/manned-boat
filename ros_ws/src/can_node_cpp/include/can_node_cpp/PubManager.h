//
// Created by Ishaan Sayal.
// Copyright (c) 2026 Stevens Electric Boatworks.

#pragma once
#include <rclcpp/node.hpp>
#include <std_msgs/msg/int16.hpp>

namespace eboat
{

    class MotorPubs
    {
        //0x2030:2 // EXAMPLE

    public:
        rclcpp::Publisher<std_msgs::msg::Int16>::SharedPtr voltage;
        explicit MotorPubs(rclcpp::Node* node, const std::string& motorName)
        {
            voltage = node->create_publisher<std_msgs::msg::Int16>("/" + motorName + "/voltage", 10);
        }
    };

    class PubManger
    {
        rclcpp::Node* node;
    public:
        std::unique_ptr<MotorPubs> motorA;
        std::unique_ptr<MotorPubs> motorB;

        rclcpp::Publisher<std_msgs::msg::Int16>::SharedPtr genericStatus;

        explicit PubManger(rclcpp::Node* node)
        {
            this->node = node;
            genericStatus = node->create_publisher<std_msgs::msg::Int16>("/can_subsystem/generic_status", 10);
            motorA = std::make_unique<MotorPubs>(node, "motor_a");
            motorB = std::make_unique<MotorPubs>(node, "motor_b");

        }


    };
}
