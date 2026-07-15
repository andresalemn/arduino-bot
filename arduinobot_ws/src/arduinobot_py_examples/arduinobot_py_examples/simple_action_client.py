"""
Simple action client node for the arduinobot_py_examples package.

Provides :class:`SimpleActionClient`, a ROS 2 node that sends a
:class:`arduinobot_msgs.action.Fibonacci` goal to the ``fibonacci`` action
server, handles feedback during execution, and logs the final result.
Intended as a teaching example for ROS 2 action client patterns.
"""

import rclpy
from rclpy.node import Node
from rclpy.action import ActionClient
from arduinobot_msgs.action import Fibonacci


class SimpleActionClient(Node):
    """A ROS 2 node that sends a Fibonacci action goal and tracks its progress.

    On startup, waits for the ``fibonacci`` action server, sends a goal
    requesting a sequence of order 10, and handles goal acceptance, periodic
    feedback, and the final result through separate callbacks.

    **Actions Used:**
        - ``fibonacci`` (:class:`arduinobot_msgs.action.Fibonacci`): Requests
          a Fibonacci sequence computation.
          - *Goal*: ``order`` (int32) — fixed to 10 in this implementation.
          - *Feedback*: ``partial_sequence`` (int32[]) — logged at each step.
          - *Result*: ``sequence`` (int32[]) — logged on completion.
    """

    def __init__(self):
        """Initialize the action client node and dispatch a goal.

        Blocks until the ``fibonacci`` action server is available, then
        asynchronously sends a goal with ``order = 10`` and registers
        :meth:`responseCallback` to handle acceptance or rejection.
        """
        super().__init__("simple_action_client")
        self.action_client = ActionClient(self, Fibonacci, "fibonacci")
        self.goal = Fibonacci.Goal()
        self.goal.order = 10

        self.action_client.wait_for_server()
        self.future = self.action_client.send_goal_async(
            self.goal, feedback_callback=self.feedbackCallback
        )
        self.future.add_done_callback(self.responseCallback)

    def responseCallback(self, future):
        """Handle the server's acceptance or rejection of the goal.

        If the goal is rejected, logs a warning and returns early. If
        accepted, asynchronously requests the final result and registers
        :meth:`resultCallback`.

        :param future: Future resolved with the goal handle.
        :type future: rclpy.task.Future
        """
        goal_handle = future.result()
        if not goal_handle.accepted:
            self.get_logger().info('Goal rejected :(')
            return

        self.get_logger().info('Goal accepted :)')

        self.future = goal_handle.get_result_async()
        self.future.add_done_callback(self.resultCallback)

    def resultCallback(self, future):
        """Log the completed Fibonacci sequence and shut down.

        :param future: Future resolved with the action result wrapper.
        :type future: rclpy.task.Future
        """
        result = future.result().result
        self.get_logger().info('Result: {0}'.format(result.sequence))
        rclpy.shutdown()

    def feedbackCallback(self, feedback_msg):
        """Log intermediate feedback from the action server.

        :param feedback_msg: Feedback message containing the partial sequence
            computed so far.
        :type feedback_msg: arduinobot_msgs.action.Fibonacci.Impl.FeedbackMessage
        """
        feedback = feedback_msg.feedback
        self.get_logger().info('Received feedback: {0}'.format(feedback.partial_sequence))


def main(args=None):
    """Spin up the :class:`SimpleActionClient` node and block until shutdown."""
    rclpy.init(args=args)
    action_client = SimpleActionClient()
    rclpy.spin(action_client)


if __name__ == '__main__':
    main()