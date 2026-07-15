import os
from launch import LaunchDescription
from launch_ros.actions import Node
from launch.actions import IncludeLaunchDescription, DeclareLaunchArgument
from launch.conditions import IfCondition, UnlessCondition
from launch.substitutions import LaunchConfiguration
from ament_index_python.packages import get_package_share_directory


def generate_launch_description():

    # use_cpp selects which implementation of the slider control node to run.
    # Both implementations are functionally identical; the C++ version exists
    # for performance comparison during sim-to-real validation.
    use_cpp_arg = DeclareLaunchArgument(
        "use_cpp",
        default_value="False",
        description="Set to True to use the C++ slider_control node instead of the Python one"
    )

    use_cpp = LaunchConfiguration("use_cpp")

    # Bring up the ros2_control stack (controller_manager, joint_state_broadcaster,
    # arm_controller, gripper_controller). is_sim=True skips robot_state_publisher
    # and the hardware ros2_control node because the simulation provides them.
    controller = IncludeLaunchDescription(
            os.path.join(
                get_package_share_directory("arduinobot_controller"),
                "launch",
                "controller.launch.py"
            ),
            launch_arguments={"is_sim": "True"}.items()
        )

    # The GUI publishes on /joint_states by default. It is remapped to
    # /joint_commands so that slider_control can subscribe to it without
    # conflicting with the /joint_states topic published by joint_state_broadcaster.
    joint_state_publisher_gui_node = Node(
        package="joint_state_publisher_gui",
        executable="joint_state_publisher_gui",
        remappings=[
            ("/joint_states", "/joint_commands"),
        ]
    )

    # Only one of the two implementations is started, chosen by use_cpp.
    slider_control_node_py = Node(
        package="arduinobot_controller",
        executable="slider_control",
        condition=UnlessCondition(use_cpp)
    )

    slider_control_node_cpp = Node(
        package="arduinobot_controller",
        executable="slider_control_cpp",
        condition=IfCondition(use_cpp)
    )

    return LaunchDescription(
        [
            use_cpp_arg,
            controller,
            joint_state_publisher_gui_node,
            slider_control_node_py,
            slider_control_node_cpp,
        ]
    )