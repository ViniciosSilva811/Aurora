
/* ------------------------------------------------------------------------------------------ *\
|                                                                                              |
|                                                                                              |
|                                      Código Principal                                        |
|                                                                                              |
|                                                                                              |
\* ------------------------------------------------------------------------------------------ */

// ----------------------------------- BIBLIOTECAS USADAS ----------------------------------- //

#include <Arduino.h>
#include <Headers.h>
#include "Stubborn_DCMotor.h"

// ------------------------------------------------------------------------------------------ //

#define pino_sensor_esquerda_extremo A13 
#define pino_sensor_esquerda_centro  A11
#define pino_sensor_frente           A9
#define pino_sensor_centro           A7
#define pino_sensor_direita_centro   A5
#define pino_sensor_direita_extremo  A3

int see;                             // Sensor esquerdo extremo
int sec;                             // Sensor esquerdo central
int sf;                              // Sensor frontal
int sc;                              // Sensor central
int sdc;                             // Sensor direito central
int sde;                             // Sensor direito extremo

int limite = 400;                    // Valor intermediário entre preto e branco

// ---------------------------------- OBJETOS DAS BIBIOTECAS -------------------------------- //

Stubborn_DCMotor motor_direito (1);
Stubborn_DCMotor motor_esquerdo(2);

// ----------------------------------------- SETUP ------------------------------------------ //

void setup() {
  configurar_pinos();
  inicializar_serial();
}

// ------------------------------------------ LOOP ------------------------------------------ //

void loop() {
  ler_sensores_de_linha();
  mostrar_valores();
  delay(500);
  // seguir_linha();
}

// ---------------------------------------- FUNÇÕES ----------------------------------------- //

void configurar_pinos() {
  pinMode(pino_sensor_esquerda_extremo, INPUT);
  pinMode(pino_sensor_esquerda_centro, INPUT);
  pinMode(pino_sensor_frente, INPUT);
  pinMode(pino_sensor_centro, INPUT);
  pinMode(pino_sensor_direita_centro, INPUT);
  pinMode(pino_sensor_direita_extremo, INPUT);
}
//

void inicializar_serial() {
  Serial.begin(115200);
}

// ------------------------------------------------------------------------------------------ //

void ler_sensores_de_linha() {
  see = analogRead(pino_sensor_esquerda_extremo);
  sec = analogRead(pino_sensor_esquerda_centro);
  sf  = analogRead(pino_sensor_frente);
  sc  = analogRead(pino_sensor_centro);
  sdc = analogRead(pino_sensor_direita_centro);
  sde = analogRead(pino_sensor_direita_extremo);
}

void mostrar_valores() {
  Serial.println(see);
  Serial.print("  ");
  Serial.print(sec); 
  Serial.print("  ");
  Serial.print(sf);
  Serial.print("  ");
  Serial.print(sc);
  Serial.print("  ");
  Serial.print(sdc);
  Serial.print("  ");
  Serial.print(sde);
  Serial.println(" ");
}

// ------------------------------------------------------------------------------------------ //

void seguir_linha() {
  if (sf > limite) {
    if (see > limite and sde > limite and sc > limite and sec > limite and sdc > limite) {
      // Cruzamento
      // Adicionar verificar cor!
      andar_para_frente();
    }
    else if(see > limite ) {
      if(sec > limite and sc > limite and sdc <= limite and sde <= limite) {
        // Cruzamento
        // Adicionar verificar cor!
        andar_para_frente();
      }
      else if(sec <= limite and sc > limite and sdc <= limite and sde <= limite) {
        // Curva simples para a esquerda
        virar_para_esquerda();
      }
    }
    else if(sde > limite ) {
      if(sdc > limite and sc > limite and sec <= limite and see <= limite) {
        // Cruzamento
        // Adicionar verificar cor!
        andar_para_frente();
      }
      else if(sdc <= limite and sc > limite and sec <= limite and see <= limite) {
        // Curva simples para a direita
        virar_para_direita();
      }
    }
    else if(see <= limite and sde <= limite and sc > limite and sec <= limite and sdc <= limite) {
      // Linha reta
      andar_para_frente();
    }
  }
  else {
    if (see > limite and sde > limite and sc > limite and sec > limite and sdc > limite) {
      // T
      // Adicionar verificar cor!
    }
    else if(see > limite) {
      if(sde <= limite and sc > limite and sec > limite and sdc <= limite) {
        // Curva de 90° para a esquerda
        girar_90_graus(PARA_A_ESQUERDA);
      }
      else if(sec <= limite and sc > limite and sdc <= limite and sde <= limite) {
        // Curva simples para a esquerda
        virar_para_esquerda();
      }
    }
    else if (sde > limite) {
      if(see <= limite and sc > limite and sdc > limite and sec <= limite) {
        // Curva de 90° para a direita
        girar_90_graus(PARA_A_DIREITA);
      }
      else if(sdc <= limite and sc > limite and sec <= limite and see <= limite) {
        // Curva simples para a direita
        virar_para_direita();
      }
    }
  }
}

// ------------------------------------------------------------------------------------------ //

void andar_para_frente() {
  motor_direito.run(FORWARD);
  motor_esquerdo.run(FORWARD);
}

// ------------------------------------------------------------------------------------------ //

void andar_para_tras() {
  motor_direito.run(BACKWARD);
  motor_esquerdo.run(BACKWARD);
}

// ------------------------------------------------------------------------------------------ //

void virar_para_direita() {
  motor_direito.run(BACKWARD);
  motor_esquerdo.run(FORWARD);
}

// ------------------------------------------------------------------------------------------ //

void virar_para_esquerda() {
  motor_direito.run(FORWARD);
  motor_esquerdo.run(BACKWARD);
}

// ------------------------------------------------------------------------------------------ //

void girar_90_graus(int direcao) {
  switch (direcao){
    case PARA_A_DIREITA:
      virar_para_direita();
      delay(500);
      break;
    case PARA_A_ESQUERDA:
      virar_para_esquerda();
      delay(500);
      break;
  }

  do {
    sf = analogRead(pino_sensor_frente);
  } while (sf <= limite);

  andar_para_frente();
  delay(1000);
}

// ------------------------------------------------------------------------------------------ //

void mover_somente_motor_direito() {
  motor_direito.run(FORWARD);
  motor_esquerdo.run(BRAKE);
}

// ------------------------------------------------------------------------------------------ //

void mover_somente_motor_esquerdo() {
  motor_direito.run(FORWARD);
  motor_esquerdo.run(BRAKE);
}

// -------------------------------------- FIM DO CÓDIGO ------------------------------------- //
