
/* ----------------------------------------------------------------------------------------- *\
|                                                                                             |
|                                                                                             |
|                                       Código Principal                                      |
|                                                                                             |
|                                                                                             |
\* ----------------------------------------=------------------------------------------------ */


// ----------------------------------- BIBLIOTECAS USADAS ---------------------------------- //

#include <Arduino.h>
#include <Headers.h>
#include "Stubborn_DCMotor.h"

// ----------------------------------------------------------------------------------------- //

#define pino_sensor_esquerda_extremo A0
#define pino_sensor_esquerda_centro  A1
#define pino_sensor_frente           A3
#define pino_sensor_centro           A4
#define pino_sensor_direita_centro   A5
#define pino_sensor_direita_extremo  A6


int sensor_esquerda_extremo,
    sensor_esquerda_centro,
    sensor_frente,
    sensor_centro,
    sensor_direita_centro,
    sensor_direita_extremo;

// --------------------------------- OBJETOS DAS BIBIOTECAS -------------------------------- //

Stubborn_DCMotor motor_direito (1);
Stubborn_DCMotor motor_esquerdo(2);

// ---------------------------------------- SETUP ------------------------------------------ //

void setup() {
  iniciar();
}

// ---------------------------------------- LOOP ------------------------------------------ //

void loop() {

}

// -------------------------------------- FUNÇÕES ----------------------------------------- //

void iniciar() {
  pinMode(pino_sensor_esquerda_extremo, INPUT);
  pinMode(pino_sensor_esquerda_centro, INPUT);
  pinMode(pino_sensor_frente, INPUT);
  pinMode(pino_sensor_centro, INPUT);
  pinMode(pino_sensor_direita_centro, INPUT);
  pinMode(pino_sensor_direita_extremo, INPUT);
}

// ----------------------------------------------------------------------------------------- //

void ler_sensores_de_linha() {
  sensor_esquerda_extremo = analogRead(pino_sensor_esquerda_extremo);
  sensor_esquerda_centro  = analogRead(pino_sensor_esquerda_centro);
  sensor_frente           = analogRead(pino_sensor_frente);
  sensor_centro           = analogRead(pino_sensor_centro);
  sensor_direita_centro   = analogRead(pino_sensor_direita_centro);
  sensor_direita_extremo  = analogRead(pino_sensor_direita_extremo);
}

// ----------------------------------------------------------------------------------------- //

void andar_para_frente() {
  motor_direito.run(FORWARD);
  motor_esquerdo.run(FORWARD);
}

// ----------------------------------------------------------------------------------------- //

void andar_para_tras() {
  motor_direito.run(BACKWARD);
  motor_esquerdo.run(BACKWARD);
}

// ----------------------------------------------------------------------------------------- //

void virar_para_direita() {
  motor_direito.run(BACKWARD);
  motor_esquerdo.run(FORWARD);
}

// ----------------------------------------------------------------------------------------- //

void virar_para_esquerda() {
  motor_direito.run(FORWARD);
  motor_esquerdo.run(BACKWARD);
}

// ----------------------------------------------------------------------------------------- //

void mover_somente_motor_direito() {
  motor_direito.run(FORWARD);
  motor_esquerdo.run(BRAKE);
}

// ----------------------------------------------------------------------------------------- //

void mover_somente_motor_esquerdo() {
  motor_direito.run(FORWARD);
  motor_esquerdo.run(BRAKE);
}

// ------------------------------------- FIM DO CÓDIGO ------------------------------------- //