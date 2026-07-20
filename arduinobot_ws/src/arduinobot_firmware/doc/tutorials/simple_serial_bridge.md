# ROS 2 Serial Bridge Tutorial (Receivers and Transmitters)

This tutorial explains how to compile, launch, and test the ROS 2 serial receiver and transmitter nodes (available in both C++ and Python) to communicate with an Arduino.

---

## Prerequisites

- The `arduinobot_firmware` ROS 2 package is built:
  ```bash
  cd ~/ros2/arduino-bot/arduinobot_ws
  colcon build --packages-select arduinobot_firmware
  ```
- Your user belongs to the `dialout` group:
  ```bash
  groups $USER
  ```
  If missing, run `sudo usermod -aG dialout $USER` and log back in.
- The workspace is sourced in all active terminals:
  ```bash
  source ~/ros2/arduino-bot/arduinobot_ws/install/setup.bash
  ```
- The target Arduino is plugged in. Determine its port (typically `/dev/ttyACM0` or `/dev/ttyUSB0`) via:
  ```bash
  ls -l /dev/ttyACM* /dev/ttyUSB*
  ```

---

## 1. Simple Serial Transmitter Nodes

The transmitters subscribe to the `/serial_transmitter` topic and write any incoming string data onto the serial interface.

### Running the C++ Implementation

1. Start the C++ transmitter node:
   ```bash
   ros2 run arduinobot_firmware simple_serial_transmitter --ros-args -p port:=/dev/ttyACM0
   ```
2. In a second terminal, send a message to turn on or trigger a action (e.g. sending '0' or '1'):
   ```bash
   ros2 topic pub /serial_transmitter std_msgs/msg/String "data: '0'" --once
   ros2 topic pub /serial_transmitter std_msgs/msg/String "data: '1'" --once
   ```

### Running the Python Implementation

1. Start the Python transmitter script:
   ```bash
   ros2 run arduinobot_firmware simple_serial_transmitter.py --ros-args -p port:=/dev/ttyACM0
   ```
2. In a second terminal, publish data:
   ```bash
   ros2 topic pub /serial_transmitter std_msgs/msg/String "data: '0'" --once
   ```

---

## 2. Simple Serial Receiver Nodes

The receivers poll the serial interface at 100 Hz, decode any incoming text line as UTF-8, and broadcast it on the `/serial_receiver` topic.

### Running the C++ Implementation

1. Start the C++ receiver node:
   ```bash
   ros2 run arduinobot_firmware simple_serial_receiver --ros-args -p port:=/dev/ttyACM0
   ```
2. In a second terminal, echo the published topic to see incoming telemetry or serial debug prints from the Arduino:
   ```bash
   ros2 topic echo /serial_receiver
   ```

### Running the Python Implementation

1. Start the Python receiver script:
   ```bash
   ros2 run arduinobot_firmware simple_serial_receiver.py --ros-args -p port:=/dev/ttyACM0
   ```
2. In a second terminal, echo the output topic:
   ```bash
   ros2 topic echo /serial_receiver
   ```
