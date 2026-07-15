# Lifecycle Node Tutorial

This tutorial walks through launching and manually driving the
`simple_lifecycle_node` through its state-machine transitions using the
`ros2 lifecycle` CLI. It also shows how to verify that the node only processes
messages when it is in the **active** state.

## Prerequisites

- The workspace has been built and sourced.
- Three separate terminal windows are available.

```bash
# Run in every terminal before the steps below
. ~/ros2/arduino-bot/arduinobot_ws/install/setup.bash
```

---

## Step 1 — Launch the node (Terminal 1)

```bash
ros2 run arduinobot_py_examples simple_lifecycle_node
```

The node starts in the **unconfigured** state and produces no output until a
transition is requested.

---

## Step 2 — Inspect the initial state (Terminal 2)

Confirm the node is visible and check its current state:

```bash
ros2 lifecycle nodes
# /simple_lifecycle_node

ros2 lifecycle get /simple_lifecycle_node
# unconfigured [1]
```

List the transitions available from the *unconfigured* state:

```bash
ros2 lifecycle list /simple_lifecycle_node
# - configure [1]
#     Start: unconfigured
#     Goal:  configuring
# - shutdown [5]
#     Start: unconfigured
#     Goal:  shuttingdown
```

Verify that no application topics are exposed yet (only ROS infrastructure
topics should be present):

```bash
ros2 topic list
# /parameter_events
# /rosout
# /simple_lifecycle_node/transition_event
```

---

## Step 3 — Configure the node

Trigger the *configure* transition. This causes `on_configure()` to run, which
creates the `chatter` subscription:

```bash
ros2 lifecycle set /simple_lifecycle_node configure
# Transitioning successful
```

The node is now in the **inactive** state. The `chatter` subscription exists,
but `msgCallback` will discard any messages because the state is not *active*.

---

## Step 4 — Activate the node

```bash
ros2 lifecycle set /simple_lifecycle_node activate
# Transitioning successful
```

> **Note:** `on_activate()` introduces a deliberate 2-second delay to simulate
> hardware initialisation. The CLI will block for approximately 2 seconds before
> reporting success.

The node is now **active** and will log messages received on `chatter`.

---

## Step 5 — Publish a test message (Terminal 3)

```bash
ros2 topic pub /chatter std_msgs/msg/String "data: 'Hi'"
```

Switch back to Terminal 1. You should see repeated lines such as:

```
[INFO] [simple_lifecycle_node]: I heard: Hi
```

---

## Step 6 — Deactivate and clean up

Stop message processing without destroying the subscription:

```bash
ros2 lifecycle set /simple_lifecycle_node deactivate
# Transitioning successful
```

Messages published to `chatter` are now silently dropped again.

Return the node to the *unconfigured* state, which destroys the subscription:

```bash
ros2 lifecycle set /simple_lifecycle_node cleanup
# Transitioning successful
```

Finally, shut down the node:

```bash
ros2 lifecycle set /simple_lifecycle_node shutdown
# Transitioning successful
```

---

## State Machine Summary

```
unconfigured
    │  configure
    ▼
 inactive ◄──────── deactivate ────────┐
    │                                  │
    │  activate                        │
    ▼                                  │
  active ──────────────────────────────┘
    │
    │  (cleanup from inactive, shutdown from any state)
    ▼
unconfigured / finalized
```
