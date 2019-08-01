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

#ifndef _Stubborn_DCMotor_h
#define _Stubborn_DCMotor_h

// ------------------------------------------------------------------------------------- //

#include <Arduino.h>

// ------------------------------------------------------------------------------------- //

#define FORWARD  1
#define BACKWARD 2
#define BRAKE    3
#define RELEASE  4

// ------------------------------ Classe Stubborn_DCMotor ------------------------------ //

class Stubborn_DCMotor {
	public:
		Stubborn_DCMotor(uint8_t motornum);
		void setSpeed(uint8_t velocidade);
		void run(uint8_t sentido);
	private:
		uint8_t _motornum;
		uint8_t _velocidade;
		uint8_t _sentido;
		uint8_t a, b;
};

// ------------------------------------------------------------------------------------ //

#endif

// --------------------------------- FIM DO CÓDIGO ------------------------------------ //
