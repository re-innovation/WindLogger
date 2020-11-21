#ifndef _SD_H_
#define _SD_H_

void SD_Setup();
void SD_CreateFileForToday();
void SD_SetDeviceID(char * id);

void SD_SetSampleTime(long newSampleTime);
bool SD_CardIsPresent();
void SD_WriteDataToCard();
void SD_PrintDataToSerial();
void SD_ForcePendingWrite();
bool SD_WriteIsPending();
void SD_ResetCounter();
void SD_SecondTick();

#endif