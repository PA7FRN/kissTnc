#include "afskSampler.h"

HdlcAfskRx* g_afskRx;
HdlcAfskTx* g_afskTx;
byte loopBackSample;

AfskSampler::AfskSampler(HdlcAfskRx* afskRx, HdlcAfskTx* afskTx) {
  g_afskRx = afskRx;
  g_afskTx = afskTx;

  ADCSRA = 0;
  ADCSRB = 0;
  ADMUX |= (0 & 0x07);   // pin A0
  ADMUX |= (1 << REFS0); // set reference voltage
  ADMUX |= (1 << ADLAR); // left align ADC value to 8 bits from ADCH register

  ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // 128 prescaler 9600 Hz
  ADCSRA |= (1 << ADATE); // enable auto trigger
  ADCSRA |= (1 << ADIE);  // enable interrupts when measurement complete
  ADCSRA |= (1 << ADEN);  // enable ADC
  ADCSRA |= (1 << ADSC);  // start ADC measurements  
}

ISR(ADC_vect) {
  if (g_afskTx->loopbackTest) {
    g_afskRx->decodeSample(loopBackSample);
  }
  else {
    g_afskRx->decodeSample(ADCH);
  }
  loopBackSample = g_afskTx->encodeSample();
}


