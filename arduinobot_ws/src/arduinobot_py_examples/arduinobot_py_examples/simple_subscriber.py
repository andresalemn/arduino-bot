"""
Simple subscriber node for the arduinobot_py_examples package.

Provides :class:`SimpleSubscriber`, a minimal ROS 2 node that listens to
:class:`std_msgs.msg.String` messages on the ``chatter`` topic and logs each
received message. Intended as a teaching example for ROS 2 subscriber patterns.
"""

import rclpy
from rclpy.node import Node
from std_msgs.msg import String


class SimpleSubscriber(Node):
    """A ROS 2 node that subscribes to String messages on the ``chatter`` topic.

    Logs every received message at the INFO level. Designed to be used
    alongside :class:`simple_publisher.SimplePublisher`.

    **Subscribed Topics:**
        - ``chatter`` (:class:`std_msgs.msg.String`): Incoming string messages
          to be logged.
    """

    def __init__(self):
        """Initialize the subscriber node.

        Creates a subscription to the ``chatter`` topic with a queue depth of
        10, binding received messages to :meth:`msgCallback`.
        """
        super().__init__("simple_subscriber")
        self.sub_ = self.create_subscription(String, "chatter", self.msgCallback, 10)

    def msgCallback(self, msg):
        """Log a received String message.

        :param msg: The incoming message.
        :type msg: std_msgs.msg.String
        """
        self.get_logger().info("I heard: %s" % msg.data)


def main():
    """Spin up the :class:`SimpleSubscriber` node and block until shutdown."""
    rclpy.init()

    simple_subscriber = SimpleSubscriber()
    rclpy.spin(simple_subscriber)

    simple_subscriber.destroy_node()
    rclpy.shutdown()


if __name__ == '__main__':
    main()