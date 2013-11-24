#ifndef _COMM_TASK_

extern uint8 COMM_timer;

void comm_init(void);
void comm_envia(unsigned int palavras);
char comm_recebe(void);
void comm_task(void);

#endif
