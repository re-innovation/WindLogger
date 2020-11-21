/*
 * battery.cpp
 *
 * Application battery functionality for Wind Data logger
 *
 * Matt Little/James Fowkes
 * July 2015
 */

#include <Arduino.h>

#include "utility.h"
#include "battery.h"

/* 
 * Private Variables
 */
static char s_batteryVoltStr[6];      // Hold the battery voltage as a string

/* 
 * Public Functions
 */

/* 
 * BATT_UpdateBatteryVoltage
 * Called by application to set new battery voltage from ADC reading
 */
void BATT_UpdateBatteryVoltage(void)
{
	// *********** BATTERY VOLTAGE ***************************************
    // From Vcc-470k-DATA-100k-GND potential divider
    // This is to test in case battery voltage has dropped too low - alert?
    float batteryVoltage;
    batteryVoltage = float(analogRead(BATT_VOLTAGE_PIN))*(3.3f/1024.0f)*((470.0f+100.0f)/100.0f);        // Temporary store for float
    dtostrf(batteryVoltage, 2, 2, s_batteryVoltStr);     // Hold the battery voltage as a string
}


void BATT_WriteVoltageToBuffer(FixedLengthAccumulator * accum)
{
	if (!accum) { return; }
	accum->writeString(s_batteryVoltStr);
}
