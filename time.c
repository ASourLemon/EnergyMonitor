/* Author: Renato Jorge Caleira Nunes */

/* Description:
   This module contains the functions that allow us to control the time that passed.
   This control is made according a 200us interruption ( AVR standard ).
   It's possible control intervals in the range of 200us, 10ms, 1s, etc.
*/

/* Globais includes */
#include "node.h"

#if _SIMUL_
 #include "simul.h"
#endif

/* Specifics includes for the WinAVR compiler */
#if ! _SIMUL_
 #include <avr/io.h>
 #include <avr/interrupt.h>
#endif

/* Specifics includes for this module */
#define _TIME_TASK_
#include "time.h"

/* Includes the modules that uses 'timers' */
#include "sys0.h"
#include "comm.h"

/* Global variables */
static uint8 TIME_curr_time; /* incremented each 200 us interruption */
static uint8 TIME_last_time;

/* TIME_init() : Function that is responsible to iniatialize the time task. */
void TIME_init() 
{
#if ! _SIMUL_  
  /* Specification for the CM and CM2 boards */
#if (_NODE_BOARD_TYPE_ == NODE_BOARD_CM || _NODE_BOARD_TYPE_ == NODE_BOARD_CM2)

  TCCR0 = 0X02; 
  TCNT0 = 56; 
  TIMSK |= 0x02;

#endif 

  /* Specification for Arduino ATMega328p */
#if (_NODE_BOARD_TYPE_ == NODE_BOARD_ARDUINO)
  /* - TCCR0B : Timer/Counter Control Register 
     - 0000 0000 0000 0011 : clock select bit description [ CLK_i/o / 64 => 16Mhz/64 = 250Khz = 4us ]  
  */ 
  TCCR0B |= 0X03; 
  /* - OCR0A : Output Compare Register */
  OCR0A = 50; /* 50 units of 4us = 200us */
  /* - TIMSK0 : Timer/Counter Interrupt Mask Register 
     - 0000 0000 0000 0010 :  Timer/Counter0 Output Compare Match A Interrupt Enable
  */
  TIMSK0 |= 0x02; 

#endif
#endif 

#if _SIMUL_ 
  printf("TIME_init(): TIME_curr_time=%u\n",TIME_curr_time);
  printf("TIME_init(): Done!\n");
#endif  
}

#if ! _SIMUL_

/* Configuration for CM an CM2 board */
#if (_NODE_BOARD_TYPE_ == NODE_BOARD_CM || _NODE_BOARD_TYPE_ == NODE_BOARD_CM2)
ISR(TIMER0_OVF_vect) 
{
  TCNT0 += 56;
  ++TIME_curr_time;
}
#endif

/* Configuration for ATMega328 board*/
#if (_NODE_BOARD_TYPE_ == NODE_BOARD_ARDUINO)
/* Interruption Handler function for Timer 0 Compare Match A.
   This routine is called each 200us. */
ISR(TIMER0_COMPA_vect) 
{
  OCR0A += 50; // Add's 200us to Output Compare Register A 
  ++TIME_curr_time;
}
#endif 
#endif 

#if _SIMUL_  /* [ */

/* TIME_simul_update_time(uint8 time_value) : Function responsible to mark the time passed
   in simulation mode. The variable time_value represents N units of 200us. */
int TIME_simul_update_time(uint8 time_value)
{
  TIME_curr_time += time_value;
  return TIME_curr_time;
}
#endif 

/* Constants definition zone */
#define TIME_1MS_N200US    5  /* 1 ms (5*200us) */
#define TIME_10MS_N200US  50  /* 10 ms (50*200us) */
#define TIME_50MS_N10MS    5  /* 50 ms (5*10ms) */
#define TIME_100MS_N10MS  10  /* 100 ms (10*10ms) */
#define TIME_1S_N10MS    100  /* 1 s (100*10ms) */
#define TIME_1M_N1S       60  /* 1 min (60*1s) */

/* TIME_task() : Function that is responsible to actualize the variables of the
   time tasks, the timers. The timers are updated in task time and not in 
   interruption time. That task can't updated the timers in parts of code that are
   in loop, like in a situtation of active wait. In that case the variable must be 
   updated directly in the interrupt handler routine.
   The base unity to mark the time is 200us. It's possible mark times in a range of
   1ms, 10ms, 50ms, 100ms, 1s and 1min. */
void TIME_task()
{
  /* N * 200us */
  uint8 elapsed_time; 
  /* Variables used to configure the time update*/
  static uint8 t10ms_n200us = 0;
  static uint8 t100ms_n10ms = 0;

#if 0 
  /* To correctly configure the variables according the time units desired,
     the propper variables need be copied to the above sections .
  */
  static uint8 t1ms_n200us = 0;
  static uint8 t10ms_n200us = 0;  <--
  static uint8 t50ms_n10ms = 0;
  static uint8 t100ms_n10ms = 0;  <--
  static uint8 t1s_n10ms = 0;
  static uint8 t1m_n1s = 0;
#endif
  
  /* Disable Interrupts */
  disable_interrupts; 

  elapsed_time = TIME_curr_time - TIME_last_time;
  TIME_last_time = TIME_curr_time;

  /* Enable Interrupts*/ 
  enable_interrupts; 

  /* Configuration for Internet timer */
#if 0
  NET_timer += elapsed_time; 
#endif 

  /*Configuration for 1ms timer*/
#if 0  
  t1ms_n200us += elapsed_time;
  if(t1ms_n200us >= TIME_1MS_N200US)
    t1ms_n200us -= TIME_1MS_N200US;
#endif
  
  /* Configuration for 10ms timer */
#if 1 
  t10ms_n200us += elapsed_time;
  if(t10ms_n200us >= TIME_10MS_N200US)
  {
    t10ms_n200us -= TIME_10MS_N200US; 
    ++SYS0_led_timer;
    ++COMM_timer;
#if 0 
    ++SWITCH_read_timer;
    ++RELAY_timer;
#endif

    /* Configuration for 50ms timer */ 
#if 0  
    ++t50ms_n10ms;
    if(t50ms_n10ms >= TIME_50MS_N10MS)
      t50ms_n10ms = 0;
#endif 
    
    /* Configuration for 100ms timer */
#if 0 
    ++t100ms_n10ms;
    if(t100ms_n10ms >= TIME_100MS_N10MS)
      {
	t100ms_n10ms = 0;
#if 0 
	++MSG_curr_time;
	++MSG_timer; 
	++SYS_msg_timer;
	++SWITCH_msg_timer;
	++RELAY_msg_timer;
#endif
      }
#endif 
    
#if 0 
    ++t1s_n10ms;
    if(t1s_n10ms >= TIME_1S_N10MS)
      {
	t1s_n10ms = 0;
#if 0
	++t1m_n1s;
	if(t1m_n1s >= TIME_1M_N1S)
	  t1m_n1s = 0;
#endif 
      }
#endif
  }
#endif 
}

#if 0
::07::10::11::12::13::14::15::16::17::18::19
#endif
