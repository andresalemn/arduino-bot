# arduinobot_py_examples

A collection of minimal Python ROS 2 nodes that demonstrate the core
communication and lifecycle patterns of the `rclpy` ecosystem, written as
hands-on learning examples for the ArduinoBot project.

## Purpose and responsibilities

This package is **purely educational**. Each module isolates a single ROS 2
concept so it can be studied and run independently. It does not contain
application-level business logic or hardware drivers.

## Package structure

```
arduinobot_py_examples/
├── arduinobot_py_examples/
│   ├── __init__.py
│   ├── simple_publisher.py        # Topic publisher example
│   ├── simple_subscriber.py       # Topic subscriber example
│   ├── simple_parameter.py        # Parameter declaration & change callback
│   ├── simple_service_server.py   # Service server example
│   ├── simple_service_client.py   # Asynchronous service client example
│   ├── simple_action_server.py    # Action server (Fibonacci sequence)
│   ├── simple_action_client.py    # Action client (Fibonacci sequence)
│   └── simple_lifecycle_node.py   # Managed lifecycle node example
├── doc/
│   └── tutorials/
│       └── lifecycle_node.md      # Step-by-step lifecycle node walkthrough
├── resource/
├── test/
├── package.xml
├── setup.cfg
└── setup.py
```

## Nodes

### Regular nodes

| Executable | Node name | Description |
|---|---|---|
| `simple_publisher` | `simple_publisher` | Publishes a counter string on `/chatter` at 1 Hz. |
| `simple_subscriber` | `simple_subscriber` | Subscribes to `/chatter` and logs each received message. |
| `simple_parameter` | `simple_parameter` | Declares two typed parameters and reacts to runtime changes via a callback. |
| `simple_service_server` | `simple_service_server` | Provides the `add_two_ints` service; returns the sum of two integers. |
| `simple_service_client` | `simple_service_client` | Calls `add_two_ints` asynchronously with two integers passed as CLI args. |

### Lifecycle nodes

| Executable | Node name | Description |
|---|---|---|
| `simple_lifecycle_node` | `simple_lifecycle_node` | Demonstrates the ROS 2 managed-node lifecycle; subscribes to `/chatter` only while **active**. |

Expected lifecycle behaviour:

| Transition | Callback | Effect |
|---|---|---|
| `configure` | `on_configure` | Creates the `/chatter` subscription. |
| `activate` | `on_activate` | Delegates to parent after a 2 s simulated hardware init delay. |
| `deactivate` | `on_deactivate` | Delegates to parent; subscription persists but messages are discarded. |
| `cleanup` | `on_cleanup` | Destroys the subscription; node returns to *unconfigured*. |
| `shutdown` | `on_shutdown` | Destroys the subscription and terminates. |

See `doc/tutorials/lifecycle_node.md` for a full step-by-step walkthrough.

### Action nodes

| Executable | Node name | Description |
|---|---|---|
| `simple_action_server` | `simple_action_server` | Serves `Fibonacci` action goals; computes the sequence iteratively with 1 Hz feedback. |
| `simple_action_client` | `simple_action_client` | Sends a `Fibonacci` goal with `order = 10` and logs feedback and the final result. |

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
| `/fibonacci` | `arduinobot_msgs/action/Fibonacci` | **Server** | `simple_action_server` |
| `/fibonacci` | `arduinobot_msgs/action/Fibonacci` | **Client** | `simple_action_client` |

The Fibonacci action computes a sequence up to a requested `order`, publishing
`partial_sequence` feedback after each new element (with a 1-second delay
between elements) and returning the full sequence on completion.

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
colcon build --packages-select arduinobot_py_examples
source install/setup.bash
```

## Run

```bash
# Publisher / Subscriber pair
ros2 run arduinobot_py_examples simple_publisher
ros2 run arduinobot_py_examples simple_subscriber

# Parameter node
ros2 run arduinobot_py_examples simple_parameter

# Service pair
ros2 run arduinobot_py_examples simple_service_server
ros2 run arduinobot_py_examples simple_service_client 3 7   # argv[1]=A  argv[2]=B

# Action pair
ros2 run arduinobot_py_examples simple_action_server
ros2 run arduinobot_py_examples simple_action_client

# Lifecycle node
ros2 run arduinobot_py_examples simple_lifecycle_node
# In a second terminal, drive its state machine:
ros2 lifecycle set /simple_lifecycle_node configure
ros2 lifecycle set /simple_lifecycle_node activate
```

## Dependencies

| Dependency | Purpose |
|---|---|
| `rclpy` | Core ROS 2 Python client library |
| `std_msgs` | `String` message type for `/chatter` |
| `rcl_interfaces` | `SetParametersResult` message type |
| `arduinobot_msgs` | Custom `AddTwoInts` service and `Fibonacci` action definitions |
