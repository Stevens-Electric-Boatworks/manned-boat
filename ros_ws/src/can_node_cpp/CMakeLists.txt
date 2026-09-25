cmake_minimum_required(VERSION 3.20)
project(can_node_cpp)
set(CMAKE_CXX_STANDARD 17)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)


# LelyCANOpen Start

find_package(PkgConfig REQUIRED)
pkg_check_modules(LELY REQUIRED
        liblely-coapp
        liblely-co
        liblely-io2
        liblely-ev
        liblely-can
        liblely-util
)

# LelyCANOpen END

add_executable(can_node src/can_node_cpp.cpp
        src/state_lib/states/StandbyState.cpp
        src/can/CANMotor.cpp
        src/can/CANBusService.cpp
        src/state_lib/states/InitializationState.cpp
        src/state_lib/ControlLoop.cpp
        src/monitor/MonitorLoop.cpp
        src/can/SharedStore.cpp
        include/can_node_cpp/can/MotorSDOParam.h
)
target_link_libraries(can_node PUBLIC rclcpp::rclcpp ${std_msgs_TARGETS} ${LELY_LIBRARIES})
target_include_directories(
        can_node
        PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include"
        PRIVATE ${LELY_INCLUDE_DIRS}
)
install(TARGETS
        can_node
        DESTINATION lib/${PROJECT_NAME}
)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  # the following line skips the linter which checks for copyrights
  # comment the line when a copyright and license is added to all source files
  set(ament_cmake_copyright_FOUND TRUE)
  # the following line skips cpplint (only works in a git repo)
  # comment the line when this package is in a git repo and when
  # a copyright and license is added to all source files
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()
