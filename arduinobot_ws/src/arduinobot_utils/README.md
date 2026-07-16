# arduinobot_utils

Helper utilities for the Arduinobot robot arm package. It provides services to convert between Euler angles and quaternions in both C++ and Python implementations.

## Purpose and Responsibilities
The main responsibility of this package is to handle mathematical coordinate and rotation transformations (specifically between Euler angles and Quaternions) required by the Arduinobot simulation, control, and telemetry nodes. It encapsulates these conversions in ROS 2 services so they can be consumed by other nodes in the system.

## Package Structure
```text
arduinobot_utils/
├── CMakeLists.txt                      # Build configuration for C++ and Python files
├── package.xml                         # Package metadata and dependencies
├── arduinobot_utils/                   # Python module containing source code
│   ├── __init__.py
│   └── angle_conversion.py            # Python service server for angle conversion
├── src/                                # C++ source directory
│   └── angle_conversion.cpp            # C++ service server for angle conversion
└── include/                            # C++ headers directory
```

## Nodes

### `angles_conversion_service_server` / `angles_conversion_service_server_cpp`
- **Type**: Regular ROS 2 Nodes.
- **Languages**: Python (implemented in [angle_conversion.py](file:///home/control/ros2/arduino-bot/arduinobot_ws/src/arduinobot_utils/arduinobot_utils/angle_conversion.py)) and C++ (implemented in [angle_conversion.cpp](file:///home/control/ros2/arduino-bot/arduinobot_ws/src/arduinobot_utils/src/angle_conversion.cpp)).
- **Description**: Exposes services to perform 3D rotation representation conversions.

### Services Provided

- **`euler_to_quaternion`** ([arduinobot_msgs/srv/EulerToQuaternion](file:///home/control/ros2/arduino-bot/arduinobot_ws/src/arduinobot_msgs/srv/EulerToQuaternion.srv))
  Converts Euler angles (roll, pitch, yaw) in radians to a quaternion (x, y, z, w).
  
- **`quaternion_to_euler`** ([arduinobot_msgs/srv/QuaternionToEuler](file:///home/control/ros2/arduino-bot/arduinobot_ws/src/arduinobot_msgs/srv/QuaternionToEuler.srv))
  Converts a quaternion (x, y, z, w) to Euler angles (roll, pitch, yaw) in radians.

## Dependencies
- `rclcpp` (ROS 2 C++ client library)
- `rclpy` (ROS 2 Python client library)
- `tf2` / `tf_transformations` (Libraries used for spatial math/rotations)
- `arduinobot_msgs` (Package containing the service definitions)

## Build and Run Instructions

### Build
To build the package, run the following command from the workspace root:
```bash
colcon build --packages-select arduinobot_utils
```

### Run
To run the C++ service server:
```bash
ros2 run arduinobot_utils angle_conversion
```

To run the Python service server:
```bash
ros2 run arduinobot_utils angle_conversion.py
```
