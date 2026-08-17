# arduinobot_bringup

Bringup package for the Arduinobot robot arm. It aggregates and orchestrates top-level launch files for combining motion planning, hardware interfaces, and user interaction nodes to control the real robot.

---

## Package responsibilities

- Provide a single entry point for launching the real robotic arm.
- Coordinate dependencies and parameter propagation to downstream modules (`arduinobot_controller`, `arduinobot_moveit`, and `arduinobot_remote`).

---

## Package structure

```
arduinobot_bringup/
├── launch/
│   └── real_robot.launch.py  # Launches real robot controller, MoveIt, and remote interface
├── CMakeLists.txt
└── package.xml
```

---

## Launch files

### `real_robot.launch.py`

Composes the entire control and planning system for the real robot:
- Launches `arduinobot_controller/controller.launch.py` with `is_sim:=False` to start the serial bridge and load the controller managers.
- Launches `arduinobot_moveit/moveit.launch.py` with `is_sim:=False` for MoveIt motion planning and RViz2 interface.
- Launches `arduinobot_remote/remote_interface.launch.py` with `is_sim:=False` to accept remote task commands.

---

## Build

```bash
cd ~/ros2/arduino-bot/arduinobot_ws
colcon build --packages-select arduinobot_bringup
source install/setup.bash
```

---

## Run

**Launch the real robot pipeline**

```bash
ros2 launch arduinobot_bringup real_robot.launch.py
```

---

## Dependencies

| Dependency | Type | Purpose |
|---|---|---|
| `ros2launch` | build + exec | ROS 2 Launch command tool. |
| `arduinobot_controller` | exec | Controls joint states and wraps serial connection. |
| `arduinobot_moveit` | exec | MoveIt 2 configuration and execution nodes. |
| `arduinobot_remote` | exec | Provides interfaces for remote joint command routing. |
