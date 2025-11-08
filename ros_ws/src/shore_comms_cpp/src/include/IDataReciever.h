#pragma once

#include <nlohmann/json_fwd.hpp>
class IDataReciever{
public:
    void on_data_recieve(nlohmann::json jsonData);
};