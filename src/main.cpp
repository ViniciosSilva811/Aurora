
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
#include "Adafruit_TCS34725softi2c.h"

// ------------------------------ VARIÁVEIS E CONSTANTES GLOBAIS ---------------------------- //

#define pino_sensor_esquerda_extremo A3 
#define pino_sensor_esquerda_centro  A5
#define pino_sensor_frente           A7
#define pino_sensor_centro           A9
#define pino_sensor_direita_centro   A11
#define pino_sensor_direita_extremo  A13

int see;                             // Sensor esquerdo extremo
int sec;                             // Sensor esquerdo central
int sf;                              // Sensor frontal
int sc;                              // Sensor central
int sdc;                             // Sensor direito central
int sde;                             // Sensor direito extremo

int limite = 200;                    // Valor intermediário entre preto e branco

#define SDAD 38                      // SDA para o sensor direito
#define SCLD 36                      // SDL para o sensor direito
#define SDAE 42                      // SDA para o sensor esquerdo
#define SCLE 40                      // SDA para o sensor esquerdo

uint16_t RE, GE, BE, CE, RD, GD, BD, CD;

// ---------------------------------- OBJETOS DAS BIBIOTECAS -------------------------------- //

Stubborn_DCMotor motor_direito (1);
Stubborn_DCMotor motor_esquerdo(2);

Adafruit_TCS34725softi2c sensor_direita  = Adafruit_TCS34725softi2c(TCS34725_INTEGRATIONTIME_700MS, TCS34725_GAIN_1X, SDAD, SCLD);
Adafruit_TCS34725softi2c sensor_esquerda = Adafruit_TCS34725softi2c(TCS34725_INTEGRATIONTIME_700MS, TCS34725_GAIN_1X, SDAE, SCLE);

// ----------------------------------------- SETUP ------------------------------------------ //

void setup() {
  configurar_pinos();
  configurar_velocidade_inicial_dos_motores();
  inicializar_monitor_serial();
}

// ------------------------------------------ LOOP ------------------------------------------ //

void loop() {
  ler_sensores_de_cor();
  mostrar_valores();
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

// ------------------------------------------------------------------------------------------ //

void configurar_velocidade_inicial_dos_motores() {
  motor_direito.setSpeed(255);
  motor_esquerdo .setSpeed(255);
}

// ------------------------------------------------------------------------------------------ //

void inicializar_monitor_serial() {
  Serial.begin(115200);
}

// -----------------------------------------s------------------------------------------------- //

void ler_sensores_de_linha() {
  see = analogRead(pino_sensor_esquerda_extremo);
  sec = analogRead(pino_sensor_esquerda_centro);
  sf  = analogRead(pino_sensor_frente);
  sc  = analogRead(pino_sensor_centro);
  sdc = analogRead(pino_sensor_direita_centro);
  sde = analogRead(pino_sensor_direita_extremo);
}

// ------------------------------------------------------------------------------------------ //

void ler_sensores_de_cor() {
  sensor_direita. getRawData(&RD, &GD, &BD, &CD);
  sensor_esquerda.getRawData(&RE, &GE, &BE, &CE);
}

// ------------------------------------------------------------------------------------------ //

void mostrar_valores() {
  // Serial.print(see);
  // Serial.print("  ");
  // Serial.print(sec); 
  // Serial.print("  ");
  // Serial.print(sf);
  // Serial.print("  ");
  // Serial.print(sc);
  // Serial.print("  ");
  // Serial.print(sdc);
  // Serial.print("  ");
  // Serial.println(sde);
  Serial.print("          ");
  Serial.print(RE);
  Serial.print(" ");
  Serial.print(GE);
  Serial.print(" ");
  Serial.print(BE);
  Serial.print("    ");
  Serial.print(RD);
  Serial.print(" ");
  Serial.print(GD);
  Serial.print(" ");
  Serial.println(BD);
}

// ------------------------------------------------------------------------------------------ //

void seguir_linha() {
  if (sf > limite) {
    if (see > limite and sde > limite and sc > limite and sec > limite and sdc > limite) {
      // CRUZAMENTO TOTAL
      andar_para_frente();
      delay(200);
    }

    else if (see > limite and sde <= limite and sc > limite and sec > limite) {
      // CRUZAMENTO COM PPRETO NA ESQUERDA
      andar_para_frente();
      delay(200);
    }

    else if (sde > limite and see <= limite and sc > limite and sdc > limite) {
      // CRUZAMENTO COM PRETO NA DIREITA
      andar_para_frente();
      delay(200);
    }

    else if (sec > limite and see <= limite and sdc <= limite and sc > limite) {
      // CURVA SIMPLES PARA A ESQUERDA
      virar_para_esquerda();
      delay(100);
    }

    else if (sdc > limite and sde <= limite and sec <= limite and sc > limite) {
      // CURVA SIMPLES PARA A DIREITA
      virar_para_direita();
      delay(100);
    }

    else if (sdc <= limite and sec <= limite and sc > limite and see <= limite and sde <= limite) {
      // LINHA RETA
      andar_para_frente();
      delay(100);
    }
  }
  else {
    if (see > limite and sde <= limite and sc > limite and sec > limite) {
      // CURVA DE 90° PARA A ESQUERDA
      girar_90_graus(PARA_A_ESQUERDA);
    }

    else if (sde > limite and see <= limite and sc > limite and sdc > limite) {
      // CURVA DE 90° PARA A DIREITA
      girar_90_graus(PARA_A_DIREITA);
    }
    
    else if (sec > limite and see <= limite and sdc <= limite and sc > limite) {
      // CURVA SIMPLES PARA A ESQUERDA
      virar_para_esquerda();
      delay(100);
    }

    else if (sdc > limite and sde <= limite and sec <= limite and sc > limite) {
      // CURVA SIMPLES PARA A DIREITA
      virar_para_direita();
      delay(100);
    }

    else if (see > limite and sc <= limite and sf <= limite and sde <= limite) {
      retornar_para_a_linha(PELA_ESQUERDA);
    }

    else if (sde > limite and sc <= limite and sf <= limite and see <= limite) {
      retornar_para_a_linha(PELA_DIREITA);
    }
    
    else {
      andar_para_frente();
      delay(100);
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
  motor_direito.run(FORWARD);
  motor_esquerdo.run(BACKWARD);
}

// ------------------------------------------------------------------------------------------ //

void virar_para_esquerda() {
  motor_direito.run(BACKWARD);
  motor_esquerdo.run(FORWARD);
}

// ------------------------------------------------------------------------------------------ //

void girar_90_graus(byte direcao) {
  andar_para_frente();
  delay(500);
  
  switch (direcao){
    case PARA_A_DIREITA:
      virar_para_direita();
      delay(1000);
      break;
    case PARA_A_ESQUERDA:
      virar_para_esquerda();
      delay(1000);
      break;
  }

  do {
    sf = analogRead(pino_sensor_frente);
  } while (sf <= 600);

  switch (direcao){
    case PARA_A_DIREITA:
      virar_para_direita();
      delay(100);
      break;
    case PARA_A_ESQUERDA:
      virar_para_esquerda();
      delay(100);
      break;
  }

  andar_para_frente();
  delay(100);

}

// ------------------------------------------------------------------------------------------ //

void retornar_para_a_linha(byte lado) {
  switch (lado) {
    case PELA_ESQUERDA:
      do {
        virar_para_esquerda();
        sf = analogRead(pino_sensor_frente);
      } while (sf <= 400);

      do {
        andar_para_frente();
        sc = analogRead(pino_sensor_centro);
      } while (sc <= 400);

      do {
        virar_para_direita();
        sf = analogRead(pino_sensor_frente);
      } while (sf <= 400);
      
      break;

    case PELA_DIREITA:
      do {
        virar_para_direita();
        sf = analogRead(pino_sensor_frente);
      } while (sf <= 400);

      do {
        andar_para_frente();
        sc = analogRead(pino_sensor_centro);
      } while (sc <= 400);

      do {
        virar_para_esquerda();
        sf = analogRead(pino_sensor_frente);
      } while (sf <= 400);

      break;
  }
}

// ------------------------------------------------------------------------------------------ //

void parar() {
  motor_direito.run(RELEASE);
  motor_esquerdo.run(RELEASE);
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
