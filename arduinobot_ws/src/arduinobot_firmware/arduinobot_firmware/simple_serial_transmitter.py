#!/usr/bin/env python3
"""
Module providing a ROS 2 node to transmit serial messages to Arduino.
"""
import rclpy
from rclpy.node import Node
from std_msgs.msg import String
import serial


class SimpleSerialTransmitter(Node):
    """
    ROS 2 Node that subscribes to a topic and forwards incoming string messages to a serial port.
    """

    def __init__(self):
        """
        Initialize the SimpleSerialTransmitter node.

        Declares 'port' and 'baud_rate' parameters, opens the serial connection,
        and sets up a subscription to the 'serial_transmitter' topic.
        """
        super().__init__("simple_serial_transmitter")
        
        self.declare_parameter("port", "/dev/ttyACM0")
        self.declare_parameter("baud_rate", 115200)

        self.port_ = self.get_parameter("port").value
        self.baud_rate_ = self.get_parameter("baud_rate").value
        
        self.sub_ = self.create_subscription(String, "serial_transmitter", self.msgCallback, 10)
        self.arduino_ = serial.Serial(port=self.port_, baudrate=self.baud_rate_, timeout=0.1)

    def msgCallback(self, msg):
        """
        Callback triggered whenever a new String message is received.

        Encodes the message data as UTF-8 and writes it directly to the serial port.

        :param msg: The incoming String message.
        """
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