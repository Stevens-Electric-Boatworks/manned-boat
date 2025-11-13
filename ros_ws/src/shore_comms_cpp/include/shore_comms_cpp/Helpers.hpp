#include "builtin_interfaces/msg/time.hpp"
static double get_time_from_msg(const builtin_interfaces::msg::Time &time) {
    return time.sec * 1000.0 + time.nanosec / 1.0e6;
}