/**
 * @file simple_serial_receiver.cpp
 * @brief ROS 2 Node that receives serial data from Arduino and publishes it.
 *
 * This node opens a serial connection to the designated port, polls the serial
 * port at 100 Hz, reads lines of text, and publishes them as ROS 2 String messages.
 */

#include <rclcpp/rclcpp.hpp>
#include <std_msgs/msg/string.hpp>

#include <chrono>

#include <libserial/SerialPort.h>

using namespace std::chrono_literals;

/**
 * @class SimpleSerialReceiver
 * @brief Node that listens to serial port signals and broadcasts them onto a ROS 2 topic.
 */
class SimpleSerialReceiver : public rclcpp::Node
{
public:
  /**
   * @brief Construct a new Simple Serial Receiver object.
   *
   * Declares parameters, opens the serial connection at 115200 baud,
   * sets up the publisher, and starts a wall timer to poll for data.
   */
  SimpleSerialReceiver() : Node("simple_serial_receiver")
  {
    declare_parameter<std::string>("port", "/dev/ttyUSB0");

    port_ = get_parameter("port").as_string();

    arduino_.Open(port_);
    arduino_.SetBaudRate(LibSerial::BaudRate::BAUD_115200);

    pub_ = create_publisher<std_msgs::msg::String>("serial_receiver", 10);
    timer_ = create_wall_timer(0.01s, std::bind(&SimpleSerialReceiver::timerCallback, this));
  }

  /**
   * @brief Destroy the Simple Serial Receiver object.
   *
   * Ensures the serial interface connection is closed correctly.
   */
  ~SimpleSerialReceiver()
  {
    arduino_.Close();
  }

  /**
   * @brief Callback function executed by the timer to poll the serial device.
   *
   * Verifies if any bytes are available to read. If yes, reads the line
   * and publishes it on the 'serial_receiver' topic.
   */
  void timerCallback()
  {
    if(rclcpp::ok() && arduino_.IsDataAvailable())
    {
      auto message = std_msgs::msg::String();
      arduino_.ReadLine(message.data);
      pub_->publish(message);
    }
  }

private:
  rclcpp::Publisher<std_msgs::msg::String>::SharedPtr pub_;
  rclcpp::TimerBase::SharedPtr timer_;
  std::string port_;
  LibSerial::SerialPort arduino_;
};


int main(int argc, char* argv[])
{
  rclcpp::init(argc, argv);
  auto node = std::make_shared<SimpleSerialReceiver>();
  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}