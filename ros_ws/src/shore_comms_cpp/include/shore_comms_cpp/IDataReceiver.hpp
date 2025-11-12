//
// Created by ishaan on 11/11/25.
//

#pragma once
#include <functional>
#include <string>

struct IDataReceiverBase {
    virtual ~IDataReceiverBase() = default;
};


template<typename T>
struct IDataReceiver : public IDataReceiverBase {
    virtual ~IDataReceiver(){}

    explicit IDataReceiver(const std::string& topic_name) : topic_name(topic_name){};

    virtual void on_data(T) = 0;

    virtual void set_callback(std::function<void(T)> callback) {
        this->callback = callback;
    }

    std::string topic_name;
    std::function<void(T)> callback;
};
