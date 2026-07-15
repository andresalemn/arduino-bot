cmake_minimum_required(VERSION 3.8)
project(arduinobot_controller)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Find dependencies
find_package(ament_cmake REQUIRED)
find_package(ament_cmake_python REQUIRED)
find_package(trajectory_msgs REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(rclcpp REQUIRED)
find_package(rclpy REQUIRED)

# C++ slider control node
add_executable(slider_control_cpp src/slider_control.cpp)
ament_target_dependencies(slider_control_cpp rclcpp trajectory_msgs sensor_msgs)

# Install Python package (module only)
ament_python_install_package(${PROJECT_NAME})

# Install Python executable - RENAME removes the .py so ROS2 finds it as 'slider_control'
install(
  PROGRAMS
    arduinobot_controller/slider_control.py
  RENAME slider_control
  DESTINATION lib/${PROJECT_NAME}
)

install(
  TARGETS slider_control_cpp
  DESTINATION lib/${PROJECT_NAME}
)

install(
  DIRECTORY config launch
  DESTINATION share/${PROJECT_NAME}
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