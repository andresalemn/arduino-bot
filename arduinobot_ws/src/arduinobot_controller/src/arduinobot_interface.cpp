/**
 * @file arduinobot_interface.cpp
 * @brief Implements the ros2_control hardware interface for the Arduinobot robot arm.
 *
 * This file handles serial communication with the Arduino, translating joint commands
 * from ros2_control into a simple serial protocol, and updating states.
 */

#include "arduinobot_controller/arduinobot_interface.hpp"
#include <hardware_interface/types/hardware_interface_type_values.hpp>
#include <pluginlib/class_list_macros.hpp>

namespace arduinobot_controller
{

/**
 * @brief Utility function to compensate zeros for message format.
 *
 * Prepends leading zeros to ensure joint values match the expected
 * fixed-length digit format on the serial receiver.
 *
 * @param value The integer value to check.
 * @return A string containing the appropriate number of leading zeros.
 */
std::string compensateZeros(const int value)
{
  std::string compensate_zeros = "";
  if(value < 10){
    compensate_zeros = "00";
  } else if(value < 100){
    compensate_zeros = "0";
  } else {
    compensate_zeros = "";
  }
  return compensate_zeros;
}
  
/**
 * @brief Construct a new Arduinobot Interface object.
 */
ArduinobotInterface::ArduinobotInterface()
{
}

/**
 * @brief Destroy the Arduinobot Interface object.
 *
 * Ensures that the serial port is closed safely if it is still open.
 */
ArduinobotInterface::~ArduinobotInterface()
{
  if (arduino_.IsOpen())
  {
    try
    {
      arduino_.Close();
    }
    catch (...)
    {
      RCLCPP_FATAL_STREAM(rclcpp::get_logger("ArduinobotInterface"),
                          "Something went wrong while closing connection with port " << port_);
    }
  }
}

/**
 * @brief Initialize the hardware interface.
 *
 * Reads configuration parameters from the URDF (such as the target serial port)
 * and prepares memory allocation for command and state interface variables.
 *
 * @param hardware_info Structure containing hardware description parameters from URDF.
 * @return CallbackReturn SUCCESS on initialization success, FAILURE otherwise.
 */
CallbackReturn ArduinobotInterface::on_init(const hardware_interface::HardwareInfo &hardware_info)
{
  CallbackReturn result = hardware_interface::SystemInterface::on_init(hardware_info);
  if (result != CallbackReturn::SUCCESS)
  {
    return result;
  }

  try
  {
    port_ = info_.hardware_parameters.at("port");
  }
  catch (const std::out_of_range &e)
  {
    RCLCPP_FATAL(rclcpp::get_logger("ArduinobotInterface"), "No Serial Port provided! Aborting");
    return CallbackReturn::FAILURE;
  }

  position_commands_.reserve(info_.joints.size());
  position_states_.reserve(info_.joints.size());
  prev_position_commands_.reserve(info_.joints.size());

  return CallbackReturn::SUCCESS;
}

/**
 * @brief Export the state interfaces provided by this hardware interface.
 *
 * Exposes joint position states for reading by controller managers and controllers.
 *
 * @return A vector of StateInterface objects.
 */
std::vector<hardware_interface::StateInterface> ArduinobotInterface::export_state_interfaces()
{
  std::vector<hardware_interface::StateInterface> state_interfaces;

  // Provide only a position Interface
  for (size_t i = 0; i < info_.joints.size(); i++)
  {
    state_interfaces.emplace_back(hardware_interface::StateInterface(
        info_.joints[i].name, hardware_interface::HW_IF_POSITION, &position_states_[i]));
  }

  return state_interfaces;
}

/**
 * @brief Export the command interfaces provided by this hardware interface.
 *
 * Exposes joint position command variables that can be written to by controllers.
 *
 * @return A vector of CommandInterface objects.
 */
std::vector<hardware_interface::CommandInterface> ArduinobotInterface::export_command_interfaces()
{
  std::vector<hardware_interface::CommandInterface> command_interfaces;

  // Provide only a position Interface
  for (size_t i = 0; i < info_.joints.size(); i++)
  {
    command_interfaces.emplace_back(hardware_interface::CommandInterface(
        info_.joints[i].name, hardware_interface::HW_IF_POSITION, &position_commands_[i]));
  }

  return command_interfaces;
}

/**
 * @brief Activate the hardware interface.
 *
 * Resets state arrays and establishes the serial connection with the Arduino
 * at a baud rate of 115200.
 *
 * @param previous_state The state before this transition.
 * @return CallbackReturn SUCCESS if activated and connection established, FAILURE otherwise.
 */
CallbackReturn ArduinobotInterface::on_activate(const rclcpp_lifecycle::State &previous_state)
{
  RCLCPP_INFO(rclcpp::get_logger("ArduinobotInterface"), "Starting robot hardware ...");

  // Reset commands and states
  position_commands_ = { 0.0, 0.0, 0.0, 0.0 };
  prev_position_commands_ = { 0.0, 0.0, 0.0, 0.0 };
  position_states_ = { 0.0, 0.0, 0.0, 0.0 };

  try
  {
    arduino_.Open(port_);
    arduino_.SetBaudRate(LibSerial::BaudRate::BAUD_115200);
  }
  catch (...)
  {
    RCLCPP_FATAL_STREAM(rclcpp::get_logger("ArduinobotInterface"),
                        "Something went wrong while interacting with port " << port_);
    return CallbackReturn::FAILURE;
  }

  RCLCPP_INFO(rclcpp::get_logger("ArduinobotInterface"),
              "Hardware started, ready to take commands");
  return CallbackReturn::SUCCESS;
}

/**
 * @brief Deactivate the hardware interface.
 *
 * Safely closes the active serial connection to the Arduino.
 *
 * @param previous_state The state before this transition.
 * @return CallbackReturn SUCCESS.
 */
CallbackReturn ArduinobotInterface::on_deactivate(const rclcpp_lifecycle::State &previous_state)
{
  RCLCPP_INFO(rclcpp::get_logger("ArduinobotInterface"), "Stopping robot hardware ...");

  if (arduino_.IsOpen())
  {
    try
    {
      arduino_.Close();
    }
    catch (...)
    {
      RCLCPP_FATAL_STREAM(rclcpp::get_logger("ArduinobotInterface"),
                          "Something went wrong while closing connection with port " << port_);
    }
  }

  RCLCPP_INFO(rclcpp::get_logger("ArduinobotInterface"), "Hardware stopped");
  return CallbackReturn::SUCCESS;
}

/**
 * @brief Read joint states from hardware.
 *
 * Since this is an open-loop controller setup, we assume the joint state matches
 * the last sent command target.
 *
 * @param time The current execution time.
 * @param period The duration since the last read invocation.
 * @return hardware_interface::return_type OK.
 */
hardware_interface::return_type ArduinobotInterface::read(const rclcpp::Time &time,
                                                          const rclcpp::Duration &period)
{
  // Open Loop Control - assuming the robot is always where we command to be
  position_states_ = position_commands_;
  return hardware_interface::return_type::OK;
}

/**
 * @brief Write command variables out to the physical Arduino hardware.
 *
 * Compares current target commands to previous states. If they differ, formats
 * the joints into a protocol string (e.g. "bXXX,sXXX,eXXX,gXXX,") and sends
 * the payload across the serial port.
 *
 * @param time The current execution time.
 * @param period The duration since the last write invocation.
 * @return hardware_interface::return_type OK if send was successful or no update was needed, ERROR otherwise.
 */
hardware_interface::return_type ArduinobotInterface::write(const rclcpp::Time &time,
                                                           const rclcpp::Duration &period)
{
  if (position_commands_ == prev_position_commands_)
  {
    // Nothing changed, do not send any command
    return hardware_interface::return_type::OK;
  }

  std::string msg;
  int base = static_cast<int>(((position_commands_.at(0) + (M_PI / 2)) * 180) / M_PI);
  msg.append("b");
  msg.append(compensateZeros(base));
  msg.append(std::to_string(base));
  msg.append(",");
  int shoulder = 180 - static_cast<int>(((position_commands_.at(1) + (M_PI / 2)) * 180) / M_PI);
  msg.append("s");
  msg.append(compensateZeros(shoulder));
  msg.append(std::to_string(shoulder));
  msg.append(",");
  int elbow = static_cast<int>(((position_commands_.at(2) + (M_PI / 2)) * 180) / M_PI);
  msg.append("e");
  msg.append(compensateZeros(elbow));
  msg.append(std::to_string(elbow));
  msg.append(",");
  int gripper = static_cast<int>(((-position_commands_.at(3)) * 180) / (M_PI / 2));
  msg.append("g");
  msg.append(compensateZeros(gripper));
  msg.append(std::to_string(gripper));
  msg.append(",");

  try
  {
    RCLCPP_INFO_STREAM(rclcpp::get_logger("ArduinobotInterface"), "Sending new command " << msg);
    arduino_.Write(msg);
  }
  catch (...)
  {
    RCLCPP_ERROR_STREAM(rclcpp::get_logger("ArduinobotInterface"),
                        "Something went wrong while sending the message "
                            << msg << " to the port " << port_);
    return hardware_interface::return_type::ERROR;
  }

  prev_position_commands_ = position_commands_;

  return hardware_interface::return_type::OK;
}
}  // namespace arduinobot_controller

PLUGINLIB_EXPORT_CLASS(arduinobot_controller::ArduinobotInterface, hardware_interface::SystemInterface)