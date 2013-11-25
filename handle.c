#include "handle.h"

void high_led(int num_led){
	digital(num_led, HIGH);
}

void low_led(int num_led){
	digital(num_led, LOW);
}


void dispatch(char* packet) {
	int lenght = packet[0];
	char opcode = packet[1];
	char pin = packet[2];
	if(opcode == 'H')
		high_led(atoi(pin));
	else if(opcode == 'L')
		low_led(atoi(pin));
}
