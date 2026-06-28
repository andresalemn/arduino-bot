#!/usr/bin/env python3
import rclpy
from rclpy.node import Node
from std_msgs.msg import String
import serial


class SimpleSerialTransmitter(Node):

    def __init__(self):
        super().__init__("simple_serial_transmitter")
        
        self.declare_parameter("port", "/dev/ttyACM0")
        self.declare_parameter("baud_rate", 115200)

        self.port_ = self.get_parameter("port").value
        self.baud_rate_ = self.get_parameter("baud_rate").value
        
        self.sub_ = self.create_subscription(String, "serial_transmitter", self.msgCallback, 10)
        self.arduino_ = serial.Serial(port=self.port_, baudrate=self.baud_rate_, timeout=0.1)

    def msgCallback(self, msg):
        self.get_logger().info("I heard: %s" % msg.data)
        self.arduino_.write(msg.data.encode("utf-8"))


def main():
    rclpy.init()

    simple_serial_transmitter = SimpleSerialTransmitter()
    rclpy.spin(simple_serial_transmitter)
    
    simple_serial_transmitter.destroy_node()
    rclpy.shutdown()


if __name__ == '__main__':
    main()


    # How to call this node:
    # control@alienware:~/ros2/arduino-bot/arduinobot_ws$ ros2 run arduinobot_firmware simple_serial_transmitter.py --ros-args -p port:=/dev/ttyACM0

    # In a second terminal:
    # control@alienware:~/ros2/arduino-bot/arduinobot_ws$ ros2 topic pub /serial_transmitter std_msgs/msg/String "data: '0'" 
    # control@alienware:~/ros2/arduino-bot/arduinobot_ws$ ros2 topic pub /serial_transmitter std_msgs/msg/String "data: '1'"