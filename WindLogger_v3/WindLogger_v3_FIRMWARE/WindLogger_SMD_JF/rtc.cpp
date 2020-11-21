/*
 * rtc.cpp
 *
 * Application RTC functionality for Wind Data logger
 *
 * Matt Little/James Fowkes
 * August 2015
 */

#include <Arduino.h>
#include <Wire.h>
#include <EnableInterrupt.h>
#include <Rtc_Pcf8563.h>

#include "app.h"
#include "rtc.h"
#include "utility.h"
#include "sd.h"

/************ Real Time Clock code*******************
 * A PCF8563 RTC is attached to pins:
 * ** A4 - SDA (serial data)
 * ** A5 - SDC (serial clock)
 * ** D2 - Clock out - This gives a 1 second pulse to record the data
 
 * RTC PCF8563 code details:
 * By Joe Robertson, jmr
 * orbitalair@bellsouth.net
 */

/*
 * Defines
 */

#define I2C_RTC 0x51 // 7 bit address (without last bit - look at the datasheet)

/* 
 * Private Variables
 */

static Rtc_Pcf8563 s_rtc;
static int s_interrupt_pin;

/* 
 * Private Functions
 */

/***************************************************
 *  Name:        rtcInterruptHandler
 *
 *  Returns:     Nothing.
 *
 *  Parameters:  None.
 *
 *  Description: I use the CLK_OUT from the RTC to give me exact 1Hz signal
 *               To do this I changed the initialise the RTC with the CLKOUT at 1Hz
 *
 ***************************************************/
static void rtcInterruptHandler()
{ 
  disableInterrupt(s_interrupt_pin);

  SD_SecondTick();
  APP_SecondTick();
}

/* 
 * Public Functions
 */

/* 
 * RTC_Setup
 * Called by application to initalise the RTC
 */
void RTC_Setup(int scl, int sda, int interrupt_pin)
{

  //******Real Time Clock Set - up********
  // A4 and A5 are used as I2C interface.
  // D2 is connected to CLK OUT from RTC. This triggers an interrupt to take data
  // We need to enable pull up resistors
  s_interrupt_pin = interrupt_pin;

  pinMode(scl, INPUT);           // set pin to input
  digitalWrite(scl, HIGH);       // turn on pullup resistors
  pinMode(sda, INPUT);           // set pin to input
  digitalWrite(sda, HIGH);       // turn on pullup resistors
  pinMode(interrupt_pin, INPUT); // Interrupt pin for the RTC CLK-OUT   

  // This section configures the RTC to have a 1Hz output.
  // Its a bit strange as first we read the data from the RTC
  // Then we load it back again but including the correct second flag  
  s_rtc.formatDate(RTCC_DATE_WORLD);
  s_rtc.formatTime();
  
  int year_int = s_rtc.getYear();
  int day_int = s_rtc.getDay();
  int month_int = s_rtc.getMonth();  
  int hour_int = s_rtc.getHour();
  int min_int = s_rtc.getMinute();
  int sec_int = s_rtc.getSecond(); 
  
  Wire.begin(); // Initiate the Wire library and join the I2C bus as a master
  Wire.beginTransmission(I2C_RTC); // Select RTC
  Wire.write(0);        // Start address
  Wire.write(0);     // Control and status 1
  Wire.write(0);     // Control and status 2
  Wire.write(DecToBcd(sec_int));     // Second
  Wire.write(DecToBcd(min_int));    // Minute
  Wire.write(DecToBcd(hour_int));    // Hour
  Wire.write(DecToBcd(day_int));    // Day
  Wire.write(DecToBcd(2));    // Weekday
  Wire.write(DecToBcd(month_int));     // Month (with century bit = 0)
  Wire.write(DecToBcd(year_int));    // Year
  Wire.write(0b10000000);    // Minute alarm (and alarm disabled)
  Wire.write(0b10000000);    // Hour alarm (and alarm disabled)
  Wire.write(0b10000000);    // Day alarm (and alarm disabled)
  Wire.write(0b10000000);    // Weekday alarm (and alarm disabled)
  Wire.write(0b10000011);     // Output clock frequency enabled (1 Hz) ***THIS IS THE IMPORTANT LINE**
  Wire.write(0);     // Timer (countdown) disabled
  Wire.write(0);     // Timer value
  Wire.endTransmission();
}

/* 
 * RTC_EnableInterrupt, RTC_DisableInterrupt
 * Enables/disables the RTC interrupt.
 */
void RTC_EnableInterrupt()
{
	 enableInterrupt(s_interrupt_pin, rtcInterruptHandler, RISING);
}

void RTC_DisableInterrupt()
{
	disableInterrupt(s_interrupt_pin);	
}

/* 
 * RTC_GetDate
 * Updates the date string (in specified format) and returns a pointer to it
 */
const char * RTC_GetDate(int format)
{
	return s_rtc.formatDate(format);
}

/* 
 * RTC_GetTime
 * Updates the time string and returns a pointer to it
 */
const char * RTC_GetTime()
{
	return s_rtc.formatTime();
}

/*
 * RTC_GetYYMMDDString
 * Updates the local RTC date and fills the provided buffer with
 * the date in YYMMDD format.
 */
void RTC_GetYYMMDDString(char * buffer)
{
  s_rtc.getDate(); // Update local RTC date
  
  int day_int = s_rtc.getDay();  // Get the actual day from the RTC
  int month_int = s_rtc.getMonth();  // Get the month
  int year_int = s_rtc.getYear();
  
  buffer[0]=(year_int/10) + '0';  // Convert from int to ascii
  buffer[1]=(year_int%10) + '0';  // Convert from int to ascii 
  buffer[2]= (month_int/10) + '0';  // Convert from int to ascii
  buffer[3]= (month_int%10) + '0';  // Convert from int to ascii   
  buffer[4]= (day_int/10) + '0';  // Convert from int to ascii
  buffer[5]= (day_int%10) + '0';  // Convert from int to ascii 
}

/*
 * RTC_SetTime, RTC_SetDate
 * Sets the RTC time/date
 */
void RTC_SetTime(uint8_t hour, uint8_t minute, uint8_t second)
{
	s_rtc.setTime(hour, minute, second);
}

void RTC_SetDate(uint8_t day, uint8_t month, uint8_t year)
{
	//day, weekday, month, century(1=1900, 0=2000), year(0-99)
	s_rtc.setDate(day, 3, month, 0, year);
}
