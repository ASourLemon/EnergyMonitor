/*
  power_calculator.h : Public library for the ARDUINO power comsumption monitor.
  
*/
#ifndef _POWER_CALCULATOR_TASK_
#define _POWER_CALCULATOR_TASK_

#include "avr/io.h"
#include "v_types.h"

/* Global Variables */
int in_pin_voltage, in_pin_current; // voltage and current pins
double voltage_calibration, current_calibration, phase_calibration; // calibrations coeficients

/* Variables */
double real_power,
       apparent_power,
       power_factor,
       voltage_rms,
       current_rms;
       

/* Public functions */
void voltage(int _in_pin_voltage, double _voltage_calibration, double _phase_calibration);
void current(int _in_pin_current, double _current_calibration);

void calculate_power(int crossings, int timeout);
double calculate_current_rms(int number_of_samples);

long positive_supply_voltage();
#endif
