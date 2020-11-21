/*
 * eeprom_storage.cpp
 *
 * Application EEPROM non-voltatile storage functionality for Wind Data logger
 *
 * Matt Little/James Fowkes
 * August 2015
 */

/************ External Libraries*****************************/
#include <Arduino.h>
#include <EEPROM.h>

/*
 * Defines and Typedefs
 */

/*
 * An enumeration of the EEPROM storage locations.
 * Byte-wise indexing, so take datatype length into account
 */

enum eeprom_locations_enum
{
	LOC_DEVICE_ID = 0,
	LOC_SAMPLE_TIME = 2,
	LOC_CURRENT_OFFSET = 4,
	LOC_R1 = 6,
	LOC_R2 = 8,
	LOC_CURRENT_GAIN = 10,
	LOC_WINDVANE_POSITION = 12
};

/*
 * Public Functions
 */

/* 
 * EEPROM_GetXXX, EEPROM_SetXXX 
 * For each value in the eeprom_locations_enum,
 * get and set functions are defined here.
 */

void EEPROM_GetDeviceID(char * buffer)
{
	if (buffer)
	{
		buffer[0] = (char)EEPROM.read(LOC_DEVICE_ID);
		buffer[1] = (char)EEPROM.read(LOC_DEVICE_ID+1);
	}
}

void EEPROM_SetDeviceID(char * buffer)
{
	if (buffer)
	{
		EEPROM.write(LOC_DEVICE_ID,buffer[0]);
		EEPROM.write(LOC_DEVICE_ID+1,buffer[1]);
	}
}

uint16_t EEPROM_GetSampleTime(void)
{
	return (EEPROM.read(LOC_SAMPLE_TIME) << 8) + EEPROM.read(LOC_SAMPLE_TIME+1);	
}

void EEPROM_SetSampleTime(uint16_t sampleTime)
{
	EEPROM.write(LOC_SAMPLE_TIME, sampleTime >> 8);
    EEPROM.write(LOC_SAMPLE_TIME+1, sampleTime & 0xff);
}

uint16_t EEPROM_GetCurrentOffset(void)
{
	return (EEPROM.read(LOC_CURRENT_OFFSET) << 8) + EEPROM.read(LOC_CURRENT_OFFSET+1);
}

void EEPROM_SetCurrentOffset(uint16_t currentOffset)
{
    EEPROM.write(LOC_CURRENT_OFFSET, currentOffset >> 8);
    EEPROM.write(LOC_CURRENT_OFFSET+1, currentOffset & 0xff);  
}

uint16_t EEPROM_GetR1(void)
{
	return (EEPROM.read(LOC_R1) << 8) + EEPROM.read(LOC_R1+1);	
}

void EEPROM_SetR1(uint16_t r1)
{
    EEPROM.write(LOC_R1, r1 >> 8);
    EEPROM.write(LOC_R1+1, r1 & 0xff);  
}

uint16_t EEPROM_GetR2(void)
{
	return (EEPROM.read(LOC_R2) << 8) + EEPROM.read(LOC_R2+1);	
}

void EEPROM_SetR2(uint16_t r2)
{
    EEPROM.write(LOC_R2, r2 >> 8);
    EEPROM.write(LOC_R2+1, r2 & 0xff);  
}

uint16_t EEPROM_GetCurrentGain(void)
{
	return (EEPROM.read(LOC_CURRENT_GAIN) << 8) + EEPROM.read(LOC_CURRENT_GAIN+1);	
}

void EEPROM_SetCurrentGain(uint16_t currentGain)
{
    EEPROM.write(LOC_CURRENT_GAIN, currentGain >> 8);
    EEPROM.write(LOC_CURRENT_GAIN+1, currentGain & 0xff);  
}

bool EEPROM_GetWindwavePosition(void)
{
	return (bool)EEPROM.read(LOC_WINDVANE_POSITION);
}

void EEPROM_SetWindwavePosition(bool set)
{
	EEPROM.write(LOC_WINDVANE_POSITION, (char)set);	
}
