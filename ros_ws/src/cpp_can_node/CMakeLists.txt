cmake_minimum_required(VERSION 3.20)
project(cpp_can_node)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(std_msgs REQUIRED)
find_package(boat_data_interfaces REQUIRED)
find_package(boat_common_libs_cpp REQUIRED)

add_executable(cpp_can_node
        src/main.cpp
)

target_link_libraries(cpp_can_node PUBLIC rclcpp::rclcpp ${std_msgs_TARGETS} ${boat_common_libs_cpp_TARGETS})
target_include_directories(
        cpp_can_node
        PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include"
)
install(TARGETS
        cpp_can_node
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
