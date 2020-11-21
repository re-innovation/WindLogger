/*
 * wind.cpp
 *
 * Application wind direction/speed functionality for Wind Data logger
 *
 * Matt Little/James Fowkes
 * July 2015
 */

#include <Arduino.h>

#define LIBCALL_ENABLEINTERRUPT
#include <EnableInterrupt.h>

#include "app.h"
#include "eeprom_storage.h"
#include "utility.h"
#include "wind.h"

/* 
 * Private Variables
 */

/********** Wind Direction Storage *************/
#if READ_WIND_DIRECTION
static char s_windDirection[3]; // Hold "N", "NE", "E" etc. strings
static int s_windDirectionArray[] = {0,0,0,0,0,0,0,0};  //Holds count of each cardinal wind direction
#endif

// Variables for the Pulse Counter
#if READ_WINDSPEED
static volatile long s_livePulseCounters[2] = {0, 0};  // This counts pulses from the flow sensor  - Needs to be long to hold number
static volatile long s_pulseCountersOld[2] = {0, 0};  // This is storage for the old flow sensor - Needs to be long to hold number
#endif

static bool s_windwave_is_at_top_of_divider = false;

/* 
 * Private Functions
 */

#if READ_WINDSPEED == 1
/***************************************************
 *  Name:        pulse1
 *
 *  Returns:     Nothing.
 *
 *  Parameters:  None.
 *
 *  Description: Count pulses from Anemometer 1
 *
 ***************************************************/
static void pulse1(void)
{
  // If the anemometer has spun around
  // Increment the pulse counter
  s_livePulseCounters[0]++;
  // ***TO DO**** Might need to debounce this
}

/***************************************************
 *  Name:        pulse2
 *
 *  Returns:     Nothing.
 *
 *  Parameters:  None.
 *
 *  Description: Count pulses from Anemometer 2
 *
 ***************************************************/
static void pulse2(void)
{
  // If the anemometer has spun around
  // Increment the pulse counter
  s_livePulseCounters[1]++;
  // ***TO DO**** Might need to debounce this
}
#endif

/* 
 * Public Functions
 */

#if READ_WINDSPEED == 1

/* 
 * WIND_SetupWindPulseInterrupts
 * Configures pins and interrupts for pulse counting
 */
void WIND_SetupWindPulseInterrupts()
{
	pinMode(ANEMOMETER1, INPUT); 
	digitalWrite(ANEMOMETER1, HIGH);
	enableInterrupt(ANEMOMETER1, &pulse1, FALLING);
	pinMode(ANEMOMETER2, INPUT); 
	digitalWrite(ANEMOMETER2, HIGH);
	enableInterrupt(ANEMOMETER2, &pulse2, FALLING); 
}

void WIND_WritePulseCountToBuffer(uint8_t counter, FixedLengthAccumulator * accum)
{
	if (!accum) { return; }
	char temp[16];

	if (counter < 2)
	{
		(void)ltoa(s_pulseCountersOld[counter], temp, 10);
		accum->writeString(temp);
	}
	else
	{
		accum->writeString("??");
	}
}

/* 
 * WIND_GetLivePulseCount
 * Called by application to get the live pulse count
 */
long WIND_GetLivePulseCount(uint8_t counter)
{
	long count;

	switch(counter)
	{
		case 0:
			count = s_livePulseCounters[0];
			break;
		case 1:
			count = s_livePulseCounters[1];
			break;
		default:
			count = 0;
			break;
	}

	return count;
}


/* 
 * WIND_StoreWindPulseCounts
 * Saves the latest pulse counts and resets the live counts
 */
void WIND_StoreWindPulseCounts()
{
	s_pulseCountersOld[0] = s_livePulseCounters[0];
    s_pulseCountersOld[1] = s_livePulseCounters[1];
    s_livePulseCounters[0] = 0;
    s_livePulseCounters[1] = 0;
}

/* 
 * WIND_Debug
 * Output debugging strings if in debug mode
 */
void WIND_Debug()
{
	if (APP_InDebugMode())
	{
		Serial.print("Anemometer1: ");
	    Serial.println(WIND_GetLivePulseCount(0));
	    Serial.print("Anemometer2: ");
	    Serial.println(WIND_GetLivePulseCount(1));
	    Serial.flush();
	}
}

#else
void WIND_SetupWindPulseInterrupts() {}
void WIND_WritePulseCountToBuffer(uint8_t counter, FixedLengthAccumulator * accum)
{
	(void)counter;
	(void)accum;
}
long WIND_GetLivePulseCount(uint8_t counter) { (void)counter; return 0;}
void WIND_StoreWindPulseCounts() {}
void WIND_Debug() {};

#endif

#if READ_WIND_DIRECTION == 1

/* 
 * WIND_SetWindvanePosition
 * Configures the electrical position of the windvane (top or bottom of a potential divider)
 */
void WIND_SetWindvanePosition(bool windwave_is_at_top_of_divider)
{
	s_windwave_is_at_top_of_divider = windwave_is_at_top_of_divider;
	EEPROM_SetWindwavePosition(s_windwave_is_at_top_of_divider);
}

// ******** WIND_ConvertWindDirection *********
// This routine takes in an analog read value and converts it into a wind direction
// The Wind vane uses a series of resistors to show what direction the wind comes from
// The different values are (with a 10k to Ground):
//    R1 = 33k  => 238 N
//    R2 = 8.2k => 562 NE
//    R3 = 1k => 930 E
//    R4 = 2.2k => 839 SE
//    R5 = 3.9k => 736 S
//    R6 = 16k => 394 SW
//    R7 = 120k => 79 W
//    R8 = 64.9k => 137 NW

// The different values are (with a 10k to Vbattery):
// The value will be 1024 - vane integer reading

// This means we can 'band' the data into 8 bands

void WIND_ConvertWindDirection(int reading)
{

	if (s_windwave_is_at_top_of_divider)
	{
		reading = 1023 - reading;
	}
	
	if(reading>0&&reading<100)
	{
		s_windDirectionArray[6]++;
	}
	else if(reading>100&&reading<200)
	{
		s_windDirectionArray[7]++;
	}
	else if(reading>200&&reading<350)
	{
		s_windDirectionArray[0]++; 
	}
	else if(reading>350&&reading<450)
	{
		s_windDirectionArray[5]++;
	}  
	else if(reading>450&&reading<650)
	{
		s_windDirectionArray[1]++;
	}  
	else if(reading>650&&reading<800)
	{
		s_windDirectionArray[4]++;
	}
	else if(reading>800&&reading<900)
	{
		s_windDirectionArray[3]++;
	}
	else if(reading>900&&reading<1024)
	{
		s_windDirectionArray[2]++;
	}
	else
	{
	  // This is an error reading
	}
}

void WIND_AnalyseWindDirection()
{
	// When a data sample period is over we need to see the most frequent wind direction.
	// This needs to be converted back to a direction and stored on SD

	int data1 = s_windDirectionArray[0];
	int maxIndex = 0;
	// First need to find the maximum integer in the array
	for(int i=1;i<8;i++)
	{
		if(data1<s_windDirectionArray[i])
		{
			data1=s_windDirectionArray[i];
			maxIndex = i;
		}
	}
 	// Serial.println(maxIndex);  Testing
	
	// Clear the wind direction string and fill based on maxIndex	
	s_windDirection[0] = s_windDirection[1] = s_windDirection[2] = '\0';  
 	switch(maxIndex)
	{
		case 0:
			s_windDirection[0] = 'N';
			break;
		case 1:
			s_windDirection[0] = 'N';
			s_windDirection[1] = 'E';
			break;    
		case 2:
			s_windDirection[0] = 'E';
			break;  
		case 3:
			s_windDirection[0] = 'S';
			s_windDirection[1] = 'E';
			break;
		case 4:
			s_windDirection[0] = 'S';
			break;  
		case 5:
			s_windDirection[0] = 'S';
			s_windDirection[1] = 'W';
			break;
		case 6:
			s_windDirection[0] = 'W';
			break;
		case 7:
			s_windDirection[0] = 'N';
			s_windDirection[1] = 'W';
			break;
	}

	for(int i=0;i<8;i++)
	{
		//Resets the wind direction array
		s_windDirectionArray[i]=0;
	}
}

void WIND_WriteDirectionToBuffer(FixedLengthAccumulator * accum)
{
	if (!accum) { return; }
	accum->writeString(s_windDirection);
}

#else

void WIND_ConvertWindDirection(int reading) { (void)reading; }
void WIND_AnalyseWindDirection() {}
void WIND_WriteDirectionToBuffer(FixedLengthAccumulator * accum) { (void)accum; }

#endif
