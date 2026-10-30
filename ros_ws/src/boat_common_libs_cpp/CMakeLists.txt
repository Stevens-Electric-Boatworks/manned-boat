cmake_minimum_required(VERSION 3.8)
project(boat_common_libs_cpp)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()
if (NOT TARGET sockcanpp)
  # C++11 support
  set(sockcanpp_CONCEPT_SUPPORT OFF CACHE BOOL "Force C++ standard back to 11")

  add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../libsockcanpp ${CMAKE_CURRENT_SOURCE_DIR}/build/libsockcanpp)
endif()
# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(boat_data_interfaces REQUIRED)

####################################
# CPP Shared Library Block [BEGIN] #
# vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv #
# https://ros2-tutorial.readthedocs.io/en/latest/
# The most common use case is to merge everything you need to export
# into the same shared library called ${PROJECT_NAME}.
add_library(${PROJECT_NAME} SHARED
        src/AlarmPublisher.cpp
        src/can/CanDeviceManager.cpp
        src/can/CanIo.cpp
        src/can/devices/EBoatCANDevice.cpp
        src/can/devices/eboat/ExampleCanNode.cpp
)

target_link_libraries(boat_common_libs_cpp PUBLIC
        rclcpp::rclcpp
        ${boat_data_interfaces_TARGETS}
        sockcanpp
)

target_include_directories(${PROJECT_NAME}
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

ament_export_targets(export_${PROJECT_NAME} HAS_LIBRARY_TARGET)
ament_export_dependencies(
        rclcpp

)

install(
        DIRECTORY include/
        DESTINATION include
)

install(
        TARGETS ${PROJECT_NAME}
        EXPORT export_${PROJECT_NAME}
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
        INCLUDES DESTINATION include
)
# ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ #
# CPP Shared Library Block [END] #
##################################

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

ament_package(CONFIG_EXTRAS cmake/boat_common_libs_cpp-extras.cmake)