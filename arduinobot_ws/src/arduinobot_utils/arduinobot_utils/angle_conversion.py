#!/usr/bin/env python3
"""
Angle Conversion Module.

This module provides a ROS 2 node that implements services to convert
between Euler angles (roll, pitch, yaw) and quaternions (x, y, z, w)
using the tf_transformations library.
"""

import rclpy
from rclpy.node import Node
from arduinobot_msgs.srv import EulerToQuaternion, QuaternionToEuler
from tf_transformations import quaternion_from_euler, euler_from_quaternion


class AnglesConverter(Node):
    """
    A ROS 2 node that provides angle conversion services.

    This node registers two services:
    - 'euler_to_quaternion': Converts Euler angles to a quaternion.
    - 'quaternion_to_euler': Converts a quaternion to Euler angles.
    """

    def __init__(self):
        """Initialize the AnglesConverter node and its services."""
        super().__init__("angles_conversion_service_server")
        self.euler_to_quaternion_ = self.create_service(EulerToQuaternion, "euler_to_quaternion", self.eulerToQuaternionCallback)
        self.quaternion_to_euler = self.create_service(QuaternionToEuler, "quaternion_to_euler", self.quaternionToEulerCallback)
        self.get_logger().info("Angle Conversion Services Ready")

    def eulerToQuaternionCallback(self, req, res):
        """
        Convert Euler angles to a quaternion representation.

        :param req: The service request containing roll, pitch, and yaw.
        :type req: arduinobot_msgs.srv.EulerToQuaternion.Request
        :param res: The service response to be populated with x, y, z, and w.
        :type res: arduinobot_msgs.srv.EulerToQuaternion.Response
        :return: The populated response object containing the quaternion.
        :rtype: arduinobot_msgs.srv.EulerToQuaternion.Response
        """
        self.get_logger().info("Requested to convert euler angles roll: %f, pitch: %f, yaw: %f, into a quaternion." % (req.roll, req.pitch, req.yaw))
        # Calculate quaternion from euler
        (res.x, res.y, res.z, res.w) = quaternion_from_euler(req.roll, req.pitch, req.yaw)
        self.get_logger().info("Corresponding quaternion x: %f, y: %f, z: %f, w: %f" % (res.x, res.y, res.z, res.w))
        return res
    
    def quaternionToEulerCallback(self, req, res):
        """
        Convert a quaternion to Euler angles representation.

        :param req: The service request containing x, y, z, and w.
        :type req: arduinobot_msgs.srv.QuaternionToEuler.Request
        :param res: The service response to be populated with roll, pitch, and yaw.
        :type res: arduinobot_msgs.srv.QuaternionToEuler.Response
        :return: The populated response object containing Euler angles.
        :rtype: arduinobot_msgs.srv.QuaternionToEuler.Response
        """
        self.get_logger().info("Requested to convert quaternion x: %f, y: %f, z: %f, w: %f, into euler angles." % (req.x, req.y, req.z, req.w))
        # Calculate euler from quaternion
        (res.roll, res.pitch, res.yaw) = euler_from_quaternion([req.x, req.y, req.z, req.w])
        self.get_logger().info("Corresponding euler angles roll: %f, pitch: %f, yaw: %f" % (res.roll, res.pitch, res.yaw))
        return res


def main():
    """Initialize the rclpy context, spin the AnglesConverter node, and shutdown cleanly."""
    rclpy.init()
    node = AnglesConverter()
    rclpy.spin(node)
    node.destroy_node()
    rclpy.shutdown()


if __name__ == '__main__':
    main()