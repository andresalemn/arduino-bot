"""
Simple lifecycle node for the arduinobot_py_examples package.

Provides :class:`SimpleLifecycleNode`, a managed ROS 2 node that demonstrates
the managed-node (lifecycle) pattern. The subscription to the ``chatter`` topic
is created and destroyed in response to lifecycle state transitions, and
messages are only processed while the node is in the *active* state.

Intended as a teaching example for ROS 2 lifecycle node patterns.
See ``doc/tutorials/lifecycle_node.md`` for a step-by-step walkthrough.
"""

import time
import rclpy
from rclpy.lifecycle import Node, State, TransitionCallbackReturn
from std_msgs.msg import String


class SimpleLifecycleNode(Node):
    """A managed ROS 2 node that subscribes to ``chatter`` only when active.

    Implements the ROS 2 lifecycle state machine by overriding the standard
    transition callbacks. The ``chatter`` subscription is created during the
    *configuring* transition and destroyed during *cleaning up* or *shutting
    down*, ensuring that resources are held only when the node is configured.
    Incoming messages are discarded unless the node is in the *active* state.

    **Subscribed Topics:**
        - ``chatter`` (:class:`std_msgs.msg.String`): String messages to log
          when the node is active. The subscription exists only while the node
          is configured or active.

    **Lifecycle Transitions Implemented:**
        - ``on_configure``: Creates the ``chatter`` subscription.
        - ``on_shutdown``: Destroys the ``chatter`` subscription.
        - ``on_cleanup``: Destroys the ``chatter`` subscription.
        - ``on_activate``: Introduces a 2-second delay before delegating to
          the parent, simulating hardware initialisation.
        - ``on_deactivate``: Delegates directly to the parent implementation.
    """

    def __init__(self, node_name, **kwargs):
        """Initialize the lifecycle node.

        :param node_name: Name to register this node under in the ROS 2 graph.
        :type node_name: str
        :param kwargs: Additional keyword arguments forwarded to
            :class:`rclpy.lifecycle.Node`.
        """
        super().__init__(node_name, **kwargs)

    def on_configure(self, state: State) -> TransitionCallbackReturn:
        """Create the ``chatter`` subscription during the configuring transition.

        Called automatically by the lifecycle state machine when a *configure*
        transition is requested. Stores the created subscription in
        ``self.sub_`` so it can be destroyed by :meth:`on_cleanup` or
        :meth:`on_shutdown`.

        :param state: The state the node is transitioning from.
        :type state: rclpy.lifecycle.State
        :returns: :attr:`TransitionCallbackReturn.SUCCESS` on success.
        :rtype: rclpy.lifecycle.TransitionCallbackReturn
        """
        # Create the subscription and store it directly (not as a tuple) so
        # that destroy_subscription() receives the correct handle in cleanup.
        self.sub_ = self.create_subscription(String, "chatter", self.msgCallback, 10)
        self.get_logger().info("Lifecycle Node on_configure() called.")
        return TransitionCallbackReturn.SUCCESS

    def on_shutdown(self, state: State) -> TransitionCallbackReturn:
        """Destroy the ``chatter`` subscription during the shutting-down transition.

        :param state: The state the node is transitioning from.
        :type state: rclpy.lifecycle.State
        :returns: :attr:`TransitionCallbackReturn.SUCCESS` on success.
        :rtype: rclpy.lifecycle.TransitionCallbackReturn
        """
        self.destroy_subscription(self.sub_)
        self.get_logger().info("Lifecycle Node on_shutdown() called.")
        return TransitionCallbackReturn.SUCCESS

    def on_cleanup(self, state: State) -> TransitionCallbackReturn:
        """Destroy the ``chatter`` subscription during the cleaning-up transition.

        :param state: The state the node is transitioning from.
        :type state: rclpy.lifecycle.State
        :returns: :attr:`TransitionCallbackReturn.SUCCESS` on success.
        :rtype: rclpy.lifecycle.TransitionCallbackReturn
        """
        self.destroy_subscription(self.sub_)
        self.get_logger().info("Lifecycle Node on_cleanup() called.")
        return TransitionCallbackReturn.SUCCESS

    def on_activate(self, state: State) -> TransitionCallbackReturn:
        """Handle the activating transition, with a simulated startup delay.

        Waits 2 seconds before delegating to the parent implementation to
        simulate hardware or resource initialisation that must complete before
        the node begins processing messages.

        :param state: The state the node is transitioning from.
        :type state: rclpy.lifecycle.State
        :returns: The return value from the parent ``on_activate``.
        :rtype: rclpy.lifecycle.TransitionCallbackReturn
        """
        self.get_logger().info("Lifecycle Node on_activate() called.")
        time.sleep(2)
        return super().on_activate(state)

    def on_deactivate(self, state: State) -> TransitionCallbackReturn:
        """Handle the deactivating transition.

        :param state: The state the node is transitioning from.
        :type state: rclpy.lifecycle.State
        :returns: The return value from the parent ``on_deactivate``.
        :rtype: rclpy.lifecycle.TransitionCallbackReturn
        """
        self.get_logger().info("Lifecycle Node on_deactivate() called.")
        return super().on_deactivate(state)

    def msgCallback(self, msg):
        """Process an incoming ``chatter`` message, but only when active.

        Guards message processing with an explicit state check so that messages
        received while the node is configured-but-not-active are silently
        dropped. This guard is necessary because the subscription persists
        across the active/inactive boundary.

        :param msg: The incoming string message.
        :type msg: std_msgs.msg.String
        """
        current_state = self._state_machine.current_state
        if current_state[1] == "active":
            self.get_logger().info("I heard: %s" % msg.data)


def main():
    """Spin up :class:`SimpleLifecycleNode` with a single-threaded executor."""
    rclpy.init()
    executor = rclpy.executors.SingleThreadedExecutor()
    simple_lifecycle_node = SimpleLifecycleNode("simple_lifecycle_node")
    executor.add_node(simple_lifecycle_node)
    try:
        executor.spin()
    except (KeyboardInterrupt, rclpy.executors.ExternalShutdownException):
        simple_lifecycle_node.destroy_node()


if __name__ == "__main__":
    main()