import time
import rclpy
from rclpy.lifecycle import Node, State, TransitionCallbackReturn
from std_msgs.msg import String


class SimpleLifecycleNode(Node):
    def __init__(self, node_name, **kwargs):
        super().__init__(node_name, **kwargs)

    def on_configure(self, state: State) -> TransitionCallbackReturn:
        self.sub_ = (
            self,
            self.create_subscription(String, "chatter", self.msgCallback, 10),
        )
        self.get_logger().info("Lifecycle Node on_configure() called.")
        return TransitionCallbackReturn.SUCCESS

    def on_shutdown(self, state: State) -> TransitionCallbackReturn:
        self.destroy_subscription(self.sub_)
        self.get_logger().info("Lifecycle Node on_shutdown() called.")
        return TransitionCallbackReturn.SUCCESS

    def on_cleanup(self, state: State) -> TransitionCallbackReturn:
        self.destroy_subscription(self.sub_)
        self.get_logger().info("Lifecycle Node on_cleanup() called.")
        return TransitionCallbackReturn.SUCCESS

    def on_activate(self, state: State) -> TransitionCallbackReturn:
        self.get_logger().info("Lifecycle Node on_activate() called.")
        time.sleep(2)
        return super().on_activate(state)

    def on_deactivate(self, state: State) -> TransitionCallbackReturn:
        self.get_logger().info("Lifecycle Node on_deactivate() called.")
        return super().on_deactivate(state)

    def msgCallback(self, msg):
        current_state = self._state_machine.current_state
        if current_state[1] == "active":
            self.get_logger().info("I heard: %s" % msg.data)


def main():
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



    # To test this node:

    # * Terminal 1:

    # control@alienware:~/ros2/arduino-bot/arduinobot_ws$ . install/setup.bash 
    # control@alienware:~/ros2/arduino-bot/arduinobot_ws$ ros2 run arduinobot_py_examples simple_lifecycle_node
    
    # * Terminal 2:

    # control@alienware:~/ros2/arduino-bot/arduinobot_ws$ ros2 lifecycle nodes
    # /simple_lifecycle_node

    # control@alienware:~/ros2/arduino-bot/arduinobot_ws$ ros2 lifecycle get /simple_lifecycle_node 
    # unconfigured [1]

    # control@alienware:~/ros2/arduino-bot/arduinobot_ws$ ros2 topic list
    # /parameter_events
    # /rosout
    # /simple_lifecycle_node/transition_event

    # control@alienware:~/ros2/arduino-bot/arduinobot_ws$ ros2 lifecycle list /simple_lifecycle_node 
    # - configure [1]
    #     Start: unconfigured
    #     Goal: configuring
    # - shutdown [5]
    #     Start: unconfigured
    #     Goal: shuttingdown

    # control@alienware:~/ros2/arduino-bot/arduinobot_ws$ ros2 lifecycle set /simple_lifecycle_node configure
    # Transitioning successful

    # * Terminal 3:

    # control@alienware:~/ros2/arduino-bot/arduinobot_ws$ ros2 topic pub /chatter std_msgs/msg/String "data: 'Hi'"