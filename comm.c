/* Global Includes */
#include "node.h"
#include "time.h"
#include "v_types.h"
#include <Arduino.h>

/* Global includes from the WinAVR compiler */
#if ! _SIMUL_
#include <avr/io.h>
#include <avr/interrupt.h>
#endif

/* Specific Include for this module */
#include "comm.h"  /*##:cfg*/

/* Comunication state constants definition */
#define COMM_STATE_IDLE 0
#define COMM_STATE_RX 1
#define COMM_STATE_TX 2
#define COMM_STATE_CONSUME 3

/* Domobus array size constant definition. The minimum accepted value is 2.*/
#define COMM_RX_BUF_MAX_SIZE 20  
#define COMM_TX_BUF_MAX_SIZE 20 

/* Variables definition*/
uint8 comm_state;
uint8 COMM_timer;
uint8 aux_char; // storage the length of the available data

/* Control variables definition */
uint8 comm_buffer_index;
uint8 comm_remaining_bytes; // storage the size of the remaining bytes to be transmitted

/* Domobus array declaration :
   - comm_rx_buffer : buffer to storage the receiving data.
   - comm_tx_buffer : buffer to storage the transmitting data.
*/
uint8 comm_rx_buffer[COMM_RX_BUF_MAX_SIZE]; 
uint8 comm_tx_buffer[COMM_TX_BUF_MAX_SIZE]; 

/* Domobus array control flags*/
uint8 comm_rx_flags;
uint8 comm_tx_flags;


/* comm_get_message(2) : Function responsible of delivering packets to modules
 * module_id : id of the module that is asking for the data
 * packet : buffer to receive data, if it is available
 */
int comm_get_message(int module_id, uint8* packet){
	if(comm_rx_flags && !comm_tx_flags) {
                if(comm_rx_buffer[1] == module_id) {
                  memcpy(packet, comm_rx_buffer, COMM_RX_BUF_MAX_SIZE);
                  comm_rx_flags = 0;
  		  return 1; 
                }
	}
	return 0;
}

int comm_send_message(uint8* packet){
	if(!comm_tx_flags) {
          comm_tx_flags = 1;
	  memcpy(comm_tx_buffer, packet, COMM_TX_BUF_MAX_SIZE);    
          aux_char = comm_tx_buffer[0]+1;
          return 1;
	}
	return 0;
}


/* comm_task() : Function responsible to manage the communication between the hardware and the serial port. 
   
   - This routine uses the following macros from AVR Lib :
   * UCSR0A : Control register from AVR 
   * UDR0 : Data Register
   */
void comm_task(){
  switch(comm_state)
    {	
    case COMM_STATE_IDLE:
      {	
	// Condition that verifies if there are data to receive
	if(COMM_timer > 200 && comm_rx_flags == 1){ 
		memset(comm_tx_buffer, 0, sizeof(comm_tx_buffer));

		break;
	}
	// Condition that verifies if there are data to be transmitted
	if(comm_tx_flags == 1){
	  comm_remaining_bytes = comm_tx_buffer[0]+1;  // increases the variable to match with the Domobus array specification
	  comm_buffer_index = 0;
	  comm_state = COMM_STATE_TX;
	  break;
	}
	/* Condition that verifies if there is data available :
	   - RXC0 (00x80) : bit to verify if the receive is complete
	*/
	if(UCSR0A & 0x80){ 
	  aux_char = UDR0;
	  aux_char -= '0'; // Receives the first byte
	  comm_remaining_bytes = aux_char;
	  if(comm_rx_flags == 1){ // Buffer is full
	    comm_state = COMM_STATE_CONSUME;
	    COMM_timer = 0;
	    break;
	  }
	  // Tests if the package fits in the buffer
	  if(aux_char >= COMM_RX_BUF_MAX_SIZE){
	    comm_state = COMM_STATE_CONSUME;
	    COMM_timer = 0;
	    break;
	  }
	  comm_rx_buffer[0] = aux_char; 
	  comm_buffer_index = 1;
	  comm_state = COMM_STATE_RX;
	  COMM_timer = 0;
	  break;
	}
	break;
      }
      
      /*  Case that the length of the data in UDR0 is larger then the Domobus array size */ 	
    case COMM_STATE_CONSUME:{
      /* Condition that verifies if there is data available :
	 - RXC0 (00x80) : bit to verify if the receive is complete
      */
      if(UCSR0A & 0x80){
	aux_char = UDR0;
	comm_remaining_bytes--;
	if(comm_remaining_bytes == 0){
	  comm_state = COMM_STATE_IDLE;
	  COMM_timer = 0;
	}
	break;
      }
      if(COMM_timer >= 100){ //1s
	comm_buffer_index = 0;
	comm_state = COMM_STATE_IDLE; 	
	COMM_timer = 0;
      }
      break;
    }
      /* Case for reading data from the serial port */
    case COMM_STATE_RX: {
      /* Condition that verifies if there is data available :
	 - RXC0 (00x80) : bit to verify if the receive is complete
      */
      if(UCSR0A & 0x80){
	comm_rx_buffer[comm_buffer_index] = UDR0;
	comm_buffer_index++;
	comm_remaining_bytes--;

	COMM_timer = 0;
	if(comm_remaining_bytes == 0){
	  comm_rx_flags = 1; // Buffer is full
	  comm_state = COMM_STATE_IDLE; 	
	}
	break;
      }
      if(COMM_timer >= 100){ // Verifies if after 1s passed, there no more data available
	comm_buffer_index = 0;
	comm_state = COMM_STATE_IDLE; 	
	COMM_timer = 0;
      }
      break;
    }
      /* Case for transmitting data from the serial port*/
    case COMM_STATE_TX: {
        /* Condition that verifies if there :
	 - UDRE0 (00x20) : bit to verify if the data register is empty
      */
      if(UCSR0A & 0x20 && comm_remaining_bytes > 0){ 
	if(comm_buffer_index == 0){ 
	  UDR0 = comm_tx_buffer[0] + '0';
	  comm_buffer_index++;
	  comm_remaining_bytes--;

	} else {
	  // Insert the data in the UDR0
	  UDR0 = comm_tx_buffer[comm_buffer_index];
	  comm_buffer_index++;
	  comm_remaining_bytes--;
	  if(comm_remaining_bytes == 0){
	    comm_tx_flags = 0;
	    comm_state = COMM_STATE_IDLE;
	    COMM_timer = 0;
	  }
	}
      }
      break;
    }
    default: break;
    }	
}

/* comm_init(): Function responsible to start the communication module.
   
   - This routine uses the following macros from AVR Lib.
   * UCSR0B : Control register from AVR
   * UCSR0C : Control register from AVR
   * UBRR0H : Baud Rate Register High
   * UBRR0L : Baud Rate Register Low
 */
void comm_init()
{ 
  /* RXCIE0 & TXCIE0 : set to allow receive and transmission complete interruptions. */
  UCSR0B = 0x18; 
  /* UCSZ01 & UCSZ00 : character size 1 and 0. Used to set the bit size, for this project the size is 8-bit. */
  UCSR0C = 0x06;
  UBRR0H = 0x00; 
  /* Set the baud rate to 9600*/
  UBRR0L = 0x67; 

  COMM_timer = 0;
  comm_buffer_index = 0;
  comm_rx_flags = 0;

  comm_state = COMM_STATE_IDLE;
  aux_char = 'c';
}

/* comm_send(unsigned int words) : Function that is responsible to insert in the UDR0 the data to be transmitted. */
void comm_send(unsigned int words)
{
  UDR0 = words;
}
/* comm_receive() : Function that is responsible to store the receiving data in a buffer. */
char comm_receive()
{
  unsigned int buffer;
  buffer = UDR0;
  return buffer;
}

#if 0
::x0::x1::x2::x3::x4::x5::x6::x7::x8::x9
::05::07::13::14::15::16::17::18::19
#endif
