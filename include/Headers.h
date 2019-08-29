
/* ------------------------------------------------------------------------------------------ *\
|                                                                                              |
|                                                                                              |
|                                           Headers.h                                          |
|                        Aqui são declarados todos os cabeçalhos das funções                   |
|                                que serão usadas no código principal                          |
|                                                                                              |
|                                                                                              |
\* ------------------------------------------------------------------------------------------ */

// ------------------------------------------------------------------------------------------ //

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

// ------------------------------------------------------------------------------------------ //

// Define o modo de eperação de todos os pinos
void configurar_pinos();

// Configura a velocidade inicial dos motores
void configurar_velocidade_inicial_dos_motores();

// Inicia a comunicação com o Monitor Serial
void inicializar_monitor_serial();

// Inicia os sensores de cor
void inicializar_sensores_de_cor();

boolean passou_um_segundo_desde_a_ultima_leitura_nos_sensores_de_cor();

void inicializar_sensor_de_obstaculo();

// Mostrar os valores obtidos dos sensores
void mostrar_valores(char opcao);

// Fazer leitura nos sensores de linha
void fazer_leitura_nos_sensores_de_linha();

void checar_sensores_de_cor();

void verificar_se_existe_obstaculo_a_frente();
 
void executar_rotina_correspondente();

// Seguir linha
void seguir_linha();

// Função que é chamada toda vez que tem uma interrupção
void tratar_interrupcoes();


void desviar_obstaculo();

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

// Função de emergência para caso o robô se perca da linha
void retornar_para_a_linha(byte lado); 

// Parar o robô
void parar();

void mover_somente_motor_direito();
void mover_somente_motor_esquerdo();

// ------------------------------------------------------------------------------------------ //

#endif

// ------------------------------------------------------------------------------------------ //
