# arduinobot_description

Provides the complete robot description for the ArduinoBot: URDF/Xacro model,
STL meshes, Gazebo world, RViz configuration, and the launch files needed to
visualise or simulate the robot.

## Purpose and responsibilities

This package is the single source of truth for the robot's physical model. It
defines kinematics, collision geometry, inertial properties, sensor placement,
and the hardware interfaces exposed to `ros2_control`. It does not contain
controllers, motion planners, or application logic.

## Package structure

```
arduinobot_description/
├── urdf/
│   ├── arduinobot.urdf.xacro           # Main robot model (links, joints, macros)
│   ├── arduinobot_gazebo.xacro         # Gazebo plugin & camera sensor config
│   └── arduinobot_ros2_control.xacro   # ros2_control hardware interfaces
├── meshes/                             # STL geometry files for all links
│   ├── basement.STL                    # Base/mounting plate
│   ├── base_plate.STL                  # Rotating base plate (joint_1)
│   ├── forward_drive_arm.STL           # First arm segment (joint_2)
│   ├── horizontal_arm.STL              # Second arm segment (joint_3)
│   ├── claw_support.STL                # Gripper mounting bracket (fixed)
│   ├── right_finger.STL                # Right gripper finger (joint_4)
│   ├── left_finger.STL                 # Left gripper finger (joint_5, mimic)
│   ├── pi_camera.STL                   # RGB camera body (fixed)
│   ├── forward_drive_arm.STL           # (arm mesh reused)
│   ├── link.STL                        # Generic structural link
│   ├── plate.STL                       # Structural plate
│   ├── round_plate.STL                 # Circular mounting plate
│   ├── servo_plate.STL                 # Servo attachment plate
│   ├── triangular_link.STL             # Triangular structural member
│   └── vertical_drive_arm.STL          # Vertical arm segment
├── worlds/
│   └── my_world.sdf                    # Custom Gazebo world (RTF-tuned physics)
├── launch/
│   ├── display.launch.py               # RViz visualisation (no simulation)
│   └── gazebo.launch.py                # Full Gazebo simulation
├── rviz/
│   └── display.rviz                    # Pre-configured RViz layout
├── CMakeLists.txt
└── package.xml
```

## Robot model

The ArduinoBot is a 5-DOF desktop robotic arm with a parallel-jaw gripper and
a fixed RGB camera. All meshes are scaled at 0.01 in all axes (source STL
units are millimetres; URDF units are metres).

### Links

| Link | Mesh | Description |
|---|---|---|
| `world` | — | Fixed inertial frame anchor |
| `base_link` | `basement.STL` | Robot base; fixed to `world` |
| `base_plate` | `base_plate.STL` | Rotating turret (joint_1) |
| `forward_drive_arm` | `forward_drive_arm.STL` | First arm segment (joint_2) |
| `horizontal_arm` | `horizontal_arm.STL` | Second arm segment (joint_3) |
| `claw_support` | `claw_support.STL` | Gripper bracket; fixed to `horizontal_arm` |
| `gripper_right` | `right_finger.STL` | Right finger (joint_4) |
| `gripper_left` | `left_finger.STL` | Left finger (joint_5, mirrors joint_4) |
| `rgb_camera` | `pi_camera.STL` | Camera; fixed to `base_link` |

### Joints

| Joint | Type | Parent | Child | Axis | Limits |
|---|---|---|---|---|---|
| `virtual_joint` | fixed | `world` | `base_link` | — | — |
| `joint_1` | revolute | `base_link` | `base_plate` | Z | ±90° |
| `joint_2` | revolute | `base_plate` | `forward_drive_arm` | X | ±90° |
| `joint_3` | revolute | `forward_drive_arm` | `horizontal_arm` | X | ±90° |
| `horizontal_arm_to_claw_support` | fixed | `horizontal_arm` | `claw_support` | — | — |
| `joint_4` | revolute | `claw_support` | `gripper_right` | Z | −90° to 0° |
| `joint_5` | revolute | `claw_support` | `gripper_left` | Z | 0° to +90° (mimic joint_4 × −1) |
| `rgb_camera_joint` | fixed | `base_link` | `rgb_camera` | — | — |

All revolute joints share `effort = 30.0` and `velocity = 10.0`.

`joint_5` is a **mimic joint**: its position is always `−1 × joint_4`, so both
fingers move symmetrically with a single command.

### Sensors

| Sensor | Link | Type | Resolution | FoV | Update rate |
|---|---|---|---|---|---|
| `camera` | `rgb_camera` | Camera | 2304 × 1296 px | 69.4° H | 30 Hz |

The camera publishes on the `/image_raw` topic (Gazebo side). The
`ros_gz_bridge` entries for `/image_raw` and `/camera_info` are present but
commented out in `gazebo.launch.py`; uncomment them when the vision pipeline
is ready.

## ros2_control interfaces

Defined in `arduinobot_ros2_control.xacro`. The hardware plugin and simulation backend are selected
at xacro-evaluation time via the `is_sim` and `is_ignition` arguments:

| Argument | Default | Description |
|---|---|---|
| `is_sim` | `true` | Selects simulation hardware plugin (`ign_ros2_control/IgnitionSystem` or `gz_ros2_control-system::GazeboSimROS2ControlPlugin`) when `true`, and real serial hardware plugin (`arduinobot_controller/ArduinobotInterface`) when `false`. |
| `is_ignition` | `true` | Selects the older Ignition Gazebo integration (`ign_ros2_control`) when `true`, and the newer `gz_ros2_control` integration when `false`. |

Joints exposed to `ros2_control`:

| Joint | Command interface | State interface |
|---|---|---|
| `joint_1` | `position` [−π/2, π/2] | `position` |
| `joint_2` | `position` [−π/2, π/2] | `position` |
| `joint_3` | `position` [−π/2, π/2] | `position` |
| `joint_4` | `position` [−π/2, 0] | `position` |
| `joint_5` | `position` (mimic) | — |

Controller configuration is loaded from
`arduinobot_controller/config/arduinobot_controllers.yaml` by the Gazebo
ros2_control plugin at simulation startup.

## Launch files

### `display.launch.py` — RViz visualisation

Starts `robot_state_publisher`, `joint_state_publisher_gui`, and `rviz2`.
No simulation engine; useful for URDF development and visual verification.

```bash
ros2 launch arduinobot_description display.launch.py
# Override the model:
ros2 launch arduinobot_description display.launch.py model:=/path/to/robot.urdf.xacro
```

### `gazebo.launch.py` — Gazebo simulation

Starts `robot_state_publisher` (with `use_sim_time=True`), Gazebo via
`gz_sim.launch.py` using the custom `my_world.sdf`, spawns the robot, and
bridges the `/clock` topic to ROS 2.

```bash
ros2 launch arduinobot_description gazebo.launch.py
```

> **Note on `my_world.sdf`:** The default Gazebo empty world often runs at a
> real-time factor well below 1.0, requiring manual physics tuning via the GUI
> every session. `my_world.sdf` pre-configures ODE with a 10 ms step size and
> `real_time_factor = 1.0`, automating that step.

## Launch arguments

| Argument | Default | Description |
|---|---|---|
| `model` | `urdf/arduinobot.urdf.xacro` | Absolute path to the URDF/Xacro file |

## Topics

| Topic | Type | Direction | Source |
|---|---|---|---|
| `/robot_description` | `std_msgs/msg/String` | Published | `robot_state_publisher` |
| `/joint_states` | `sensor_msgs/msg/JointState` | Published | `joint_state_publisher_gui` (display) or ros2_control (Gazebo) |
| `/tf`, `/tf_static` | `tf2_msgs/msg/TFMessage` | Published | `robot_state_publisher` |
| `/clock` | `rosgraph_msgs/msg/Clock` | Bridged | `ros_gz_bridge` (Gazebo only) |
| `/image_raw` | `sensor_msgs/msg/Image` | Bridged (disabled) | `ros_gz_bridge` (camera, see note above) |

## Build

```bash
cd ~/ros2/arduino-bot/arduinobot_ws
colcon build --packages-select arduinobot_description
source install/setup.bash
```

## Dependencies

| Dependency | Purpose |
|---|---|
| `urdf` | URDF parsing support |
| `xacro` | Xacro macro processor for the robot model |
| `robot_state_publisher` | Publishes TF from the URDF |
| `joint_state_publisher_gui` | Manual joint control for RViz mode |
| `rviz2` | 3D visualisation |
| `ros2launch` | Launch system |
| `ros_gz_sim` | Gazebo simulator integration and spawn tool |
| `ros_gz_bridge` | ROS 2 ↔ Gazebo topic bridge |
| `gz_ros2_control` | Gazebo ros2_control plugin (ROS Iron and later) |
| `ign_ros2_control` | Gazebo ros2_control plugin (ROS Humble / Ignition) |
