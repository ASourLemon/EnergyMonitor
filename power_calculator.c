/*
  Description: This module is responsible to measure and calculate
  the power comsumption from the data received through the ARDUINO board.
*/
#include "monitor.h"
#include "node.h"
#include "time.h"
  
/* Global Variables */
int16 in_pin_voltage, in_pin_current; // voltage and current pins
double voltage_calibration, current_calibration, phase_calibration; // calibrations coeficients

/* Global Variables used in the power calculation procedure. */
int16 start_voltage;

int16 last_voltage_cross, voltage_cross; // used to measure the number of times threshold is crossed

int16 cross_count;

int16 last_sample_voltage, sample_voltage; // holds the value of the previous and the current sample 
int16 last_sample_current, sample_current;

double last_filtered_voltage, filtered_voltage; 
double last_filtered_current, filtered_current;

double phase_shifted_voltage;

double sq_voltage, sum_voltage, sq_current, sum_current, inst_phase, sum_phase; // sq = squared, sum = summation, inst = instataneous, P = phase 

void voltage(int _in_pin_voltage, double _voltage_calibration, double _phase_calibration){
  in_pin_voltage = _in_pin_voltage;
  voltage_calibration = _voltage_calibration;
  phase_calibration = _phase_calibration;
}

void current(int _in_pin_current, double _current_calibration){
  in_pin_current = _in_pin_current;
  current_calibration = _current_calibration;
}

long positive_supply_voltage(){
  long result;

#if (_NODE_BOARD_TYPE_ == NODE_BOARD_ARDUINO)
  ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);  
#endif

#if defined(__AVR__) 
  delay(2);                                        // Wait for Vref to settle
  ADCSRA |= _BV(ADSC);                             // Convert
  while (bit_is_set(ADCSRA,ADSC));
  result = ADCL;
  result |= ADCH<<8;
  result = 1126400L / result;                     //1100mV*1024 ADC steps http://openenergymonitor.org/emon/node/1186
  return result;
}


double calculate_current_rms(int NUMBER_OF_SAMPLE){

  int SUPPLYVOLTAGE = positive_supply_voltage();	

  for (int n = 0; n < NUMBER_OF_SAMPLES; n++)
    {
      last_sample_current  = sample_current;
      sample_current = analogRead(in_pin_current);
      last_filtered_current = filtered_current;
      filtered_current = 0.996*(last_filtered_current+sample_current-last_sample_current);

      // Root-mean-square method current
      // 1) square current values
      sq_current = filtered_current * filtered_current;
      // 2) sum 
      sum_current += sq_current;
    }

  double CURRENT_RATIO = current_calibration *((SUPPLYVOLTAGE/1000.0) / 1023.0);
  current_rms = CURRENT_RATIO * sqrt(sum_current / NUMBER_OF_SAMPLES); 

  //Reset accumulators
  sum_current = 0;
  //--------------------------------------------------------------------------------------       

  return current_rms;
}


void calculate_power(int crossings, int timeout)
{
  int SUPPLYVOLTAGE = positive_supply_voltage();

  int cross_count = 0;                             //Used to measure number of times threshold is crossed.
  int number_of_samples = 0;                        //This is now incremented  

  //-------------------------------------------------------------------------------------------------------------------------
  // 1) Waits for the waveform to be close to 'zero' (500 adc) part in sin curve.
  //-------------------------------------------------------------------------------------------------------------------------
  int state = 0;                                  //an indicator to exit the while loop

  unsigned long start = TIME_curr_time;    // rever para o avr : millis()-start makes sure it doesnt get stuck in the loop if there is an error.

  while(!state)                                   //the while loop...
    {
      start_voltage = read_ADC(in_pin_voltage);                    //using the voltage waveform
      if ((start_voltage < 550) && (start_voltage > 440)) state=1;  //check its within range
      if ((TIME_curr_time-start)>timeout) state = 1;
    }

  //-------------------------------------------------------------------------------------------------------------------------
  // 2) Main measurment loop
  //------------------------------------------------------------------------------------------------------------------------- 
  start = TIME_curr_time; 

  while ((cross_count < crossings) && ((TIME_curr_time-start)<timeout)) 
    {
      number_of_samples++;                            //Count number of times looped.

      last_sample_voltage = sample_voltage;                          //Used for digital high pass filter
      last_sample_current = sample_current;                          //Used for digital high pass filter
    
      last_filtered_voltage = filtered_voltage;                    //Used for offset removal
      last_filtered_current = filtered_current;                    //Used for offset removal   
    
      //-----------------------------------------------------------------------------
      // A) Read in raw voltage and current samples
      //-----------------------------------------------------------------------------
      sample_voltage = read_ADC(in_pin_voltage);                 //Read in raw voltage signal
      sample_current = read_ADC(in_pin_current);                 //Read in raw current signal

      //-----------------------------------------------------------------------------
      // B) Apply digital high pass filters to remove 2.5V DC offset (centered on 0V).
      //-----------------------------------------------------------------------------
      filtered_voltage = 0.996*(last_filtered_voltage+(sample_voltage-last_sample_voltage));
      filtered_current = 0.996*(last_filtered_current+(sample_current-last_sample_current));

      //-----------------------------------------------------------------------------
      // C) Root-mean-square method voltage
      //-----------------------------------------------------------------------------  
      sq_voltage= filtered_voltage * filtered_voltage;                 //1) square voltage values
      sum_voltage += sq_voltage;                                //2) sum

      //-----------------------------------------------------------------------------
      // D) Root-mean-square method current
      //-----------------------------------------------------------------------------   
      sq_current = filtered_current * filtered_current;                //1) square current values
      sum_current += sq_current;                                //2) sum 

      //-----------------------------------------------------------------------------
      // E) Phase calibration
      //-----------------------------------------------------------------------------
      phase_shifted_voltage = last_filtered_voltage + phase_calibration * (filtered_voltage - last_filtered_voltage); 

      //-----------------------------------------------------------------------------
      // F) Instantaneous power calc
      //-----------------------------------------------------------------------------   
      inst_phase = phase_shifted_voltage * filtered_current;          //Instantaneous Power
      sum_phase += inst_phase;                               //Sum  
    
      //-----------------------------------------------------------------------------
      // G) Find the number of times the voltage has crossed the initial voltage
      //    - every 2 crosses we will have sampled 1 wavelength 
      //    - so this method allows us to sample an integer number of half wavelengths which increases accuracy
      //-----------------------------------------------------------------------------       
      last_voltage_cross = voltage_cross;                     
      if (sample_voltage > start_voltage) voltage_cross = 1; 
      else voltage_cross = 0;
      if (number_of_samples == 1) last_voltage_cross = voltage_cross;                  

      if (last_voltage_cross != voltage_cross) cross_count++;
    }

  //-------------------------------------------------------------------------------------------------------------------------
  // 3) Post loop calculations
  //------------------------------------------------------------------------------------------------------------------------- 
  //Calculation of the root of the mean of the voltage and current squared (rms)
  //Calibration coeficients applied. 

  double VOLTAGE_RATIO = voltage_calibration *((SUPPLYVOLTAGE/1000.0) / 1023.0);
  voltage_rms = VOLTAGE_RATIO * sqrt(sum_voltage / number_of_samples); 

  double CURRENT_RATIO = current_calibration *((SUPPLYVOLTAGE/1000.0) / 1023.0);
  current_rms = CURRENT_RATIO * sqrt(sum_current / number_of_samples); 

  //Calculation power values
  real_power = VOLTAGE_RATIO * CURRENT_RATIO * sum_phase / number_of_samples;
  apparent_power = voltage_rms * current_rms;
  power_factor = real_power / apparent_power;

  //Reset accumulators
  sum_voltage = 0;
  sum_current = 0;
  sum_phase = 0;
  //--------------------------------------------------------------------------------------       
}

