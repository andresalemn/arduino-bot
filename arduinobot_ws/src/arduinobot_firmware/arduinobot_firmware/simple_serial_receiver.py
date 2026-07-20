#!/usr/bin/env python3
"""
Module providing a ROS 2 node to receive serial messages from Arduino.
"""
import serial
import rclpy
from rclpy.node import Node
from std_msgs.msg import String


class SimpleSerialReceiver(Node):
    """
    ROS 2 Node that reads lines from a serial port and publishes them as Strings.

    This node polls the serial interface at a fixed frequency, decodes the incoming
    bytes as UTF-8, and broadcasts the data on the 'serial_receiver' topic.
    """
    def __init__(self):
        """
        Initialize the SimpleSerialReceiver node.

        Declares 'port' and 'baudrate' parameters, opens the serial connection,
        and initializes the publisher and a timer to check for incoming data.
        """
        super().__init__("simple_serial_receiver")

        self.declare_parameter("port", "/dev/ttyUSB0")
        self.declare_parameter("baudrate", 115200)

        self.port_ = self.get_parameter("port").value
        self.baudrate_ = self.get_parameter("baudrate").value

        self.pub_ = self.create_publisher(String, "serial_receiver", 10)
        self.arduino_ = serial.Serial(port=self.port_, baudrate=self.baudrate_, timeout=0.1)

        self.frequency_ = 0.01
        self.timer_ = self.create_timer(self.frequency_, self.timerCallback)

    def timerCallback(self):
        """
        Timer callback that executes at 100Hz to read data from the serial port.

        Decodes the serial line as utf-8 and publishes it to the 'serial_receiver' topic.
        """
        if rclpy.ok() and self.arduino_.is_open:
            data = self.arduino_.readline()

            try:
                data.decode("utf-8")
            except:  # noqa: E722
                return

            msg = String()
            msg.data = str(data)
            self.pub_.publish(msg)


def main():
    rclpy.init()

    simple_serial_receiver = SimpleSerialReceiver()
    rclpy.spin(simple_serial_receiver)
    
    simple_serial_receiver.destroy_node()
    rclpy.shutdown()


if __name__ == '__main__':
    main()