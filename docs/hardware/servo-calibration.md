# Servo Pulse Calibration Tutorial

This tutorial explains how to use the pulse calibration sketch to find the correct `SERVOMIN` and `SERVOMAX` pulse values for each of your MG90S servos connected to the PCA9685 (Adafruit PWM Servo Shield). It assumes the firmware is already uploaded to the board and you know how to open a Serial Monitor at 115200 baud.

---

## Why calibrate each servo individually

Cheap servos like the MG90S vary slightly unit to unit, even within the same model and batch. A pulse value that lands exactly on 0° for one servo might land on 3° or -2° for another. If you use the same `SERVOMIN`/`SERVOMAX` values across all servos without checking, your commanded angles won't match the real physical position of each one.

**Important distinction:** you are **not** looking for the servo's absolute mechanical stop (the hard limit where it stalls or buzzes). You are looking for the pulse that visually corresponds to **0°** and the pulse that visually corresponds to **180°** — the two reference points your robot actually cares about. Once those two pulses are correct, the standard `writeServoAngle()` mapping function works correctly across the full 0–180° range for that servo, because the relationship between pulse and angle for these servos is linear between those two points.

So the goal is simple: **watch the horn, and note down the pulse where it visually sits at 0° and at 180°.** Nothing more.

---

## What the calibration sketch does

The sketch lets you:
- Pick which PCA9685 channel (0–15) you want to work with, without re-uploading code.
- Send raw pulse tick values directly and watch the servo move to that exact pulse.
- Switch to a different channel at any time by sending `-1`, so you can calibrate multiple servos in one session without touching the reset button.

### Code walkthrough

**Channel selection (`askForChannel()`):**
On boot, the sketch asks you which channel to calibrate. It only accepts whole numbers from 0 to 15 — anything else (letters, negative numbers, out-of-range numbers, empty input) gets rejected and it asks again. This prevents accidentally sending pulses to the wrong channel because of a typo.

**Sending a pulse:**
Once a channel is selected, typing any number into the Serial Monitor sends that value directly as a PWM pulse tick to the selected channel:
```
300
```
moves the servo on the selected channel to pulse 300. There's no angle math involved here — you are working in raw pulse ticks, which gives you finer control than whole-degree steps would.

**Switching channels without resetting:**
Sending the sentinel value:
```
-1
```
at any time re-triggers the channel selection prompt, so you can move on to the next servo without unplugging or resetting the Arduino.

---

## Step-by-step calibration process

1. Connect **one servo at a time** to the channel you intend to calibrate (or connect all of them if channels are already wired — just calibrate one at a time in software).
2. Open the Serial Monitor at 115200 baud.
3. When prompted, enter the channel number (0–15) for the servo you're calibrating.
4. Send an initial mid-range pulse (e.g. `300`) to get a baseline position.
5. Send larger pulse values and observe the horn until it visually sits at **0°** (use a printed protractor, angle guide, or just eyeballing against a fixed reference edge). Note the pulse value — this is your `SERVOMIN` for this servo.
6. Move the pulse toward the other end until the horn visually sits at **180°**. Note that pulse value — this is your `SERVOMAX` for this servo.
7. Write both values down against the servo's label/ID (see table below).
8. Send `-1` to select the next channel and repeat for the remaining servos.

---

## Applying the calibrated values

Once you have `SERVOMIN` and `SERVOMAX` for a given servo, plug them into your main firmware's helper function as usual:

```cpp
#define SERVOMIN 110  // pulse where this servo visually sits at 0°
#define SERVOMAX 485  // pulse where this servo visually sits at 180°

void writeServoAngle(uint8_t channel, int angle) {
  angle = constrain(angle, 0, 180);
  int pulse = map(angle, 0, 180, SERVOMIN, SERVOMAX);
  pwm.setPWM(channel, 0, pulse);
}
```

If you're driving multiple servos with different calibration values, use a lookup table or struct array keyed by channel/joint name instead of a single global `#define` pair, so each servo uses its own correct values.

---

## Calibration Table — J1 to J4

Fill in the pulse values you find for each servo below. Label the physical servo body (tape, marker, etc.) to match the joint name so you never mix up calibration data between units.

| Joint | Channel | SERVOMIN (pulse @ 0°) | SERVOMID (pulse @ 90°) | SERVOMAX (pulse @ 180°) | Notes                 |
|-------|---------|-----------------------|------------------------|-------------------------|-----------------------|
| J1    |15       |110                    |            -           |485                      |Joint at the fixed base|
| J2    |11       |120                    |            -           |490                      |Shoulder joint         |
| J3    |7        |110                    |            -           |475                      |Elbow joint            |
| J4    |3        |97                     |280                     |472                      |Claw or gripper        |

Feel free to reuse this table format for any additional servos you add later — just extend it with more rows (J5, J6, etc.) as your robot grows.

---

## Quick tips

- Always calibrate with the servo unloaded from any mechanical linkage first, if possible — this makes it easier to see the true 0°/180° positions without load-induced position error.
- If a servo is ever swapped or replaced, recalibrate it — do not reuse another unit's pulse values even if it's the same model.
- Keep this table alongside your firmware source, or embed it directly as comments near your calibration `#define`s, so future-you doesn't have to redo this work.