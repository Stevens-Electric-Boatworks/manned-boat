//
// Created by ishaan on 11/11/25.
//

#pragma once
#include <string>

#include "IDataLogger.hpp"

template<typename T>
struct IDataReceiver {
    virtual ~IDataReceiver(){}

    explicit IDataReceiver(std::string topic_name){};

    virtual void on_data(std::function<void(T data)>) = 0;

    std::string topic_name;
};
