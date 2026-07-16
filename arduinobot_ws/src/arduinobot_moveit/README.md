# arduinobot_moveit

MoveIt 2 configuration package for the Arduinobot robot arm project. It provides motion planning, kinematic configurations, joint limits, and collision avoidance setups.

## Purpose and Responsibilities
This package is responsible for configuring and launching the MoveIt 2 motion planning framework for the Arduinobot arm. It groups joint parameters, limits, kinematics algorithms, and collision matrices together, allowing upper-level applications (such as MoveIt interfaces) to compute collision-free joint trajectories and execute them on simulated or physical hardware.

## Package Structure
```text
arduinobot_moveit/
├── CMakeLists.txt                      # CMake build script
├── package.xml                         # Package metadata and dependencies
├── config/                             # MoveIt configuration files
│   ├── arduinobot.srdf                 # Semantic Robot Description Format (SRDF)
│   ├── moveit_controllers.yaml         # Links MoveIt to ROS 2 trajectory controllers
│   ├── joint_limits.yaml               # Defines velocity/acceleration limits for each joint
│   ├── kinematics.yaml                 # Kinematic solver configurations for planning groups
│   ├── initial_positions.yaml          # Default startup positions for joints
│   ├── pilz_cartesian_limits.yaml      # Cartesian speed limits for Pilz planner
│   ├── planning_python_api.yaml        # Configurations for planning API
│   └── config6.rviz                    # Preconfigured RViz visualization profile
└── launch/                             # ROS 2 launch files
    └── moveit.launch.py                # Main launch file for MoveIt and RViz
```

## Motion Planning Configuration

### Planning Groups
Configured inside [config/arduinobot.srdf](config/arduinobot.srdf):
- **`arm`**: Includes `virtual_joint`, `joint_1`, `joint_2`, `joint_3`, and the fixed `horizontal_arm_to_claw_support` joint.
- **`gripper`**: Includes `joint_4` and `joint_5` representing the actuated fingers.

### Controller Integration
Configured inside [config/moveit_controllers.yaml](config/moveit_controllers.yaml):
- Connects MoveIt planning outputs to the ROS 2 controller interface using action namespaces:
  - `/arm_controller/follow_joint_trajectory` (Type: `FollowJointTrajectory`)
  - `/gripper_controller/follow_joint_trajectory` (Type: `FollowJointTrajectory`)

---

## Launch Files

### [moveit.launch.py](launch/moveit.launch.py)
Spins up the main MoveIt execution node (`move_group`) and opens RViz2 configured to visualize the motion planning scene.
- **Arguments**:
  - `is_sim` (Default: `True`): Toggles whether to use simulation time (`use_sim_time`).

---

## Dependencies
- `rviz2` (Robot visualizer)
- `moveit_config_utils` (MoveIt 2 configuration loading utility)
- `moveit_ros_move_group` (Exposes action servers for planning and control)
- `arduinobot_description` (Provides URDF robot description files)

---

## Build and Run Instructions

### Build
To build this package:
```bash
colcon build --packages-select arduinobot_moveit
```

### Run
To launch MoveIt 2 and RViz:
```bash
ros2 launch arduinobot_moveit moveit.launch.py
```

To run with physical hardware (simulated time disabled):
```bash
ros2 launch arduinobot_moveit moveit.launch.py is_sim:=False
```
