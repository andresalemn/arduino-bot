# Usage tutorial: simple_servo_control.cpp 

## Running the Serial Transmitter Node and Sending Angle Commands

This tutorial walks through starting the [`simple_serial_transmitter`](../../src/simple_serial_transmitter.cpp) ROS2 node and sending angle commands to an Arduino running the [`simple_servo_control.cpp`](../../platform-io/src/simple_servo_control.cpp) PWM/servo firmware. It assumes the firmware has **already been uploaded to the board** — uploading via PlatformIO is covered in a separate tutorial.

---

## Pre-flight Checklist

Go through this list before trying to send any commands. Most "it doesn't work" issues trace back to one of these.

- [ ] **Firmware is already flashed to the Arduino via PlatformIO.** This tutorial does not upload code — the board must already be running the [`simple_servo_control.cpp`](../../platform-io/src/simple_servo_control.cpp) sketch.
- [ ] Arduino is connected to the computer via USB.
- [ ] No other program is holding the serial port open (close Arduino IDE Serial Monitor, `screen`, `minicom`, PlatformIO Serial Monitor, etc.).
- [ ] The port exists and matches what you'll pass to the node:
  ```bash
  ls -l /dev/ttyACM*
  ```
- [ ] Your user has permission to access the serial port:
  ```bash
  groups $USER
  ```
  If `dialout` is not listed, add yourself and re-login (see Troubleshooting).
- [ ] The `arduinobot_firmware` ROS2 package is built:
  ```bash
  cd ~/ros2/arduino-bot/arduinobot_ws
  colcon build --packages-select arduinobot_firmware
  ```
- [ ] The workspace is sourced in every terminal you use:
  ```bash
  source ~/ros2/arduino-bot/arduinobot_ws/install/setup.bash
  ```
- [ ] Baud rate matches on both ends (115200 in this setup) — this is already hardcoded in both the node and the sketch, but worth remembering if you ever change one side.
- [ ] The PCA9685 shield is powered from an external 5–6V supply (not just USB) if you're driving MG90S servos, with grounds tied together.

---

## Step 1 — Open a Terminal and Source the Workspace

```bash
cd ~/ros2/arduino-bot/arduinobot_ws
source install/setup.bash
```

Do this in **every new terminal** you open for this workflow — ROS2 won't find your package otherwise.

---

## Step 2 — Launch the Serial Transmitter Node

```bash
ros2 run arduinobot_firmware simple_serial_transmitter --ros-args -p port:=/dev/ttyACM0
```

If your board enumerates on a different port, substitute it here (e.g. `/dev/ttyACM1`).

**What a healthy launch looks like:** the terminal stays open with no errors printed, and the command doesn't return you to the prompt (the node is spinning and waiting for messages).

---

## Step 3 — Verify the Node Is Actually Running

Open a **second terminal**, source the workspace again, then check:

```bash
source ~/ros2/arduino-bot/arduinobot_ws/install/setup.bash
ros2 node list
ros2 topic list
```

You should see:
- `/simple_serial_transmitter` in the node list
- `/serial_transmitter` in the topic list

If either is missing, go back to Step 2 — the node likely failed silently (this node doesn't currently check whether `Open()` on the serial port succeeded, so a bad port or permissions issue won't throw an obvious error).

---

## Step 4 — Send an Angle Command

Still in the second terminal:

```bash
ros2 topic pub /serial_transmitter std_msgs/msg/String "data: '90'" --once
```

Use `--once` so the publisher sends a single message and exits, rather than publishing continuously.

You should see:
- The publisher terminal confirms the message was published.
- The node's terminal (Step 2) logs something like `New message received, publishing on serial port: 90`.
- The connected servo moves to the commanded angle.

Try a few values to confirm the full range works:

```bash
ros2 topic pub /serial_transmitter std_msgs/msg/String "data: '0'" --once
ros2 topic pub /serial_transmitter std_msgs/msg/String "data: '90'" --once
ros2 topic pub /serial_transmitter std_msgs/msg/String "data: '180'" --once
```

---

## Troubleshooting

### Node launches but nothing happens when publishing

| Check | How |
|---|---|
| Wrong port | `ls -l /dev/ttyACM*` and compare to what you passed with `-p port:=` |
| Port permission denied | `groups $USER` — add to `dialout` if missing (see below) |
| Port already in use by another program | Close Serial Monitor / `screen` / `minicom` / other ROS node |
| Firmware not actually uploaded, or wrong sketch on board | Re-upload the correct sketch via PlatformIO |
| Baud mismatch | Confirm `Serial.begin(115200)` in the sketch matches `BAUD_115200` in the node |

### Fixing dialout permissions

```bash
sudo usermod -aG dialout $USER
```
Then either log out and back in, or run `newgrp dialout` in your current terminal for it to apply immediately.

### `ros2 topic pub` hangs or errors immediately

- Make sure you sourced the workspace in that terminal (`source install/setup.bash`).
- Double check the topic name and message type exactly match: `/serial_transmitter` and `std_msgs/msg/String`.
- Confirm `ros2 topic list` actually shows the topic before publishing to it.

### Servo doesn't move, but no errors anywhere in ROS

- Add a `Serial.println(angle);` debug line temporarily in the Arduino sketch and watch it with a Serial Monitor **after** stopping the ROS node (only one program can hold the port at a time).
- Confirm the servo is on the channel number your sketch expects (`SERVO_CHANNEL`).
- Confirm the PCA9685 shield has external power connected — USB power alone is often insufficient for MG90S servos and can cause silent brownouts.
- Double check I2C wiring (SDA/SCL) if using a bare PCA9685 board rather than a shield.

### Angle moves but seems clipped or jittery at the extremes

- Revisit `SERVOMIN` / `SERVOMAX` calibration values in the sketch — these vary slightly per servo unit.

### Multi-digit angles behave oddly (e.g. "1", "8", "0" arriving separately instead of "180")

- This points to a timing issue with `Serial.readString()`'s 1ms timeout. It's usually fine for a single `ros2 topic pub --once`, but if you start chaining rapid publishes, consider whether the read timing on the Arduino side needs adjusting.

---

## Quick Reference

```bash
# Terminal 1 — launch the bridge node
source ~/ros2/arduino-bot/arduinobot_ws/install/setup.bash
ros2 run arduinobot_firmware simple_serial_transmitter --ros-args -p port:=/dev/ttyACM0

# Terminal 2 — verify and send commands
source ~/ros2/arduino-bot/arduinobot_ws/install/setup.bash
ros2 node list
ros2 topic list
ros2 topic pub /serial_transmitter std_msgs/msg/String "data: '90'" --once
```