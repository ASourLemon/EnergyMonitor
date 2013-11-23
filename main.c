/**/
/* Ficheiro: main.c **/
/* includes globais */
#define _MAIN_  /* usado em node.h */

#include "node.h"

#if _SIMUL_
 #include "simul.h"
#endif


/* includes globais do compilador WinAVR */

#if ! _SIMUL_
 #include <avr/io.h>
 #include <avr/interrupt.h>
#endif


/* includes espec�ficos deste m�dulo */

#include "time.h"
#include "sys0.h"
#include "comm.h"

/* Inicializa o microcontrolador AVR.
-------------------------------------------------------------------------*/
void /**/avr_init(void)
{

#if (_NODE_BOARD_TYPE_ == NODE_BOARD_CM || _NODE_BOARD_TYPE_ == NODE_BOARD_CM2)
/*
  Ap�s reset todos os portos est�o inicializados como inputs (DDRx = 0x00).
  Activa pull-ups dos portos (para evitar ru�do el�ctrico...)
*/
  PORTA = 0xFF;
  PORTB = 0xFF;
  PORTC = 0xFF;
  PORTD = 0x7F; /* ATT - o pin7 possui uma resist�ncia � massa */


#elif (_NODE_BOARD_TYPE_ == NODE_BOARD_ARDUINO)
  DDRB = 0x20; /* pino 5 liga ao LED do Arduino => output*/
  DDRC = 0x00; /* all inputs */
  DDRD = 0x00; /* all inputs */

  /* ATT:  PORTA  n�o existe no processador do Arduino (ATmega328P) */
  PORTB = 0xDF; /* 1101 1111 - pino (output) do led = 0; Ver >>>sys0.c */
  PORTC = 0xFF; /* pull-ups todos activos */
  PORTD = 0xFF; /* pull-ups todos activos */

#endif

#if _SIMUL_  /* [ */
  printf("MAIN: avr_init(): Done!\n");
#endif  /* _SIMUL_ */ /* ] */
}



/* Ciclo de execu��o principal. Inicializa os v�rios m�dulos/tarefas e
  aplica��es e em seguida fica em ciclo chamando sequencialmente
  (round-Robin) as diferentes tarefas e aplica��es.
-------------------------------------------------------------------------*/
int /**/main(void)
{
	#if _SIMUL_
	SIMUL_init();
	#endif

	avr_init();
	TIME_init();
	enable_interrupts; /* Activa todas as interrup��es (node.h) */
	  
//	char dadosquerecebe;
	SYS0_init();
	SYS0_led_num_pulses(3);
 	comm_init();

	while(1)
	{
		TIME_task();
		comm_task();
		SYS0_task();
//		comm_envia(125, '1');
//  		dadosquerecebe = comm_recebe();
//  		comm_envia(dadosquerecebe, '1');
	}
}
