# Arduinobot - ROS 2 & Arduino Robotic Arm Manipulator

## Project Overview

This repository serves as a personal archive, reference, and extended workspace for the code and hardware implementations developed while following the [Robotics and ROS 2 - Learn by Doing: Manipulators](https://www.udemy.com/course/robotics-and-ros-2-learn-by-doing-manipulators/) course on Udemy. 

The primary purpose is to preserve my learning journey, custom hardware modifications, and package improvements in an easily accessible format. It is not intended as an official or alternative distribution of the course materials, but rather as an archive of my hands-on implementation and custom enhancements.

---

## About Arduinobot

The Arduinobot is a 4-DOF desktop robotic arm constructed from 3D-printed links and driven by **MG90S metal-gear servo motors** (upgraded from the original course's SG90 plastic-gear servos). It utilizes an Arduino Uno for physical joint command execution and is simulated digitally inside Gazebo and MoveIt 2.

### Custom Circuitry & PCA9685 Integration

Unlike the original course baseline—which drives the servos directly from the Arduino Uno digital output pins using `Servo.h`—my implementation migrates joint control to the **PCA9685 16-Channel 12-Bit PWM Driver Shield** connected over I2C.

This hardware modification offers several benefits:
*   **Reduced Wiring:** Requires only 4 pins from the Arduino (5V, GND, SDA, SCL) to control all joints.
*   **Dedicated Power Delivery:** Allows external 5V/6V power to be supplied directly to the servo terminals, protecting the Arduino from current spikes and brownouts.
*   **Pin Optimization:** Frees up the remaining Arduino digital and analog pins for sensors, telemetry, or future expansions.

Below is the Fritzing layout of the modified circuitry used for this project:

![Fritzing Circuit Layout](./resources/Electronics/robot-electronics_bb.jpg)

---

## Repository Structure

Here is a breakdown of the main directories and their contents:

*   **[`arduinobot_ws/`](./arduinobot_ws/)**:
    The main ROS 2 Colcon workspace containing package source code:
    *   **[`arduinobot_bringup`](./arduinobot_ws/src/arduinobot_bringup)**: Launches the entire robot stack in simulation or on real hardware with a single command.
    *   **[`arduinobot_controller`](./arduinobot_ws/src/arduinobot_controller)**: Defines the C++ `ros2_control` system interface (`ArduinobotInterface`) that handles serial packaging and transmission to the microcontroller.
    *   **[`arduinobot_description`](./arduinobot_ws/src/arduinobot_description)**: Houses the URDF/Xacro models, visual and collision STL meshes, and custom Gazebo worlds.
    *   **[`arduinobot_moveit`](./arduinobot_ws/src/arduinobot_moveit)**: MoveIt 2 configuration, kinematics solvers, and SRDF definition.
    *   **[`arduinobot_remote`](./arduinobot_ws/src/arduinobot_remote)**: Implements Flask-based web service nodes acting as an endpoint for Alexa voice control commands.
    *   **[`arduinobot_msgs`](./arduinobot_ws/src/arduinobot_msgs)**: Holds custom action (`ArduinobotTask`) and service definitions.
    *   **[`arduinobot_utils`](./arduinobot_ws/src/arduinobot_utils)**: Includes nodes to convert rotation coordinates (Euler angles $\leftrightarrow$ Quaternions).
    *   **[`arduinobot_firmware`](./arduinobot_ws/src/arduinobot_firmware)**: Hosts microcontroller entry points (`robot_control.cpp` for multi-joint control) and calibration/testing firmware.
    *   **[`arduinobot_cpp_examples`](./arduinobot_ws/src/arduinobot_cpp_examples)** / **[`arduinobot_py_examples`](./arduinobot_ws/src/arduinobot_py_examples)**: Educational publisher, subscriber, service, and action templates.

*   **[`resources/`](./resources/)**:
    Hardware design sheets, CAD files, and reference notes:
    *   `BOM/`: Complete parts list and components spreadsheets.
    *   `CAD/`: SolidWorks assembly (`Arm.SLDASM`) and component parts.
    *   `Electronics/`: Fritzing schematics, custom parts, and wiring reference diagrams.
    *   `Frames/`: Rendered coordinate TF transform tree maps.
    *   `Notes/`: Rotation mathematical theory notes.
    *   `Presentations/`: Course slide reference files.
    *   `STL_3D_Print/` / `STL_Gazebo/`: STL meshes optimized for 3D printing and Gazebo loading.

---

## Getting Started

To compile and execute the packages inside this repository, you need a working installation of **ROS 2 Humble** on Ubuntu 22.04.

1.  **Clone the repository:**
    ```bash
    git clone https://github.com/andresalemn/arduino-bot.git
    cd arduino-bot
    ```
2.  **Build the workspace:**
    ```bash
    cd arduinobot_ws
    colcon build --symlink-install
    ```
3.  **Source the setup files:**
    ```bash
    source install/setup.bash
    ```
4.  **Run the robot in simulation:**
    To spin up Gazebo, MoveIt 2 planners, and RViz2 simultaneously:
    ```bash
    ros2 launch arduinobot_bringup simulated_robot.launch.py
    ```
5.  **Run the real robot:**
    With your Arduino connected via serial and loaded with the `ROBOT_CONTROL` firmware:
    ```bash
    ros2 launch arduinobot_bringup real_robot.launch.py
    ```

---

## Instructor's Resources & Thanks

I extend my sincerest gratitude to **[@Antonio Brandi](https://github.com/AntoBrandi)** for creating this comprehensive course on robot manipulation and ROS 2. 

I highly recommend this course to anyone looking to master robot modeling, kinematics, control, and sim-to-real workflows. Antonio's step-by-step practical style makes complex topics highly approachable.

Anto runs **[Learn by doing](https://learnbydoing.dev)**, offering excellent tutorials and guides. The baseline code files for this course can be found in the original GitHub repositories:
*   [Arduino-Bot Repository](https://github.com/AntoBrandi/Arduino-Bot)
*   [Robotics & ROS 2 Manipulators Course Repository](https://github.com/AntoBrandi/Robotics-and-ROS-2-Learn-by-Doing-Manipulators)

---

## License

This content is for personal educational use. My custom code implementations and modifications are provided "as is" under the [MIT License](LICENSE.txt).

---

## Contact

Feel free to connect or reach out regarding questions, feedback, or collaborations:

*   **Personal Website:** [andresalemn.github.io](https://andresalemn.github.io)
*   **GitHub:** [andresalemn](https://github.com/andresalemn)
*   **LinkedIn:** [andresalemn](https://www.linkedin.com/in/andresalemn)
