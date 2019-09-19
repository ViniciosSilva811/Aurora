
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
#include "Adafruit_VL53L0X.h"
#include "Adafruit_TCS34725softi2c.h"

// ------------------------------ VARIÁVEIS E CONSTANTES GLOBAIS ---------------------------- //

#define pino_sensor_esquerda_extremo A3 
#define pino_sensor_esquerda_centro  A5
#define pino_sensor_frente           A7
#define pino_sensor_centro           A9
#define pino_sensor_direita_centro   A11
#define pino_sensor_direita_extremo  A13
#define pino_sensor_traseiro         A1

#define SDAD 36                                     // Pino SDA do sensor de cor direito
#define SCLD 38                                     // Pino SCL do sensor de cor direito
#define SDAE 40                                     // Pino SDA do sensor do cor esquerdo
#define SCLE 42                                     // Pino SCL do sensor de cor esquerdo

#define pino_sensor_de_obstaculo 18
#define pino_sensor_de_toque_traseiro 52

#define buzzer 53

#define ENDERECO_VL53L0X_FRENTE    0x31
#define ENDERECO_VL53L0X_DIREITA   0x32
#define ENDERECO_VL53L0X_ESQUERDA  0x33

#define PINO_XSHUT_VL53L0X_FRENTE     22
#define PINO_XSHUT_VL53L0X_DIREITA    24
#define PINO_XSHUT_VL53L0X_ESQUERDA   26

int see;                                            // Sensor esquerdo extremo
int sec;                                            // Sensor esquerdo central
int sf;                                             // Sensor frontal
int sc;                                             // Sensor central
int sdc;                                            // Sensor direito central
int sde;                                            // Sensor direito extremo
int st;                                             // Sensor traseiro

short limite  = 200;                                // Valor intermediário entre preto e branco
short _limite = 500; 

uint16_t RE, GE, BE, CE, RD, GD, BD, CD;

unsigned long tempo_desde_a_ultima_leitura_de_cor = 0;

byte resultado;

volatile short interrupcao = 0;

boolean obstaculo = false;

char ultima_curva = 'N';

unsigned int medida_lida_pelo_vl53l0x_frente;
unsigned int medida_lida_pelo_vl53l0x_direita;
unsigned int medida_lida_pelo_vl53l0x_esquerda;

VL53L0X_RangingMeasurementData_t medida_frente;
VL53L0X_RangingMeasurementData_t medida_direita;
VL53L0X_RangingMeasurementData_t medida_esquerda;

unsigned long tempo_desde_a_ultima_leitura_nos_sensores_laterais = 0;
unsigned long tempo_de_seguranca = 0;

// ---------------------------------- OBJETOS DAS BIBIOTECAS -------------------------------- //

Stubborn_DCMotor motor_direito (1);
Stubborn_DCMotor motor_esquerdo(2);

Adafruit_VL53L0X vl53l0x_frente   = Adafruit_VL53L0X();
Adafruit_VL53L0X vl53l0x_direita  = Adafruit_VL53L0X();
Adafruit_VL53L0X vl53l0x_esquerda = Adafruit_VL53L0X();

Adafruit_TCS34725softi2c sensor_direito  = Adafruit_TCS34725softi2c(TCS34725_INTEGRATIONTIME_700MS, TCS34725_GAIN_1X, SDAD, SCLD);
Adafruit_TCS34725softi2c sensor_esquerdo = Adafruit_TCS34725softi2c(TCS34725_INTEGRATIONTIME_700MS, TCS34725_GAIN_1X, SDAE, SCLE);

// ----------------------------------------- SETUP ------------------------------------------ //

void setup() {
  configurar_pinos();
  configurar_velocidade_inicial_dos_motores();
  inicializar_sensores_de_cor();
  inicializar_sensor_de_obstaculo();
  iniciar_sensores_vl53l0x();
  inicializar_monitor_serial();
  soar_um_bipe();
  delay(100);
  soar_um_bipe();
  andar_para_frente();
}

// ------------------------------------------ LOOP ------------------------------------------ //

void loop() {
  fazer_leitura_nos_sensores_de_linha();
  seguir_linha();
  verificar_se_existe_obstaculo_a_frente();
  tentar_identificar_a_rampa();
}

// ---------------------------------------- FUNÇÕES ----------------------------------------- //

void configurar_pinos() {
  pinMode(pino_sensor_esquerda_extremo, INPUT);
  pinMode(pino_sensor_esquerda_centro, INPUT);
  pinMode(pino_sensor_frente, INPUT);
  pinMode(pino_sensor_centro, INPUT);
  pinMode(pino_sensor_direita_centro, INPUT);
  pinMode(pino_sensor_direita_extremo, INPUT);
  pinMode(pino_sensor_traseiro, INPUT);
  pinMode(pino_sensor_de_obstaculo, INPUT_PULLUP);
  pinMode(pino_sensor_de_toque_traseiro, INPUT_PULLUP);
  pinMode(buzzer, OUTPUT);
  pinMode(PINO_XSHUT_VL53L0X_FRENTE, OUTPUT);
  pinMode(PINO_XSHUT_VL53L0X_DIREITA, OUTPUT);
  pinMode(PINO_XSHUT_VL53L0X_ESQUERDA, OUTPUT);
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

void soar_um_bipe() {
  tone(buzzer, 1000);
  delay(200);
  noTone(buzzer);
}

// ------------------------------------------------------------------------------------------ //

void mostrar_valores_lidos(char opcao) {
  Serial.print(F("          "));

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
  
  else if (opcao == 'C') {
    Serial.print(RE);
    Serial.print(F(" "));
    Serial.print(GE);
    Serial.print(F(" "));
    Serial.print(BE);
    Serial.print(F("  "));
    Serial.print(CE);
    Serial.print(F("    "));
    Serial.print(RD);
    Serial.print(F(" "));
    Serial.print(GD);
    Serial.print(F(" "));
    Serial.print(BD);
    Serial.print(F("  "));
    Serial.println(CD);
  }

  else if (opcao == 'O') {
    Serial.print(F("                "));
    Serial.print(F("Sensor frontal: "));
    Serial.print(medida_lida_pelo_vl53l0x_frente);
    Serial.print(F("                "));
    Serial.print(F("Sensor direito: "));
    Serial.print(medida_lida_pelo_vl53l0x_direita);
    Serial.print(F("                "));
    Serial.print(F("Sensor esquerdo: "));
    Serial.println(medida_lida_pelo_vl53l0x_esquerda);
  }
}

/* ------------------------------------------------------------------------------------------ *\
|                                                                                              |
|                                                                                              |
|                             BLOCO RELACIONADO À SEGUIR LINHA                                 |
|                                                                                              |
|                                                                                              |
\* ------------------------------------------------------------------------------------------ */

void fazer_leitura_nos_sensores_de_linha() {
  see = analogRead(pino_sensor_esquerda_extremo) + 130;
  sec = analogRead(pino_sensor_esquerda_centro);
  sf  = analogRead(pino_sensor_frente);
  sc  = analogRead(pino_sensor_centro);
  sdc = analogRead(pino_sensor_direita_centro);
  sde = analogRead(pino_sensor_direita_extremo) + 130;
}

// ------------------------------------------------------------------------------------------ //

void seguir_linha() {
  if (sf > limite) {
    if (see > _limite and sde > _limite and sc > _limite and sec > _limite and sdc > _limite) {
      // CRUZAMENTO TOTAL
      // Serial.println(F("                CRUZAMENTO TOTAL\n"))

      if (passou_dois_segundos_desde_a_ultima_leitura_nos_sensores_de_cor()) {
        andar_para_frente();
        delay(100);

        do {
          sec = analogRead(pino_sensor_esquerda_centro);
          sdc = analogRead(pino_sensor_direita_centro);
        } while (sec > _limite and sdc > _limite);
        
        parar();

        andar_para_tras();
        delay(100);

        parar();
        delay(500);

        checar_sensores_de_cor();
        executar_rotina_correspondente();
      }
      else {
        andar_para_frente();
        delay(100);
      }
    }

    else if (see > _limite and sde <= limite and sc > _limite and sec > _limite and sdc <= limite) {
      // CRUZAMENTO COM PRETO NA ESQUERDA
      // Serial.println(F("                CRUZAMENTO COM PRETO NA ESQUERDA\n"))

      if (passou_dois_segundos_desde_a_ultima_leitura_nos_sensores_de_cor()) {
        andar_para_frente();
        delay(100);

        do {
          see = analogRead(pino_sensor_esquerda_extremo);
        } while (see > _limite);

        parar();

        andar_para_tras();
        delay(100);

        parar();
        delay(500);

        checar_sensores_de_cor();
        executar_rotina_correspondente();
      }
      else {
        andar_para_frente();
        delay(100);
      }
    }

    else if (sde > _limite and see <= limite and sc > _limite and sdc > _limite and sec <= limite) {
      // CRUZAMENTO COM PRETO NA DIREITA
      // Serial.println(F("                CRUZAMENTO COM PRETO NA DIREITA\n"))

      if (passou_dois_segundos_desde_a_ultima_leitura_nos_sensores_de_cor()) {
        andar_para_frente();
        delay(100);

        do {
          sde = analogRead(pino_sensor_direita_extremo);
        } while (sde > _limite);

        parar();

        andar_para_tras();
        delay(100);

        parar();
        delay(500);

        checar_sensores_de_cor();
        executar_rotina_correspondente();
      }
      else {
        andar_para_frente();
        delay(100);
      }
    }

    else if (sec > limite and see <= limite and sdc <= limite and sc > limite) {
      // CURVA SIMPLES PARA A ESQUERDA
      // Serial.println(F("                CURVA SIMPLES PARA A ESQUERDA\n"));
      virar_para_esquerda();
      delay(50);
      andar_para_frente();
    }

    else if (sdc > limite and sde <= limite and sec <= limite and sc > limite) {
      // CURVA SIMPLES PARA A DIREITA
      // Serial.println(F("                CURVA SIMPLES PARA A DIREITA\n"));
      virar_para_direita();
      delay(50);
      andar_para_frente();
    }

    else if (sdc <= limite and sec <= limite and sc > limite and see <= limite and sde <= limite) {
      // LINHA RETA
      // Serial.println(F("                LINHA RETA\n"));
      andar_para_frente();
    }
  }
  else {
    if (see > _limite and sc > _limite and sde > _limite) {
      // Interseção T
      // Serial.println(F("Interseção T"))

      andar_para_frente();
      delay(50);

      do {
        sde = analogRead(pino_sensor_direita_extremo);
      } while (sde > limite);

      parar();

      andar_para_tras();
      delay(100);

      parar();
      delay(500);

      checar_sensores_de_cor();
      executar_rotina_correspondente();
    }

    else if (see > _limite and sde <= limite and sc > _limite and sec > _limite) {
      // CURVA DE 90° PARA A ESQUERDA
      // Serial.println(F("                CURVA DE 90° PARA A ESQUERDA\n"));

      andar_para_frente();
      delay(50);

      fazer_leitura_nos_sensores_de_linha();

      if (see > limite and sc > limite and sde > limite and passou_dois_segundos_desde_a_ultima_leitura_nos_sensores_de_cor()) {
        andar_para_frente();
        delay(100);
        
        do {
          sde = analogRead(pino_sensor_esquerda_extremo);
        } while (sde > limite);

        parar();

        andar_para_tras();
        delay(100);

        parar();
        delay(500);

        checar_sensores_de_cor();
        executar_rotina_correspondente();
      }

      else {
        girar_90_graus(PARA_A_ESQUERDA);
        ultima_curva = 'E';
      }
    }

    else if (sde > _limite and see <= limite and sc > _limite and sdc > _limite) {
      // CURVA DE 90° PARA A DIREITA
      // Serial.println(F("CURVA DE 90° PARA A DIREITA"));
      andar_para_frente();
      delay(50);

      fazer_leitura_nos_sensores_de_linha();

      if (see > limite and sc > limite and sde > limite and passou_dois_segundos_desde_a_ultima_leitura_nos_sensores_de_cor()) {
        andar_para_frente();
        delay(100);

        do {
          sde = analogRead(pino_sensor_direita_extremo);
        } while (sde > limite);

        parar();

        andar_para_tras();
        delay(100);

        parar();
        delay(500);

        checar_sensores_de_cor();
        executar_rotina_correspondente();
      }

      else {
        girar_90_graus(PARA_A_DIREITA);
        ultima_curva = 'D';
      }
    }

    else if (sc > limite and see <= limite and sde <= limite and sec <= limite and sdc <= limite) {
      // Serial.println(F("                GAP\n"));
      andar_para_frente();
      delay(50);
    }

    else if (sc > limite and sec > limite and sdc <= limite and see <= limite and sde <= limite) {
      // Serial.println(F("                CURVA SIMPLES PARA A ESQUERDA\n"));
      virar_para_esquerda();
      delay(50);
      andar_para_frente();
    }

    else if (sc > limite and sdc > limite and sec <= limite and see <= limite and sde <= limite) {
      // Serial.println(F("                CURVA SIMPLES PARA A DIREITA\n"));
      virar_para_direita();
      delay(50);
      andar_para_frente();
    }

    else if (see > _limite and sc <= limite and sde <= limite and sdc <= limite) {
      // Serial.println(F("                RETORNANDO PARA A LINHA PELA ESQUERDA\n"));
      retornar_para_a_linha(PELA_ESQUERDA);
    }

    else if (sde > _limite and sc <= limite and see <= limite and sec <= limite) {
      // Serial.println(F("                RETORNANDO PARA A LINHA PELA DIREITA\n"));
      retornar_para_a_linha(PELA_DIREITA);
    }
  }
}

/* ------------------------------------------------------------------------------------------ *\
|                                                                                              |
|                                                                                              |
|                                 BLOCO RELACIONADO À COR                                      |
|                                                                                              |
|                                                                                              |
\* ------------------------------------------------------------------------------------------ */

void inicializar_sensores_de_cor() {
  sensor_direito.begin();
  sensor_esquerdo.begin();
}

// ------------------------------------------------------------------------------------------ //

void checar_sensores_de_cor() {
  sensor_direito. getRawData(&RD, &GD, &BD, &CD);
  sensor_esquerdo.getRawData(&RE, &GE, &BE, &CE);

  tempo_desde_a_ultima_leitura_de_cor = millis();
 
  // mostrar_valores_lidos('C');

  if (CE < 12000 and CD < 12000) { 
    resultado =  MEIA_VOLTA;
    // Serial.println(F("          MEIA VOLTA"));
  }

  else if (CE < 12000 and CD > 12000) {
    resultado =  VIRAR_PARA_A_ESQUERDA;
    ultima_curva = 'E';
    // Serial.println(F("          VIRAR PARA A ESQUERDA"));
  }
  
  else if (CE > 12000 and CD < 12000) {
    resultado =  VIRAR_PARA_A_DIREITA;
    ultima_curva = 'D';
    // Serial.println(F("          VIRAR PARA A DIREITA"));
  }

  else {
    resultado =  NENHUM_VERDE_ENCONTRADO;
    // Serial.println(F("          NENHUM ADESIVO VERDE ENCONTRADO"));
  }
}

// ------------------------------------------------------------------------------------------ //

void executar_rotina_correspondente() {

  switch (resultado) {
    case MEIA_VOLTA:
      soar_um_bipe();
      meia_volta();
      break;

    case VIRAR_PARA_A_ESQUERDA:
      soar_um_bipe();
      andar_para_frente();
      delay(200);
      girar_90_graus(PARA_A_ESQUERDA);
      break;

    case VIRAR_PARA_A_DIREITA:
      soar_um_bipe();
      andar_para_frente();
      delay(200);
      girar_90_graus(PARA_A_DIREITA);
      break;
    
    case NENHUM_VERDE_ENCONTRADO:
      andar_para_frente();
      break;
  }
}

// ------------------------------------------------------------------------------------------ //

boolean passou_dois_segundos_desde_a_ultima_leitura_nos_sensores_de_cor () {
  if (millis() >= (tempo_desde_a_ultima_leitura_de_cor + 2000)) {
    return true;
  }
  return false;
}

/* ------------------------------------------------------------------------------------------ *\
|                                                                                              |
|                                                                                              |
|                          BLOCO RELACINADO AO DESVIO DE OBSTÁCULOS                            |
|                                                                                              |
|                                                                                              |
\* ------------------------------------------------------------------------------------------ */

void inicializar_sensor_de_obstaculo() {
  attachInterrupt(digitalPinToInterrupt(pino_sensor_de_obstaculo), tratar_interrupcoes, FALLING);
}

// ------------------------------------------------------------------------------------------ //

void tratar_interrupcoes() {
  interrupcao ++;
  // Serial.println("Funcionando!");
  if (interrupcao == 1) {
    obstaculo = true;
  }
}

// ------------------------------------------------------------------------------------------ //

void verificar_se_existe_obstaculo_a_frente() {
  if (obstaculo) {
    desviar_obstaculo();
    obstaculo = false;
    interrupcao = 0;
  }
}

// ------------------------------------------------------------------------------------------ //

void desviar_obstaculo() {

  andar_para_tras();

  soar_um_bipe();
  
  delay(300);

  do {
    fazer_leitura_no_sensor_vl53l0x_frontal();
  } while (medida_lida_pelo_vl53l0x_frente <= 3);

  // Desviar para a direita
  if (ultima_curva == 'D') {
    virar_para_direita();
    delay(2500);

    motor_esquerdo.setSpeed(40);
    
    andar_para_frente();

    tempo_de_seguranca = millis();

    do {
      fazer_leitura_no_sensor_vl53l0x_esquerdo();

      if (medida_lida_pelo_vl53l0x_esquerda <= 3) {
        motor_esquerdo.setSpeed(255);
        virar_para_direita();
        delay(50);
        motor_esquerdo.setSpeed(40);
        andar_para_frente();
      }

      // O robô só começará a procurar pela linha preta após 5 segundos
      // após início do desvio do obstáculo
      if (millis() >= tempo_de_seguranca + 10000) { 
        sc = analogRead(pino_sensor_centro);

        if (sc > limite) {
          motor_esquerdo.setSpeed(255);
          soar_um_bipe();
          rotina_de_alinhamento_pos_obstaculo(PARA_A_DIREITA);
          break;
        }
      }
    } while (true);
  }

  // Desviar para a esquerda
  else {
    virar_para_esquerda();
    delay(2500);

    motor_direito.setSpeed(45);
    
    andar_para_frente();

    tempo_de_seguranca = millis();

    do {
      fazer_leitura_no_sensor_vl53l0x_direito();

      if (medida_lida_pelo_vl53l0x_direita <= 3) {
        motor_direito.setSpeed(255);
        virar_para_esquerda();
        delay(50);
        motor_direito.setSpeed(45);
        andar_para_frente();
      }
      
      // O robô só começará a procurar pela linha preta após 5 segundos
      // após início do desvio do obstáculo

      if (millis() >= tempo_de_seguranca + 7000) {
        
        sc = analogRead(pino_sensor_centro);

        if (sc > limite) {
          motor_direito.setSpeed(255);
          soar_um_bipe();
          rotina_de_alinhamento_pos_obstaculo(PARA_A_ESQUERDA);
          break;
        }
      }
    } while (true);
  }
}

// ------------------------------------------------------------------------------------------ //

void rotina_de_alinhamento_pos_obstaculo(byte direcao) {
  andar_para_frente();
  delay(600);
  
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
    st = analogRead(pino_sensor_traseiro);
  } while (st <= 850);

  // O robô anda para trás até que a chave seja tocada 
  andar_para_tras();
  
  while (fazer_leitura_no_sensor_de_toque_traseiro() == 1) {
    delay(50);
  }
  
  // O robô anda para frente e volta a seguir linha
  andar_para_frente();

  fazer_leitura_nos_sensores_de_linha();
  seguir_linha();
}

// ------------------------------------------------------------------------------------------ //

byte fazer_leitura_no_sensor_de_toque_traseiro() {
  return digitalRead(pino_sensor_de_toque_traseiro);
}

// ------------------------------------------------------------------------------------------ //

void iniciar_sensores_vl53l0x() {
  // Hibernado todos os sensores
  digitalWrite(PINO_XSHUT_VL53L0X_FRENTE, LOW);
  digitalWrite(PINO_XSHUT_VL53L0X_DIREITA, LOW);
  digitalWrite(PINO_XSHUT_VL53L0X_ESQUERDA, LOW);
  delay(10);

  // Ativando todos os sensores
  digitalWrite(PINO_XSHUT_VL53L0X_FRENTE, HIGH);
  digitalWrite(PINO_XSHUT_VL53L0X_DIREITA, HIGH);
  digitalWrite(PINO_XSHUT_VL53L0X_ESQUERDA, HIGH);
  delay(10);

  // Ativando o sensor da frente e hibernando os outros
  digitalWrite(PINO_XSHUT_VL53L0X_FRENTE, HIGH);
  digitalWrite(PINO_XSHUT_VL53L0X_DIREITA, LOW);
  digitalWrite(PINO_XSHUT_VL53L0X_ESQUERDA, LOW);
  delay(10);

  // iniciando o sensor frontal
  vl53l0x_frente.begin(ENDERECO_VL53L0X_FRENTE);
  delay(10);

  // Ativando o sensor direito
  digitalWrite(PINO_XSHUT_VL53L0X_DIREITA, HIGH);
  delay(10);

  // Iniciando o sensor direito
  vl53l0x_direita.begin(ENDERECO_VL53L0X_DIREITA);
  delay(10);

  // Ativando o sensor esquerdo
  digitalWrite(PINO_XSHUT_VL53L0X_ESQUERDA, HIGH);
  delay(10);

  // Iniciando o sensor esquerdo
  vl53l0x_esquerda.begin(ENDERECO_VL53L0X_ESQUERDA);
  delay(10);
}

// ------------------------------------------------------------------------------------------ //

void fazer_leitura_no_sensor_vl53l0x_frontal() {
  vl53l0x_frente.rangingTest(&medida_frente);
  
  if (medida_frente.RangeStatus != 4) {
    medida_lida_pelo_vl53l0x_frente = medida_frente.RangeMilliMeter/10;
  }
  else {
    medida_lida_pelo_vl53l0x_frente = 819;
  }
}

// ------------------------------------------------------------------------------------------ //

void fazer_leitura_no_sensor_vl53l0x_direito() {
  vl53l0x_direita.rangingTest(&medida_direita);

  if (medida_direita.RangeStatus != 4) {
    medida_lida_pelo_vl53l0x_direita = medida_direita.RangeMilliMeter/10;
  }
  else {
    medida_lida_pelo_vl53l0x_direita = 819;
  }
}

// ------------------------------------------------------------------------------------------ //

void fazer_leitura_no_sensor_vl53l0x_esquerdo() {
  vl53l0x_esquerda.rangingTest(&medida_esquerda);

  if (medida_esquerda.RangeStatus != 4) {
    medida_lida_pelo_vl53l0x_esquerda = medida_esquerda.RangeMilliMeter/10;
  }
  else {
    medida_lida_pelo_vl53l0x_esquerda = 819;
  }
}

// ------------------------------------------------------------------------------------------ //

void fazer_leitura_nos_sensores_vl53l0x() {
  fazer_leitura_no_sensor_vl53l0x_frontal();
  fazer_leitura_no_sensor_vl53l0x_direito();
  fazer_leitura_no_sensor_vl53l0x_esquerdo();
}

/* ------------------------------------------------------------------------------------------ *\
|                                                                                              |
|                                                                                              |
|                                 BLOCO RELACINADO À RAMPA                                     |
|                                                                                              |
|                                                                                              |
\* ------------------------------------------------------------------------------------------ */

boolean ja_passou_5_segundos_desde_a_ultima_leitura_nos_sensores_laterais() {
  if (millis() >= (tempo_desde_a_ultima_leitura_nos_sensores_laterais + 5000)) {
    tempo_desde_a_ultima_leitura_nos_sensores_laterais = millis();
    return true;
  }
  else {
    return false;
  }
}

// ------------------------------------------------------------------------------------------ //

boolean ja_passou_2_segundos_desde_a_ultima_leitura_nos_sensores_laterais() {
  if (millis() >= (tempo_desde_a_ultima_leitura_nos_sensores_laterais + 2000)) {
    tempo_desde_a_ultima_leitura_nos_sensores_laterais = millis();
    return true;
  }
  else {
    return false;
  }
}

// ------------------------------------------------------------------------------------------ //

void tentar_identificar_a_rampa() {
  if (ja_passou_5_segundos_desde_a_ultima_leitura_nos_sensores_laterais()) {
    fazer_leitura_no_sensor_vl53l0x_direito();
    fazer_leitura_no_sensor_vl53l0x_esquerdo();

    if (medida_lida_pelo_vl53l0x_direita <= 15 and medida_lida_pelo_vl53l0x_esquerda <= 15) {
      soar_um_bipe();
      delay(100);
      soar_um_bipe();
      modo_rampa();
    }
  }
}

// ------------------------------------------------------------------------------------------ //

void modo_rampa() {
  while (true) {
    if (ja_passou_2_segundos_desde_a_ultima_leitura_nos_sensores_laterais()) {
      fazer_leitura_no_sensor_vl53l0x_direito();
      fazer_leitura_no_sensor_vl53l0x_esquerdo();

      // Caso um dos sensores apresente uma distância maior que 30, significa que o robô
      // chegou à sala de resgate, logo ele deve entrar em modo de resgate das vítimas

      if (medida_lida_pelo_vl53l0x_direita >= 30 or medida_lida_pelo_vl53l0x_esquerda >= 30) {
        modo_resgate();
      }
    }

    fazer_leitura_nos_sensores_de_linha();
    
    if (sf > limite and see > limite and sde > limite and sc > limite and sdc > limite and sec > limite) {
      andar_para_frente();
      delay(50);
    }

    else if (sc > limite and see <= limite and sde <= limite and sec <= limite and sdc <= limite) {
      // Serial.println(F("                GAP\n"));
      andar_para_frente();
      delay(50);
    }

    else if (sc > limite and sec > limite and sdc <= limite and see <= limite and sde <= limite) {
      // Serial.println(F("                CURVA SIMPLES PARA A ESQUERDA\n"));
      virar_para_esquerda();
      delay(50);
      andar_para_frente();
    }

    else if (sc > limite and sdc > limite and sec <= limite and see <= limite and sde <= limite) {
      // Serial.println(F("                CURVA SIMPLES PARA A DIREITA\n"));
      virar_para_direita();
      delay(50);
      andar_para_frente();
    }

    else if (see > _limite and sec <= limite and sc <= limite and sde <= limite and sdc <= limite) {
      // Serial.println(F("                RETORNANDO PARA A LINHA PELA ESQUERDA\n"));

      retornar_para_a_linha(PELA_ESQUERDA);
    }

    else if (sde > _limite and sdc <= limite and sc <= limite and see <= limite and sec <= limite) {
      // Serial.println(F("                RETORNANDO PARA A LINHA PELA DIREITA\n"));
      retornar_para_a_linha(PELA_DIREITA);
    }
  }
}

/* ------------------------------------------------------------------------------------------ *\
|                                                                                              |
|                                                                                              |
|                          BLOCO RELACINADO AO RESGATE DAS VÍTIMAS                             |
|                                                                                              |
|                                                                                              |
\* ------------------------------------------------------------------------------------------ */

// ------------------------------------------------------------------------------------------ //

void modo_resgate() {
  parar();

  while (true) {
    soar_um_bipe();
    delay(200);
  }
}

/* ------------------------------------------------------------------------------------------ *\
|                                                                                              |
|                                                                                              |
|                          BLOCO RELACINADO AOS MOVIMENTOS DO ROBÔ                             |
|                                                                                              |
|                                                                                              |
\* ------------------------------------------------------------------------------------------ */

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
  delay(600);
  
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
  } while (sf <= limite);

  switch (direcao){
    case PARA_A_DIREITA:
      virar_para_direita();
      delay(400);
      break;
    case PARA_A_ESQUERDA:
      virar_para_esquerda();
      delay(400);
      break;
  }

  andar_para_frente();

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
  motor_esquerdo.run(FORWARD);
  motor_direito.run(RELEASE);
}

// -------------------------------------- FIM DO CÓDIGO ------------------------------------- //
