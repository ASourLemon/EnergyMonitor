/* Author: Renato Jorge Caleira Nunes */

#ifndef _TIME_TASK_

void TIME_init(void);
void TIME_task(void);

#if _SIMUL_  
int TIME_simul_update_time(uint8 time_value); 
#endif 

#endif


