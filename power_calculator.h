/*
  monitor.h : Public library for the ARDUINO power comsumption monitor.
  
*/
#ifndef _POWER_CALCULATOR_TASK_

#include "v_types.h"

/* Variables */
double real_power,
       apparent_power,
       power_factor,
       voltage_rms,
       current_rms;

/* Public functions */
void voltage(int16 _in_pin_voltage, double _voltage_calibration, double _phase_calibration);
void current(int16 _in_pin_current. double _current_calibration);

void calculate_power(int16 crossings, int16 timeout);
double calculate_current_rms(int16 NUMBER_OF_SAMPLES);

long positive_supply_voltage();
