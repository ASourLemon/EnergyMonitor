#include "handle.h"

#define ENERGY_STATE_IDLE 0
#define ENERGY_STATE_RX 1
#define ENERGY_STATE_TX 2
#define ENERGY_STATE_CALCULE 3
#define ENERGY_STATE_ERR 4
#define ENERGY_STATE_ACK 5

#define MODULE_ID '0'
#define PACKET_MAX_SIZE 20
#define NUM_DEC 1000
#define RE_SEND_TIMEOUT '5'

#define CALC_POWER_OP 'C'

uint8 rx_packet[PACKET_MAX_SIZE]; 
uint8 tx_packet[PACKET_MAX_SIZE];
uint8 ack_message[] = {'3'-'0', '1', '1', '1'};
uint8 energy_state = ENERGY_STATE_IDLE;

uint8 ENERGY_timer = '5'-'0';


void energy_init(){
  
  in_pin_voltage = 2;
  in_pin_current = 1;
  voltage_calibration = 220;
  current_calibration = 111;
  phase_calibration = 0;
}


void energy_task() {
	switch(energy_state) {
  
		case ENERGY_STATE_IDLE:{
			if(comm_get_message(MODULE_ID, rx_packet)) {
			  energy_state = ENERGY_STATE_RX;
                        }
			break;
                }
                
		case ENERGY_STATE_RX: {
                        uint8 op_code=rx_packet[2];

			if(op_code == CALC_POWER_OP) {
                            ack_message[2] = '1';
		            energy_state = ENERGY_STATE_CALCULE;
                        }
			else {
                          ack_message[2] = '0';
                          energy_state = ENERGY_STATE_ERR;
                        }
			break;
		}

		case ENERGY_STATE_CALCULE: {
                        ack_message[3] = '1';
                        if(comm_send_message(ack_message)) {
      			  double power = calculate_current_rms(10);
      			  int16 p_int=floor(power);
      			  int16 p_dec=floor((power-p_int)*NUM_DEC);
      			  int8* p_aux = (int8*)&p_int;
      			  tx_packet[0] = '6' - '0';
      			  tx_packet[1] = MODULE_ID;
      			  tx_packet[2] = CALC_POWER_OP;
  			  tx_packet[3] = p_aux[0];
                          tx_packet[4] = p_aux[1];
  			  p_aux = (int8*)&p_dec;
                          tx_packet[5] = p_aux[0];
                          tx_packet[6] = p_aux[1];
                          energy_state = ENERGY_STATE_TX;
                        }
			break;
		}

		case ENERGY_STATE_TX: {
			if(comm_send_message(tx_packet)) {
                          energy_state = ENERGY_STATE_ACK;
                        }
			break;
                }
                
                case ENERGY_STATE_ACK: {
                    if(comm_get_message(MODULE_ID, rx_packet)) {
                        energy_state = ENERGY_STATE_IDLE;
                    }
                  break;  
                }

		case ENERGY_STATE_ERR: {
                        if(comm_send_message(ack_message))
                          energy_state = ENERGY_STATE_IDLE;
                        break;

		}
    }
}
