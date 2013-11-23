/* Ficheiro: time.h **/

/* Autor: Renato Jorge Caleira Nunes */

/*  Historial:
25/08/2002 - Versão inicial.
25/08/2010 - Alterações cosméticas. Adaptação ao módulo de simulação.
*/ /* ::hist */


#ifndef _TIME_TASK_

void TIME_init(void);
void TIME_task(void);

#if _SIMUL_  /* [ */
int TIME_simul_update_time(uint8 time_value); /*:simul*/
#endif  /* _SIMUL_ */ /* ] */

#endif

/* Fim do ficheiro time.h **/
/* Autor: Renato Jorge Caleira Nunes */

