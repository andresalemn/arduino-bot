# arduinobot_controller — Launch File Tutorials

Two launch files are provided by this package. Each tutorial below walks
through what the launch file does, what arguments it accepts, and how to
run it.

---

## Tutorial 1 — `controller.launch.py`

### Purpose

Brings up the full `ros2_control` stack for the Arduinobot:

- `joint_state_broadcaster` — publishes the current joint positions to
  `/joint_states`.
- `arm_controller` — a `JointTrajectoryController` for `joint_1`,
  `joint_2`, and `joint_3`.
- `gripper_controller` — a `JointTrajectoryController` for `joint_4`.

When running on **real hardware** (`is_sim:=False`), it additionally starts:

- `robot_state_publisher` — parses the URDF and publishes TF transforms.
- `ros2_control_node` — loads the hardware interface that communicates
  with the Arduino over serial.

When running in **simulation** (`is_sim:=True`, the default), those two
nodes are skipped because Gazebo and its `ros2_control` plugin already
provide them.

### Launch arguments

| Argument | Default | Description |
|---|---|---|
| `is_sim` | `True` | Set to `False` when running on real hardware. |

### Usage

**Simulation mode (default)**

```bash
ros2 launch arduinobot_controller controller.launch.py
```

**Real-hardware mode**

```bash
ros2 launch arduinobot_controller controller.launch.py is_sim:=False
```

> **Note:** In real-hardware mode the `ros2_control_node` reads
> `config/arduinobot_controllers.yaml` and opens the serial port defined
> in the URDF `<ros2_control>` tag. Make sure the Arduino is connected and
> the port path is correct before launching.

---
