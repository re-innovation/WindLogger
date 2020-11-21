/*
 * temperature.cpp
 *
 * Application temperature functionality for Wind Data logger
 *
 * Matt Little/James Fowkes
 * July 2015
 */

#include <Arduino.h>

/*
 * Application Includes
 */

#include "app.h"
#include "utility.h"

/* 
 * Defines and Typedefs
 */

enum {
  T_KELVIN=0,
  T_CELSIUS,
  T_FAHRENHEIT
};

#define THERMISTOR_PIN A0  // This is the analog pin for the thermistor

/* Thermistor data for use in thermistor_to_temperature function */
struct thermistor
{
	float B;
	float T0;
	float R0;
};

#if READ_TEMPERATURE == 1

// Choose one thermsitor (comment out the others)
//static struct thermistor s_thermistor = {4300.0f,298.15f,10000.0f};			// Epicos K164 10K
static struct thermistor s_thermistor = {4126.0f,298.15f,10000.0f};					// GT 10K
//static struct thermistor s_thermistor = {4090.0f,298.15f,47000.0f};	// Vishay 10K

/*
 * Private Functions
 */

/* thermistor_to_temperature
 * Outputs: 
 * 	the actual temperature (float)
 * Inputs:
 * 	1. data - ADC reading
 * 	2. OuputUnit - output in celsius, kelvin or fahrenheit
 * 	3. Your balance resistor resistance in ohms
 *	4. Set true if thermistor is a pullup
 */

static float thermistor_to_temperature(float data, int OutputUnit, float R_Balance, bool pullup)
{
  float R,T;

  if (pullup)
  {
  	R = (1024.0f*R_Balance/data)-R_Balance;
  }
  else
  {
  	R = (data*R_Balance)/(1024.0f-data);
  }

  T=1.0f/(1.0f/s_thermistor.T0+(1.0f/s_thermistor.B)*log(R/s_thermistor.R0));

  switch(OutputUnit) {
    case T_CELSIUS :
      T-=273.15f;
    break;
    case T_FAHRENHEIT :
      T=9.0f*(T-273.15f)/5.0f+32.0f;
    break;
    default:
    break;
  };

  return T;
}

/*
 * Public Functions
 */

void TEMP_WriteTemperatureToBuffer(FixedLengthAccumulator * accum)
{
  if (!accum) { return; }

  float data = float(analogRead(THERMISTOR_PIN));
  float tempC = thermistor_to_temperature(data, T_CELSIUS, 10000.0f, true);
  
  char tempCstr[6];  // A string buffer to hold the converted string
  dtostrf(tempC, 2, 2, tempCstr);  // Convert the temperature value (double) into a string

  accum->writeString(tempCstr);

  if(APP_InDebugMode())
  {
    Serial.print("Therm: ");
    Serial.println(tempCstr);  
  }
}

#else

void TEMP_WriteTemperatureToBuffer(FixedLengthAccumulator * accum)
{
	(void)accum;
}

#endif
