#if TEST_UNO

#include <Arduino.h>

void setup() {
  // put your setup code here, to run once:
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  Serial.begin(115200);
  Serial.setTimeout(1);

}

void loop() {
  // put your main code here, to run repeatedly:

  if (Serial.available())
    {
      int x = Serial.readString().toInt();
      if(x==0)
      {
        digitalWrite(LED_BUILTIN, LOW);
      }
      else if (x == 1)
      {
        digitalWrite(LED_BUILTIN, HIGH);
      }
    }
    delay(0.1);
}

#endif