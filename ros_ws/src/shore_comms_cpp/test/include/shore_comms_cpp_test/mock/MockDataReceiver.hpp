//
// Created by ishaan on 11/17/25.
//

#pragma once

#include "shore_comms_cpp/IDataReceiver.hpp"

template<typename T>
class MockDataReceiver: public IDataReceiver<T> {
public:
    explicit MockDataReceiver() : IDataReceiver<T>("/test") {

    }
    void on_data(T data) override {
        this->callback(data);
    }
};


