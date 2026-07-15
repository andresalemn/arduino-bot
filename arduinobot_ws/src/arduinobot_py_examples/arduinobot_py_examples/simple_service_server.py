"""
Simple service server node for the arduinobot_py_examples package.

Provides :class:`SimpleServiceServer`, a ROS 2 node that advertises the
``add_two_ints`` service using the :class:`arduinobot_msgs.srv.AddTwoInts`
interface. Intended as a teaching example for ROS 2 service server patterns.
"""

import rclpy
from rclpy.node import Node
from arduinobot_msgs.srv import AddTwoInts


class SimpleServiceServer(Node):
    """A ROS 2 node that provides the ``add_two_ints`` service.

    Accepts requests containing two integers and responds with their sum.

    **Services:**
        - ``add_two_ints`` (:class:`arduinobot_msgs.srv.AddTwoInts`): Adds
          two integer values and returns the result.
    """

    def __init__(self):
        """Initialize the service server node.

        Creates and advertises the ``add_two_ints`` service, binding incoming
        requests to :meth:`serviceCallback`.
        """
        super().__init__("simple_service_server")
        self.service_ = self.create_service(AddTwoInts, "add_two_ints", self.serviceCallback)
        self.get_logger().info("Service add_two_ints Ready")

    def serviceCallback(self, req, res):
        """Handle an ``add_two_ints`` service request.

        Computes the sum of the two request integers, stores it in the
        response, logs both the inputs and the result, and returns the
        populated response.

        :param req: Service request containing fields ``a`` and ``b``.
        :type req: arduinobot_msgs.srv.AddTwoInts.Request
        :param res: Service response to be populated with field ``sum``.
        :type res: arduinobot_msgs.srv.AddTwoInts.Response
        :returns: The populated response object.
        :rtype: arduinobot_msgs.srv.AddTwoInts.Response
        """
        self.get_logger().info("New Request Received a: %d, b: %d" % (req.a, req.b))
        res.sum = req.a + req.b
        self.get_logger().info("Returning sum: %d" % res.sum)
        return res


def main():
    """Spin up the :class:`SimpleServiceServer` node and block until shutdown."""
    rclpy.init()

    simple_service_server = SimpleServiceServer()
    rclpy.spin(simple_service_server)

    simple_service_server.destroy_node()
    rclpy.shutdown()


if __name__ == '__main__':
    main()