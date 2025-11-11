//
// Created by ishaan on 11/11/25.
//

#pragma once
#include "IDataReceiver.h"


template <typename T>
class IDataLogger: IDataReceiver<T> {
public:
    explicit IDataLogger(std::string topic_name) : IDataReceiver<T>(topic_name){}
};


