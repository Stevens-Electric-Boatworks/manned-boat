//
// Created by ishaan on 11/11/25.
//

#pragma once
#include <functional>
#include <string>

template<typename T>
class IDataReceiver {
public:
    virtual ~IDataReceiver() = default;

    explicit IDataReceiver(std::string& topic_name, std::function<void(T)>& callback);

    virtual void send_data(T);
};
