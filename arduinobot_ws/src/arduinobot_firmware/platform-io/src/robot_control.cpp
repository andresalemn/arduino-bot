#if ROBOT_CONTROL

/**
 * @file robot_control.cpp
 * @brief Arduinobot firmware migrated from Servo.h to Adafruit_PWMServoDriver
 *        (PCA9685 16-channel PWM shield). Drives 4 joints: base, shoulder,
 *        elbow, gripper. Receives the same serial protocol used by the
 *        ROS2 hardware interface: "bXXX,sXXX,eXXX,gXXX," (3-digit zero
 *        padded values per joint).
 *
 * PlatformIO .cpp entry point - requires <Arduino.h> since this isn't a .ino file.
 */

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

// ---------------------------------------------------------------------------
// PCA9685 channel assignment - change these if you plug servos into different
// channels on the shield. Nothing else in the code needs to change.
// ---------------------------------------------------------------------------
#define BASE_CHANNEL     15
#define SHOULDER_CHANNEL 11
#define ELBOW_CHANNEL    7
#define GRIPPER_CHANNEL  3

// Joint indices - used by the serial parser and to index into servos[].
// Keep these in the same order as the servos[] array below.
#define BASE_IDX     0
#define SHOULDER_IDX 1
#define ELBOW_IDX    2
#define GRIPPER_IDX  3
#define NUM_JOINTS   4

// ---------------------------------------------------------------------------
// Motion tuning
// ---------------------------------------------------------------------------
#define SERVO_FREQ    50  // Analog servos run at ~50Hz
#define STEP_DEGREES  1   // Degrees per step during smooth motion
#define STEP_DELAY_MS 5   // Delay between steps (ms)
// NOTE: like the original Servo.h version, this blocks loop() while a joint
// is in motion (uses delay()). Serial bytes arriving mid-motion just wait in
// the hardware RX buffer until the current move finishes. This preserves the
// original design's behavior; a non-blocking millis()-based stepper would be
// a reasonable future upgrade if simultaneous multi-joint motion is needed.

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

// ---------------------------------------------------------------------------
// Per-servo calibration
//
// pulse_min / pulse_max: PCA9685 pulse ticks (0-4095 @ 50Hz) corresponding to
//   angle_min / angle_max respectively. Find these using the pulse calibration
//   sketch - send raw pulses and visually note where the horn sits at the two
//   reference angles for THIS servo.
//
// angle_min / angle_max: the joint's usable angle range. Base/shoulder/elbow
//   use 0-180. The gripper's mirrored-claw mechanism only needs 0-90 to reach
//   its full open/close travel.
//
// start_angle: commanded on boot so every joint has a known starting position.
//
// Placeholders below - fill in with your actual measured pulse values per
// joint (see your J1-J4 calibration table).
// ---------------------------------------------------------------------------
struct ServoCalibration {
  uint8_t channel;
  int pulse_min;
  int pulse_max;
  int angle_min;
  int angle_max;
  int start_angle;
};

ServoCalibration servos[NUM_JOINTS] = {
  // channel,        pulse_min, pulse_max, angle_min, angle_max, start_angle
  { BASE_CHANNEL,     110,       485,       0,         180,       90 },  // BASE_IDX
  { SHOULDER_CHANNEL, 120,       485,       0,         180,       90 },  // SHOULDER_IDX
  { ELBOW_CHANNEL,    110,       485,       0,         180,       90 },  // ELBOW_IDX
  { GRIPPER_CHANNEL,   97,       280,       0,         90,        0  },  // GRIPPER_IDX
};

// Tracks each joint's last-commanded angle. The PCA9685 has no readback
// (unlike Servo::read(), which queries the pulse timer), so we maintain this
// manually as the equivalent of "current position" for smooth stepping.
int current_angle[NUM_JOINTS];

// ---------------------------------------------------------------------------
// Serial parser state
//
// idx: which joint the incoming digits belong to (matches *_IDX above)
// val_idx: current write position in the digit buffer
// value: holds up to 3 digits + null terminator
// ---------------------------------------------------------------------------
uint8_t idx = 0;
uint8_t val_idx = 0;
char value[4] = "000";

/**
 * @brief Clamp an angle to a joint's valid range, map it to the joint's
 *        calibrated pulse range, and send it to the PCA9685.
 *
 * Replaces the old writeServoAngle(Servo&, int) - now takes a joint index so
 * it can look up per-servo calibration (channel, pulse range, angle range)
 * internally instead of the caller managing a Servo object directly.
 *
 * @param dof_index Index into servos[] / current_angle[] (0-3)
 * @param angle Target angle in degrees, any int - will be clamped safely.
 */
void writeServoAngle(uint8_t dof_index, int angle)
{
  ServoCalibration &cal = servos[dof_index];

  // Clamp defensively - protects against malformed/out-of-range serial input
  // (e.g. a stray negative value or parsing glitch) reaching the PCA9685.
  angle = constrain(angle, cal.angle_min, cal.angle_max);

  int pulse = map(angle, cal.angle_min, cal.angle_max, cal.pulse_min, cal.pulse_max);
  pwm.setPWM(cal.channel, 0, pulse);
}

/**
 * @brief Moves a joint smoothly from its last known angle to a new goal,
 *        one degree (STEP_DEGREES) at a time. Equivalent to the original
 *        reach_goal(Servo&, int), but reads/writes current_angle[] instead
 *        of relying on Servo::read().
 *
 * @param dof_index Index into servos[] / current_angle[] (0-3)
 * @param goal Target angle in degrees (will be clamped inside writeServoAngle)
 */
void moveToAngle(uint8_t dof_index, int goal)
{
  ServoCalibration &cal = servos[dof_index];
  goal = constrain(goal, cal.angle_min, cal.angle_max);

  int pos = current_angle[dof_index];

  if (goal >= pos)
  {
    for (; pos <= goal; pos += STEP_DEGREES)
    {
      writeServoAngle(dof_index, pos);
      delay(STEP_DELAY_MS);
    }
  }
  else
  {
    for (; pos >= goal; pos -= STEP_DEGREES)
    {
      writeServoAngle(dof_index, pos);
      delay(STEP_DELAY_MS);
    }
  }

  // Make sure we land exactly on goal even if STEP_DEGREES doesn't divide
  // the distance evenly.
  writeServoAngle(dof_index, goal);
  current_angle[dof_index] = goal;
}

/**
 * @brief Resets the digit buffer to a known "000" state.
 *
 * PATCH NOTE (parsing bug fix): the original sketch only overwrote buffer
 * positions that received new digits, so a short value sent without full
 * 3-digit zero-padding (e.g. "90," instead of "090,") would leave a stale
 * digit from the previous command in value[2], corrupting atoi(). The
 * companion ROS2 interface (compensateZeros()) always sends exactly 3
 * digits, so this never triggered in normal operation - but resetting
 * explicitly here makes the Arduino correct on its own, independent of the
 * sender always behaving.
 */
void resetValueBuffer()
{
  value[0] = '\0';
  val_idx = 0;
}

void setup()
{
  Serial.begin(115200);
  Serial.setTimeout(1);

  pwm.begin();
  pwm.setOscillatorFrequency(27000000);
  pwm.setPWMFreq(SERVO_FREQ);
  delay(10);

  // Initialize each joint to its configured start angle, and seed
  // current_angle[] so moveToAngle() has a valid reference point from the
  // very first command.
  for (uint8_t i = 0; i < NUM_JOINTS; i++)
  {
    current_angle[i] = servos[i].start_angle;
    writeServoAngle(i, servos[i].start_angle);
  }
}

void loop()
{
  if (Serial.available())
  {
    char chr = Serial.read();

    if (chr == 'b')
    {
      idx = BASE_IDX;
      val_idx = 0;
    }
    else if (chr == 's')
    {
      idx = SHOULDER_IDX;
      val_idx = 0;
    }
    else if (chr == 'e')
    {
      idx = ELBOW_IDX;
      val_idx = 0;
    }
    else if (chr == 'g')
    {
      idx = GRIPPER_IDX;
      val_idx = 0;
    }
    else if (chr == ',')
    {
      value[val_idx] = '\0';
      int val = atoi(value);
      moveToAngle(idx, val);
      resetValueBuffer();
    }
    else if (isDigit(chr))
    {
      // PATCH NOTE (validation + overflow guard): the original accepted any
      // non-prefix, non-comma byte here, including '\r'/'\n' or other stray
      // characters some serial senders append, and never checked val_idx
      // against the buffer size - a run of >3 characters before a comma
      // would write past value[3] (undefined behavior). Restricting to
      // isDigit() and bounds-checking val_idx fixes both without changing
      // the wire protocol at all.
      if (val_idx < 3)
      {
        value[val_idx] = chr;
        val_idx++;
      }
      // else: silently drop extra digits rather than overflow the buffer.
      // A 3-digit protocol should never legitimately need a 4th digit.
    }
    // else: any other stray byte (e.g. '\r', '\n') is now ignored instead
    // of being written into the numeric buffer.
  }
}

#endif