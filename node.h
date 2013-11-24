/* Author: Renato Jorge Caleira Nunes */

/* Description :
   This file contains de global configuration of a node. It's mandatory
   configure this file according the specifics characteristics of each node,
   in that case the node is the board used.
*/

/* Configure the simulation mode :
   - Simulation Inactive : _SIMUL_ = 0
   - Simulation Active : _SIMUL_ = 1
 */
#define _SIMUL_  0  

/* Constants that identify the type of the board */
#define NODE_BOARD_CM       0  /* CM board */
#define NODE_BOARD_CM2      1  /* CM2 board */
#define NODE_BOARD_ARDUINO  2  /* ATMega328 board */
#define NODE_BOARD_xxx      3  /* slot for a new board */

#define _NODE_BOARD_TYPE_   2  /* Atributes the board type to ATMega328 */

/* Configure the use of messages by the nodes :
   - Messages aren't used : _NODE_USES_MSG = 0 
   - Messages are used : _NODE_USES_MSG = 1
*/
#define _NODE_USES_MSG_  1


#include "v_types.h"

#define disable_interrupts cli()
#define enable_interrupts  sei()

/* This section defines the identification of the applications.
   The identification starts in 0 e needs to be sequential.
   At maximum 8 applications can exists in silmultaneous,
   numbered from 0 to 7.
*/
			
#define NODE_APP_SYS     0 
#define NODE_APP_SWITCH  1  
#define NODE_APP_RELAY   2
#define NODE_NUM_APPS    3  // MAX_VALUE = 8


#ifdef _MAIN_  /* [ */
	
	/* ::global vars **/

#endif  /* ] */



#ifndef _MAIN_  /* [ */

	/* ::extern declarations **/


#endif  /* ] */


