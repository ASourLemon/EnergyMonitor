#include "handle.h"


void high_led(int num_led){
  digitalWrite(num_led, HIGH);
}

void low_led(int num_led){
  digitalWrite(num_led, LOW);
}

uint8 read_distance(){
  unsigned long pulseTime=0;
  unsigned long distance=0;
  digitalWrite(INIT_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(INIT_PIN, LOW);
  pulseTime = pulseIn(ECHO_PIN, HIGH, 0);
  return pulseTime/58;
}


uint8 dispatch(uint8 opcode, uint8 param, uint8* resp) {
        uint8 tx_packet[TX_PACKET_MAX_SIZE];
        memset(tx_packet, 0, sizeof(tx_packet));
        if(opcode == 'H') {
          high_led(param);
          tx_packet[0] = '1' -'0';
          tx_packet[1] = '1';
        }
        else if(opcode == 'L') {
          low_led(param);
          tx_packet[0] = '1' -'0';
          tx_packet[1] = '1';
        }
        else if(opcode == 'R') {
          tx_packet[0]='1';
          tx_packet[1]=read_distance(); 
        }
        else {
          tx_packet[0] = '1' -'0';
          tx_packet[1] = '0';
        }
        memcpy(resp, tx_packet, TX_PACKET_MAX_SIZE);
        return 1;
}
