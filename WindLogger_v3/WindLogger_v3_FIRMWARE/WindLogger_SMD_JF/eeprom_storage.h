#ifndef _EEPROM_STORAGE_H_
#define _EEPROM_STORAGE_H_

void EEPROM_GetDeviceID(char * buffer);
void EEPROM_SetDeviceID(char * buffer);

uint16_t EEPROM_GetSampleTime(void);
void EEPROM_SetSampleTime(uint16_t sampleTime);

uint16_t EEPROM_GetCurrentOffset(void);
void EEPROM_SetCurrentOffset(uint16_t currentOffset);

uint16_t EEPROM_GetR1(void);
void EEPROM_SetR1(uint16_t r1);

uint16_t EEPROM_GetR2(void);
void EEPROM_SetR2(uint16_t r2);

uint16_t EEPROM_GetCurrentGain(void);
void EEPROM_SetCurrentGain(uint16_t currentGain);

bool EEPROM_GetWindwavePosition(void);
void EEPROM_SetWindwavePosition(bool set);

#endif
