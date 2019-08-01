
#include <Stubborn_DCMotor.h>

Stubborn_DCMotor motor_direito  (1);
Stubborn_DCMotor motor_esquerdo (2);

void setup(){
  motor_esquerdo.setSpeed(100);
  motor_direito.setSpeed(100);
}

void loop(){
  
}