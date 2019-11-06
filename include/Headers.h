
/* ------------------------------------------------------------------------------------------ *\
|                                                                                              |
|                                                                                              |
|                                           Headers.h                                          |
|                        Aqui são declarados todos os cabeçalhos das funções                   |
|                                que serão usadas no código principal                          |
|                                                                                              |
|                                                                                              |
\* ------------------------------------------------------------------------------------------ */

#ifndef Headers_h
#define Headers_h

#define PARA_A_ESQUERDA 1
#define PARA_A_DIREITA  2

#define PELA_ESQUERDA   1
#define PELA_DIREITA    2

#define MEIA_VOLTA 1
#define VIRAR_PARA_A_DIREITA 2
#define VIRAR_PARA_A_ESQUERDA 3
#define NENHUM_VERDE_ENCONTRADO 4

/* ------------------------------------------------------------------------------------------ *\
|                                                                                              |
|                                                                                              |
|                                       BLOCO INÍCIO                                           |
|                                                                                              |
|                                                                                              |
\* ------------------------------------------------------------------------------------------ */

// Define o modo de eperação de todos os pinos
void configurar_pinos();

// Inicia a comunicação com o Monitor Serial
void inicializar_monitor_serial();

// Inicializa os sensores de cor
void inicializar_sensores_de_cor();

// Inicializa o sensor de obstáculo
void inicializar_sensor_de_obstaculo();

// Inicializa os sensores de distância
void inicializar_sensores_vl53l0x();

// Inicializa os servo motores
void inicializar_servo_motores();

// Configura a velocidade dos motores
void configurar_velocidade_dos_motores(byte valor);

/* ------------------------------------------------------------------------------------------ *\
|                                                                                              |
|                                                                                              |
|                                        BLOCO MOTOR                                           |
|                                                                                              |
|                                                                                              |
\* ------------------------------------------------------------------------------------------ */

// Locomover o robô para a frente
void andar_para_frente();

// Locomover o robô para trás
void andar_para_tras();

// Girar o robô em torno do seu próprio eixo para a direita
void virar_para_direita();

// Girar o robô em torno do seu próprio eixo para esquerda
void virar_para_esquerda();

// Girar o robô 90° para a direção que for passada como parâmetro
void girar_90_graus(byte direcao);

// Girar o robô 180° em torno do seu próprio eixo
void meia_volta();

// Parar o robô
void parar();

// Abrir a garra
void abrir_garra();

// Fechar a garra
void fechar_garra();

// Subir a garra 
void subir_garra();

// Descer a garra
void descer_garra();

// Abrir caçamba
void abrir_cacamba();

// Fachar caçamba
void fechar_cacamba();

/* ------------------------------------------------------------------------------------------ *\
|                                                                                              |
|                                                                                              |
|                                       BLOCO SAÍDAS                                           |
|                                                                                              |
|                                                                                              |
\* ------------------------------------------------------------------------------------------ */

// Reproduz um bibe no buzzer
void soar_um_bipe();

// Reproduz um bibe no buzzer
void soar_dois_bipes();

// ---------------------------------- FUNÇÃO DE DEBUG --------------------------------------- //

/* Mostrar os valores obtidos dos sensores

  'L' para mostrar os valores lidos pelos sensores de linha
  'V' para mostrar os valores lidos pelos sensores vl53l0x
  'C' para mostrar os valores lidos pelos sensores de cor
*/
void mostrar_valores_lidos(char opcao);

/* ------------------------------------------------------------------------------------------ *\
|                                                                                              |
|                                                                                              |
|                                       BLOCO SENSOR                                           |
|                                                                                              |
|                                                                                              |
\* ------------------------------------------------------------------------------------------ */

// Fazer leitura nos sensores de linha da frente
void fazer_leitura_nos_sensores_de_linha_principais();

void fazer_leitura_no_sensor_de_linha_traseiro();

void checar_sensores_de_cor();

byte fazer_leitura_no_sensor_de_toque_traseiro();

void fazer_leitura_nos_sensores_vl53l0x();

void fazer_leitura_no_sensor_vl53l0x_frontal_cima();

void fazer_leitura_no_sensor_vl53l0x_frontal_centro();

void fazer_leitura_no_sensor_vl53l0x_frontal_baixo();

void fazer_leitura_no_sensor_vl53l0x_direito();

void fazer_leitura_no_sensor_vl53l0x_esquerdo();

/* ------------------------------------------------------------------------------------------ *\
|                                                                                              |
|                                                                                              |
|                                     BLOCO SEGUIR LINHA                                       |
|                                                                                              |
|                                                                                              |
\* ------------------------------------------------------------------------------------------ */

// Seguir linha
void seguir_linha();

// Função de emergência para caso o robô se perca da linha
void retornar_para_a_linha(byte lado); 

/* ------------------------------------------------------------------------------------------ *\
|                                                                                              |
|                                                                                              |
|                                         BLOCO COR                                            |
|                                                                                              |
|                                                                                              |
\* ------------------------------------------------------------------------------------------ */

void executar_rotina_correspondente();

boolean passou_meio_segundo_desde_a_ultima_leitura_nos_sensores_de_cor ();

/* ------------------------------------------------------------------------------------------ *\
|                                                                                              |
|                                                                                              |
|                                      BLOCO OBSTÁCULO                                         |
|                                                                                              |
|                                                                                              |
\* ------------------------------------------------------------------------------------------ */

// Função que é chamada toda vez em que há uma interrupção
void tratar_interrupcoes();

void verificar_se_existe_obstaculo_a_frente();

void desviar_obstaculo();

// Função que faz o robô se alinhar após deviar de um obstáculo
void rotina_de_alinhamento_pos_obstaculo(byte direcao);

/* ------------------------------------------------------------------------------------------ *\
|                                                                                              |
|                                                                                              |
|                                        BLOCO RAMPA                                           |
|                                                                                              |
|                                                                                              |
\* ------------------------------------------------------------------------------------------ */

void procurar_a_rampa();

void modo_rampa();

boolean passou_3_segundos_desde_a_ultima_leitura_nos_sensores_laterais();

boolean passou_1_segundo_desde_a_ultima_leitura_nos_sensores_laterais();

/* ------------------------------------------------------------------------------------------ *\
|                                                                                              |
|                                                                                              |
|                          BLOCO RELACINADO AO RESGATE DAS VÍTIMAS                             |
|                                                                                              |
|                                                                                              |
\* ------------------------------------------------------------------------------------------ */

void modo_resgate();

void verificar_situacoes_e_mostrar_resultado();

// ------------------------------------------------------------------------------------------ //

#endif

// ------------------------------------------------------------------------------------------ //
