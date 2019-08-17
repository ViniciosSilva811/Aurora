
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

// ------------------------------------------------------------------------------------------ //

void configurar_pinos();

void configurar_velocidade_inicial_dos_motores();

void inicializar_serial();

void mostrar_valores();

// Fazer leitura nos sensores de linha
void ler_sensores_de_linha();

// Seguir linha
void seguir_linha();

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

// Função de emergência para caso o robô se perca da linha
void retornar_para_a_linha(byte lado); 

// Parar o robô
void parar();

void mover_somente_motor_direito();
void mover_somente_motor_esquerdo();

// ------------------------------------------------------------------------------------------ //

#endif

// ------------------------------------------------------------------------------------------ //
