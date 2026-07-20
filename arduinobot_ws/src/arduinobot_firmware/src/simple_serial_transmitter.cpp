/**
 * @file simple_serial_transmitter.cpp
 * @brief ROS 2 Node that receives ROS messages and writes them to a serial port.
 *
 * This node subscribes to a String topic and relays any received data directly
 * over serial to an attached Arduino.
 */

#include <rclcpp/rclcpp.hpp>
#include <std_msgs/msg/string.hpp>
#include <libserial/SerialPort.h>

using std::placeholders::_1;

/**
 * @class SimpleSerialTransmitter
 * @brief Node that subscribes to 'serial_transmitter' and forwards incoming messages to a serial interface.
 */
class SimpleSerialTransmitter : public rclcpp::Node
{
public:
  /**
   * @brief Construct a new Simple Serial Transmitter object.
   *
   * Declares target port parameters, subscribes to the transmitter topic, and open
   * the serial connection.
   */
  SimpleSerialTransmitter() : Node("simple_serial_transmitter")
  {
    declare_parameter<std::string>("port", "/dev/ttyACM0");
    std::string port_ = get_parameter("port").as_string();
    
    sub_ = create_subscription<std_msgs::msg::String>(
        "serial_transmitter", 10, std::bind(&SimpleSerialTransmitter::msgCallback, this, _1));
    
    arduino_.Open(port_);  
    arduino_.SetBaudRate(LibSerial::BaudRate::BAUD_115200);    
  }

  /**
   * @brief Destroy the Simple Serial Transmitter object.
   *
   * Ensures that the serial communication port is closed.
   */
  ~SimpleSerialTransmitter()
  {
    arduino_.Close();
  }

  /**
   * @brief Callback function triggered when a new String message is received.
   *
   * Writes the message payload bytes onto the active serial line.
   *
   * @param msg The incoming String message.
   */
  void msgCallback(const std_msgs::msg::String &msg)
  {
    RCLCPP_INFO_STREAM(get_logger(), "New message recieved, publishing on serial port: " << msg.data);
    arduino_.Write(msg.data);
  }

private:
  rclcpp::Subscription<std_msgs::msg::String>::SharedPtr sub_;
  LibSerial::SerialPort arduino_;
};


int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<SimpleSerialTransmitter>());
  rclcpp::shutdown();
  return 0;
}