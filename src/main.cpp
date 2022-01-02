#include <Arduino.h>

const uint16_t aPrescale1[] = {0, 1, 8, 64, 256, 1024};
const uint16_t aPrescale2[] = {0, 1, 8, 32, 64, 128, 256, 1024};

void enablePwmPin(unsigned long ulFrequency, int Percentage);
uint8_t findprescaler(unsigned long ulFrequency, int timer);

void setup()
{
  
  enablePwmPin(1000000L, 30);
}

void loop()
{
  
}

/**
 * 
 * @param  {unsigned long} ulFrequency : desired frequeny in herz
 * @param  {int} nTimer    : number of timers in the controller
 * @return {uint8_t}       : prescaler setting
 */
uint8_t findPrescaler(unsigned long ulFrequency, int nTimer)
{
  uint8_t nPrescaler = 1;
  switch (nTimer)
  {
  case 0:
  case 1:
    while (nPrescaler < (sizeof(aPrescale1) / sizeof(uint16_t) - 1) && (F_CPU / ulFrequency / aPrescale1[nPrescaler] > (nTimer == 0 ? 255L : 65535L))) // timer 1 is 16-bit with 64K levels
      nPrescaler++;
    break;
  case 2:
    while (nPrescaler < (sizeof(aPrescale2) / sizeof(uint16_t) - 1) && (F_CPU / ulFrequency / aPrescale2[nPrescaler] > 255L)) // timer 2 is 8-bit, but has more prescaler options
      nPrescaler++;
    break;
  }
  return nPrescaler;
}

/**
 * FIXME: Duty cyle
 * @param  {unsigned long} ulFrequency : the desired frequency in herz
 * @param  {int} nPeriodPercentage : thge PWM duty cycle (does not work atm)
 */
void enablePwmPin(unsigned long ulFrequency, int nPeriodPercentage)
{
  int nPreferredPin = 11;
  byte nPrescale = 1;
  nPrescale = findPrescaler(ulFrequency, 2);

  ulFrequency *= 2;
  TCCR2A = _BV(COM2A0) | _BV(WGM21) | _BV(WGM20);
  TCCR2B = 1 << WGM22 | nPrescale << CS20;
  OCR2A = (F_CPU / (ulFrequency * aPrescale2[nPrescale])) - 1;
  unsigned long ulPercentage = (nPeriodPercentage > 50 ? (100 - nPeriodPercentage) : nPeriodPercentage);
  unsigned long ulResult = ((OCR2A + 1) * ulPercentage * 10L) / 1000L - 1;
  OCR2B = ulResult;

  pinMode(nPreferredPin, OUTPUT);
}