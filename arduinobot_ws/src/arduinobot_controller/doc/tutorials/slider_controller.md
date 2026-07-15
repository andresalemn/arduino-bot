# arduinobot_controller — Launch File Tutorials

Two launch files are provided by this package. Each tutorial below walks
through what the launch file does, what arguments it accepts, and how to
run it.

---

## Tutorial 2 — `slider_controller.launch.py`

### Purpose

Provides an interactive way to move all Arduinobot joints using the
`joint_state_publisher_gui` slider panel. It composes three things:

1. The full `ros2_control` stack (via `controller.launch.py` with
   `is_sim:=True`).
2. A `joint_state_publisher_gui` node whose output is remapped from the
   default `/joint_states` to `/joint_commands`, so it does not conflict
   with the `/joint_states` topic that the `joint_state_broadcaster`
   already owns.
3. A *slider control* node that reads `/joint_commands` and forwards the
   positions as `JointTrajectory` goals to the arm and gripper
   controllers.

The slider control node is available in two implementations — Python and
C++ — selectable at launch time via the `use_cpp` argument. Both are
functionally identical; the C++ version exists for performance comparison
during sim-to-real validation.

### Launch arguments

| Argument | Default | Description |
|---|---|---|
| `use_cpp` | `False` | Set to `True` to run the compiled C++ `slider_control_cpp` node instead of the Python one. |

### Verifying the package with Gazebo

The recommended way to confirm that the slider pipeline works end-to-end
is to run Gazebo alongside the slider launch. Open **three terminals**,
all sourced from the same workspace.

#### Terminal 1 — Build the workspace

```bash
cd ~/ros2/arduino-bot/arduinobot_ws
colcon build --symlink-install
source install/setup.bash
```

> **Tip:** After the first build you only need to rebuild if you change
> C++ source files. Python nodes and launch files installed with
> `--symlink-install` are picked up automatically without rebuilding.

#### Terminal 2 — Launch Gazebo

```bash
source ~/ros2/arduino-bot/arduinobot_ws/install/setup.bash
ros2 launch arduinobot_description gazebo.launch.py
```

This starts Gazebo with a pre-tuned world (`my_world.sdf`), spawns the
Arduinobot model, and bridges the Gazebo clock to ROS 2. Wait until the
robot appears in the Gazebo viewport before continuing.

#### Terminal 3a — Launch the slider (Python node, default)

```bash
source ~/ros2/arduino-bot/arduinobot_ws/install/setup.bash
ros2 launch arduinobot_controller slider_controller.launch.py
```

#### Terminal 3b — Launch the slider (C++ node)

```bash
source ~/ros2/arduino-bot/arduinobot_ws/install/setup.bash
ros2 launch arduinobot_controller slider_controller.launch.py use_cpp:=True
```

A GUI window with sliders for all four joints will appear. Moving a
slider sends a `JointTrajectory` goal through the
`arm_controller`/`gripper_controller` topics, and the robot in Gazebo
should move accordingly.

### Data flow

```
joint_state_publisher_gui
        |  /joint_commands  (JointState)
        v
 slider_control  (Python or C++)
        |
        +---> arm_controller/joint_trajectory    (JointTrajectory)
        |              |
        |              v
        |       arm_controller  ---> joint_1, joint_2, joint_3
        |
        +---> gripper_controller/joint_trajectory (JointTrajectory)
                       |
                       v
                gripper_controller ---> joint_4
```

> **Note:** The `/joint_states -> /joint_commands` remapping on
> `joint_state_publisher_gui` is intentional. Without it, the GUI would
> publish on `/joint_states` and overwrite the topic that
> `joint_state_broadcaster` owns, causing the robot state in Gazebo and
> RViz to flicker.
