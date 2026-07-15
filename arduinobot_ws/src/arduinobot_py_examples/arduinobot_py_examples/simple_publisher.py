"""
Simple publisher node for the arduinobot_py_examples package.

Provides :class:`SimplePublisher`, a minimal ROS 2 node that periodically
publishes :class:`std_msgs.msg.String` messages to the ``chatter`` topic.
Intended as a teaching example for ROS 2 publisher patterns.
"""

import rclpy
from rclpy.node import Node
from std_msgs.msg import String


class SimplePublisher(Node):
    """A ROS 2 node that publishes String messages at a fixed rate.

    Publishes incrementing counter messages to the ``chatter`` topic once per
    second using a wall timer.

    **Published Topics:**
        - ``chatter`` (:class:`std_msgs.msg.String`): Periodic greeting messages
          that include a monotonically increasing counter value.
    """

    def __init__(self):
        """Initialize the publisher node.

        Creates the ``chatter`` publisher with a queue depth of 10 and starts
        a 1 Hz wall timer that drives :meth:`timerCallback`.
        """
        super().__init__("simple_publisher")
        self.pub_ = self.create_publisher(String, "chatter", 10)
        self.counter_ = 0
        self.frequency_ = 1.0
        self.get_logger().info("Publishing at %d Hz" % self.frequency_)
        self.timer_ = self.create_timer(self.frequency_, self.timerCallback)

    def timerCallback(self):
        """Publish a new String message on every timer tick.

        Builds a message containing a greeting and the current counter value,
        publishes it on the ``chatter`` topic, then increments the counter.
        """
        msg = String()
        msg.data = "Hello Alemaniaco ROS2 - counter: %d" % self.counter_
        self.pub_.publish(msg)
        self.counter_ += 1


def main():
    """Spin up the :class:`SimplePublisher` node and block until shutdown."""
    rclpy.init()
    simple_publisher = SimplePublisher()
    rclpy.spin(simple_publisher)
    simple_publisher.destroy_node()
    rclpy.shutdown()


if __name__ == '__main__':
    main()