# Lifecycle Node Tutorial (C++)

This tutorial walks through launching and manually driving the C++
`simple_lifecycle_node` through its state-machine transitions using the
`ros2 lifecycle` CLI. It also verifies that the node only processes messages
when it is in the **active** state.

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
ros2 run arduinobot_cpp_examples simple_lifecycle_node
```

The node starts in the **unconfigured** state. No topics are subscribed and no
output is produced until a lifecycle transition is requested.

---

## Step 2 — Inspect the initial state (Terminal 2)

Confirm the node is registered as a managed node and check its state:

```bash
ros2 lifecycle nodes
# /simple_lifecycle_node

ros2 lifecycle get /simple_lifecycle_node
# unconfigured [1]
```

List the transitions available from *unconfigured*:

```bash
ros2 lifecycle list /simple_lifecycle_node
# - configure [1]
#     Start: unconfigured
#     Goal:  configuring
# - shutdown [5]
#     Start: unconfigured
#     Goal:  shuttingdown
```

Verify that only ROS infrastructure topics are exposed (no `chatter` yet):

```bash
ros2 topic list
# /parameter_events
# /rosout
# /simple_lifecycle_node/transition_event
```

---

## Step 3 — Configure the node

Trigger the *configure* transition. `on_configure()` runs and creates the
`chatter` subscription:

```bash
ros2 lifecycle set /simple_lifecycle_node configure
# Transitioning successful
```

The node is now **inactive**. The subscription exists, but `msgCallback` will
drop any messages because the state is not *active*.

---

## Step 4 — Activate the node

```bash
ros2 lifecycle set /simple_lifecycle_node activate
# Transitioning successful
```

> **Note:** `on_activate()` delegates to the base-class implementation first
> and then sleeps for 2 seconds to simulate hardware initialisation. The CLI
> will block for approximately 2 seconds before reporting success.

The node is now **active** and will log messages received on `chatter`.

---

## Step 5 — Publish a test message (Terminal 3)

```bash
ros2 topic pub /chatter std_msgs/msg/String "data: 'Hi'"
```

Switch back to Terminal 1. You should see repeated lines such as:

```
[INFO] [simple_lifecycle_node]: Lifecycle node heard: Hi
```

---

## Step 6 — Deactivate and clean up

Stop message processing without destroying the subscription:

```bash
ros2 lifecycle set /simple_lifecycle_node deactivate
# Transitioning successful
```

Messages published to `chatter` are now silently dropped again.

Return the node to the *unconfigured* state, which resets the subscription
`shared_ptr` and frees all associated resources:

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
    │  configure  →  on_configure(): creates chatter subscription
    ▼
 inactive ◄──────── deactivate ─── on_deactivate(): base class ──────┐
    │                                                                  │
    │  activate  →  on_activate(): base class + 2 s sleep             │
    ▼                                                                  │
  active ─────────────────────────────────────────────────────────────┘
    │
    │  cleanup (from inactive)  →  on_cleanup(): resets subscription
    │  shutdown (from any)      →  on_shutdown(): resets subscription
    ▼
unconfigured / finalized
```

---

## Key Implementation Notes

- The `chatter` subscription is created as a `shared_ptr` and stored in
  `sub_`. Calling `sub_.reset()` in `on_cleanup()` / `on_shutdown()` is all
  that is needed to release the subscription and its resources.
- Unlike the Python equivalent, the C++ `on_activate()` calls
  `LifecycleNode::on_activate(state)` **before** the sleep, ensuring the
  base-class activation completes before the simulated delay.
- The `msgCallback` uses `get_current_state().label()` rather than relying on
  whether the subscription is active, which guards against callbacks queued
  during a state transition.
