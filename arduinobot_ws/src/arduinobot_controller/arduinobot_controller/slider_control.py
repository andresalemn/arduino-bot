#!/usr/bin/env python3
"""Bridge between joint GUI slider commands and the arm/gripper trajectory controllers.

Subscribes to a JointState topic (typically produced by joint_state_publisher_gui,
remapped to /joint_commands) and republishes each message as two separate
JointTrajectory goals — one for the 3-DOF arm and one for the gripper — so
the ros2_control trajectory controllers can execute them immediately.

This module is intentionally stateless: every incoming joint-state message is
forwarded directly without buffering or interpolation.
"""

import rclpy
from rclpy.node import Node
from sensor_msgs.msg import JointState
from trajectory_msgs.msg import JointTrajectory, JointTrajectoryPoint


class SliderControl(Node):
    """ROS 2 node that converts JointState slider commands into JointTrajectory goals.

    Subscribes to:
        joint_commands (sensor_msgs/JointState): Joint positions from the slider GUI.

    Publishes to:
        arm_controller/joint_trajectory (trajectory_msgs/JointTrajectory):
            Position goals for joint_1, joint_2, and joint_3.
        gripper_controller/joint_trajectory (trajectory_msgs/JointTrajectory):
            Position goal for joint_4.
    """

    def __init__(self):
        """Initialise publishers, subscriber, and log a startup message."""
        super().__init__("slider_control")
        self.arm_pub_ = self.create_publisher(JointTrajectory, "arm_controller/joint_trajectory", 10)
        self.gripper_pub_ = self.create_publisher(JointTrajectory, "gripper_controller/joint_trajectory", 10)
        self.sub_ = self.create_subscription(JointState, "joint_commands", self.sliderCallback, 10)
        self.get_logger().info("Slider Control Node started")

    def sliderCallback(self, msg):
        """Split an incoming JointState into arm and gripper trajectory goals.

        Expects at least 4 positions in *msg*:

        - ``positions[0:3]`` → arm joints (joint_1, joint_2, joint_3)
        - ``positions[3]``   → gripper joint (joint_4)

        :param msg: Incoming joint state from the GUI slider or any remapped source.
        :type msg: sensor_msgs.msg.JointState
        """
        arm_controller = JointTrajectory()
        gripper_controller = JointTrajectory()
        arm_controller.joint_names = ["joint_1", "joint_2", "joint_3"]
        gripper_controller.joint_names = ["joint_4"]

        arm_goal = JointTrajectoryPoint()
        gripper_goal = JointTrajectoryPoint()
        arm_goal.positions = msg.position[:3]
        gripper_goal.positions = [msg.position[3]]

        arm_controller.points.append(arm_goal)
        gripper_controller.points.append(gripper_goal)

        self.arm_pub_.publish(arm_controller)
        self.gripper_pub_.publish(gripper_controller)


def main():
    """Entry point: spin the SliderControl node."""
    rclpy.init()

    simple_publisher = SliderControl()
    rclpy.spin(simple_publisher)
    
    simple_publisher.destroy_node()
    rclpy.shutdown()


if __name__ == '__main__':
    main()