
/* ------------------------------------------------------------------------------------------ *\
|                                                                                              |
|                                                                                              |
|                                      Código Principal                                        |
|                                                                                              |
|                                                                                              |
\* ------------------------------------------------------------------------------------------ */

// ----------------------------------- BIBLIOTECAS USADAS ----------------------------------- //

#include <Arduino.h>
#include <Servo.h>
#include <Headers.h>
#include "Stubborn_DCMotor.h"
#include "Adafruit_VL53L0X.h"
#include "Adafruit_TCS34725softi2c.h"

// --------------------------------------- CONSTANTES --------------------------------------- //

#define pino_sensor_traseiro         A1
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

#define pino_sensor_de_obstaculo           18
#define pino_sensor_de_toque_traseiro      52

#define pino_buzzer                        51

#define ENDERECO_VL53L0X_FRONTAL_CIMA      0x31
#define ENDERECO_VL53L0X_FRONTAL_CENTRO    0x32
#define ENDERECO_VL53L0X_FRONTAL_BAIXO     0x33
#define ENDERECO_VL53L0X_DIREITO           0x34
#define ENDERECO_VL53L0X_ESQUERDO          0x35

#define PINO_XSHUT_VL53L0X_FRONTAL_CIMA    25
#define PINO_XSHUT_VL53L0X_DIREITO         23
#define PINO_XSHUT_VL53L0X_ESQUERDO        24
#define PINO_XSHUT_VL53L0X_FRONTAL_BAIXO   22
#define PINO_XSHUT_VL53L0X_FRONTAL_CENTRO  26

#define pino_servo_garra_direito           26
#define pino_servo_garra_esquerdo          27
#define pino_servo_guindaste_direito       28
#define pino_servo_guindaste_esquerdo      29
#define pino_servo_cacamba                 30

// --------------------------------------- VARIÁVEIS ---------------------------------------- //

int see;                                            // Sensor esquerdo extremo
int sec;                                            // Sensor esquerdo central
int sf;                                             // Sensor frontal
int sc;                                             // Sensor central
int sdc;                                            // Sensor direito central
int sde;                                            // Sensor direito extremo
int st;                                             // Sensor traseiro

int limite  = 200;                                // Valor intermediário entre preto e branco
int _limite = 500; 

uint16_t RE, GE, BE, CE, RD, GD, BD, CD;            // Variáveis do sensor de cor

unsigned long tempo_desde_a_ultima_leitura_de_cor = 0;

byte resultado;

volatile int interrupcao = 0;

boolean obstaculo = false;

char ultima_curva = 'E';

int medida_lida_pelo_vl53l0x_frontal_cima;
int medida_lida_pelo_vl53l0x_frontal_centro;
int medida_lida_pelo_vl53l0x_frontal_baixo;
int medida_lida_pelo_vl53l0x_direito;
int medida_lida_pelo_vl53l0x_esquerdo;

VL53L0X_RangingMeasurementData_t medida_frente_cima;
VL53L0X_RangingMeasurementData_t medida_frente_centro;
VL53L0X_RangingMeasurementData_t medida_frente_baixo;
VL53L0X_RangingMeasurementData_t medida_direita;
VL53L0X_RangingMeasurementData_t medida_esquerda;

unsigned long tempo_desde_a_ultima_leitura_nos_sensores_laterais = 0;

// Tempo em segundos para que o robô volte a procurar a linha
unsigned long tempo_de_seguranca = 0;

int vezes_que_encontrei_a_bolinha = 0;

// ---------------------------------- OBJETOS DAS BIBIOTECAS -------------------------------- //

Servo servo_garra_direito,
      servo_garra_esquerdo,
      servo_guindaste_direito,
      servo_guindaste_esquerdo,
      servo_cacamba;

Stubborn_DCMotor motor_direito (1);
Stubborn_DCMotor motor_esquerdo(2);

Adafruit_VL53L0X vl53l0x_frontal_cima    = Adafruit_VL53L0X();
Adafruit_VL53L0X vl53l0x_frontal_centro  = Adafruit_VL53L0X();
Adafruit_VL53L0X vl53l0x_frontal_baixo   = Adafruit_VL53L0X();
Adafruit_VL53L0X vl53l0x_direita         = Adafruit_VL53L0X();
Adafruit_VL53L0X vl53l0x_esquerda        = Adafruit_VL53L0X();

Adafruit_TCS34725softi2c sensor_direito  = Adafruit_TCS34725softi2c(TCS34725_INTEGRATIONTIME_700MS, TCS34725_GAIN_1X, SDAD, SCLD);
Adafruit_TCS34725softi2c sensor_esquerdo = Adafruit_TCS34725softi2c(TCS34725_INTEGRATIONTIME_700MS, TCS34725_GAIN_1X, SDAE, SCLE);

// ----------------------------------------- SETUP ------------------------------------------ //

void setup() {
  configurar_pinos();
  configurar_velocidade_dos_motores(200);
  inicializar_sensores_de_cor();
  inicializar_sensor_de_obstaculo();
  inicializar_sensores_vl53l0x();
  inicializar_monitor_serial();
  soar_dois_bipes();
  andar_para_frente();
}

// ------------------------------------------ LOOP ------------------------------------------ //

void loop() {
  fazer_leitura_nos_sensores_de_linha_principais();
  seguir_linha();
  verificar_se_existe_obstaculo_a_frente();
  procurar_a_rampa();
}

/* ------------------------------------------------------------------------------------------ *\
|                                                                                              |
|                                                                                              |
|                                      BLOCO INÍCIO                                            |
|                                                                                              |
|                                                                                              |
\* ------------------------------------------------------------------------------------------ */

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
  pinMode(pino_buzzer, OUTPUT);
  pinMode(PINO_XSHUT_VL53L0X_FRONTAL_CIMA, OUTPUT);
  pinMode(PINO_XSHUT_VL53L0X_FRONTAL_CENTRO, OUTPUT);
  pinMode(PINO_XSHUT_VL53L0X_FRONTAL_BAIXO, OUTPUT);
  pinMode(PINO_XSHUT_VL53L0X_DIREITO, OUTPUT);
  pinMode(PINO_XSHUT_VL53L0X_ESQUERDO, OUTPUT);
}

// ------------------------------------------------------------------------------------------ //

void inicializar_monitor_serial() {
  Serial.begin(115200);
}

// ------------------------------------------------------------------------------------------ //

void inicializar_sensores_de_cor() {
  sensor_direito.begin();
  sensor_esquerdo.begin();
}

// ------------------------------------------------------------------------------------------ //

void inicializar_sensor_de_obstaculo() {
  attachInterrupt(digitalPinToInterrupt(pino_sensor_de_obstaculo), tratar_interrupcoes, FALLING);
}

// ------------------------------------------------------------------------------------------ //

void inicializar_sensores_vl53l0x() {
  // Hibernado todos os sensores
  digitalWrite(PINO_XSHUT_VL53L0X_FRONTAL_CIMA, LOW);
  digitalWrite(PINO_XSHUT_VL53L0X_FRONTAL_CENTRO, LOW);
  digitalWrite(PINO_XSHUT_VL53L0X_FRONTAL_BAIXO, LOW);
  digitalWrite(PINO_XSHUT_VL53L0X_DIREITO, LOW);
  digitalWrite(PINO_XSHUT_VL53L0X_ESQUERDO, LOW);
  delay(10);

  // Ativando todos os sensores
  digitalWrite(PINO_XSHUT_VL53L0X_FRONTAL_CIMA, HIGH);
  digitalWrite(PINO_XSHUT_VL53L0X_FRONTAL_CENTRO, HIGH);
  digitalWrite(PINO_XSHUT_VL53L0X_FRONTAL_BAIXO, HIGH);
  digitalWrite(PINO_XSHUT_VL53L0X_DIREITO, HIGH);
  digitalWrite(PINO_XSHUT_VL53L0X_ESQUERDO, HIGH);
  delay(10);

  // Ativando o sensor da frente e hibernando os outros
  digitalWrite(PINO_XSHUT_VL53L0X_FRONTAL_CIMA, HIGH);
  digitalWrite(PINO_XSHUT_VL53L0X_FRONTAL_CENTRO, LOW);
  digitalWrite(PINO_XSHUT_VL53L0X_FRONTAL_BAIXO, LOW);
  digitalWrite(PINO_XSHUT_VL53L0X_DIREITO, LOW);
  digitalWrite(PINO_XSHUT_VL53L0X_ESQUERDO, LOW);
  delay(10);

  // iniciando o sensor frontal cima
  vl53l0x_frontal_cima.begin(ENDERECO_VL53L0X_FRONTAL_CIMA);
  delay(10);

  // digitalWrite(PINO_XSHUT_VL53L0X_FRONTAL_CENTRO, HIGH);
  // delay(10);

  // iniciando o sensor frontal centro
  // vl53l0x_frontal_centro.begin(ENDERECO_VL53L0X_FRONTAL_CENTRO);
  // delay(10);
  

  // digitalWrite(PINO_XSHUT_VL53L0X_FRONTAL_BAIXO, HIGH);
  // delay(10);

  // // iniciando o sensor frontal baixo
  // vl53l0x_frontal_baixo.begin(ENDERECO_VL53L0X_FRONTAL_BAIXO);
  // delay(10);
  
  // Ativando o sensor direito
  digitalWrite(PINO_XSHUT_VL53L0X_DIREITO, HIGH);
  delay(10);

  // Iniciando o sensor direito
  vl53l0x_direita.begin(ENDERECO_VL53L0X_DIREITO);
  delay(10);

  // Ativando o sensor esquerdo
  digitalWrite(PINO_XSHUT_VL53L0X_ESQUERDO, HIGH);
  delay(10);

  // Iniciando o sensor esquerdo
  vl53l0x_esquerda.begin(ENDERECO_VL53L0X_ESQUERDO);
  delay(10);
}

// ------------------------------------------------------------------------------------------ //

void inicializar_servo_motores() {
  servo_garra_direito.attach(pino_servo_garra_direito);
  servo_garra_esquerdo.attach(pino_servo_garra_esquerdo);
  servo_guindaste_direito.attach(pino_servo_guindaste_direito);
  servo_guindaste_esquerdo.attach(pino_servo_guindaste_esquerdo);
  servo_cacamba.attach(pino_servo_cacamba);
}

// ------------------------------------------------------------------------------------------ //

void configurar_velocidade_dos_motores(byte valor) {
  motor_direito. setSpeed(valor);
  motor_esquerdo.setSpeed(valor);
}

/* ------------------------------------------------------------------------------------------ *\
|                                                                                              |
|                                                                                              |
|                                          BLOCO MOTOR                                         |
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
  delay(350);
  
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
  } while (sf <= _limite);

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

}

// ------------------------------------------------------------------------------------------ //

void meia_volta() {
  andar_para_tras();
  delay(200);

  virar_para_esquerda();
  delay(1250);

  do {
    sf = analogRead(pino_sensor_frente);
  } while (sf <= 600);

  andar_para_frente();
  delay(200);
}

// ------------------------------------------------------------------------------------------ //

void parar() {
  motor_direito.run(RELEASE);
  motor_esquerdo.run(RELEASE);
}

// ------------------------------------------------------------------------------------------ //

void abrir_garra() {  
  servo_garra_direito.write(0);
  servo_garra_esquerdo.write(180);
}

// ------------------------------------------------------------------------------------------ //

void fechar_garra() {
  servo_garra_direito.write(180);
  servo_garra_esquerdo.write(0);
}

// ------------------------------------------------------------------------------------------ //

void subir_garra() {
  servo_guindaste_direito.write(0);
  servo_guindaste_esquerdo.write(180);
}

// ------------------------------------------------------------------------------------------ //

void descer_garra() {
  servo_guindaste_direito.write(180);
  servo_guindaste_esquerdo.write(0);
}

// ------------------------------------------------------------------------------------------ //

void abrir_cacamba() {
  servo_cacamba.write(180);
}

// ------------------------------------------------------------------------------------------ //

void fechar_cacamba() {
  servo_cacamba.write(0);
}

/* ------------------------------------------------------------------------------------------ *\
|                                                                                              |
|                                                                                              |
|                                         BLOCO SAÍDAS                                         |
|                                                                                              |
|                                                                                              |
\* ------------------------------------------------------------------------------------------ */

void soar_um_bipe() {
  tone(pino_buzzer, 1000);
  delay(200);
  noTone(pino_buzzer);
}

// ------------------------------------------------------------------------------------------ //

void soar_dois_bipes() {
  soar_um_bipe();
  delay(100);
  soar_um_bipe();
}

// ---------------------------------- FUNÇÃO DE DEBUG --------------------------------------- //

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

  else if (opcao == 'V') {
    Serial.print(F("SensorF cima: "));
    Serial.print(medida_lida_pelo_vl53l0x_frontal_cima);
    Serial.print(F("    "));
    Serial.print(F("SensorF centro: "));
    Serial.print(medida_lida_pelo_vl53l0x_frontal_centro);
    Serial.print(F("    "));
    Serial.print(F("SensorF baixo: "));
    Serial.print(medida_lida_pelo_vl53l0x_frontal_baixo);
    Serial.print(F("    "));
    Serial.print(F("Sensor direito: "));
    Serial.print(medida_lida_pelo_vl53l0x_direito);
    Serial.print(F("    "));
    Serial.print(F("Sensor esquerdo: "));
    Serial.println(medida_lida_pelo_vl53l0x_esquerdo);
  }
}

/* ------------------------------------------------------------------------------------------ *\
|                                                                                              |
|                                                                                              |
|                                     BLOCO SENSOR                                             |
|                                                                                              |
|                                                                                              |
\* ------------------------------------------------------------------------------------------ */

void fazer_leitura_nos_sensores_de_linha_principais() {
  see = analogRead(pino_sensor_esquerda_extremo) + 130;
  sec = analogRead(pino_sensor_esquerda_centro);
  sf  = analogRead(pino_sensor_frente);
  sc  = analogRead(pino_sensor_centro);
  sdc = analogRead(pino_sensor_direita_centro);
  sde = analogRead(pino_sensor_direita_extremo) + 130;
}

// ------------------------------------------------------------------------------------------ //

void fazer_leitura_no_sensor_de_linha_traseiro() {
  st = analogRead(pino_sensor_traseiro);
}

// ------------------------------------------------------------------------------------------ //

void checar_sensores_de_cor() {
  sensor_direito. getRawData(&RD, &GD, &BD, &CD);
  sensor_esquerdo.getRawData(&RE, &GE, &BE, &CE);

  tempo_desde_a_ultima_leitura_de_cor = millis();
 
  // mostrar_valores_lidos('C');

  if (CE < 15000 and CD < 15000) { 
    resultado =  MEIA_VOLTA;
    // Serial.println(F("          MEIA VOLTA"));
  }


  else if (CE < 15000 and CD > 15000) {
    resultado =  VIRAR_PARA_A_ESQUERDA;
    ultima_curva = 'E';
    // Serial.println(F("          VIRAR PARA A ESQUERDA"));
  }
  
  else if (CE > 15000 and CD < 15000) {
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

byte fazer_leitura_no_sensor_de_toque_traseiro() {
  return digitalRead(pino_sensor_de_toque_traseiro);
}

// ------------------------------------------------------------------------------------------ //

void fazer_leitura_no_sensor_vl53l0x_frontal_cima() {
  vl53l0x_frontal_cima.rangingTest(&medida_frente_cima);
  
  if (medida_frente_cima.RangeStatus != 4) {
    medida_lida_pelo_vl53l0x_frontal_cima = medida_frente_cima.RangeMilliMeter/10;
  }
  else {
    medida_lida_pelo_vl53l0x_frontal_cima = 819;
  }
  delay(10);
}

// ------------------------------------------------------------------------------------------ //

void fazer_leitura_no_sensor_vl53l0x_frontal_centro() {
  vl53l0x_frontal_centro.rangingTest(&medida_frente_centro);
  
  if (medida_frente_centro.RangeStatus != 4) {
    medida_lida_pelo_vl53l0x_frontal_centro = medida_frente_centro.RangeMilliMeter/10;
  }
  else {
    medida_lida_pelo_vl53l0x_frontal_centro = 819;
  }
  delay(10);
}

// ------------------------------------------------------------------------------------------ //

void fazer_leitura_no_sensor_vl53l0x_frontal_baixo() {
  vl53l0x_frontal_baixo.rangingTest(&medida_frente_baixo);
  
  if (medida_frente_baixo.RangeStatus != 4) {
    medida_lida_pelo_vl53l0x_frontal_baixo = medida_frente_baixo.RangeMilliMeter/10;
  }
  else {
    medida_lida_pelo_vl53l0x_frontal_baixo = 819;
  }
  delay(10);
}

// ------------------------------------------------------------------------------------------ //

void fazer_leitura_no_sensor_vl53l0x_direito() {
  vl53l0x_direita.rangingTest(&medida_direita);

  if (medida_direita.RangeStatus != 4) {
    medida_lida_pelo_vl53l0x_direito = medida_direita.RangeMilliMeter/10;
  }
  else {
    medida_lida_pelo_vl53l0x_direito = 819;
  }
}

// ------------------------------------------------------------------------------------------ //

void fazer_leitura_no_sensor_vl53l0x_esquerdo() {
  vl53l0x_esquerda.rangingTest(&medida_esquerda);

  if (medida_esquerda.RangeStatus != 4) {
    medida_lida_pelo_vl53l0x_esquerdo = medida_esquerda.RangeMilliMeter/10;
  }
  else {
    medida_lida_pelo_vl53l0x_esquerdo = 819;
  }
}

// ------------------------------------------------------------------------------------------ //

void fazer_leitura_nos_sensores_vl53l0x() {
  fazer_leitura_no_sensor_vl53l0x_frontal_cima();
  fazer_leitura_no_sensor_vl53l0x_frontal_centro();
  fazer_leitura_no_sensor_vl53l0x_frontal_baixo();
  fazer_leitura_no_sensor_vl53l0x_direito();
  fazer_leitura_no_sensor_vl53l0x_esquerdo();
}

/* ------------------------------------------------------------------------------------------ *\
|                                                                                              |
|                                                                                              |
|                                     BLOCO SEGUIR LINHA                                       |
|                                                                                              |
|                                                                                              |
\* ------------------------------------------------------------------------------------------ */

void seguir_linha() {
  if (sf > limite) {
    if (see > _limite and sde > _limite and sc > _limite and sec > _limite and sdc > _limite) {
      // CRUZAMENTO TOTAL
      // Serial.println(F("                CRUZAMENTO TOTAL\n"))

      if (passou_meio_segundo_desde_a_ultima_leitura_nos_sensores_de_cor()) {
        andar_para_frente();
        delay(50);

        do {
          sec = analogRead(pino_sensor_esquerda_centro);
          sdc = analogRead(pino_sensor_direita_centro);
          delay(10);
        } while (sec > _limite and sdc > _limite);
        
        parar();

        andar_para_tras();
        delay(60);

        parar();
        delay(500);

        checar_sensores_de_cor();
        executar_rotina_correspondente();
      }
      else {
        andar_para_frente();
      }
    }

    else if (see > limite and sde <= limite and sc > _limite and sec > _limite and sdc <= _limite) {
      // CRUZAMENTO COM PRETO NA ESQUERDA
      // Serial.println(F("                CRUZAMENTO COM PRETO NA ESQUERDA\n"))

      if (passou_meio_segundo_desde_a_ultima_leitura_nos_sensores_de_cor()) {
        andar_para_frente();
        delay(50);

        do {
          see = analogRead(pino_sensor_esquerda_extremo);
          delay(10);
        } while (see > _limite);

        parar();

        andar_para_tras();
        delay(60);

        parar();
        delay(500);

        checar_sensores_de_cor();
        executar_rotina_correspondente();
      }
      else {
        andar_para_frente();
      }
    }

    else if (sde > _limite and see <= limite and sc > _limite and sdc > _limite and sec <= limite) {
      // CRUZAMENTO COM PRETO NA DIREITA
      // Serial.println(F("                CRUZAMENTO COM PRETO NA DIREITA\n"))

      if (passou_meio_segundo_desde_a_ultima_leitura_nos_sensores_de_cor()) {
        andar_para_frente();
        delay(50);

        do {
          sde = analogRead(pino_sensor_direita_extremo);
          delay(10);
        } while (sde > _limite);

        parar();

        andar_para_tras();
        delay(50);

        parar();
        delay(500);

        checar_sensores_de_cor();
        executar_rotina_correspondente();
      }
      else {
        andar_para_frente();
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
      delay(25);

      do {
        sde = analogRead(pino_sensor_direita_extremo);
        delay(10);
      } while (sde > limite);

      parar();

      andar_para_tras();
      delay(50);

      parar();
      delay(500);

      checar_sensores_de_cor();
      executar_rotina_correspondente();
    }

    else if (see > limite and sde <= limite and sc > _limite and sec > _limite) {
      // CURVA DE 90° PARA A ESQUERDA
      // Serial.println(F("                CURVA DE 90° PARA A ESQUERDA\n"));

      andar_para_frente();

      fazer_leitura_nos_sensores_de_linha_principais();

      if (see > limite and sc > limite and sde > limite and passou_meio_segundo_desde_a_ultima_leitura_nos_sensores_de_cor()) {
        andar_para_frente();
        delay(50);
        
        do {
          sde = analogRead(pino_sensor_esquerda_extremo);
          delay(10);
        } while (sde > limite);

        parar();

        andar_para_tras();
        delay(60);

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

    else if (sde > limite and see <= limite and sc > _limite and sdc > _limite) {
      // CURVA DE 90° PARA A DIREITA
      // Serial.println(F("CURVA DE 90° PARA A DIREITA"));
      
      andar_para_frente();

      fazer_leitura_nos_sensores_de_linha_principais();

      if (see > limite and sc > limite and sde > limite and passou_meio_segundo_desde_a_ultima_leitura_nos_sensores_de_cor()) {
        andar_para_frente();
        delay(50);

        do {
          sde = analogRead(pino_sensor_direita_extremo);
          delay(10);
        } while (sde > limite);

        parar();

        andar_para_tras();
        delay(60);

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
    }

    else if (sc > limite and sec > limite and sdc <= limite and see <= limite and sde <= limite) {
      // Serial.println(F("                CURVA SIMPLES PARA A ESQUERDA\n"));
      virar_para_esquerda();
      delay(10);
      andar_para_frente();
    }

    else if (sc > limite and sdc > limite and sec <= limite and see <= limite and sde <= limite) {
      // Serial.println(F("                CURVA SIMPLES PARA A DIREITA\n"));
      virar_para_direita();
      delay(10);
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

/* ------------------------------------------------------------------------------------------ *\
|                                                                                              |
|                                                                                              |
|                                           BLOCO COR                                          |
|                                                                                              |
|                                                                                              |
\* ------------------------------------------------------------------------------------------ */

void executar_rotina_correspondente() {

  switch (resultado) {
    case MEIA_VOLTA:
      soar_um_bipe();
      meia_volta();
      break;

    case VIRAR_PARA_A_ESQUERDA:
      soar_um_bipe();
      andar_para_frente();
      delay(50);
      girar_90_graus(PARA_A_ESQUERDA);
      break;

    case VIRAR_PARA_A_DIREITA:
      soar_um_bipe();
      andar_para_frente();
      delay(50);
      girar_90_graus(PARA_A_DIREITA);
      break;
    
    case NENHUM_VERDE_ENCONTRADO:
      sf = analogRead(pino_sensor_frente);
      
      if (sf > _limite) {
        andar_para_frente();
      }
      
      else {
        andar_para_tras();
        delay(50);
      }

      break;
  }
}

// ------------------------------------------------------------------------------------------ //

boolean passou_meio_segundo_desde_a_ultima_leitura_nos_sensores_de_cor () {
  if (millis() >= (tempo_desde_a_ultima_leitura_de_cor + 500)) {
    return true;
  }
  return false;
}

/* ------------------------------------------------------------------------------------------ *\
|                                                                                              |
|                                                                                              |
|                                      BLOCO OBSTÁCULO                                         |
|                                                                                              |
|                                                                                              |
\* ------------------------------------------------------------------------------------------ */

void tratar_interrupcoes() {
  interrupcao ++;
  if (interrupcao == 1 and (millis() > 1000L)) {
    obstaculo = true;
  } 
  else {
    interrupcao = 0;
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

  unsigned long tempo_andar_para_tras = millis() + 200;

  while(millis() <= tempo_andar_para_tras);

  // Desviar para a direita
  if (ultima_curva == 'D') {
    virar_para_direita();

    delay(1200);

    andar_para_frente();
  
    motor_esquerdo.setSpeed(25);

    tempo_de_seguranca = millis();

    do {
      fazer_leitura_no_sensor_vl53l0x_esquerdo();

      if (medida_lida_pelo_vl53l0x_esquerdo <= 3) {
        motor_esquerdo.setSpeed(255);
        virar_para_direita();
        delay(50);
        motor_esquerdo.setSpeed(25);
        andar_para_frente();
      }

      // O robô só começará a procurar pela linha preta após 7 segundos
      // após início do desvio do obstáculo
      if (millis() >= (tempo_de_seguranca + 7000)) { 
        sc = analogRead(pino_sensor_centro);

        if (sc > limite) {
          configurar_velocidade_dos_motores(200);
          soar_um_bipe();
          rotina_de_alinhamento_pos_obstaculo(PARA_A_DIREITA);
          break;
        }
      }
    } while (true);
  }

  // Desviar para a esquerda
  else if(ultima_curva == 'E') {
    virar_para_esquerda();

    delay(1250);
    
    andar_para_frente();

    motor_direito.setSpeed(30);

    tempo_de_seguranca = millis();

    do {
      fazer_leitura_no_sensor_vl53l0x_direito();

      if (medida_lida_pelo_vl53l0x_direito <= 3) {
        motor_direito.setSpeed(255);
        virar_para_esquerda();
        delay(50);
        motor_direito.setSpeed(30);
        andar_para_frente();
      }
      
      // O robô só começará a procurar pela linha preta após 7 segundos
      // após início do desvio do obstáculo

      if (millis() >= tempo_de_seguranca + 7000) {
        
        sc = analogRead(pino_sensor_centro);

        if (sc > limite) {
          configurar_velocidade_dos_motores(200);
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
  delay(150);
  
  switch (direcao){
    case PARA_A_DIREITA:
      virar_para_direita();
      break;
    case PARA_A_ESQUERDA:
      virar_para_esquerda();
      delay(150);
      break;
  }

  do {
    fazer_leitura_no_sensor_de_linha_traseiro();
  } while (st <= 950);

  // O robô anda para trás até que a chave seja tocada 
  andar_para_tras();
  
  while (fazer_leitura_no_sensor_de_toque_traseiro() != 0) {
    delay(50);
  }
  
  // O robô anda para frente e volta a seguir linha
  andar_para_frente();

  fazer_leitura_nos_sensores_de_linha_principais();
  seguir_linha();
}

/* ------------------------------------------------------------------------------------------ *\
|                                                                                              |
|                                                                                              |
|                                         BLOCO RAMPA                                          |
|                                                                                              |
|                                                                                              |
\* ------------------------------------------------------------------------------------------ */

boolean passou_3_segundos_desde_a_ultima_leitura_nos_sensores_laterais() {
  if (millis() >= (tempo_desde_a_ultima_leitura_nos_sensores_laterais + 3000)) {
    tempo_desde_a_ultima_leitura_nos_sensores_laterais = millis();
    return true;
  }
  else {
    return false;
  }
}

// ------------------------------------------------------------------------------------------ //

boolean passou_1_segundo_desde_a_ultima_leitura_nos_sensores_laterais() {
  if (millis() >= (tempo_desde_a_ultima_leitura_nos_sensores_laterais + 1000)) {
    tempo_desde_a_ultima_leitura_nos_sensores_laterais = millis();
    return true;
  }
  else {
    return false;
  }
}

// ------------------------------------------------------------------------------------------ //

void procurar_a_rampa() {
  if (passou_3_segundos_desde_a_ultima_leitura_nos_sensores_laterais()) {
    fazer_leitura_no_sensor_vl53l0x_direito();
    fazer_leitura_no_sensor_vl53l0x_esquerdo();

    if (medida_lida_pelo_vl53l0x_direito <= 15 and medida_lida_pelo_vl53l0x_esquerdo <= 15) {
      soar_um_bipe();
      modo_rampa();
    }
  }
}

// ------------------------------------------------------------------------------------------ //

void modo_rampa() {
  configurar_velocidade_dos_motores(255);
  
  while (true) {
    if (passou_1_segundo_desde_a_ultima_leitura_nos_sensores_laterais()) {
      fazer_leitura_no_sensor_vl53l0x_direito();
      fazer_leitura_no_sensor_vl53l0x_esquerdo();

      // Caso um dos sensores apresente uma distância maior que 30, significa que o robô
      // chegou à sala de resgate. Então, ele entra em modo de resgate

      if (medida_lida_pelo_vl53l0x_direito >= 30 or medida_lida_pelo_vl53l0x_esquerdo >= 30) {
        modo_resgate();
      }
    }

    fazer_leitura_nos_sensores_de_linha_principais();
    
    seguir_linha_rampa();
  }
}

// ------------------------------------------------------------------------------------------ //

void seguir_linha_rampa() {
  if (sf > limite and see > limite and sde > limite and sc > limite and sdc > limite and sec > limite) {
    andar_para_frente();
  }

  else if (sc > limite and see <= limite and sde <= limite and sec <= limite and sdc <= limite) {
    // Serial.println(F("                GAP\n"));
    andar_para_frente();
  }

  else if (sc > limite and sec > limite and sdc <= limite and see <= limite and sde <= limite) {
    // Serial.println(F("                CURVA SIMPLES PARA A ESQUERDA\n"));
    virar_para_esquerda();
    delay(10);
    andar_para_frente();
  }

  else if (sc > limite and sdc > limite and sec <= limite and see <= limite and sde <= limite) {
    // Serial.println(F("                CURVA SIMPLES PARA A DIREITA\n"));
    virar_para_direita();
    delay(10);
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
/* ------------------------------------------------------------------------------------------ *\
|                                                                                              |
|                                                                                              |
|                          BLOCO RELACINADO AO RESGATE DAS VÍTIMAS                             |
|                                                                                              |
|                                                                                              |
\* ------------------------------------------------------------------------------------------ */

void modo_resgate() {
  soar_um_bipe();

  andar_para_frente();
  delay(1000);

  do {
    fazer_leitura_no_sensor_vl53l0x_frontal_cima();
    fazer_leitura_no_sensor_vl53l0x_direito();
    if (medida_lida_pelo_vl53l0x_direito <= 3) {
      virar_para_esquerda();
      delay(100);
      andar_para_frente();
    }
  } while (medida_lida_pelo_vl53l0x_frontal_cima >= 40);

  parar();

  // while (true) {
  //   virar_para_esquerda();
  //   delay(50);
  //   parar();
  //   fazer_leitura_no_sensor_vl53l0x_frontal_cima();
  //   fazer_leitura_no_sensor_vl53l0x_frontal_centro();
  //   fazer_leitura_no_sensor_vl53l0x_frontal_baixo();
  //   mostrar_valores_lidos('O');
  //   delay(100);
  //   verificar_situacoes_e_mostrar_resultado();
  // }
  delay(250000);
}

// ------------------------------------------------------------------------------------------ //

void verificar_situacoes_e_mostrar_resultado() {

  if (medida_lida_pelo_vl53l0x_frontal_centro - medida_lida_pelo_vl53l0x_frontal_baixo >= 8) {
    vezes_que_encontrei_a_bolinha ++;
    
    if (vezes_que_encontrei_a_bolinha > 5) {
      parar();
      soar_um_bipe();
      virar_para_esquerda();
      delay(400);
      Serial.print("          Bolinha encontrada");

      if (medida_lida_pelo_vl53l0x_frontal_cima - medida_lida_pelo_vl53l0x_frontal_centro >= 10) {
        Serial.print(" e a área de resgate também");
      }
      Serial.println();

      andar_para_frente();

      do {
        fazer_leitura_no_sensor_vl53l0x_frontal_baixo();
      } while (medida_lida_pelo_vl53l0x_frontal_baixo >= 5);

      parar();
      vezes_que_encontrei_a_bolinha = 0;
      delay(5000);
    }
  }

  else if ((medida_lida_pelo_vl53l0x_frontal_cima - medida_lida_pelo_vl53l0x_frontal_centro) > 10) {
    Serial.println("        Área de resgate encontrada");
  }
}

// -------------------------------------- FIM DO CÓDIGO ------------------------------------- //
