# arduinobot_remote

Remote control interface for the Arduinobot robot arm. It hosts a Python Flask web server for receiving commands from the Alexa Skills Kit, and runs action servers in C++ and Python to plan and execute trajectories via MoveIt 2.

---

## Package responsibilities

- Exposes a Flask web application that acts as an Alexa Skill backend, translating Alexa Voice Service HTTPS requests into ROS 2 action goals.
- Provides the `/task_server` Action Server in C++ and Python to map target task IDs to joint-space planning targets using MoveIt 2.

---

## Package structure

```
arduinobot_remote/
├── arduinobot_remote/            # Python module
│   ├── alexa_interface.py        # Flask server + ROS 2 action client
│   └── task_server.py            # Python MoveItPy task action server
├── doc/alexa/                    # Alexa skill architecture and setup guides
│   ├── 1_introduction.md          # Alexa Skill concepts (intents, utterances)
│   ├── 2_architecture.md          # System request flow diagram
│   ├── 3_flask_ngrok_setup.md     # Installation and HTTPS tunneling setup
│   └── 4_usage_tutorial.md        # Interactive testing & troubleshooting
├── include/                      # C++ headers
├── launch/
│   └── remote_interface.launch.py # Top-level launcher for task server and Flask
├── src/
│   └── task_server.cpp           # C++ MoveGroupInterface task action server
├── CMakeLists.txt
└── package.xml
```

---

## Nodes

### `task_server` (C++) / `task_server.py` (Python)
Action servers that listen for trajectory planning goals. The C++ version is run by default; the Python implementation (`MoveItPy`) can be selected via the `use_python:=True` launch argument.

**Actions Provided**

| Action | Type | Description |
|---|---|---|
| `/task_server` | `arduinobot_msgs/action/ArduinobotTask` | Accepts `task_number` to plan and execute a trajectory. |

### `alexa_interface.py`
Forks a thread to run the ROS 2 Action Client and starts a Flask server on the main thread (listening on port 5000) to receive requests forwarded by ngrok.

---

## Launch files

### `remote_interface.launch.py`

Brings up the remote task server (C++ or Python) and the Flask receiver interface.

| Argument | Default | Description |
|---|---|---|
| `is_sim` | `True` | Forwarded to `use_sim_time` on all nodes. |
| `use_python` | `False` | When `True`, runs the Python version of the task server instead of the C++ binary. |

---

## Build

```bash
cd ~/ros2/arduino-bot/arduinobot_ws
colcon build --packages-select arduinobot_remote
source install/setup.bash
```

---

## Run

**Start remote interface (default C++ Task Server)**

```bash
ros2 launch arduinobot_remote remote_interface.launch.py
```

**Start remote interface (Python Task Server)**

```bash
ros2 launch arduinobot_remote remote_interface.launch.py use_python:=True
```

*Note: For complete setup details including ngrok configurations and Alexa skills mapping, refer to [Flask/Ngrok Setup Guide](doc/alexa/3_flask_ngrok_setup.md).*

---

## Dependencies

| Dependency | Type | Purpose |
|---|---|---|
| `rclcpp` | build + exec | ROS 2 C++ client library. |
| `rclcpp_action` | build + exec | Action server/client implementation in C++. |
| `rclcpp_components` | build + exec | Composable node support. |
| `moveit_ros_planning_interface` | build + exec | MoveIt 2 planning interface for C++. |
| `arduinobot_msgs` | build + exec | Custom task action types. |
| `ros2launch` | exec | Launch system utilities. |
| `arduinobot_moveit` | exec | MoveIt config packages. |
