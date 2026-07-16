# arduinobot_msgs

This package contains custom ROS 2 interface definitions (actions and services) for the Arduinobot robot arm project.

## Purpose and Responsibilities
The package acts as a central interface repository, declaring data structures and protocols for communication between different components of the Arduinobot robot. It contains no execution nodes, only `.srv` and `.action` definitions.

## Package Structure
```text
arduinobot_msgs/
├── CMakeLists.txt              # Rules to generate interface targets
├── package.xml                 # Package metadata and dependencies
├── action/                     # ROS 2 custom action definitions
│   ├── ArduinobotTask.action
│   └── Fibonacci.action
└── srv/                        # ROS 2 custom service definitions
    ├── AddTwoInts.srv
    ├── EulerToQuaternion.srv
    └── QuaternionToEuler.srv
```

## Interface Definitions

### Actions

#### [ArduinobotTask.action](action/ArduinobotTask.action)
Executes a predefined robot arm task and reports execution progress.
- **Goal**: `int32 task_number` (The ID of the task to execute)
- **Result**: `bool success` (Completion status)
- **Feedback**: `int32 percentage` (Current completion progress percentage [0 to 100])

#### [Fibonacci.action](action/Fibonacci.action)
Action interface to compute the Fibonacci sequence up to a given order.
- **Goal**: `int32 order` (Number of steps of the Fibonacci sequence to generate)
- **Result**: `int32[] sequence` (The full computed Fibonacci sequence)
- **Feedback**: `int32[] partial_sequence` (The intermediate sequence computed so far)
- **Examples**:
  - C++ Server/Client: [arduinobot_cpp_examples Actions](../arduinobot_cpp_examples/README.md#actions)
  - Python Server/Client: [arduinobot_py_examples Actions](../arduinobot_py_examples/README.md#action-nodes)

---

### Services

#### [AddTwoInts.srv](srv/AddTwoInts.srv)
Computes the sum of two 64-bit integers.
- **Request**: `int64 a`, `int64 b` (Integers to add)
- **Response**: `int64 sum` (Result of addition)
- **Examples**:
  - C++ Server/Client: [arduinobot_cpp_examples Services](../arduinobot_cpp_examples/README.md#services)
  - Python Server/Client: [arduinobot_py_examples Services](../arduinobot_py_examples/README.md#services)

#### [EulerToQuaternion.srv](srv/EulerToQuaternion.srv)
Converts Euler angles (roll, pitch, yaw) in radians to a quaternion.
- **Request**: `float64 roll`, `float64 pitch`, `float64 yaw` (Rotation around X, Y, Z axes respectively)
- **Response**: `float64 x`, `float64 y`, `float64 z`, `float64 w` (Corresponding quaternion fields)
- **Examples**:
  - C++ and Python Servers: [arduinobot_utils Nodes](../arduinobot_utils/README.md#nodes)

#### [QuaternionToEuler.srv](srv/QuaternionToEuler.srv)
Converts a quaternion to Euler angles (roll, pitch, yaw) in radians.
- **Request**: `float64 x`, `float64 y`, `float64 z`, `float64 w` (Input quaternion fields)
- **Response**: `float64 roll`, `float64 pitch`, `float64 yaw` (Rotation around X, Y, Z axes respectively)
- **Examples**:
  - C++ and Python Servers: [arduinobot_utils Nodes](../arduinobot_utils/README.md#nodes)

---

## Build Instructions
To build the messages package:
```bash
colcon build --packages-select arduinobot_msgs
```
