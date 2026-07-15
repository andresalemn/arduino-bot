"""
Simple parameter node for the arduinobot_py_examples package.

Provides :class:`SimpleParameter`, a ROS 2 node that demonstrates parameter
declaration and dynamic parameter-change callbacks. Intended as a teaching
example for ROS 2 parameter patterns.
"""

import rclpy
from rclpy.node import Node
from rcl_interfaces.msg import SetParametersResult
from rclpy.parameter import Parameter


class SimpleParameter(Node):
    """A ROS 2 node that declares parameters and reacts to runtime changes.

    Declares two parameters — an integer and a string — and registers a
    callback that validates and logs any updates requested via
    ``ros2 param set`` or the parameter services.

    **Parameters:**
        - ``simple_int_param`` (int, default ``28``): An example integer
          parameter.
        - ``simple_string_param`` (str, default ``"Antonio"``): An example
          string parameter.
    """

    def __init__(self):
        """Initialize the parameter node.

        Declares both parameters with their default values and registers
        :meth:`paramChangeCallback` as the on-set-parameters handler.
        """
        super().__init__("simple_parameter")
        self.declare_parameter("simple_int_param", 28)
        self.declare_parameter("simple_string_param", "Antonio")

        self.add_on_set_parameters_callback(self.paramChangeCallback)

    def paramChangeCallback(self, params):
        """Validate and log incoming parameter change requests.

        Iterates over the requested parameter changes. For each known parameter
        that matches the expected type, logs the new value and marks the change
        as successful. Unknown parameters or type mismatches are silently
        ignored and the result remains unsuccessful.

        :param params: List of parameters being set.
        :type params: list[rclpy.parameter.Parameter]
        :returns: Result indicating whether each change was accepted.
        :rtype: rcl_interfaces.msg.SetParametersResult
        """
        result = SetParametersResult()

        for param in params:
            if param.name == "simple_int_param" and param.type_ == Parameter.Type.INTEGER:
                self.get_logger().info("Param simple_int_param changed! New value is: %d" % param.value)
                result.successful = True

            if param.name == "simple_string_param" and param.type_ == Parameter.Type.STRING:
                self.get_logger().info("Param simple_string_param changed! New value is: %s" % param.value)
                result.successful = True

        return result


def main(args=None):
    """Spin up the :class:`SimpleParameter` node and block until shutdown."""
    rclpy.init(args=args)
    node = SimpleParameter()
    rclpy.spin(node)
    node.destroy_node()
    rclpy.shutdown()


if __name__ == "__main__":
    main()
