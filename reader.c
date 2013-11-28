#include "reader.h"

void init_ADC()
{
  // Select Vref=AVcc
  ADMUX |= (1<<REFS0);
  //set prescaller to 128 and enable ADC 
  ADCSRA |= (1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0)|(1<<ADEN);    
}

uint16_t read_ADC(uint8_t _channel)
{
  //select ADC channel with safety mask
  ADMUX = (ADMUX & 0xF0) | (_channel & 0x0F);
  //single conversion mode
  ADCSRA |= (1<<ADSC);
  // wait until ADC conversion is complete
  while( ADCSRA & (1<<ADSC) );
  return ADC;
}
