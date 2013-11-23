/* Ficheiro: sys0.c **/

/* Descri��o:
   Aplica��o de sistema que existe em todos os n�s e � respons�vel por diversas
  fun��es de gest�o como, por exemplo, comando do LED "alive", pulsar do
  watchdog, receber msgs de broadcast, obter endere�o para o n� (caso ainda
  n�o lhe tenha sido atribu�do um), monitorizar erros ocorridos nas outras
  aplica��es, etc.
*/

/* Autor: Renato Jorge Caleira Nunes */


/*  Historial:
03/11/2011 - Cria��o deste m�dulo. Vers�o b�sica do m�dulo SYS original em
 que apenas pisca o LED e controla o watchdog.
*/ /* ::hist */


/*  Notas:
:cfg - Assinala op��es de configura��o
:avr - Assinala particularidades ligadas ao tipo de microcontrolador usado
:init - Assinala particularidades ligadas � inicializa��o de vari�veis
ATT - Assinala uma particularidade a ter em aten��o
!!  - Assinala uma instru��o que n�o deve ser alterada
:err - Assinala um erro interno
cast - Assinala um cast
:dbg - Assinala c�digo usado para debug
*/


/* includes globais */

#include "node.h"

#if _SIMUL_
 #include "simul.h"
#endif


/* includes globais do compilador WinAVR */

#if ! _SIMUL_
 #include <avr/io.h>
 #include <avr/interrupt.h>
 /* #include <avr/wdt.h>  /* acesso ao watchdog interno para for�ar reset */
#endif


/* includes espec�ficos deste m�dulo */

#define _SYS0_TASK_

#include "sys0.h"  /*##:cfg*/


	/* ::define **/

#if (_NODE_BOARD_TYPE_ == NODE_BOARD_CM)  /* [ */

  /* placa CM: LED "alive" = PD6 */
#define SYS0_LED_PORT  PORTD  /* porto D (placas CM) */
#define SYS0_LED_DDR    DDRD  /* Data Direction Regist.(DDR) D */

#define SYS0_LED_PIN_MASK  0x40  /* PIN6 : 0100 0000 */

  /* placa CM: watchdog = PD4  (::00) */
#define SYS0_WD_PORT  PORTD  /* porto D (placas CM) */
#define SYS0_WD_DDR    DDRD  /* Data Direction Regist.(DDR) D */

#define SYS0_WD_PIN_MASK  0x10  /* PIN4 : 0001 0000 */

#define SYS0_WD_TOGGLE    SYS0_WD_PORT ^= SYS0_WD_PIN_MASK

#endif  /* ] */


#if (_NODE_BOARD_TYPE_ == NODE_BOARD_CM2)  /* [ */

  /* placa CM2: LED "alive" = watchdog = PD4 */
#define SYS0_LED_PORT  PORTD  /* porto D (placas CM2) */
#define SYS0_LED_DDR    DDRD  /* Data Direction Regist.(DDR) D */

#define SYS0_LED_PIN_MASK  0x10  /* PIN4 : 0001 0000 */

#endif  /* ] */


#if (_NODE_BOARD_TYPE_ == NODE_BOARD_ARDUINO)  /* [ */

  /* placa ARDUINO: LED "alive" = PB5 */
#define SYS0_LED_PORT  PORTB  /* porto B (Arduino) */
#define SYS0_LED_DDR    DDRB  /* Data Direction Regist.(DDR) B */

#define SYS0_LED_PIN_MASK  0x20  /* PIN5 : 0010 0000 */

  /* n�o existe watchdog externo no Arduino */

#endif  /* ] */



#if (_NODE_BOARD_TYPE_ == NODE_BOARD_CM || _NODE_BOARD_TYPE_ == NODE_BOARD_CM2)  /* [ */

	/* LED "alive" � activo a LOW (placas CM e CM2) */

#define SYS0_LED_TURN_ON    SYS0_LED_PORT &= ~SYS0_LED_PIN_MASK
#define SYS0_LED_TURN_OFF   SYS0_LED_PORT |= SYS0_LED_PIN_MASK

#define SYS0_LED_TOGGLE     SYS0_LED_PORT ^= SYS0_LED_PIN_MASK

#endif  /* ] */


#if (_NODE_BOARD_TYPE_ == NODE_BOARD_ARDUINO)  /* [ */

	/* LED "alive" � activo a HIGH (placa ARDUINO) */

#define SYS0_LED_TURN_ON    SYS0_LED_PORT |= SYS0_LED_PIN_MASK
#define SYS0_LED_TURN_OFF   SYS0_LED_PORT &= ~SYS0_LED_PIN_MASK

#define SYS0_LED_TOGGLE     SYS0_LED_PORT ^= SYS0_LED_PIN_MASK

#endif  /* ] */



#define SYS0_LED_PULSE_TIME   20  /* 200 ms */ /*##:cfg*/ /*::01*/
#define SYS0_LED_PAUSE_TIME   50  /* 500 ms */
  /* NOTA: Lembrar que o per�odo m�ximo do watchdog nas placas CM e CM2 ronda
    os 1.2 segundos. No caso da placa Arduino n�o existe watchdog externo. */



/* Vari�veis globais ::vars **/

uint8 SYS0_led_timer;  /* 10 ms */



/* Vari�veis locais **/

static uint8 SYS0_led_num_toggles, SYS0_led_toggle_n; /* ver >>:06 */




/* Atribui o valor adequado a SYS0_led_num_toggles que define o n�mero de
  impulsos que o LED "alive" ir� emitir.
   SYS0_led_num_toggles = ("num_pulses" * 2) - 1.
+------------------------------------------------------------------------*/
void /**/SYS0_led_num_pulses(uint8 num_pulses)
{
  SYS0_led_num_toggles = (num_pulses << 1) - 1;
}



/* Inicializa��o da tarefa SYS0.
+------------------------------------------------------------------------*/
void /**/SYS0_init(void)
{

  /* led init + external watchdog init */

#if (_NODE_BOARD_TYPE_ == NODE_BOARD_CM || _NODE_BOARD_TYPE_ == NODE_BOARD_CM2)  /* [ */
  /* LED "alive" */
  SYS0_LED_TURN_OFF; /* port pin = 1 (activa pull-up) */
  SYS0_LED_DDR |= SYS0_LED_PIN_MASK; /* set port pin as output */
#endif  /* ] */

#if (_NODE_BOARD_TYPE_ == NODE_BOARD_CM)  /* [ */
  /* Na placa CM o pino de controlo do watchdog � diferente do LED "alive" */
  SYS0_WD_PORT |= SYS0_WD_PIN_MASK; /* port pin = 1 (activa pull-up) */
  SYS0_WD_DDR |= SYS0_WD_PIN_MASK; /* set port pin as output */
#endif  /* ] */


#if (_NODE_BOARD_TYPE_ == NODE_BOARD_ARDUINO)  /* [ */
  /* Arduino: o pino do LED "alive" j� foi inicializado; Ver avr_init() em
    MAIN.C */
#endif  /* ] */


  /*SYS0_led_timer = 0; /* Desnecess�rio */ /*:init*/
  SYS0_led_num_pulses(1);
  /*SYS0_led_toggle_n = 0; /* Desnecess�rio */ /*:init*/

  /*SYS0_led_timer = 0; /* Desnecess�rio */ /*:init*/
}



/* Tarefa SYS0. Apenas controla o led "alive".
+------------------------------------------------------------------------*/
void /**/SYS0_task(void)
{

  /* Pulsa LED "alive" e actua watchdog */

/* NOTA 1: Na placa CM2 o pulsar o LED tamb�m actua o watchdog; na placa CM
  o watchdog � controlado por um pino distinto (ver >>:00). Na placa Arduino
  n�o existe watchdog.
   O LED pode emitir uma s�rie de impulsos separados por uma pausa. Os valores
  dos tempos envolvidos est�o definidos em >>:01. O n�mero de impulsos depende
  do valor de SYS0_led_num_toggles. Para N impulsos esta vari�vel deve conter o
  valor 2*N-1. Ver fun��o SYS0_led_num_pulses().
   NOTA 2: N�o esquecer que o per�odo m�ximo do watchdog ronda os 1.2s (placas
  CM e CM2).
*/

  if(SYS0_led_toggle_n == 0) /*::06*/
  {
    if(SYS0_led_timer >= SYS0_LED_PAUSE_TIME)
    {
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



/* Fim do ficheiro sys0.c **/
/* Autor: Renato Jorge Caleira Nunes */

#if 0
::x0::x1::x2::x3::x4::x5::x6::x7::x8::x9
::05::07::13::14::15::16::17::18::19
#endif
