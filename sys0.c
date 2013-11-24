/* Author: Renato Jorge Caleira Nunes */

/* Description :
  This module exists in all nodes of the system. In this moment is responsible to manage the LED 'alive' state, 
  like control the number of the pulses and the time interval between two consecutive pulses.
*/

/* Global Includes */
#include "node.h"

#if _SIMUL_
#include "simul.h"
#endif


/* Specific includes for the WinAVR compiler */
#if ! _SIMUL_
#include <avr/io.h>
#include <avr/interrupt.h>
#endif


/* Specific includes for this module */
#define _SYS0_TASK_
#include "sys0.h" 

/* Configuration for CM board */
#if (_NODE_BOARD_TYPE_ == NODE_BOARD_CM)  

#define SYS0_LED_PORT  PORTD  
#define SYS0_LED_DDR    DDRD 
#define SYS0_LED_PIN_MASK  0x40
#define SYS0_WD_PORT  PORTD 
#define SYS0_WD_DDR    DDRD 
#define SYS0_WD_PIN_MASK  0x10 
#define SYS0_WD_TOGGLE    SYS0_WD_PORT ^= SYS0_WD_PIN_MASK

#endif 

/* Configuration for CM2 board */
#if (_NODE_BOARD_TYPE_ == NODE_BOARD_CM2)  

#define SYS0_LED_PORT  PORTD  
#define SYS0_LED_DDR    DDRD 
#define SYS0_LED_PIN_MASK  0x10  

#endif 

/* Configuration for ATMega328 board */
#if (_NODE_BOARD_TYPE_ == NODE_BOARD_ARDUINO)  
/* In ARDUINO board the LED 'alive' is in the PB5 (Port B and Pin 5) */
#define SYS0_LED_PORT  PORTB  /* Port B in Arduino board */
#define SYS0_LED_DDR    DDRB  /* Data Direction Register B */
#define SYS0_LED_PIN_MASK  0x20  /* PIN5 : 0010 0000 */

#endif 

/* Configuration for boards CM and CM2 */
#if (_NODE_BOARD_TYPE_ == NODE_BOARD_CM || _NODE_BOARD_TYPE_ == NODE_BOARD_CM2) 

#define SYS0_LED_TURN_ON    SYS0_LED_PORT &= ~SYS0_LED_PIN_MASK
#define SYS0_LED_TURN_OFF   SYS0_LED_PORT |= SYS0_LED_PIN_MASK
#define SYS0_LED_TOGGLE     SYS0_LED_PORT ^= SYS0_LED_PIN_MASK

#endif  

/* Configuration for ATMega328 board */
#if (_NODE_BOARD_TYPE_ == NODE_BOARD_ARDUINO) 
/* LED "alive" is activate in HIGH */
#define SYS0_LED_TURN_ON    SYS0_LED_PORT |= SYS0_LED_PIN_MASK
#define SYS0_LED_TURN_OFF   SYS0_LED_PORT &= ~SYS0_LED_PIN_MASK
#define SYS0_LED_TOGGLE     SYS0_LED_PORT ^= SYS0_LED_PIN_MASK

#endif 

/* Defintion of LED pulse and pause time */
#define SYS0_LED_PULSE_TIME   20  /* 200 ms */
#define SYS0_LED_PAUSE_TIME   50  /* 500 ms */

/* Global variables */
uint8 SYS0_led_timer;  /* 10 ms */

/* Local Variables */
static uint8 SYS0_led_num_toggles, SYS0_led_toggle_n; 



/* SYS0_led_num_pulses(uint8 num_pulses) : Function that is responsible to define
   the number of pulses to be emitted by the LED 'alive'. */  
void SYS0_led_num_pulses(uint8 num_pulses)
{
  /* The following expression is equals to : SYS0_led_num_toggles = (num_pulses * 2) - 1 */
  SYS0_led_num_toggles = (num_pulses << 1) - 1; // shift left is applied to num_pulses
}

/* SYS0_init() : Function that is responsible to initialize the SYS0 task.*/
void SYS0_init()
{
  /* Initialization for the CM and CM2 board */
#if (_NODE_BOARD_TYPE_ == NODE_BOARD_CM || _NODE_BOARD_TYPE_ == NODE_BOARD_CM2) 
  SYS0_LED_TURN_OFF;
  SYS0_LED_DDR |= SYS0_LED_PIN_MASK;
#endif 
  /* Initialization for the CM board */
#if (_NODE_BOARD_TYPE_ == NODE_BOARD_CM) 
  SYS0_WD_PORT |= SYS0_WD_PIN_MASK; 
  SYS0_WD_DDR |= SYS0_WD_PIN_MASK;
#endif 


#if (_NODE_BOARD_TYPE_ == NODE_BOARD_ARDUINO)
  /* The LED 'alive' is already initialized by the avr_init() rountine.
   For further details see main.c */
#endif 

  /* Set the number of pulses to 1 */
  SYS0_led_num_pulses(1);
 
}

/* SYS0_task() : Function responsible to control the LED 'alive' */
void SYS0_task(void)
{
  /* Pulses the LED and actuate in the watchdog (the last only for the CM and CM2 boards) */
  if(SYS0_led_toggle_n == 0)
    {
      if(SYS0_led_timer >= SYS0_LED_PAUSE_TIME)
	{
	  /* Invert the actual LED state */
	  SYS0_LED_TOGGLE;
	  SYS0_led_timer = 0;
	  SYS0_led_toggle_n = SYS0_led_num_toggles;

#if (_NODE_BOARD_TYPE_ == NODE_BOARD_CM)
	  SYS0_WD_TOGGLE;
#endif
	}
    }
  else
    {
      if(SYS0_led_timer >= SYS0_LED_PULSE_TIME)
	{
	  SYS0_LED_TOGGLE;
	  SYS0_led_timer = 0;
	  --SYS0_led_toggle_n;

#if (_NODE_BOARD_TYPE_ == NODE_BOARD_CM)
	  SYS0_WD_TOGGLE;
#endif
	}
    }
}

#if 0
::x0::x1::x2::x3::x4::x5::x6::x7::x8::x9
::05::07::13::14::15::16::17::18::19
#endif
