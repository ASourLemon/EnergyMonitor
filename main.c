#define _MAIN_  

/* Global Includes */
#include "node.h"

#if _SIMUL_
 #include "simul.h"
#endif

/* Global includes from the WinAVR compiler */
#if ! _SIMUL_
 #include <avr/io.h>
 #include <avr/interrupt.h>
#endif

/* Specific Include for this module */
#include "time.h"
#include "sys0.h"
#include "comm.h"

/* avr_init() : Function responsible to setup the AVR microcontroller.*/
void avr_init(void)
{

#if (_NODE_BOARD_TYPE_ == NODE_BOARD_CM || _NODE_BOARD_TYPE_ == NODE_BOARD_CM2)
  PORTA = 0xFF;
  PORTB = 0xFF;
  PORTC = 0xFF;
  PORTD = 0x7F;

  /* Configuration for ATMega328
   - The following macros from AVR Lib are used in this configuration :
   * DDRB : The PortB Data Direction Register
   * DDRC : The PortC Data Direction Register
   * DDRD : The PortD Data Direction Register 
   */ 
#elif (_NODE_BOARD_TYPE_ == NODE_BOARD_ARDUINO)
  DDRB = 0x20; /* pin 5 of port B is a input port */
  DDRC = 0x00; /* all inputs */
  DDRD = 0x00; /* all inputs */

  PORTB = 0xDF; /* 0000 0000 1101 1111 - write data in all ports, except in pin 5*/
  PORTC = 0xFF; /* pull-ups : all actives */
  PORTD = 0xFF; /* pull-ups : all actives */
#endif

#if _SIMUL_ 
  printf("MAIN: avr_init(): Done!\n");
#endif 
}

/* main() : Function that executes the main routine. Initialize the several
   modules and applications. After that enters in a Round-Robin sequential
   cycle that executes the different tasks and applications. */
 */
int main()
{
	#if _SIMUL_
	SIMUL_init();
	#endif

	avr_init();
	TIME_init();
	enable_interrupts; 

	SYS0_init();
	SYS0_led_num_pulses(3);
 	comm_init();

	while(1)
	{
		TIME_task();
		comm_task();
		SYS0_task();
	}
}
