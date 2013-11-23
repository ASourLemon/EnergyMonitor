/* includes globais */

#include "node.h"
#include "time.h"
#include "v_types.h"
/* includes globais do compilador WinAVR */

#if ! _SIMUL_
#include <avr/io.h>
#include <avr/interrupt.h>
#endif


/* includes espec�ficos deste m�dulo */


#define COMM_STATE_IDLE 0
#define COMM_STATE_RX 1
#define COMM_STATE_TX 2
#define COMM_STATE_CONSUME 3

#define COMM_RX_BUF_MAX_SIZE 20  // não pode ser inferior a 2
#define COMM_TX_BUF_MAX_SIZE 20  // não pode ser inferior a 2 

#include "comm.h"  /*##:cfg*/

uint8 comm_state;
uint8 COMM_timer;
uint8 aux_char;

uint8 comm_buffer_index;
uint8 comm_remaining_bytes;

uint8 comm_rx_buffer[COMM_RX_BUF_MAX_SIZE]; 
uint8 comm_rx_flags;
uint8 comm_tx_buffer[COMM_TX_BUF_MAX_SIZE]; 
uint8 comm_tx_flags;

void comm_task(){
  switch(comm_state)
    {	
    case COMM_STATE_IDLE:
      {	
      /*
	if(COMM_timer < 100)	
	break;
	COMM_timer = 0;
	UDR0 = aux_char;		
      */
	if(COMM_timer > 200 && comm_rx_flags == 1){ // martelada de teste
	  aux_char = comm_rx_buffer[0]+1; //ATT
	  comm_buffer_index = 0;
	  for( ; aux_char > 0; aux_char--){
	    comm_tx_buffer[comm_buffer_index] = comm_rx_buffer[comm_buffer_index];
	    comm_buffer_index++;
	  }
	comm_rx_flags = 0;
	comm_tx_flags = 1;
	break;
	}
	
	if(comm_tx_flags == 1){
	  comm_remaining_bytes = comm_tx_buffer[0]+1; //ATT
	comm_buffer_index = 0;
	comm_state = COMM_STATE_TX;
	break;
      }

				
      if(UCSR0A & 0x80){ //recebeu primeiro caractere do pacote
	aux_char = UDR0;
	aux_char -= '0'; //## BUM!!!!
	comm_remaining_bytes = aux_char;
	if(comm_rx_flags == 1){ //buffer cheio
	  comm_state = COMM_STATE_CONSUME;
	  COMM_timer = 0;
	  break;
	}
	//testar se pacote cabe no buffer silencio em caso de nao dar
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
		
    case COMM_STATE_CONSUME:{
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
      
    case COMM_STATE_RX: { 
      if(UCSR0A & 0x80){ //recebeu caracter
	comm_rx_buffer[comm_buffer_index] = UDR0;
	comm_buffer_index++;
	comm_remaining_bytes--;

	COMM_timer = 0;
	if(comm_remaining_bytes == 0){
	  comm_rx_flags = 1; //buffer cheio
	  comm_state = COMM_STATE_IDLE; 	
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

    case COMM_STATE_TX: {
      if(UCSR0A & 0x20 && comm_remaining_bytes > 0){ // UDR livre e bytes para enviar
	if(comm_buffer_index == 0){ //## BUM
	  UDR0 = comm_tx_buffer[0] + '0';
	  comm_buffer_index++;
	  comm_remaining_bytes--;

	}
	else{
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



	


void comm_init()
{ 
  //	SREG = 0x80; //habilita interrupções
  //	UCSR0A = 0x20; // seta 'USART Data Register Empty - UDRE' (buffer vazio, pronto para ser escrito) 
  UCSR0B = 0x18; // 0001 1000 -> habilita receive e transmit, 
  UCSR0C = 0x06; // seta UCSZn1:0 para caracteres de 8 bits, reseta UMSELn1:0 para Asynchronous USART, 1 stop bit

  UBRR0H = 0x00; 
  UBRR0L = 0x67; //baud rate = 9600, 103 em decimal = 0x67

  COMM_timer = 0;
  comm_buffer_index = 0;
  comm_rx_flags = 0;

  comm_state = COMM_STATE_IDLE;
  aux_char = 'c';
  //	fsm_();
}

void comm_envia(unsigned int palavras /*, unsigned char quantos*/)
{
  UDR0 = palavras;
}

char comm_recebe()
{
  unsigned int buffer;
  buffer = UDR0;
  return buffer;
}

#if 0
::x0::x1::x2::x3::x4::x5::x6::x7::x8::x9
::05::07::13::14::15::16::17::18::19
#endif
