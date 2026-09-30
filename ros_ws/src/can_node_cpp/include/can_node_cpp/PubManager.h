//
// Created by Ishaan Sayal.
// Copyright (c) 2026 Stevens Electric Boatworks.

#pragma once
#include <rclcpp/node.hpp>
#include <std_msgs/msg/int16.hpp>
#include <std_msgs/msg/u_int16.hpp>
#include <utility>

namespace eboat
{

    class MotorPubs
    {
        //0x2030:2 // EXAMPLE

    public:
        rclcpp::Publisher<std_msgs::msg::Int16>::SharedPtr voltage;
        rclcpp::Publisher<std_msgs::msg::UInt16>::SharedPtr queue_size;
        explicit MotorPubs(rclcpp::Node* node, std::string  motorName) : motorName(std::move(motorName))
        {
            this->node = node;
            voltage = createPub<std_msgs::msg::Int16>("voltage");
            queue_size = createPub<std_msgs::msg::UInt16>("sdo_queue_size");
        }
    private:
        rclcpp::Node* node;
        std::string motorName;
        template <typename T>
        [[nodiscard]] std::shared_ptr<rclcpp::Publisher<T>> createPub(const std::string& topicName)
        {
            return node->create_publisher<T>("/" + motorName + "/" + topicName, 10);
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
