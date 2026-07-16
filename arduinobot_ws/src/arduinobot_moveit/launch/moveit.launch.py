import os
from launch import LaunchDescription
from moveit_configs_utils import MoveItConfigsBuilder
from launch_ros.actions import Node
from launch.actions import DeclareLaunchArgument
from launch.substitutions import LaunchConfiguration
from ament_index_python.packages import get_package_share_directory

"""
Launch file to spin up MoveIt 2 motion planning for Arduinobot.
Lunches the move_group node and RViz2 visualization with the configured semantic details, 
kinematics, joint limits, and controller settings.
"""

def generate_launch_description():

    # Argument to toggle between simulation and hardware time
    is_sim = LaunchConfiguration("is_sim")
    
    is_sim_arg = DeclareLaunchArgument(
        "is_sim",
        default_value="True",
        description="Whether to use simulation (use_sim_time)"
    )

    # Build the MoveIt 2 configuration using MoveItConfigsBuilder.
    # Reads kinematic, joint limit, and controller configuration files.
    moveit_config = (
        MoveItConfigsBuilder("arduinobot", package_name="arduinobot_moveit")
        .robot_description(file_path=os.path.join(
            get_package_share_directory("arduinobot_description"), "urdf", "arduinobot.urdf.xacro"
        ))
        .robot_description_semantic(file_path=os.path.join(
            get_package_share_directory("arduinobot_moveit"), "config", "arduinobot.srdf"
        ))
        .trajectory_execution(file_path=os.path.join(
            get_package_share_directory("arduinobot_moveit"), "config", "moveit_controllers.yaml"
        ))
        .to_moveit_configs()
    )

    # Start the Move Group node which exposes action servers for planning and control
    move_group_node = Node(
        package="moveit_ros_move_group",
        executable="move_group",
        output="screen",
        parameters=[moveit_config.to_dict(), 
                    {"use_sim_time": is_sim},
                    {"publish_robot_description_semantic": True}],
        arguments=["--ros-args", "--log-level", "info"],
    )

    # Start RViz2 configured with MoveIt display plugins
    rviz_config = os.path.join(
        get_package_share_directory("arduinobot_moveit"),
            "config",
            "config6.rviz",
    )
    rviz_node = Node(
        package="rviz2",
        executable="rviz2",
        name="rviz2",
        output="log",
        arguments=["-d", rviz_config],
        parameters=[
            moveit_config.robot_description,
            moveit_config.robot_description_semantic,
            moveit_config.robot_description_kinematics,
            moveit_config.joint_limits,
        ],
    )

    return LaunchDescription(
        [
            is_sim_arg,
            move_group_node, 
            rviz_node
        ]
    )