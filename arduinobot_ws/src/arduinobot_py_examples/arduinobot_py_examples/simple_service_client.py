"""
Simple service client node for the arduinobot_py_examples package.

Provides :class:`SimpleServiceClient`, a ROS 2 node that calls the
``add_two_ints`` service using the :class:`arduinobot_msgs.srv.AddTwoInts`
interface. The two operands are supplied as command-line arguments.
Intended as a teaching example for ROS 2 service client patterns.
"""

import sys
import rclpy
from rclpy.node import Node
from arduinobot_msgs.srv import AddTwoInts


class SimpleServiceClient(Node):
    """A ROS 2 node that asynchronously calls the ``add_two_ints`` service.

    Waits for the service to become available, sends a single request with
    the provided operands, and logs the response via a done callback.

    **Services Used:**
        - ``add_two_ints`` (:class:`arduinobot_msgs.srv.AddTwoInts`): Requests
          the sum of two integers.

    :param a: First integer operand.
    :type a: int
    :param b: Second integer operand.
    :type b: int
    """

    def __init__(self, a, b):
        """Initialize the service client node and dispatch a request.

        Blocks in a polling loop until the ``add_two_ints`` service is
        available, then dispatches an asynchronous call with *a* and *b*
        and registers :meth:`responseCallback` as the completion handler.

        :param a: First integer operand to send in the request.
        :type a: int
        :param b: Second integer operand to send in the request.
        :type b: int
        """
        super().__init__("simple_service_client")
        self.client_ = self.create_client(AddTwoInts, "add_two_ints")

        while not self.client_.wait_for_service(timeout_sec=1.0):
            self.get_logger().info("service not available, waiting again...")

        self.req_ = AddTwoInts.Request()
        self.req_.a = a
        self.req_.b = b
        self.future_ = self.client_.call_async(self.req_)
        self.future_.add_done_callback(self.responseCallback)

    def responseCallback(self, future):
        """Log the result returned by the ``add_two_ints`` service.

        :param future: Completed future carrying the service response.
        :type future: rclpy.task.Future
        """
        self.get_logger().info('Service Response %d' % future.result().sum)


def main(args=None):
    """Parse command-line arguments and spin up :class:`SimpleServiceClient`.

    Expects exactly two positional arguments after the executable name:
    the integer operands *A* and *B*.

    :returns: ``-1`` if the argument count is wrong, ``None`` otherwise.
    """
    rclpy.init(args=args)

    if len(sys.argv) != 3:
        print("Wrong number of arguments! Usage: Simple_service client A B")
        return -1

    node = SimpleServiceClient(int(sys.argv[1]), int(sys.argv[2]))
    rclpy.spin(node)
    node.destroy_node()
    rclpy.shutdown()


if __name__ == "__main__":
    main()
