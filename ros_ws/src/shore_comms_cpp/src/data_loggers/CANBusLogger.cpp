//
// Created by ishaan on 11/11/25.
//


#include <boat_data_interfaces/msg/can_bus_status.hpp>

#include "shore_comms_cpp/IDataLogger.h"
#include "shore_comms_cpp/IDataReceiver.h"

using type = boat_data_interfaces::msg::CANBusStatus;
class CANBusLogger: IDataLogger<type> {
public:
    CANBusLogger() : IDataLogger<type>("/electrical/can_data") {

    }

};