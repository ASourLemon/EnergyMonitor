#ifndef __HANDLE_H__
#define __HANDLE_H__

#include <Arduino.h>
#include "v_types.h"

#define TX_PACKET_MAX_SIZE 20

#define ECHO_PIN 8
#define INIT_PIN 7

#define LED4_PIN 6
#define LED3_PIN 5
#define LED2_PIN 4
#define LED1_PIN 3
#define LED0_PIN 2


uint8 dispatch(uint8 opcode, uint8 param, uint8* resp);


#endif
