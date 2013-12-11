#ifndef __HANDLE_H__
#define __HANDLE_H__

#include "v_types.h"
#include "reader.h"
#include "power_calculator.h"
#include "comm.h"
#include "time.h"
#include <Arduino.h>

#define TX_PACKET_MAX_SIZE 20

void energy_task();

#endif
