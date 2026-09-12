<h1 align="center">arduinobot</h1>
<p align="center">Containerized ROS 2 Humble workspace running in Docker, targeting an Arduino-based robotic arm.</p>

<p align="center">
  <a href="https://docs.ros.org/en/humble/"><img src="https://img.shields.io/badge/ROS_2-Humble_LTS-4A4E9E?style=flat-square&logo=ros&logoColor=white" alt="ROS 2 Humble"></a>
  <a href="https://ubuntu.com/"><img src="https://img.shields.io/badge/Ubuntu-22.04_LTS-D9581E?style=flat-square&logo=ubuntu&logoColor=white" alt="Ubuntu 22.04"></a>
  <a href="https://www.docker.com/"><img src="https://img.shields.io/badge/Docker-Dev_Container-2496ED?style=flat-square&logo=docker&logoColor=white" alt="Docker"></a>
  <a href="LICENSE.txt"><img src="https://img.shields.io/badge/License-MIT-3C9D5C?style=flat-square&logo=opensourceinitiative&logoColor=white" alt="License MIT"></a>
</p>

---

## Overview

**arduinobot** is a 4-DOF desktop robotic arm constructed from 3D-printed links, driven by **MG90S metal-gear servo motors**, and controlled via ROS 2 Humble. 

This repository serves as a personal archive, hardware modification workspace, and extended reference built upon the [Robotics and ROS 2 - Learn by Doing: Manipulators](https://www.udemy.com/course/robotics-and-ros-2-learn-by-doing-manipulators/) course on Udemy by Antonio Brandi.

<!-- MEDIA: Main Project Banner or Digital Twin & Physical Hardware Side-by-Side Image -->
<!-- <p align="center"> -->
<!--   <img src="resources/media/gazebo_sim_demo.gif" width="48%" alt="Gazebo Simulation Demo" /> -->
<!--   <img src="resources/media/physical_arm_demo.jpg" width="48%" alt="Physical Arm Manipulator" /> -->
<!-- </p> -->

---

## Core Capabilities & Features

- **🤖 Sim-to-Real Pipeline:** Seamless execution across digital twin simulation (Gazebo + RViz2) and physical hardware via custom `ros2_control` system interfaces.
- **🧠 Trajectory Motion Planning:** Integrated **MoveIt 2** configuration with kinematics, joint limits, and collision avoidance for arm and parallel gripper planning groups.
- **🗣️ Alexa Voice Remote Control:** Cloud-to-robot interface using Amazon Alexa Skills, forwarding voice intents (`Pick`, `Sleep`, `Wake`) through a Flask/ngrok bridge directly into ROS 2 action goals.
- **👁️ Integrated Vision Simulation:** RGB camera sensor model mounted on the base link for visual inspection and future perception pipelines.

---

## Key Enhancements & Infrastructure

This repository extends the original course baseline with custom hardware, software, and devops improvements:

### ⚙️ Hardware Modifications
- **MG90S Metal-Gear Servos:** Upgraded from SG90 plastic servos for higher torque and durability.
- **PCA9685 16-Channel PWM Driver Shield:** Offloads joint PWM signals over I2C (`SDA`/`SCL`).
- **Dedicated Servo Power Delivery:** External power terminal connection to protect the microcontroller from voltage dips and current spikes.

### 🚀 Software & Infrastructure Improvements
- **Docker & Dev Container Workflow:** Instant, reproducible ROS 2 Humble development environment without local host dependencies.
- **Microcontroller Integration:** Unified PlatformIO workspace structure alongside ROS 2 packages.
- **Structured Documentation System:** Centralized guides covering hardware wiring, pulse calibration, Alexa voice control, and Docker setup.
- **CI/CD Automation Ready:** Containerized simulation image setup for future automated GitHub Actions build pipelines.

> [!NOTE]
> For complete wiring schematics and pulse tuning procedures, see the **[Electronics & PCA9685 Wiring Guide](./docs/hardware/electronics.md)** and **[Servo Pulse Calibration Tutorial](./docs/hardware/servo-calibration.md)**.

---

## Software Features & ROS 2 Architecture

The codebase is structured into modular ROS 2 packages inside [`arduinobot_ws/`](./arduinobot_ws/):

| Package | Purpose & Features |
|---|---|
| [`arduinobot_bringup`](./arduinobot_ws/src/arduinobot_bringup) | Orchestrates full robot stack launching (Sim & Real) |
| [`arduinobot_controller`](./arduinobot_ws/src/arduinobot_controller) | C++ `ros2_control` hardware interface (`ArduinobotInterface`) over serial |
| [`arduinobot_description`](./arduinobot_ws/src/arduinobot_description) | URDF/Xacro kinematic models, STL meshes, and custom Gazebo worlds |
| [`arduinobot_moveit`](./arduinobot_ws/src/arduinobot_moveit) | MoveIt 2 motion planning, kinematics configuration, and SRDF |
| [`arduinobot_remote`](./arduinobot_ws/src/arduinobot_remote) | Flask server endpoint for **Alexa Voice Control** & task servers |
| [`arduinobot_firmware`](./arduinobot_ws/src/arduinobot_firmware) | Microcontroller firmware (`robot_control.cpp`) & PlatformIO setup |
| [`arduinobot_msgs`](./arduinobot_ws/src/arduinobot_msgs) | Custom action (`ArduinobotTask`) and service interface definitions |
| [`arduinobot_utils`](./arduinobot_ws/src/arduinobot_utils) | Coordinate transformation utilities (Euler $\leftrightarrow$ Quaternions) |
| [`arduinobot_cpp_examples`](./arduinobot_ws/src/arduinobot_cpp_examples) | Educational C++ ROS 2 templates (Publishers, Actions, Lifecycle) |
| [`arduinobot_py_examples`](./arduinobot_ws/src/arduinobot_py_examples) | Educational Python ROS 2 templates |

<!-- MEDIA: MoveIt 2 Motion Planning Animation / Video GIF -->
<!-- <p align="center"> -->
<!--   <img src="resources/media/moveit_planning_demo.gif" alt="MoveIt 2 Trajectory Planning" /> -->
<!-- </p> -->

---

## Quick Start & Execution Modes

### Option A: VS Code Dev Container (Recommended for Simulation)

Develop without installing ROS 2, Gazebo, or MoveIt 2 on your host OS.

1. Open this repository in **VS Code** and select **"Reopen in Container"**.
2. Run simulation inside the container terminal:
   ```bash
   cd arduinobot_ws
   ros2 launch arduinobot_bringup simulated_robot.launch.py
   ```

> [!TIP]
> For complete environment details, see the **[Dev Container Setup Guide](./docs/docker/02-devcontainer-setup.md)** or try the **[Standalone Simulation Demo](./docs/docker/03-simulation-demo.md)**.

### Option B: Native Host Environment (Required for Physical Robot)

To operate the real arm connected via USB serial:

```bash
cd arduinobot_ws
colcon build --symlink-install
source install/setup.bash
ros2 launch arduinobot_bringup real_robot.launch.py
```

---

## Voice Remote Control (Alexa Integration)

Control arduinobot using custom voice commands ("Alexa, tell my arduinobot to pick"):

<!-- MEDIA: Alexa Voice Control Sequence / Video Placeholder -->
<!-- <p align="center"> -->
<!--   <img src="resources/media/alexa_voice_demo.gif" alt="Alexa Voice Control Demo" /> -->
<!-- </p> -->

- **[1. Alexa Skills Introduction](./docs/alexa/1_introduction.md)**
- **[2. System Architecture & Request Flow](./docs/alexa/2_architecture.md)**
- **[3. Flask Backend & Ngrok Setup](./docs/alexa/3_flask_ngrok_setup.md)**
- **[4. Usage Tutorial & Troubleshooting](./docs/alexa/4_usage_tutorial.md)**

---

## Documentation Sitemap

All project documentation is organized under the [`docs/`](./docs/) directory:

- 🐳 **[Docker & Dev Container Documentation](./docs/docker/01-overview.md)**
- ⚡ **[Electronics & Hardware Setup](./docs/hardware/electronics.md)**
- 🎯 **[Servo Pulse Calibration](./docs/hardware/servo-calibration.md)**
- 🗣️ **[Alexa Voice Skill Documentation](./docs/alexa/1_introduction.md)**
- 🗺️ **[Engineering & RobotOps Roadmap](./docs/ROADMAP.md)**

---

## Acknowledgements & Credits

Sincere thanks to **[Antonio Brandi](https://github.com/AntoBrandi)** for creating the course *Robotics and ROS 2 - Learn by Doing: Manipulators*.

- **Course:** [Robotics & ROS 2 - Manipulators on Udemy](https://www.udemy.com/course/robotics-and-ros-2-learn-by-doing-manipulators/)
- **Tutorials:** [Learn by doing](https://learnbydoing.dev)
- **Original Baseline Repo:** [AntoBrandi/Arduino-Bot](https://github.com/AntoBrandi/Arduino-Bot)

---

## License

My custom code implementations and hardware modifications are provided under the [MIT License](LICENSE.txt).

---

## Contact

- **Website:** [andresalemn.github.io](https://andresalemn.github.io)
- **GitHub:** [@andresalemn](https://github.com/andresalemn)
- **LinkedIn:** [andresalemn](https://www.linkedin.com/in/andresalemn)
