#ifndef _RTC_H_
#define _RTC_H_

// Defines

// Public Functions
void RTC_Setup(int scl, int sda, int interrupt_pin);
void RTC_EnableInterrupt();
void RTC_DisableInterrupt();

const char * RTC_GetDate(int format = 0);
const char * RTC_GetTime();
void RTC_GetYYMMDDString(char * buffer);

void RTC_SetTime(uint8_t hour, uint8_t minute, uint8_t second);
void RTC_SetDate(uint8_t day, uint8_t month, uint8_t year);

#endif
