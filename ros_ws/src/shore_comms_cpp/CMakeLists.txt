cmake_minimum_required(VERSION 3.8)
project(shore_comms_cpp)

if (CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    add_compile_options(-Wall -Wextra -Wpedantic)
endif ()
set(shore_comms_cpp_srcs
        src/data_receivers/ROSDataReceiver.cpp
        src/data_loggers/CANBusStateLogger.cpp
        src/data_loggers/MotorDataLogger.cpp
        src/data_loggers/BoatTimeLogger.cpp
        src/data_loggers/ROSOutLogger.cpp
        src/data_transmitter/WebsocketDataTransmitter.cpp
        src/DataLoggers.cpp
)
add_executable(shore src/shore_comms.cpp ${shore_comms_cpp_srcs})
target_include_directories(shore PUBLIC include/)

set(USE_TLS ON CACHE BOOL "Enable TLS support for IXWebSocket")

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)
find_package(boat_data_interfaces REQUIRED)
find_package(boat_common_libs_cpp REQUIRED)

add_subdirectory(${CMAKE_SOURCE_DIR}/../IXWebSocket IXWebSocket_build)

# Create library from sources
target_link_libraries(shore
        PUBLIC
        rclcpp::rclcpp
        ${std_msgs_TARGETS}
        ${boat_data_interfaces_TARGETS}
        ${boat_common_libs_cpp_TARGETS}
        ixwebsocket
)

install(TARGETS shore DESTINATION lib/${PROJECT_NAME})

ament_package()
