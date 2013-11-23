// unsigned char buffer[20];
//int n_numeros_para_transmitir;

#ifndef _COMM_TASK_
extern uint8 COMM_timer;

void comm_init(void);
void comm_envia(unsigned int palavras /*, unsigned char quantos*/);
char comm_recebe(void);
void comm_task(void);

#endif
