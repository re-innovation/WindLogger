/*
 * external_volts_amps.cpp
 *
 * External voltage and current functionality for Wind Data logger
 *
 * Matt Little/James Fowkes
 * July 2015
 */

#include <Arduino.h>
#include <EEPROM.h>

#include "app.h"
#include "utility.h"
#include "external_volts_amps.h"
#include "eeprom_storage.h"

/* 
 * Private Variables
 */

///********* External Voltage ****************/
#if READ_EXTERNAL_VOLTS == 1
static int  s_r1, s_r2;  // The potential divider values  
static float s_externalVoltage;        // Temporary store for float
static char  s_externalVoltStr[6];      // Hold the battery voltage as a string
#endif

///********* Current 1 ****************/
#if READ_EXTERNAL_AMPS == 1
static long int s_currentData1;      // Temp holder for value
static float s_current1;        // Temporary store for float
static float s_currentOffset;  // Holds the offset current
static char s_current1Str[7];      // Hold the current as a string
static int s_iGain;    // Holds the current conversion factor in mV/A
#endif

/* 
 * Public Functions
 */

#if READ_EXTERNAL_AMPS == 1
/* 
 * VA_SetCurrentOffset
 * Called by application to setup current offset
 */
void VA_SetCurrentOffset(int newOffset)
{
	// Convert the current offset to a voltage
  	s_currentOffset = float(newOffset)*3.3f/1023.0f;
}

/* 
 * VA_SetCurrentGain
 * Called by application to setup current gain
 */
void VA_SetCurrentGain(int newGain)
{
	s_iGain = newGain;
}

/* 
 * VA_StoreNewCurrentOffset
 * Called by application to read a new offset at 0A
 * and store in EEPROM
 */
void VA_StoreNewCurrentOffset(void)
{
    s_currentData1 = 0;  // Reset this holder
    for(int i = 0;i<=19;i++)
    {  
      s_currentData1 += analogRead(CURRENT_1_PIN);
      delay(20);
    }           
    
    s_currentData1 /= 20;

    VA_SetCurrentOffset(s_currentData1);
    
    Serial.print("Ioffset:");
    Serial.print(s_currentOffset);
    Serial.println("V");

    // Write the offset to EEPROM   
    EEPROM_SetCurrentOffset(s_currentData1);
}

/* 
 * VA_StoreNewCurrentGain
 * Called by application to set a new current gain
 * and store in EEPROM
 */
void VA_StoreNewCurrentGain(int value)
{
    s_iGain = value; // Use this new value
    Serial.print("I Gain:");
    Serial.println(value);   
    // Write this info to EEPROM   
    EEPROM_SetCurrentGain(value);
}

/* 
 * VA_UpdateExternalCurrent
 * Called by application to read the external current
 */
void VA_UpdateExternalCurrent(void)
{
    s_currentData1 = 0;  // Reset the value

    // Lets average the data here over 20 samples.
    for(int i = 0;i<=19;i++)
    {  
      s_currentData1 += analogRead(CURRENT_1_PIN);
      delay(2);
    }

    s_current1 = float(s_currentData1)/20.0f;  
    s_current1 = (s_current1*3.3f/1023.0f) - s_currentOffset;
    // Current 1 holds the incoming voltage.
     
    // ********** LEM HTFS 200-P SENSOR *********************************
    // Voutput is Vref +/- 1.25 * Ip/Ipn 
    // Vref = Vsupply/2 +/1 0.025V (Would be best to remove this with analog stage)
    //s_current1 = (s_current1*200.0f)/1.25f;
    s_current1 = s_current1*float(s_iGain);  
  
//    // ************* ACS*** Hall Effect **********************
//    // Output is Input Voltage - offset / mV per Amp sensitivity
//    // Datasheet says 60mV/A     

    // Convert the current to a string.
    dtostrf(s_current1,2,2, s_current1Str);     // Hold the battery voltage as a string
}

void VA_WriteExternalCurrentToBuffer(FixedLengthAccumulator * accum)
{
    if (!accum) { return; }
    accum->writeString(s_current1Str);
}

#else

void VA_UpdateExternalCurrent(void) {}

void VA_SetCurrentGain(int gain) { (void)gain; } 
void VA_SetCurrentOffset(int offset) { (void)offset; } 

void VA_StoreNewCurrentOffset(void) {} 
void VA_StoreNewCurrentGain(int gain) { (void)gain; } 

void VA_WriteExternalCurrentToBuffer(FixedLengthAccumulator * accum) {(void)accum;}

#endif

#if READ_EXTERNAL_VOLTS == 1
/* 
 * VA_SetVoltageDivider
 * Called by application to set the voltage divider parameters
 */
void VA_SetVoltageDivider(uint16_t newR1, uint16_t newR2)
{
	s_r1 = newR1;
	s_r2 = newR2;
}

/* 
 * VA_StoreNewResistor1
 * VA_StoreNewResistor2
 * Called by application to set new R1 and R2 values
 * and store in EEPROM
 */
void VA_StoreNewResistor1(int value)
{
    s_r1 = value;  // Use this new value
    Serial.print("R1:");
    Serial.println(value);   
    // Write this info to EEPROM   
    EEPROM_SetR1(value);    
}

void VA_StoreNewResistor2(int value)
{
    s_r2 = value; // Use this new value
    Serial.print("R2:");
    Serial.println(value);   
    // Write this info to EEPROM   
    EEPROM_SetR2(value);
}

/* 
 * VA_UpdateExternalVoltage
 * Called by application to read the external voltage
 */
void VA_UpdateExternalVoltage(void)
{
	s_externalVoltage = float(analogRead(VOLTAGE_PIN))*(3.3f/1023.0f)*((float(s_r1)+float(s_r2))/float(s_r2));
    dtostrf(s_externalVoltage,2,2, s_externalVoltStr);
}


void VA_WriteExternalVoltageToBuffer(FixedLengthAccumulator * accum)
{
    if (!accum) { return; }
    accum->writeString(s_externalVoltStr);
}

#else

void VA_UpdateExternalVoltage(void) {}

void VA_StoreNewResistor1(int r1) { (void)r1; } 
void VA_StoreNewResistor2(int r2) { (void)r2; }

void VA_SetVoltageDivider(uint16_t r1, uint16_t r2) { (void)r1; (void)r2; }

void VA_WriteExternalVoltageToBuffer(FixedLengthAccumulator * accum) {(void)accum;}

#endif
