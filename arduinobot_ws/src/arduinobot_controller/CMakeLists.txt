cmake_minimum_required(VERSION 3.8)
project(arduinobot_controller)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Find dependencies
find_package(ament_cmake REQUIRED)
find_package(ament_cmake_python REQUIRED)
# Find ROS 2 dependencies
find_package(trajectory_msgs REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(rclcpp REQUIRED)
find_package(rclpy REQUIRED)
find_package(hardware_interface REQUIRED)
find_package(pluginlib REQUIRED)
find_package(rclcpp_lifecycle REQUIRED)

# Find LibSerial package using pkg-config
find_package(PkgConfig)
pkg_check_modules(SERIAL libserial)

# C++ slider control node
add_executable(slider_control_cpp src/slider_control.cpp)
ament_target_dependencies(slider_control_cpp rclcpp trajectory_msgs sensor_msgs)

# Build the Arduinobot ros2_control SystemInterface plugin library
add_library(${PROJECT_NAME} SHARED 
  src/arduinobot_interface.cpp
)

# Include directories for the library, including LibSerial's include dirs
target_include_directories(${PROJECT_NAME} PRIVATE 
  include
  "${SERIAL_INCLUDE_DIRS}"
)

# Link LibSerial libraries
target_link_libraries(${PROJECT_NAME}
 "${SERIAL_LDFLAGS}")

# Manage ROS 2 package dependencies for the library target
ament_target_dependencies(${PROJECT_NAME}
  hardware_interface
  pluginlib
  rclcpp
  rclcpp_lifecycle
)

# Export the ros2_control plugin description XML file
pluginlib_export_plugin_description_file(hardware_interface arduinobot_controller.xml)

# Install Python package (module only)
ament_python_install_package(${PROJECT_NAME})

# Install Python scripts
install(
  PROGRAMS
    arduinobot_controller/slider_control.py
  # RENAME slider_control
  DESTINATION lib/${PROJECT_NAME}
)

# Install the hardware interface shared library
install(
  TARGETS ${PROJECT_NAME}
  DESTINATION lib
)

# Install the public headers
install(
  DIRECTORY include
  DESTINATION include
)

# Install the C++ slider control executable
install(
  TARGETS slider_control_cpp
  DESTINATION lib/${PROJECT_NAME}
)

# Install config, launch, and XML plugin description files
install(
  DIRECTORY config launch
  DESTINATION share/${PROJECT_NAME}
)

install(
  FILES arduinobot_controller.xml
  DESTINATION share/${PROJECT_NAME}
)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  # the following line skips the linter which checks for copyrights
  # comment the line when a copyright and license is added to all source files
  set(ament_cmake_copyright_UNSUPPORTED TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

# Export include directories for downstream packages
ament_export_include_directories(
  include
)

ament_export_libraries(
  ${PROJECT_NAME}
)

ament_export_dependencies(
  hardware_interface
  pluginlib
  rclcpp
  rclcpp_lifecycle
)

ament_package()