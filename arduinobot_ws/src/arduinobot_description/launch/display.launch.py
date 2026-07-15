# Launch file: display.launch.py
#
# Brings up the ArduinoBot model in RViz for offline visual inspection and
# joint-position testing. No simulation engine is involved — joint states are
# driven interactively through joint_state_publisher_gui.
#
# Nodes launched:
#   - robot_state_publisher   : publishes TF transforms from the URDF.
#   - joint_state_publisher_gui : provides a GUI slider panel to manually
#                                  set joint positions (useful for URDF
#                                  validation without a controller stack).
#   - rviz2                   : opens with the pre-configured display.rviz
#                                  layout so the robot model and TF frames
#                                  are visible immediately.
#
# Launch arguments:
#   model : Absolute path to the URDF/Xacro file to load.
#           Defaults to arduinobot.urdf.xacro in this package.

from launch import LaunchDescription
from launch_ros.actions import Node
from launch_ros.parameter_descriptions import ParameterValue
from launch.actions import DeclareLaunchArgument
from launch.substitutions import Command, LaunchConfiguration
import os
from ament_index_python.packages import get_package_share_directory


def generate_launch_description():
    model_arg = DeclareLaunchArgument(
        name="model",
        default_value=os.path.join(
            get_package_share_directory("arduinobot_description"),
            "urdf",
            "arduinobot.urdf.xacro",
        ),
        description="Absolute path to the robot URDF file",
    )

    # Evaluate the xacro file at launch time and pass the resulting URDF XML
    # string as the robot_description parameter.
    robot_description = ParameterValue(Command(["xacro ", LaunchConfiguration("model")]))

    robot_state_publisher = Node(
        package="robot_state_publisher",
        executable="robot_state_publisher",
        parameters=[{"robot_description": robot_description}],
    )

    # GUI sliders for manually commanding joint positions — replaces a live
    # controller stack during URDF development and visual verification.
    joint_state_publisher_gui = Node(
        package="joint_state_publisher_gui",
        executable="joint_state_publisher_gui",
    )

    rviz_node = Node(
        package="rviz2",
        executable="rviz2",
        name="rviz2",
        output="screen",
        arguments=[
            "-d",
            os.path.join(
                get_package_share_directory("arduinobot_description"),
                "rviz",
                "display.rviz",
            ),
        ],
    )

    return LaunchDescription([
        model_arg,
        robot_state_publisher,
        joint_state_publisher_gui,
        rviz_node,
    ])