# Launch file: gazebo.launch.py
#
# Brings up the ArduinoBot model inside Gazebo (Ignition/gz-sim) with a
# custom world and spawns the robot. A ros_gz_bridge is started to relay the
# Gazebo clock to ROS 2 so that nodes using sim time stay synchronised.
#
# Nodes / included launches:
#   - robot_state_publisher   : publishes TF from URDF; use_sim_time=True so
#                                  it tracks the Gazebo clock, not wall time.
#   - gz_sim.launch.py        : Gazebo simulator (from ros_gz_sim), started
#                                  with worlds/my_world.sdf which pre-configures
#                                  physics (10 ms step, RTF=1.0) to avoid the
#                                  sluggish real-time factor seen with the default
#                                  Gazebo empty world.
#   - ros_gz_sim create       : spawns the robot into the running Gazebo scene
#                                  by reading the robot_description topic.
#   - ros_gz_bridge           : bidirectional bridge for /clock so all ROS 2
#                                  nodes using use_sim_time receive Gazebo time.
#
# Launch arguments:
#   model : Absolute path to the URDF/Xacro file to load.
#           Defaults to arduinobot.urdf.xacro in this package.
#
# Environment variables set:
#   GZ_SIM_RESOURCE_PATH : points to the parent of the package share directory
#                           so Gazebo can resolve mesh paths of the form
#                           "package://arduinobot_description/meshes/...".

import os
from pathlib import Path
from ament_index_python.packages import get_package_share_directory

from launch import LaunchDescription
from launch.actions import (
    DeclareLaunchArgument,
    IncludeLaunchDescription,
    SetEnvironmentVariable,
)
from launch.substitutions import Command, LaunchConfiguration
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch_ros.actions import Node
from launch_ros.parameter_descriptions import ParameterValue


def generate_launch_description():
    arduinobot_description_dir = get_package_share_directory("arduinobot_description")

    # Custom world with physics pre-tuned to RTF=1.0 and a 10 ms step size.
    # Without this, Gazebo starts with its default empty world whose real-time
    # factor often runs well below 1.0 on first launch, requiring manual
    # adjustment through the GUI on every session.
    world_path = os.path.join(arduinobot_description_dir, "worlds", "my_world.sdf")

    model_arg = DeclareLaunchArgument(
        name="model",
        default_value=os.path.join(
            arduinobot_description_dir, "urdf", "arduinobot.urdf.xacro"
        ),
        description="Absolute path to the robot URDF file",
    )

    # GZ_SIM_RESOURCE_PATH must point to the *parent* of the package share
    # directory so Gazebo can resolve "package://arduinobot_description/..."
    # URIs in the URDF meshes without an extra resource-path plugin.
    gazebo_resource_path = SetEnvironmentVariable(
        name="GZ_SIM_RESOURCE_PATH",
        value=[str(Path(arduinobot_description_dir).parent.resolve())],
    )

    ros_distro = os.environ["ROS_DISTRO"]
    # Commented-out is_ignition / physics_engine flags kept for reference:
    # they were used to switch between Ignition (Humble) and gz-sim (Iron+)
    # backends. The current setup targets gz-sim (Iron / Jazzy).
    # is_ignition = "True" if ros_distro == "humble" else "False"
    # physics_engine = "" if ros_distro == "humble" else "--phyisics-engine gz-physics-bullet-featherstone-plugin"

    # Evaluate the xacro at launch time. The is_ignition xacro arg (commented
    # out below) selects between ign_ros2_control and gz_ros2_control plugins
    # inside the URDF; kept for future use if multi-distro support is needed.
    robot_description = ParameterValue(
        Command(
            [
                "xacro ",
                LaunchConfiguration("model"),
                # " is_ignition:=",
                # is_ignition,
            ]
        ),
        value_type=str,
    )

    # use_sim_time=True is critical: it makes robot_state_publisher consume
    # the /clock topic published by the bridge instead of the system clock,
    # keeping TF timestamps consistent with Gazebo's simulation time.
    robot_state_publisher = Node(
        package="robot_state_publisher",
        executable="robot_state_publisher",
        parameters=[{"robot_description": robot_description, "use_sim_time": True}],
    )

    # -v 4 : verbosity level 4 (info-level Gazebo logs).
    # -r   : run the simulation immediately without pausing on startup.
    gazebo = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(
            [
                os.path.join(
                    get_package_share_directory("ros_gz_sim"),
                    "launch",
                    "gz_sim.launch.py",
                )
            ]
        ),
        launch_arguments=[("gz_args", f"-v 4 -r {world_path}")],
    )

    # Spawns the robot model into Gazebo by reading the robot_description
    # topic (populated by robot_state_publisher above).
    gz_spawn_entity = Node(
        package="ros_gz_sim",
        executable="create",
        output="screen",
        arguments=["-topic", "robot_description", "-name", "arduinobot"],
    )

    # Bridge the Gazebo clock to ROS 2 so that all nodes with use_sim_time=True
    # receive consistent simulation timestamps.
    # Camera topics (/image_raw, /camera_info) are commented out; uncomment and
    # add the corresponding bridge entries when the vision pipeline is enabled.
    gz_ros2_bridge = Node(
        package="ros_gz_bridge",
        executable="parameter_bridge",
        arguments=[
            "/clock@rosgraph_msgs/msg/Clock[gz.msgs.Clock",
            # "/image_raw@sensor_msgs/msg/Image[gz.msgs.Image",
            # "/camera_info@sensor_msgs/msg/CameraInfo[gz.msgs.CameraInfo",
        ],
    )

    # joint_state_publisher_gui and rviz2 are commented out for Gazebo mode:
    # joint states are owned by the ros2_control stack inside the simulator,
    # and visualisation can be added separately if needed.
    # joint_state_publisher_gui = Node(...)
    # rviz_node = Node(...)

    return LaunchDescription(
        [
            model_arg,
            gazebo_resource_path,
            robot_state_publisher,
            # joint_state_publisher_gui,
            # rviz_node,
            gazebo,
            gz_spawn_entity,
            gz_ros2_bridge,
        ]
    )
