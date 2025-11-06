//
// Created by ishaan on 11/6/25.
//

#ifndef BUILD_ALARMPUBLISHER_H
#define BUILD_ALARMPUBLISHER_H
#include "rclcpp/node.hpp"

class AlarmPublisher {
public:
    explicit AlarmPublisher(const rclcpp::Node* node);
};
#endif //BUILD_ALARMPUBLISHER_H