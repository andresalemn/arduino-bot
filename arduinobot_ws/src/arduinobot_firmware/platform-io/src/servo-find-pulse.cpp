#if FIND_SERVO_PULSE

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

#define SERVO_CHANNEL 0
#define SERVO_FREQ 50

int calibrationChannel = -1; // will hold the validated channel number

int askForChannel();

void setup() {
  Serial.begin(115200);
  Serial.setTimeout(1);

  pwm.begin();
  pwm.setOscillatorFrequency(27000000);
  pwm.setPWMFreq(SERVO_FREQ);
  delay(10);

  calibrationChannel = askForChannel();

  Serial.println("\nSend a pulse tick value (e.g. 300) to move directly to it.");
  Serial.println("Send -1 at any time to select a new channel.");

  pwm.setPWM(calibrationChannel, 0, 300);
}

void loop() {
  if (Serial.available()) {
    int value = Serial.readString().toInt();

    if (value == -1) {
      calibrationChannel = askForChannel();
      Serial.println("\nSend a pulse tick value (e.g. 300) to move directly to it.");
      return; // skip treating -1 itself as a pulse
    }

    int pulse = constrain(value, 60, 600);
    Serial.print("Setting pulse: ");
    Serial.println(pulse);
    pwm.setPWM(calibrationChannel, 0, pulse);
  }
  delay(0.1);
}

int askForChannel() {
  Serial.println("\nEnter channel number to calibrate (0-15):");
  
  while (true) {
    if (Serial.available()) {
      String input = Serial.readString();
      input.trim(); // remove whitespace/newlines

      // Validate: non-empty, digits only
      bool isValid = input.length() > 0;
      for (unsigned int i = 0; i < input.length(); i++) {
        if (!isDigit(input.charAt(i))) {
          isValid = false;
          break;
        }
      }

      if (isValid) {
        int channel = input.toInt();
        if (channel >= 0 && channel <= 15) {
          Serial.print("Calibrating channel: ");
          Serial.println(channel);
          return channel;
        }
      }

      Serial.println("Invalid input. Enter an integer from 0 to 15:");
    }
    delay(10);
  }
}

#endif