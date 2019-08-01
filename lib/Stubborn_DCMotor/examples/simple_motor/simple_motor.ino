
#include <Stubborn_DCMotor.h>

Stubborn_DCMotor motor(1);

void setup() {
  motor.setSpeed(100);
}

void loop() {
  motor.run(FORWARD);
  delay(4000);
  motor.run(RELEASE);
  delay(1000);
  motor.run(BACKWARD);
  delay(4000);
  motor.run(RELEASE);
  delay(1000);
}
