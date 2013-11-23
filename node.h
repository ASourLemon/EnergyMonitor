/* Ficheiro: node.h **/

/* Descrição:
   Contém as definições globais de um nó. Este ficheiro NECESSITA ser
  devidamente configurado face às características específicas de cada nó.
  Ver indicações abaixo.
*/

/* Autor: Renato Jorge Caleira Nunes */

/*  Historial:
12/04/08 - Criação deste módulo e versão inicial.
*/ /* ::hist */

/*  Notas:
:cfg - Assinala opções de configuração
ATT - Assinala uma particularidade a ter em atenção
!!  - Assinala uma instrução que não deve ser alterada
:err - Assinala um erro interno
cast - Assinala um cast
:dbg - Assinala código usado para debug
*/


#define _SIMUL_  0  /* ##:cfg  0=Simulacao INACTIVA; 1=Simulacao ACTIVA */


  /* constantes que identificam o tipo de placa; ver _NODE_BOARD_TYPE_ */
#define NODE_BOARD_CM       0  /* placa CM */
#define NODE_BOARD_CM2      1  /* placa CM2 */
#define NODE_BOARD_ARDUINO  2  /* placa Arduino 2009 */
#define NODE_BOARD_xxx      3  /* placa ... (new board...) */

#define _NODE_BOARD_TYPE_   2  /*##:cfg (ver acima) */


#define _NODE_USES_MSG_  1  /*##:cfg  0:não usa MSG  1: usa MSG */


#include "v_types.h"

#define disable_interrupts cli()
#define enable_interrupts  sei()

/*	ATT:	::05
   A identificação das aplicações começa em 0 e necessita ser sequencial;
  no máximo podem existir 8 aplicações (entre 0 e 7).
   A obrigatoriedade de a numeração ser sequencial visa poupar memória.
  (ver módulo prop.c).
*/
			/* ATT:  ver  NODE_BROADCAST_APP_INDEX  abaixo */
#define NODE_APP_SYS     0  /*:cfg*/ /* !! à partida SYS é sempre a app 0 */
#define NODE_APP_SWITCH  1  /*##:cfg*/
#define NODE_APP_RELAY   2

#define NODE_NUM_APPS    3  /*##:cfg (ver acima) ::00*/
		/* ATT: não pode ser superior 8 */  /* ver >>>net.c */




#ifdef _MAIN_  /* [ */
	
	/* ::global vars **/

#endif  /* ] */



#ifndef _MAIN_  /* [ */

	/* ::extern declarations **/


#endif  /* ] */







/* Fim do ficheiro node.h **/
/* Autor: Renato Jorge Caleira Nunes */

/*
  ::next -> ::08
*/
