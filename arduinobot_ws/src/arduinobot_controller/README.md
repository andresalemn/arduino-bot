# arduinobot_controller

ROS 2 controller package for the Arduinobot robot arm. Provides a
`ros2_control` hardware interface for serial communication with an Arduino,
`JointTrajectoryController` instances for the arm and gripper, and utility
nodes for forwarding joint commands from a GUI slider panel to those
controllers.

---

## Package responsibilities

- Declare and load the `ArduinobotInterface` hardware interface plugin,
  which reads joint positions from and writes position commands to the
  Arduino over a serial connection.
- Configure and spawn the `arm_controller`, `gripper_controller`, and
  `joint_state_broadcaster` through `ros2_control`'s controller manager.
- Provide `slider_control` nodes (Python and C++) that bridge the
  `joint_state_publisher_gui` output to the trajectory controllers.
- Expose launch files for both simulation and real-hardware scenarios.

---

## Package structure

```
arduinobot_controller/
├── arduinobot_controller/      # Python module
│   ├── __init__.py
│   └── slider_control.py       # Python slider control node
├── config/
│   └── arduinobot_controllers.yaml  # ros2_control parameter file
├── doc/tutorials/
│   └── controller_launch.md     # Step-by-step launch file usage guide
│   └── slider_controller.md     # Step-by-step launch file usage guide
├── include/
│   └── arduinobot_interface.hpp  # ros2_control hardware interface header
├── launch/
│   ├── controller.launch.py    # ros2_control stack launch
│   └── slider_controller.launch.py  # Slider GUI + controller launch
├── src/
│   └── slider_control.cpp      # C++ slider control node
├── CMakeLists.txt
├── package.xml
├── setup.cfg
└── setup.py
```

---

## Nodes

### `slider_control` (Python) / `slider_control_cpp` (C++)

Both are regular (non-lifecycle) nodes. They are functionally identical;
the C++ version exists for performance comparison during sim-to-real
validation. The active implementation is selected at launch time via the
`use_cpp` argument.

**Subscriptions**

| Topic | Type | Description |
|---|---|---|
| `joint_commands` | `sensor_msgs/JointState` | Joint positions from the GUI slider, remapped from the default `/joint_states` output of `joint_state_publisher_gui`. |

**Publications**

| Topic | Type | Description |
|---|---|---|
| `arm_controller/joint_trajectory` | `trajectory_msgs/JointTrajectory` | Position goals for `joint_1`, `joint_2`, `joint_3`. |
| `gripper_controller/joint_trajectory` | `trajectory_msgs/JointTrajectory` | Position goal for `joint_4`. |

### `ArduinobotInterface` (ros2_control hardware interface plugin)

This is not a standalone node. It is a `hardware_interface::SystemInterface`
plugin loaded and lifecycle-managed by `controller_manager`. It opens a
serial connection to the Arduino on activation and closes it on
deactivation.

The serial port path is read from the `<ros2_control>` block in the robot
URDF at startup. No ROS parameters are exposed directly; configuration
is done through the URDF hardware description.

---

## Parameters

Controller parameters are defined in
[`config/arduinobot_controllers.yaml`](config/arduinobot_controllers.yaml)
and loaded by `controller_manager` at startup.

| Parameter | Value | Description |
|---|---|---|
| `controller_manager.update_rate` | `10` Hz | Control loop frequency. |
| `arm_controller.joints` | `[joint_1, joint_2, joint_3]` | Joints managed by the arm trajectory controller. |
| `arm_controller.command_interfaces` | `[position]` | Command interface type. |
| `arm_controller.open_loop_control` | `true` | Commands are sent directly without state feedback. |
| `arm_controller.allow_integration_in_goal_trajectories` | `true` | Allows velocity/acceleration integration when only positions are specified. |
| `gripper_controller.joints` | `[joint_4]` | Joint managed by the gripper trajectory controller. |
| `gripper_controller.command_interfaces` | `[position]` | Command interface type. |
| `gripper_controller.open_loop_control` | `true` | Commands are sent directly without state feedback. |

---

## Launch files

### `controller.launch.py`

Spawns the `ros2_control` controller stack: `joint_state_broadcaster`,
`arm_controller`, and `gripper_controller`.

In real-hardware mode (`is_sim:=False`) it additionally starts
`robot_state_publisher` and `ros2_control_node` (with the Arduino hardware
interface). In simulation mode (default) those are provided by Gazebo and
its `ros2_control` plugin.

| Argument | Default | Description |
|---|---|---|
| `is_sim` | `True` | Skip hardware nodes when running inside a simulator. |

### `slider_controller.launch.py`

Composes the full slider pipeline: the `ros2_control` stack (simulation
mode), `joint_state_publisher_gui`, and the slider control node. Intended
for interactive joint-level testing without writing a dedicated action
client.

| Argument | Default | Description |
|---|---|---|
| `use_cpp` | `False` | Use the C++ `slider_control_cpp` node instead of the Python one. |

> See [`doc/tutorials/controller_launch.md`](doc/tutorials/controller_launch.md) 
> and [`doc/tutorials/slider_controller.md`](doc/tutorials/slider_controller.md) 
> for a step-by-step walkthrough including a 3-terminal Gazebo example.

---

## Build

```bash
cd ~/ros2/arduino-bot/arduinobot_ws
colcon build --packages-select arduinobot_controller --symlink-install
source install/setup.bash
```

`--symlink-install` is recommended during development: Python nodes and
launch files are picked up immediately without a rebuild after edits.

---

## Run

**ros2_control stack only (simulation)**

```bash
ros2 launch arduinobot_controller controller.launch.py
```

**ros2_control stack only (real hardware)**

```bash
ros2 launch arduinobot_controller controller.launch.py is_sim:=False
```

**Slider GUI — Python node (default)**

```bash
ros2 launch arduinobot_controller slider_controller.launch.py
```

**Slider GUI — C++ node**

```bash
ros2 launch arduinobot_controller slider_controller.launch.py use_cpp:=True
```

---

## Dependencies

| Dependency | Type | Purpose |
|---|---|---|
| `rclcpp` | build + exec | C++ ROS 2 client library. |
| `rclpy` | build + exec | Python ROS 2 client library. |
| `trajectory_msgs` | build + exec | `JointTrajectory` message type. |
| `sensor_msgs` | build + exec | `JointState` message type. |
| `controller_manager` | exec | Manages the ros2_control lifecycle. |
| `robot_state_publisher` | exec | Publishes TF from the URDF (real hardware only). |
| `joint_state_publisher_gui` | exec | Slider panel used by `slider_controller.launch.py`. |
| `arduinobot_description` | exec | Provides the URDF/Xacro robot description. |
| `xacro` | exec | Processes the Xacro robot description at launch time. |
| `ros2launch` | exec | Required to invoke `ros2 launch`. |
