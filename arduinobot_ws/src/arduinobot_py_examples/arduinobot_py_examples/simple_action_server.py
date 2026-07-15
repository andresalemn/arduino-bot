#!/usr/bin/env python3
"""
Simple action server node for the arduinobot_py_examples package.

Provides :class:`SimpleActionServer`, a ROS 2 node that handles
:class:`arduinobot_msgs.action.Fibonacci` action goals. For each accepted
goal the server computes a Fibonacci sequence of the requested order,
publishing partial sequences as feedback at 1 Hz. Intended as a teaching
example for ROS 2 action server patterns.
"""

import time
import rclpy
from rclpy.node import Node
from rclpy.action import ActionServer
from arduinobot_msgs.action import Fibonacci


class SimpleActionServer(Node):
    """A ROS 2 node that serves Fibonacci action goals.

    Accepts :class:`arduinobot_msgs.action.Fibonacci` goals specifying the
    desired sequence *order*, computes the sequence iteratively while
    publishing each new element as feedback, and returns the completed
    sequence as the action result.

    **Actions:**
        - ``fibonacci`` (:class:`arduinobot_msgs.action.Fibonacci`): Computes
          a Fibonacci sequence of the requested order.
          - *Goal*: ``order`` (int32) — number of elements to compute.
          - *Feedback*: ``partial_sequence`` (int32[]) — elements computed so far.
          - *Result*: ``sequence`` (int32[]) — the complete Fibonacci sequence.
    """

    def __init__(self):
        """Initialize the action server node.

        Creates a :class:`rclpy.action.ActionServer` for the ``fibonacci``
        action, binding goal requests to :meth:`goalCallback`.
        """
        super().__init__("simple_action_server")
        self.action_server = ActionServer(self, Fibonacci, "fibonacci", self.goalCallback)
        self.get_logger().info("Starting the server")

    def goalCallback(self, goal_handle):
        """Execute a Fibonacci action goal.

        Iteratively extends the partial sequence from the seed ``[0, 1]`` up
        to the requested *order*, sleeping 1 second between iterations to
        simulate computation time and publishing :class:`Fibonacci.Feedback`
        after each step. Marks the goal as succeeded and returns the full
        sequence.

        :param goal_handle: Handle for the accepted goal, used to publish
            feedback and report success.
        :type goal_handle: rclpy.action.server.ServerGoalHandle
        :returns: Action result containing the complete Fibonacci sequence.
        :rtype: arduinobot_msgs.action.Fibonacci.Result
        """
        self.get_logger().info("Recieved goal request with order %d" % goal_handle.request.order)
        feedback_msg = Fibonacci.Feedback()
        feedback_msg.partial_sequence = [0, 1]

        for i in range(1, goal_handle.request.order):
            feedback_msg.partial_sequence.append(
                feedback_msg.partial_sequence[i] + feedback_msg.partial_sequence[i - 1]
            )
            self.get_logger().info("Feedback: {0}".format(feedback_msg.partial_sequence))
            goal_handle.publish_feedback(feedback_msg)
            time.sleep(1)

        goal_handle.succeed()
        result = Fibonacci.Result()
        result.sequence = feedback_msg.partial_sequence
        return result


def main(args=None):
    """Spin up the :class:`SimpleActionServer` node and block until shutdown."""
    rclpy.init(args=args)
    node = SimpleActionServer()
    rclpy.spin(node)
    rclpy.shutdown()


if __name__ == "__main__":
    main()