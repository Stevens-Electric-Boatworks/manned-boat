cmake_minimum_required(VERSION 3.8)
project(shore_comms_cpp)

if (CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    add_compile_options(-Wall -Wextra -Wpedantic)
endif ()
add_executable(shore src/shore_comms.cpp)
set(USE_TLS ON CACHE BOOL "Enable TLS support for IXWebSocket")
# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)
find_package(boat_data_interfaces REQUIRED)
find_package(boat_common_libs_cpp REQUIRED)
add_subdirectory(${CMAKE_SOURCE_DIR}/../IXWebSocket IXWebSocket_build)

# uncomment the following section in order to fill in
# further dependencies manually.
# find_package(<dependency> REQUIRED)

target_link_libraries(shore
    PUBLIC
        rclcpp::rclcpp
        ${std_msgs_TARGETS}
        ${boat_data_interfaces_TARGETS}
        ${boat_common_libs_cpp_TARGETS}
        ixwebsocket
)
target_include_directories(shore PUBLIC include)
target_include_directories(shore PUBLIC src)


install(TARGETS
        shore
        DESTINATION lib/${PROJECT_NAME})

if (BUILD_TESTING)
    find_package(ament_lint_auto REQUIRED)
    # the following line skips the linter which checks for copyrights
    # comment the line when a copyright and license is added to all source files
    set(ament_cmake_copyright_FOUND TRUE)
    # the following line skips cpplint (only works in a git repo)
    # comment the line when this package is in a git repo and when
    # a copyright and license is added to all source files
    set(ament_cmake_cpplint_FOUND TRUE)
    ament_lint_auto_find_test_dependencies()
endif ()

ament_package()
