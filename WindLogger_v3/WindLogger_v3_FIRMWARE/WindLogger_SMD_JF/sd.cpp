/*
 * sd.cpp
 *
 * Application SD card functionality for Wind Data logger
 *
 * Matt Little/James Fowkes
 * August 2015
 */

/************ External Libraries*****************************/
#include <Arduino.h>
#include <Rtc_Pcf8563.h>
#include <SdFat.h>

/************ Application Libraries*****************************/

#include "app.h"
#include "utility.h"
#include "battery.h"
#include "external_volts_amps.h"
#include "wind.h"
#include "temperature.h"
#include "irradiance.h"
#include "rtc.h"
#include "sd.h"

/*
 * Defines
 */

#define SD_CHIP_SELECT_PIN 10 // The SD card Chip Select pin 10
#define SD_CARD_DETECT_PIN 9  // The SD card detect is on pin 6

#define DATA_STRING_LENGTH 128

/*
 * Private Variables
 */

static long s_dataCounter = 0;  // This holds the number of seconds since the last data store
static long s_sampleTime = 2;  // This is the time between samples for the DAQ

static volatile bool s_writePending = false;  // A flag to tell the code when to write data
static char s_last_used_date[16];

// The other SD card pins (D11,D12,D13) are all set within s_SD.h
static int s_lastCardDetect = LOW;  // This is the flag for the old reading of the card detect

// SD file system object and file
static SdFat s_sd;
static SdFile s_datafile;  

static char s_dataString[DATA_STRING_LENGTH];
static FixedLengthAccumulator s_accumulator = FixedLengthAccumulator(NULL, 0);

static char s_filename[] = "DXXXXXX.csv";  // This is a holder for the full file name
static char s_deviceID[3]; // A buffer to hold the device ID

static char comma = ',';

// These are Char Strings - they are stored in program memory to save space in data memory
// These are a mixutre of error messages and serial printed information
// These MUST be in the same order as the fields are written to the CSV file!
const char s_pstr_headers[] PROGMEM = \
  "Ref, Date, Time, " \
  WINDSPEED_HEADERS \
  WIND_DIRECTION_HEADERS \
  TEMPERATURE_HEADERS \
  IRRADIANCE_HEADERS \
  EXTERNAL_VOLTS_HEADERS \
  EXTERNAL_AMPS_HEADERS \
  "Batt V";
  
  
const char s_pstr_initialised[] PROGMEM = "Init SD OK. Headers:";
const char s_pstr_not_initialised[] PROGMEM = "Init SD Failed";
const char s_pstr_noSD[] PROGMEM = "No SD card";
const char s_pstrerroropen[] PROGMEM = "Error open";
const char s_pstr_file_already_exists[] PROGMEM = "File already exists";

/*
 * Private Functions
 */

static void write_configurable_fields(FixedLengthAccumulator * accum)
{
  #if READ_WINDSPEED == 1
  accum->writeChar(comma);
  WIND_WritePulseCountToBuffer(0, accum);
  accum->writeChar(comma);
  WIND_WritePulseCountToBuffer(1, accum);
  #endif

  #if READ_WIND_DIRECTION == 1
  accum->writeChar(comma);
  WIND_WriteDirectionToBuffer(accum);
  #endif

  #if READ_TEMPERATURE == 1
  accum->writeChar(comma);
  TEMP_WriteTemperatureToBuffer(accum);
  #endif

  #if READ_IRRADIANCE == 1
  accum->writeChar(comma);
  IRR_WriteIrradianceToBuffer(accum);
  #endif

  #if READ_EXTERNAL_VOLTS == 1
  accum->writeChar(comma);
  VA_WriteExternalVoltageToBuffer(accum);
  #endif
  
  #if READ_EXTERNAL_AMPS == 1
  accum->writeChar(comma);
  VA_WriteExternalCurrentToBuffer(accum);
  #endif
}

/*
 * writeDataString
 * Opens the current file for writing and appends the current data string
 */
static void writeDataString()
{
    s_datafile.open(s_filename, O_RDWR | O_CREAT | O_AT_END);    // Open the correct file
    // if the file is available, write to it:
    if (s_sd.exists(s_filename))
    {
      s_datafile.println(s_dataString);
      s_datafile.close();
      // print to the serial port too:
      Serial.println(s_dataString);
    }  
    // if the file isn't open, pop up an error:
    else {
      if(APP_InDebugMode())
      {
        Serial.println(PStringToRAM(s_pstrerroropen));
      }
  }
}

/*

#define DATA_STRING_LENGTH 128 
* Public Functions
 */

/*
 * SD_Setup
 * Initalises the SD card
 */
void SD_Setup()
{
    pinMode(SD_CARD_DETECT_PIN,INPUT);  // D9 is the SD card detect on pin 9.

  // make sure that the default chip select pin is set to
  // output, even if you don't use it:
    pinMode(SD_CHIP_SELECT_PIN, OUTPUT);

  // Initialize the SD card at SPI_HALF_SPEED to avoid bus errors 
  // We use SPI_HALF_SPEED here as I am using resistor level shifters.

  s_accumulator.attach(s_dataString, DATA_STRING_LENGTH);

  if (!s_sd.begin(SD_CHIP_SELECT_PIN, SPI_HALF_SPEED)) {
    if(APP_InDebugMode())
    {
      Serial.println(PStringToRAM(s_pstr_not_initialised));
    }
  // don't do anything more:
  // Want to turn on an ERROR LED here
  	return;
  }
  else
  {
  	if(APP_InDebugMode())
  	{
  		Serial.println(PStringToRAM(s_pstr_initialised));
      Serial.println(PStringToRAM(s_pstr_headers));
  	}
  }
}

/*
 * SD_SetDeviceID
 * Sets the local device ID (the ID gets written to datalogging file)
 */
void SD_SetDeviceID(char * id)
{
	s_deviceID[0] = id[0];
	s_deviceID[1] = id[1];
}

/*
 * SD_SetSampleTime
 * Changes the sample time
 */
void SD_SetSampleTime(long newSampleTime)
{
	s_sampleTime = newSampleTime;
}

/*
 * SD_CreateFileForToday
 * Creates a new file if one doesn't exist for current date.
 */
void SD_CreateFileForToday()
{
  // Check there is a file created with the date in the title
  // If it does then create a new one with the new name
  // The name is created from:
  // DMMDDYY.CSV, where YY is the year MM is the month, DD is the day
  // You must add on the '0' to convert to ASCII

	RTC_GetYYMMDDString(&s_filename[1]);

	if(APP_InDebugMode())
	{
		Serial.println(s_filename);
	}

	if(!s_sd.exists(s_filename))
	{
    // open the file for write at end like the Native SD library
		if (!s_datafile.open(s_filename, O_RDWR | O_CREAT | O_AT_END)) 
		{
      if(APP_InDebugMode())
      {
        Serial.println(PStringToRAM(s_pstrerroropen));
      }
		}
    // if the file opened okay, write to it and sync:
    s_datafile.println(PStringToRAM(s_pstr_headers));
		s_datafile.sync();
	} 

	else
	{
    if(APP_InDebugMode())
    {
      Serial.println(PStringToRAM(s_pstr_file_already_exists));
    }
	}

}

/***************************************************
 *  Name:        SD_WriteIsPending
 *
 *  Returns:     Value of s_writePending
 *
 *  Parameters:  None.
 *
 *  Description: Returns TRUE if application should begin write procedure
 *
 ***************************************************/
 bool SD_WriteIsPending()
 {
  return s_writePending;
 }

/***************************************************
 *  Name:        SD_WriteIsPending
 *
 *  Returns:     None
 *
 *  Parameters:  None
 *
 *  Description: Forces the write pending flag to true
 *
 ***************************************************/
 void SD_ForcePendingWrite()
 {
  s_writePending = true;
 }

 void update_data()
 {
  const char * current_date;
  const char * current_time;

  // *********** WIND SPEED ******************************************
  // Want to get the number of pulses and average into the sample time
  // This gives us the average wind speed
  // pulsecounterold holds the value of pulses.
  // This can be converted into the wind speed using the time and 
  // the pulse-wind speed characterisitic of the anemometer.
  // Do this as post processing - pulse count is most important.

  // *********** WIND DIRECTION **************************************
  // This can be checked every second and an average used
    // If this interrupt has happened then we want to write data to SD card:
  // Save the pulsecounter value (this will be stored to write to SD card)
  WIND_StoreWindPulseCounts();
  WIND_AnalyseWindDirection();

  // *********** TEMPERATURE *****************************************
  // Two versions of this - either with thermistor or I2C sensor (if connected)
  // Thermistor version
  // Get the temperature readings and store to variables   

  BATT_UpdateBatteryVoltage();

    // *********** EXTERNAL VOLTAGE ***************************************
    // From Vcc-680k--46k-GND potential divider
  VA_UpdateExternalVoltage();

    // ********** EXTERNAL CURRENTS **************************************
    // Measured using a hall effect current sensor
    // Either using a ACS*** or a LEM HTFS 200-P
    // Comment out whichever you are not using

  VA_UpdateExternalCurrent();

    // ******** put this data into a file ********************************
    // ****** Check filename *********************************************
    // Each day we want to write a new file.
    // Compare date with previous stored date, every second
  current_date = RTC_GetDate(RTCC_DATE_WORLD);
  current_time = RTC_GetTime();

  s_accumulator.reset();
  s_accumulator.writeChar(s_deviceID[0]);
  s_accumulator.writeChar(s_deviceID[1]);
  s_accumulator.writeChar(comma);
  s_accumulator.writeString(current_date);
  s_accumulator.writeChar(comma);
  s_accumulator.writeString(current_time);

  write_configurable_fields(&s_accumulator);

  s_accumulator.writeChar(comma); 
  BATT_WriteVoltageToBuffer(&s_accumulator);
}

void SD_WriteDataToCard()
{
  
  update_data();

  const char * current_date = RTC_GetDate(RTCC_DATE_WORLD);
  
  if(strcmp(current_date, s_last_used_date) != 0)
  {
     // If date has changed then create a new file
     memcpy(s_last_used_date, current_date, 10);
     SD_CreateFileForToday();  // Create the corrct filename (from date)
  }    

  // ************** Write it to the SD card *************
  // This depends upon the card detect.
  // If card is there then write to the file
  // If card has recently been inserted then initialise the card/filenames
  // If card is not there then flash LEDs

  if(digitalRead(SD_CARD_DETECT_PIN)==LOW&&s_lastCardDetect==HIGH)
  {
    delay(100);  // Wait for switch to settle down.
    // There was no card previously so re-initialise and re-check the filename
    SD_Setup();
    SD_CreateFileForToday();
  }
  if(digitalRead(SD_CARD_DETECT_PIN)==LOW&&s_lastCardDetect==LOW)
  {
      //Ensure that there is a card present)
      // We then write the data to the SD card here:
    writeDataString();
  }
  else
  {
     // print to the serial port too:
    Serial.println(PStringToRAM(s_pstr_noSD));
    Serial.println(s_dataString);
  }   
    
    s_lastCardDetect = digitalRead(SD_CARD_DETECT_PIN);  // Store the old value of the card detect
    
    s_writePending = false;
}

void SD_PrintDataToSerial()
{
  update_data();
  Serial.println(s_dataString);
}

/***************************************************
 *  Name:        SD_SecondTick
 *
 *  Returns:     Nothing.
 *
 *  Parameters:  None.
 *
 *  Description: To be called every second by the RTC library.
 *               Decides when to update the SD card data
 *
 ***************************************************/
void SD_SecondTick()
{
  s_dataCounter++;
  if ((s_writePending == false) && (s_dataCounter >= s_sampleTime))  // This stops us loosing data if a second is missed
  { 
    // Reset the DataCounter
    s_dataCounter = 0;  
    s_writePending = true;
  }
}

/***************************************************
 *  Name:        SD_ResetCounter
 *
 *  Returns:     Nothing.
 *
 *  Parameters:  None.
 *
 *  Description: Forces reset of the data storage counter
 *
 ***************************************************/
void SD_ResetCounter()
{
    s_dataCounter = 0;
}

/***************************************************
 *  Name:        SD_ResetCounter
 *
 *  Returns:     TRUE if SD card is present
 *
 *  Parameters:  None.
 *
 *  Description: Tests the card detect pin (LOW if card present)
 *
 ***************************************************/
bool SD_CardIsPresent()
{
  return digitalRead(SD_CARD_DETECT_PIN)==LOW;
}
