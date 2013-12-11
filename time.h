/* Author: Renato Jorge Caleira Nunes */

#ifndef _TIME_TASK_
#define _TIME_TASK_
/* Global variables */
static uint8 TIME_curr_time; /* incremented each 200 us interruption */
static uint8 TIME_last_time;

void TIME_init(void);
void TIME_task(void);

#if _SIMUL_  
int TIME_simul_update_time(uint8 time_value); 
#endif 

#endif


