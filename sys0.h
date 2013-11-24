/* Author: Renato Jorge Caleira Nunes */

#define SYS0_TRUE   1
#define SYS0_FALSE  0


#ifndef _SYS0_TASK_

extern uint8 SYS0_led_timer;  /* 10 ms */

/* public functions */
void SYS0_init(void);
void SYS0_task();
void SYS0_led_num_pulses(uint8 num_pulses);

#endif

#if 0
::00::01::02::03::04::05::06::07::08::09
#endif
