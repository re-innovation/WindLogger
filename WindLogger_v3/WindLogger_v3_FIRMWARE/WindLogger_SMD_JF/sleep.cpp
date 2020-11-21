/*
 * sleep.cpp
 *
 * Application sleep functionality for Wind Data logger
 *
 * Matt Little/James Fowkes
 * July 2015
 */

#include <Arduino.h>
#include <avr/sleep.h>
#include <avr/power.h>

#include "sleep.h"
#include "rtc.h"

/***************************************************
 *  Name:        SLEEP_SetWakeOnRTCAndSleep
 *
 *  Returns:     Nothing.
 *
 *  Parameters:  None.
 *
 *  Description: Enters the arduino into sleep mode (turns on RTC interrupt).
 *
 ***************************************************/
void SLEEP_SetWakeOnRTCAndSleep(void)
{
  RTC_EnableInterrupt();
  
  sleep_enable();
   
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);  
  
  byte old_ADCSRA = ADCSRA;  // Store the old value to re-enable 
  // disable ADC
  ADCSRA = 0;

  byte old_PRR = PRR;  // Store previous version on PRR
  // turn off various modules
  PRR = 0b11111111;
  
  sleep_cpu();
  /* The program will continue from here. */
  /************* ASLEEP *******************/
  
  // ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ //
  
  /************** WOKEN UP ***************/
  /* First thing to do is disable sleep. */
  sleep_disable();
  
  // turn ON various modules USART and ADC
  PRR = old_PRR;  
  
  // enable ADC
  ADCSRA = old_ADCSRA;  
}
