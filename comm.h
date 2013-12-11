#ifndef _COMM_TASK_

extern uint8 COMM_timer;

void comm_init(void);
void comm_envia(unsigned int palavras);
char comm_recebe(void);
void comm_task(void);

int comm_get_message(int module_id, uint8* packet);
int comm_send_message(uint8* packet);

#endif
