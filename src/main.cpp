
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

#define SDAD 36                                     // Pino SDA do sensor de cor direito
#define SCLD 38                                     // Pino SCL do sensor de cor direito
#define SDAE 40                                     // Pino SDA do sensor do cor esquerdo
#define SCLE 42                                     // Pino SCL do sensor de cor esquerdo

#define pino_sensor_de_obstaculo 18

int see;                                            // Sensor esquerdo extremo
int sec;                                            // Sensor esquerdo central
int sf;                                             // Sensor frontal
int sc;                                             // Sensor central
int sdc;                                            // Sensor direito central
int sde;                                            // Sensor direito extremo

int limite = 250;                                   // Valor intermediário entre preto e branco

uint16_t RE, GE, BE, CE, RD, GD, BD, CD;

unsigned long tempo_desde_a_ultima_leitura_de_cor = 0;

byte resultado;

volatile int interrupcao = 0;

boolean obstaculo = false;

// ---------------------------------- OBJETOS DAS BIBIOTECAS -------------------------------- //

Stubborn_DCMotor motor_direito (1);
Stubborn_DCMotor motor_esquerdo(2);

Adafruit_TCS34725softi2c sensor_direito  = Adafruit_TCS34725softi2c(TCS34725_INTEGRATIONTIME_700MS, TCS34725_GAIN_1X, SDAD, SCLD);
Adafruit_TCS34725softi2c sensor_esquerdo = Adafruit_TCS34725softi2c(TCS34725_INTEGRATIONTIME_700MS, TCS34725_GAIN_1X, SDAE, SCLE);

// ----------------------------------------- SETUP ------------------------------------------ //

void setup() {
  configurar_pinos();
  configurar_velocidade_inicial_dos_motores();
  inicializar_sensores_de_cor();
  //inicializar_sensor_de_obstaculo();
  inicializar_monitor_serial();
}

// ------------------------------------------ LOOP ------------------------------------------ //

void loop() {
  fazer_leitura_nos_sensores_de_linha();
  mostrar_valores('L');
  seguir_linha();
}

// ---------------------------------------- FUNÇÕES ----------------------------------------- //

void configurar_pinos() {
  pinMode(pino_sensor_esquerda_extremo, INPUT);
  pinMode(pino_sensor_esquerda_centro, INPUT);
  pinMode(pino_sensor_frente, INPUT);
  pinMode(pino_sensor_centro, INPUT);
  pinMode(pino_sensor_direita_centro, INPUT);
  pinMode(pino_sensor_direita_extremo, INPUT);
  pinMode(pino_sensor_de_obstaculo, INPUT_PULLUP);
}

// ------------------------------------------------------------------------------------------ //

void configurar_velocidade_inicial_dos_motores() {
  motor_direito.setSpeed(255);
  motor_esquerdo.setSpeed(255);
}

// ------------------------------------------------------------------------------------------ //

void inicializar_monitor_serial() {
  Serial.begin(115200);
}

// ------------------------------------------------------------------------------------------ //

void mostrar_valores(char opcao) {
  Serial.print("          ");

  if (opcao == 'L') {
    Serial.print(see);
    Serial.print(F("  "));
    Serial.print(sec); 
    Serial.print(F("  "));
    Serial.print(sf);
    Serial.print(F("  "));
    Serial.print(sc);
    Serial.print(F("  "));
    Serial.print(sdc);
    Serial.print(F("  "));
    Serial.println(sde);
  }
  
  if (opcao == 'C') {
    Serial.print(RE);
    Serial.print(F(" "));
    Serial.print(GE);
    Serial.print(F(" "));
    Serial.print(BE);
    Serial.print(F("  "));
    Serial.print(CE);
    Serial.print("    ");
    Serial.print(RD);
    Serial.print(F(" "));
    Serial.print(GD);
    Serial.print(F(" "));
    Serial.print(BD);
    Serial.print(F("  "));
    Serial.println(CD);
  }
}

// ------------------------------------------------------------------------------------------ //

// -------------------------- BLOCO RELACIONADO À SEGUIR LINHA ------------------------------ //

// ------------------------------------------------------------------------------------------ //

void fazer_leitura_nos_sensores_de_linha() {
  see = analogRead(pino_sensor_esquerda_extremo);
  sec = analogRead(pino_sensor_esquerda_centro);
  sf  = analogRead(pino_sensor_frente);
  sc  = analogRead(pino_sensor_centro);
  sdc = analogRead(pino_sensor_direita_centro);
  sde = analogRead(pino_sensor_direita_extremo);
}

// ------------------------------------------------------------------------------------------ //

void seguir_linha() {
  if (sf > limite) {
    if (see > limite and sde > limite and sc > limite and sec > limite and sdc > limite) {
      // CRUZAMENTO TOTAL
      if (passou_um_segundo_desde_a_ultima_leitura_nos_sensores_de_cor()) {
        andar_para_frente();
        delay(100);

        do {
          sec = analogRead(pino_sensor_esquerda_centro);
          sdc = analogRead(pino_sensor_direita_centro);
        } while (sec > limite and sdc > limite);
        
        parar();

        andar_para_tras();
        delay(200);

        parar();
        delay(1000);

        checar_sensores_de_cor();
        executar_rotina_correspondente();
      }
      else {
        andar_para_frente();
        delay(200);
      }
    }

    else if (see > limite and sde <= limite and sc > limite and sec > limite and sdc <= limite) {
      // CRUZAMENTO COM PRETO NA ESQUERDA
      if (passou_um_segundo_desde_a_ultima_leitura_nos_sensores_de_cor()) {
        andar_para_frente();
        delay(100);

        do {
          sec = analogRead(pino_sensor_esquerda_centro);
        } while (sec > limite);

        parar();

        andar_para_tras();
        delay(200);

        parar();
        delay(1000);

        checar_sensores_de_cor();
        executar_rotina_correspondente();
      }
      else {
        andar_para_frente();
        delay(200);
      }
    }

    else if (sde > limite and see <= limite and sc > limite and sdc > limite and sec <= limite) {
      // CRUZAMENTO COM PRETO NA DIREITA
      if (passou_um_segundo_desde_a_ultima_leitura_nos_sensores_de_cor()) {
        andar_para_frente();
        delay(100);

        do {
          sdc = analogRead(pino_sensor_direita_centro);
        } while (sdc > limite);

        parar();

        andar_para_tras();
        delay(200);

        parar();
        delay(1000);

        checar_sensores_de_cor();
        executar_rotina_correspondente();
      }
      else {
        andar_para_frente();
      }
    }

    else if (sec > limite and see <= limite and sdc <= limite and sc > limite) {
      // CURVA SIMPLES PARA A ESQUERDA
      virar_para_esquerda();
      delay(50);
      andar_para_frente();
    }

    else if (sdc > limite and sde <= limite and sec <= limite and sc > limite) {
      // CURVA SIMPLES PARA A DIREITA
      virar_para_direita();
      delay(50);
      andar_para_frente();
    }

    else if (sdc <= limite and sec <= limite and sc > limite and see <= limite and sde <= limite) {
      // LINHA RETA
      andar_para_frente();
    }
  }
  else {
    if (see > limite and sde <= limite and sc > limite and sec > limite) {
      // CURVA DE 90° PARA A ESQUERDA
      // VERIFICAR SE REALMENTE É UMA CURVA DE 90°, POIS PODE SER UM T OU UM CRUZAMENTO
      girar_90_graus(PARA_A_ESQUERDA);
    }

    else if (sde > limite and see <= limite and sc > limite and sdc > limite) {
      // CURVA DE 90° PARA A DIREITA
      // VERIFICAR SE REALMENTE É UMA CURVA DE 90°, POIS PODE SER UM T OU UM CRUZAMENTO      
      girar_90_graus(PARA_A_DIREITA);
    }

    else if (sc > limite and see <= limite and sde <= limite and sec <= limite and sdc <= limite) {
      andar_para_frente();
    }

    else if (sc > limite and sec > limite and sdc <= limite and see <= limite and sde <= limite) {
      // virar_para_esquerda();
      // delay(100);
      // andar_para_frente();
    }

    else if (sc > limite and sdc > limite and sec <= limite and see <= limite and sde <= limite) {
      // virar_para_direita();
      // delay(100);
      // andar_para_frente();
    }

    else if (see > limite and sec <= limite and sc <= limite and sde <= limite and sdc <= limite) {
      retornar_para_a_linha(PELA_ESQUERDA);
    }

    else if (sde > limite and sdc <= limite and sc <= limite and see <= limite and sec <= limite) {
      retornar_para_a_linha(PELA_DIREITA);
    }
  }
}

// ------------------------------------------------------------------------------------------ //

// -------------------------------- BLOCO RELACIONADO À COR --------------------------------- //

// ------------------------------------------------------------------------------------------ //

void inicializar_sensores_de_cor() {
  sensor_direito.begin();
  sensor_esquerdo.begin();
}

// ------------------------------------------------------------------------------------------ //

void checar_sensores_de_cor() {
  sensor_direito. getRawData(&RD, &GD, &BD, &CD);
  sensor_esquerdo.getRawData(&RE, &GE, &BE, &CE);

  tempo_desde_a_ultima_leitura_de_cor = millis();

  if (CE < 12000 and CD < 12000) { 
    resultado =  MEIA_VOLTA;
    Serial.println("          MEIA VOLTA");
  }

  else if (CE < 12000 and CD > 12000) {
    resultado =  VIRAR_PARA_A_ESQUERDA;
    Serial.println("          VIRAR PARA A ESQUERDA");
  }
  
  else if (CE > 12000 and CD < 12000) {
    resultado =  VIRAR_PARA_A_DIREITA;
    Serial.println("          VIRAR PARA A DIREITA");
  }

  else {
    resultado =  NENHUM_VERDE_ENCONTRADO;
    Serial.println("          NENHUM ADESIVO VERDE ENCONTRADO");
  }
}

// ------------------------------------------------------------------------------------------ //

void executar_rotina_correspondente() {

  switch (resultado) {
    case MEIA_VOLTA:
      meia_volta();
      break;

    case VIRAR_PARA_A_ESQUERDA:
      andar_para_frente();
      delay(200);
      girar_90_graus(PARA_A_ESQUERDA);
      break;

    case VIRAR_PARA_A_DIREITA:
      andar_para_frente();
      delay(200);
      girar_90_graus(PARA_A_DIREITA);
      break;
    
    case NENHUM_VERDE_ENCONTRADO:
      andar_para_frente();
      delay(100);
      break;
  }
}

// ------------------------------------------------------------------------------------------ //

boolean passou_um_segundo_desde_a_ultima_leitura_nos_sensores_de_cor () {
  if (millis() >= (tempo_desde_a_ultima_leitura_de_cor + 1000)) {
    return true;
  }
  return false;
}

// ------------------------------------------------------------------------------------------ //

// --------------------- BLOCO RELACINADO AO DESVIO DE OBSTÁCULOS --------------------------- //

// ------------------------------------------------------------------------------------------ //

void inicializar_sensor_de_obstaculo() {
  attachInterrupt(digitalPinToInterrupt(pino_sensor_de_obstaculo), tratar_interrupcoes, FALLING);
}

// ------------------------------------------------------------------------------------------ //

void tratar_interrupcoes() {
  // interrupcao ++;

  // if (interrupcao == 1) {
    obstaculo = true;
    Serial.println("Uma interrupção!");
  // }

}

// ------------------------------------------------------------------------------------------ //

void verificar_se_existe_obstaculo_a_frente() {
  if (obstaculo) {
    desviar_obstaculo();
    obstaculo = false;
  }
}

// ------------------------------------------------------------------------------------------ //

void desviar_obstaculo() {
  Serial.println("Bati em um obstáculo");
  Serial.println("Desviando...");
  delay(5000);
  Serial.println("Stubborn Champiom!");
}

// ------------------------------------------------------------------------------------------ //

// ---------------------- BLOCO RELACIONADO AOS MOVIMENTOS DO ROBÔ -------------------------- //

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

void girar_90_graus(byte direcao) {
  andar_para_frente();
  delay(300);
  
  switch (direcao){
    case PARA_A_DIREITA:
      virar_para_direita();
      delay(600);
      break;
    case PARA_A_ESQUERDA:
      virar_para_esquerda();
      delay(600);
      break;
  }

  do {
    sf = analogRead(pino_sensor_frente);
  } while (sf <= 600);

  switch (direcao){
    case PARA_A_DIREITA:
      virar_para_direita();
      delay(300);
      break;
    case PARA_A_ESQUERDA:
      virar_para_esquerda();
      delay(300);
      break;
  }

  andar_para_frente();
  delay(300);

}

// ------------------------------------------------------------------------------------------ //

void meia_volta() {
  andar_para_tras();
  delay(700);

  virar_para_esquerda();
  delay(1300);

  do {
    sf = analogRead(pino_sensor_frente);
  } while (sf <= 600);

  andar_para_frente();
  delay(400);
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
  motor_esquerdo.run(RELEASE);
}

// ------------------------------------------------------------------------------------------ //

void mover_somente_motor_esquerdo() {
  motor_direito.run(FORWARD);
  motor_esquerdo.run(RELEASE);
}

// -------------------------------------- FIM DO CÓDIGO ------------------------------------- //
