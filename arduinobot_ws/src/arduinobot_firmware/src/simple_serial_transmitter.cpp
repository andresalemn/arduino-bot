#include <rclcpp/rclcpp.hpp>
#include <std_msgs/msg/string.hpp>
#include <libserial/SerialPort.h>

using std::placeholders::_1;


class SimpleSerialTransmitter : public rclcpp::Node
{
public:
  SimpleSerialTransmitter() : Node("simple_serial_transmitter")
  {
    declare_parameter<std::string>("port", "/dev/ttyACM0");
    std::string port_ = get_parameter("port").as_string();
    
    sub_ = create_subscription<std_msgs::msg::String>(
        "serial_transmitter", 10, std::bind(&SimpleSerialTransmitter::msgCallback, this, _1));
    
    arduino_.Open(port_);  
    arduino_.SetBaudRate(LibSerial::BaudRate::BAUD_115200);    
  }

  ~SimpleSerialTransmitter()
  {
    arduino_.Close();
  }

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

    /* 
        How to call this node:  
        control@alienware:~/ros2/arduino-bot/arduinobot_ws$ ros2 run arduinobot_firmware simple_serial_transmitter --ros-args -p port:=/dev/ttyACM0

        In a second terminal:
        control@alienware:~/ros2/arduino-bot/arduinobot_ws$ ros2 topic pub /serial_transmitter std_msgs/msg/String "data: '0'" 
        control@alienware:~/ros2/arduino-bot/arduinobot_ws$ ros2 topic pub /serial_transmitter std_msgs/msg/String "data: '1'"

    */