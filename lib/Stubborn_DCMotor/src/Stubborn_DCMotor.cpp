/* ------------------------------------------------------------------------------------- *\
|                                                                                         |
|                                                                                         |
|                                   Stubborn_DCMotor.h                                    |
|                   Código baseado na biblioteca Adafruit Motor Shield	                  |
|                                   Developed by Venish                                   |
|                                                                                         |
|                                                                                         |
|                                                                                         |
\* ------------------------------------------------------------------------------------- */

// ------------------------------------------------------------------------------------- //

#include <Arduino.h>
#include "Stubborn_DCMotor.h"

// ------------------------------------------------------------------------------------- //

#define PINO_VELOCIDADE_MOTOR_1 2
#define MOTOR1_A 3
#define MOTOR1_B 4

#define MOTOR2_A 5
#define MOTOR2_B 6
#define PINO_VELOCIDADE_MOTOR_2 7

// --------------------------------- MÉTODO CONSTRUTOR --------------------------------- //

Stubborn_DCMotor::Stubborn_DCMotor(uint8_t motornum){
	
	_motornum = motornum;
	
	switch (_motornum) {
	    case 1:
	      pinMode(MOTOR1_A, OUTPUT);
	      pinMode(MOTOR1_B, OUTPUT);
	      pinMode(PINO_VELOCIDADE_MOTOR_1, OUTPUT);
	      break;
	    case 2:
	      pinMode(MOTOR2_A, OUTPUT);
	      pinMode(MOTOR2_B, OUTPUT);
	      pinMode(PINO_VELOCIDADE_MOTOR_2, OUTPUT);
	      break;
	}
}

// ---------------------- FUNÇÃO PARA CONFIGURAÇÃO DE VELOCIDADE ----------------------- //

void Stubborn_DCMotor::setSpeed(uint8_t velocidade){
	
	_velocidade = velocidade;
	
	switch (_motornum) {
	    case 1:
	      analogWrite(PINO_VELOCIDADE_MOTOR_1, _velocidade);
	      break;
	    case 2:
	      analogWrite(PINO_VELOCIDADE_MOTOR_2, _velocidade);
	      break;
	}
}

// ------------------- FUNÇÃO PARA CONFIGURAÇÃO DE SENTIDO DE ROTAÇÃO ------------------ //

void Stubborn_DCMotor::run(uint8_t sentido){
	
	_sentido = sentido;
	
	switch (_motornum) {
	    case 1:
	      a = MOTOR1_A; b = MOTOR1_B;
	      break;
	    case 2:
	      a = MOTOR2_A; b = MOTOR2_B;
	      break;
	    default:
	      return;
	}

	switch (_sentido) {
	    case FORWARD:
	      digitalWrite(a, HIGH);
	      digitalWrite(b, LOW);
	      break;
	    case BACKWARD:
	      digitalWrite(a, LOW);
	      digitalWrite(b, HIGH);
	      break;
	    case BRAKE:
	      digitalWrite(a, HIGH);
	      digitalWrite(b, HIGH);
	      break;
	    case RELEASE:
	      digitalWrite(a, LOW);
	      digitalWrite(b, LOW);
	      break;
	}
}

// ----------------------------------- FIM DO CÓDIGO ----------------------------------- //
