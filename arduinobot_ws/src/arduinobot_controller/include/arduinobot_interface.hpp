/**
 * @file arduinobot_interface.h
 * @brief Declares the hardware interface that connects ros2_control to an
 * Arduino over a serial connection.
 *
 * This class implements the SystemInterface API, exposing the robot's joint
 * state and command interfaces while handling lifecycle management and serial
 * communication with the hardware.
 */

#ifndef ARDUINOBOT_INTERFACE_H
#define ARDUINOBOT_INTERFACE_H

#include <rclcpp/rclcpp.hpp>
#include <hardware_interface/system_interface.hpp>
#include <libserial/SerialPort.h>
#include <rclcpp_lifecycle/state.hpp>
#include <rclcpp_lifecycle/node_interfaces/lifecycle_node_interface.hpp>

#include <vector>
#include <string>

namespace arduinobot_controller
{

// Short alias for the lifecycle callback return type.
using CallbackReturn =
  rclcpp_lifecycle::node_interfaces::LifecycleNodeInterface::CallbackReturn;

// Hardware interface plugin that connects ros2_control with an Arduino
// over a serial connection.
class ArduinobotInterface : public hardware_interface::SystemInterface
{
public:
  ArduinobotInterface();
  virtual ~ArduinobotInterface();

  // Lifecycle callbacks -----------------------------------------------------

  // Called when the hardware is activated.
  // This is typically where the serial port is opened and the hardware
  // becomes ready to exchange data.
  virtual CallbackReturn on_activate(
    const rclcpp_lifecycle::State &previous_state) override;

  // Called when the hardware is deactivated.
  // Used to safely stop communication and release resources.
  virtual CallbackReturn on_deactivate(
    const rclcpp_lifecycle::State &previous_state) override;

  // SystemInterface methods -------------------------------------------------

  // Called once when the hardware plugin is created.
  // Reads parameters from the URDF and initializes internal data structures.
  virtual CallbackReturn on_init(
    const hardware_interface::HardwareInfo &hardware_info) override;

  // Exposes the robot state variables (joint positions, velocities, etc.)
  // that controllers can read.
  virtual std::vector<hardware_interface::StateInterface>
  export_state_interfaces() override;

  // Exposes the command variables that controllers can write to.
  virtual std::vector<hardware_interface::CommandInterface>
  export_command_interfaces() override;

  // Reads the latest data from the Arduino and updates the state variables.
  virtual hardware_interface::return_type read(
    const rclcpp::Time & time,
    const rclcpp::Duration & period) override;

  // Sends the latest controller commands to the Arduino.
  virtual hardware_interface::return_type write(
    const rclcpp::Time & time,
    const rclcpp::Duration & period) override;

private:
  // Serial connection to the Arduino.
  LibSerial::SerialPort arduino_;

  // Serial device (for example: "/dev/ttyUSB0").
  std::string port_;

  // Desired joint positions received from ros2_control controllers.
  std::vector<double> position_commands_;

  // Previous commands, useful for avoiding sending duplicate values.
  std::vector<double> prev_position_commands_;

  // Current joint positions reported by the Arduino.
  std::vector<double> position_states_;
};

}  // namespace arduinobot_controller

#endif  // ARDUINOBOT_INTERFACE_H