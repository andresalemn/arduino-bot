#if SIMPLE_SERVO

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

// Create the PWM driver object (default I2C address 0x40)
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

// Which channel on the shield the servo is plugged into (0-15)
#define SERVO_CHANNEL 0

// Pulse length range for MG90S servos, in PCA9685 "ticks" (0-4095 @ 50Hz)
// These are typical starting values -- tune them for your specific servos
#define SERVOMIN 110   // ~500us pulse (0 degrees)
#define SERVOMAX 485   // ~2500us pulse (180 degrees)
#define SERVO_FREQ 50  // Analog servos run at ~50 Hz

// Helper: convert an angle (0-180) to a pulse length and send it
void writeServoAngle(uint8_t channel, int angle);

void setup() {
  // Start the Serial communication with ROS
  Serial.begin(115200);
  Serial.setTimeout(1);

  pwm.begin();
  pwm.setOscillatorFrequency(27000000);
  pwm.setPWMFreq(SERVO_FREQ);

  delay(10);

  // Set a known starting point for the motor
  writeServoAngle(SERVO_CHANNEL, 0);
}

void loop() {
  if (Serial.available())
  {
    int angle = Serial.readString().toInt();
    writeServoAngle(SERVO_CHANNEL, angle);
  }
  delay(0.1);
}

// Helper: convert an angle (0-180) to a pulse length and send it
void writeServoAngle(uint8_t channel, int angle) {
  angle = constrain(angle, 0, 180);
  // Serial.print("Commanded angle: "); Serial.println(angle);
  int pulse = map(angle, 0, 180, SERVOMIN, SERVOMAX);
  // Serial.print("Pulse: "); Serial.println(pulse);
  pwm.setPWM(channel, 0, pulse);
}

#endif