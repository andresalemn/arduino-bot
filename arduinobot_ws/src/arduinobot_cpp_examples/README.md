# arduinobot_cpp_examples

A collection of minimal C++ ROS 2 nodes that demonstrate the core
communication and lifecycle patterns of the `rclcpp` ecosystem, written as
hands-on learning examples for the ArduinoBot project.

## Purpose and responsibilities

This package is **purely educational**. Each source file isolates a single
ROS 2 concept so it can be studied and run independently. It does not contain
application-level business logic or hardware drivers.

## Package structure

```
arduinobot_cpp_examples/
├── doc/
│   └── tutorials/
│       └── lifecycle_node.md          # Step-by-step lifecycle node walkthrough
├── include/arduinobot_cpp_examples/   # Reserved for future public headers
├── src/
│   ├── simple_publisher.cpp        # Topic publisher example
│   ├── simple_subscriber.cpp       # Topic subscriber example
│   ├── simple_parameter.cpp        # Parameter declaration & change callback
│   ├── simple_service_server.cpp   # Service server example
│   ├── simple_service_client.cpp   # Asynchronous service client example
│   ├── simple_action_server.cpp    # Action server (Fibonacci, composable component)
│   ├── simple_action_client.cpp    # Action client (Fibonacci, composable component)
│   ├── simple_lifecycle_node.cpp   # Managed lifecycle node example
│   └── simple_moveit_interface.cpp # MoveIt 2 motion planning example
├── CMakeLists.txt
└── package.xml
```

## Nodes

### Regular nodes

| Executable | Node name | Description |
|---|---|---|
| `simple_publisher` | `simple_publisher` | Publishes a counter string on `/chatter` at 1 Hz. |
| `simple_subscriber` | `simple_subscriber` | Subscribes to `/chatter` and logs each message. |
| `simple_parameter` | `simple_parameter` | Declares two typed parameters and reacts to runtime changes via a callback. |
| `simple_service_server` | `simple_service_server` | Provides the `add_two_ints` service; returns the sum of two integers. |
| `simple_service_client` | `simple_service_client` | Calls `add_two_ints` asynchronously with two integers passed as CLI args. |
| `simple_moveit_interface` | `simple_moveit_interface` | Plans and executes joint-space motions for `arm` and `gripper` via MoveIt 2. |

### Lifecycle nodes

| Executable | Node name | Description |
|---|---|---|
| `simple_lifecycle_node` | `simple_lifecycle_node` | Demonstrates the ROS 2 managed-node lifecycle; subscribes to `/chatter` only while **active**. |

Expected lifecycle behaviour:

| Transition | Callback | Effect |
|---|---|---|
| `configure` | `on_configure` | Creates the `/chatter` subscription. |
| `activate` | `on_activate` | Calls the base implementation then sleeps 2 s to simulate hardware init. |
| `deactivate` | `on_deactivate` | Calls the base implementation; subscription persists but stops processing. |
| `cleanup` | `on_cleanup` | Resets the subscription `shared_ptr`; node returns to *unconfigured*. |
| `shutdown` | `on_shutdown` | Resets the subscription `shared_ptr` and terminates. |

See `doc/tutorials/lifecycle_node.md` for a full step-by-step walkthrough.

### Composable components (shared libraries)

These nodes are registered as ROS 2 components and can be loaded into a
running component container **or** launched as standalone executables.

| Library | Plugin | Standalone executable |
|---|---|---|
| `libsimple_action_server.so` | `arduinobot_cpp_examples::SimpleActionServer` | `simple_action_server_node` |
| `libsimple_action_client.so` | `arduinobot_cpp_examples::SimpleActionClient` | `simple_action_client_node` |

## Topics

| Topic | Type | Direction | Node(s) |
|---|---|---|---|
| `/chatter` | `std_msgs/msg/String` | **Published** | `simple_publisher` |
| `/chatter` | `std_msgs/msg/String` | **Subscribed** | `simple_subscriber`, `simple_lifecycle_node` |

## Services

| Service | Type | Role | Node |
|---|---|---|---|
| `/add_two_ints` | `arduinobot_msgs/srv/AddTwoInts` | **Server** | `simple_service_server` |
| `/add_two_ints` | `arduinobot_msgs/srv/AddTwoInts` | **Client** | `simple_service_client` |

## Actions

| Action | Type | Role | Node |
|---|---|---|---|
| `/fibonacci` | `arduinobot_msgs/action/Fibonacci` | **Server** | `SimpleActionServer` |
| `/fibonacci` | `arduinobot_msgs/action/Fibonacci` | **Client** | `SimpleActionClient` |

The Fibonacci action computes the sequence up to a requested `order`,
publishing intermediate `partial_sequence` feedback at 1 Hz and returning the
full sequence on completion. Cancellation is supported.

## Parameters

| Node | Parameter | Type | Default | Description |
|---|---|---|---|---|
| `simple_parameter` | `simple_int_param` | `int` | `28` | Example integer parameter. |
| `simple_parameter` | `simple_string_param` | `string` | `"Antonio"` | Example string parameter. |

Parameters can be changed at runtime:

```bash
ros2 param set /simple_parameter simple_int_param 42
ros2 param set /simple_parameter simple_string_param "ROS2"
```

No external configuration files are used; all defaults are declared in code.

## Launch files

This package does not provide launch files. Each node is intended to be run
directly with `ros2 run` (see below).

## Build

```bash
cd ~/ros2/arduino-bot/arduinobot_ws
colcon build --packages-select arduinobot_cpp_examples
source install/setup.bash
```

## Run

```bash
# Publisher / Subscriber pair
ros2 run arduinobot_cpp_examples simple_publisher
ros2 run arduinobot_cpp_examples simple_subscriber

# Parameter node
ros2 run arduinobot_cpp_examples simple_parameter

# Service pair
ros2 run arduinobot_cpp_examples simple_service_server
ros2 run arduinobot_cpp_examples simple_service_client 3 7   # argv[1]=A  argv[2]=B

# Action pair (standalone executables)
ros2 run arduinobot_cpp_examples simple_action_server_node
ros2 run arduinobot_cpp_examples simple_action_client_node

# Lifecycle node
ros2 run arduinobot_cpp_examples simple_lifecycle_node
# In a second terminal, drive its state machine:
ros2 lifecycle set /simple_lifecycle_node configure
ros2 lifecycle set /simple_lifecycle_node activate

# MoveIt 2 interface (requires a running move_group node)
ros2 run arduinobot_cpp_examples simple_moveit_interface
```

### Load action components into a container

```bash
ros2 run rclcpp_components component_container
ros2 component load /ComponentManager arduinobot_cpp_examples arduinobot_cpp_examples::SimpleActionServer
ros2 component load /ComponentManager arduinobot_cpp_examples arduinobot_cpp_examples::SimpleActionClient
```

## Dependencies

| Dependency | Purpose |
|---|---|
| `rclcpp` | Core ROS 2 C++ client library |
| `rclcpp_action` | Action server/client API |
| `rclcpp_components` | Composable node registration |
| `rclcpp_lifecycle` | Managed lifecycle node API |
| `std_msgs` | `String` message type for `/chatter` |
| `rcl_interfaces` | `SetParametersResult` message type |
| `arduinobot_msgs` | Custom `AddTwoInts` service and `Fibonacci` action definitions |
| `moveit_ros_planning_interface` | MoveIt 2 `MoveGroupInterface` for motion planning |

> **External requirement:** `simple_moveit_interface` requires a running
> MoveIt 2 `move_group` node with `arm` and `gripper` planning groups
> configured in the robot SRDF/URDF.
