# arduinobot_firmware

Firmware and serial communication utilities for the Arduinobot robot arm. Provides Arduino firmware configurations for testing and calibrating servo motors using a PCA9685 PWM shield, and a ROS 2 serial bridge node.

---

## Package responsibilities

- Provide C++ and Python ROS 2 nodes to transmit serial messages to the Arduino.
- Manage PlatformIO configuration and firmware source code for testing servos on the PCA9685 driver shield.
- Offer interactive calibration tools for finding joint pulse width limits.
- Guide users on running simple serial node publishers and configuring servo pulse ranges.

---

## Package structure

```
arduinobot_firmware/
├── arduinobot_firmware/        # Python module
│   ├── __init__.py
│   ├── simple_serial_receiver.py # Python serial receiver node
│   └── simple_serial_transmitter.py # Python serial transmitter node
├── doc/tutorials/
│   ├── servo_find_pulse.md     # Step-by-step pulse calibration guide
│   ├── simple_serial_bridge.md # Receiver/Transmitter usage and test guide
│   └── simple_servo.md         # Serial command and single-servo control guide
├── include/
│   └── arduinobot_firmware/    # Public headers
├── platform-io/
│   ├── src/
│   │   ├── eight-servo-test.cpp   # Sweeps 8 servos sequentially (SERVO_TEST)
│   │   ├── servo-find-pulse.cpp   # Interactive calibration tool (FIND_SERVO_PULSE)
│   │   └── simple_servo_control.cpp # Single-servo controller mapping angles (SIMPLE_SERVO)
│   └── platformio.ini          # PlatformIO board environments
├── src/
│   ├── simple_serial_receiver.cpp    # C++ serial receiver node
│   └── simple_serial_transmitter.cpp # C++ serial transmitter node
├── CMakeLists.txt
├── package.xml
├── setup.cfg
└── setup.py
```

---

## Nodes

### `simple_serial_transmitter` (C++) / `simple_serial_transmitter.py` (Python)

Both C++ and Python implementations function similarly. They subscribe to a topic and forward any incoming string payloads directly over the configured serial port to the Arduino.

**Subscriptions**

| Topic | Type | Description |
|---|---|---|
| `/serial_transmitter` | `std_msgs/msg/String` | Text payload forwarded to the Arduino. |

**Parameters**

| Parameter | Type | Default | Description |
|---|---|---|---|
| `port` | string | `/dev/ttyACM0` | Path to the target Arduino serial port interface. |
| `baud_rate` | int | `115200` | Serial transmission frequency. |

### `simple_serial_receiver` (C++) / `simple_serial_receiver.py` (Python)

Both implementations monitor incoming serial data from the Arduino. They poll the device at 100 Hz, read incoming text lines, and publish them as ROS 2 message topics.

**Publications**

| Topic | Type | Description |
|---|---|---|
| `/serial_receiver` | `std_msgs/msg/String` | Decoded string received from the Arduino. |

**Parameters**

| Parameter | Type | Default | Description |
|---|---|---|---|
| `port` | string | `/dev/ttyUSB0` | Path to the target serial device. |
| `baudrate` / `baud_rate` | int | `115200` | Serial transmission speed. |

---

## Firmware Environments

PlatformIO environments are configured in [`platform-io/platformio.ini`](platform-io/platformio.ini):

- **`env:serial-test`**: Compiles standard serial interface testing flags.
- **`env:servo-shield`**: Compiles the PCA9685 servo driver routines. Toggle active routines using the following defines:
  - `-D SERVO_TEST`: Runs the eight-servo sweep routine.
  - `-D SIMPLE_SERVO`: Runs single-servo angle control.
  - `-D FIND_SERVO_PULSE`: Runs the interactive pulse calibration utility.

---

## Build

```bash
cd ~/ros2/arduino-bot/arduinobot_ws
colcon build --packages-select arduinobot_firmware
source install/setup.bash
```

---

## Run

**Launch the ROS 2 Serial Transmitter**

```bash
ros2 run arduinobot_firmware simple_serial_transmitter --ros-args -p port:=/dev/ttyACM0
```

---

## Tutorials

Detailed procedures are documented in:
* [Servo Pulse Calibration Tutorial](doc/tutorials/servo_find_pulse.md)
* [Usage tutorial: simple_servo_control.cpp](doc/tutorials/simple_servo.md)
* [ROS 2 Serial Bridge Tutorial (Receivers and Transmitters)](doc/tutorials/simple_serial_bridge.md)

---

## Dependencies

| Dependency | Type | Purpose |
|---|---|---|
| `rclcpp` | build + exec | C++ ROS 2 client library. |
| `rclpy` | build + exec | Python ROS 2 client library. |
| `std_msgs` | build + exec | Standard message types (e.g. String). |
| `libserial-dev` | build + exec | C++ serial port library. |
