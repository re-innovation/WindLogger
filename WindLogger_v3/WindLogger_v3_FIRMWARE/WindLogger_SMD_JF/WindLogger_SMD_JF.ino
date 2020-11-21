/********************************************************
/****** Wind Logger based upon the:  ********************
/****** DataDuino - Arduino DAQ UNIT ********************
/****** by Matt Little **********************************
/****** Date: 29/1/2014 *********************************
/****** info@re-innovation.co.uk ************************
/****** www.re-innovation.co.uk *************************
/********************************************************

  See www.re-innovation.co.uk for information and construction details
  
  This is sample code for the DataDuino.

/*************Details of Code*****************************

  The DataDuino is an Arduino based SD card datalogger.
  A PCF8563 Real Time Clock is used to timestamp the data.
  
  The pin assisgnments are as follows:
  D0 - Rx Serial Data
  D1 - Tx Serial Data
  D2 - RTC interrupt (1 second pulses)
  D3 - Anemometer 1 (pulse interrupt) (& Power to External Unit - eg GSM)
  D4 - LED DATA output
  D5 - Anemometer 2 (pulse interrupt)
  D6 - Calibrate Switch (pull LOW to set)
  D7 - Rx_GSM
  D8 - Tx_GSM
  D9 - Card Detect (SD)
  D10 - Chip Select - SD card
  D11 - MOSI SD card
  D12 - MISO SD card
  D13 - SPI CLock SD card
  
  A0 - Wind Vane measurement with 10k pull-down
  A1 - Internal battery voltage measurement (from potential divider)
  A2 - External voltage measurement (with potential divider) 0-40V DC
  A3 - Current measurement
  A4 - SDA - I2C connection to RTC
  A5 - SCK - I2C connection to RTC
  
  Counts pulses from a sensor (such as a anemometer or flow sensor)
  These are pulses are averaged into a wind speed.
  We also keep a record of the maximum in the averaging period.
  For turbulence measurement 
  
  Each logger has a reference (user adjustable).
  
  Data is written to a .csv file created on an SD card.
  A new file is created each day. If file alreay present then data is appended.
  The file name is created from the reference number and the date in the format:
  DXXXXXX.csv, where DXXXXXX is the date in the format DDMMYY. 
  
  Data is stored with human readable headers:

  // This will be added to a seperate 'calibrate' mode
  When in Calibrate mode:
  You can adjust the parameters of the device using serial commands. These parameters are stored in EEPROM.
  These are:
  "T??????E"
  This will change the time to HHMMSS
  "??????E"
  This will change the date to DDMMYY
  "S?????E"
  This will change the sample period to ????? seconds. Set to 00001 for 1 second data, set to 03600 for 1 hour data.
  The minimum is 1 second data. The maximum is 99999 seconds
  "R??E"
  This will change the reference to ??. 
  "OE"
  This will take the current reading and write it to the current offset.
  "V1???E"
  "V2???E"
  These select the two resistor values from the resistor potential divider.
  Values are in k Ohm.
  R1 is the top resistor, R2 is the lower resistor.
  "I???E"
  This sets the current gain value, in mV/A
 
  
  // Addedd Interrupt code from here:
  // PinChangeIntExample, version 1.1 Sun Jan 15 06:24:19 CST 2012
  // See the Wiki at http://code.google.com/p/arduino-pinchangeint/wiki 
  // for more information.
 
  Updates: 
  30/10/12 Code written - Matt Little
  31/10/12 writing code description and sorting out new sample timings - Matt Little
  19/9/13  Getting code to work on Arduino 1.0.5 - Matt Little
  5/2/14   Adding wind logging sections - Matt Little
  5/2/14   Sorting out sleep modes - Matt Little
  5/2/14   Adding HIH humidity/temperature sensor - Matt Little
  5/2/14   Sorting out Card Detect when card removed - Matt Little
  10/2/14  Sorting out card re-enter issue - Matt Little
  17/6/14  Adding pull-up enable for calibrate pin.- Matt Little
  17/6/14  Removing humidity sensor routines. Not needed -  Matt Little
  17/6/14  Adding debounce timer for anemometer pulses - Matt Little
  17/6/14  Adding Direction vane input - Matt Little
  10/7/14  Adding additional Anemometer input - Matt Little
  10/7/14  Needs additional interrupt pin added - Matt Little
  13/8/14  Added wind direction data - Matt Little
  15/8/14  Added 'I'm Alive' LED flash every 5 seconds - Matt Little
  11/3/15  Added string output when NO SD and in Calibrate Mode
  18/3/15  Changed code for V and I SMD design - Matt Little
  20/3/15  More work on V and I code. SMD design - Matt Little
  15/4/15  Re-doing code for new hardware - Matt Little
  21/4/15  Adding voltage and current calibration factors in serial - Matt Little
  
  TO DO
  Sort out Voltage conversion (via serial) - implemented - TEST
  Sort out Current conversion (via serial) - implemented - TEST
  Sort out RPM sensor (want RPM value, so convert correctly)
  Sort out maximum wind speed in time period
 
 //*********SD CARD DETAILS***************************	
 The SD card circuit:
 SD card attached to SPI bus as follows:
 ** MOSI - pin 11
 ** MISO - pin 12
 ** CLK - pin 13
 ** CS - pin 10
 ** Card detect - pin 9
 
**********************************************************************************************************/


/************ External Libraries*****************************/
#include <stdlib.h>
#define LIBCALL_ENABLEINTERRUPT
#include <EnableInterrupt.h>
#include <Wire.h>          // Required for RTC
#include <Rtc_Pcf8563.h>   // RTC library
#include <SdFat.h>            // SD card library
#include <avr/pgmspace.h>  // Library for putting data into program memory
#include <EEPROM.h>        // For writing values to the EEPROM

/************ Application Libraries*****************************/

#include "app.h"
#include "utility.h"
#include "sleep.h"
#include "eeprom_storage.h"
#include "battery.h"
#include "external_volts_amps.h"
#include "serial_handler.h"
#include "wind.h"
#include "temperature.h"
#include "rtc.h"
#include "sd.h"

/********* I/O Pins *************/
#define RED_LED_PIN 4      // The output led is on pin 4
#define CALIBRATE_PIN 6   // This controls if we are in serial calibrate mode or not

#define FLASH_PERIOD (10)
static int s_aliveFlashCounter = 0;  // This is used to count to give flash every 10 seconds
static bool s_debugFlag = false;    // Set this if you want to be in debugging mode.
static bool s_error = false;
static bool s_calibrate_mode = false;

//**********STRINGS TO USE****************************

// These are Char Strings - they are stored in program memory to save space in data memory
// These are a mixutre of error messages and serial printed information
const char error[] PROGMEM = "ERROR";
const char dateerror[] PROGMEM = "Date ERR";

/***************************************************
 *  Name:        ledOn, ledOff
 *
 *  Returns:     Nothing.
 *
 *  Parameters:  None
 *
 *  Description: Turns status LED on or off            
 *
 ***************************************************/
static void ledOn()
{
  pinMode(RED_LED_PIN,OUTPUT);
  digitalWrite(RED_LED_PIN, HIGH);
}

static void ledOff()
{
  digitalWrite(RED_LED_PIN, LOW);
  // Set LED to be an INPUT - saves power 
  pinMode(RED_LED_PIN,INPUT);
}

/***************************************************
 *  Name:        flashLED
 *
 *  Returns:     Nothing.
 *
 *  Parameters:  None
 *
 *  Description: Per-second LED flash            
 *
 ***************************************************/
static void flashLED()
{
  pinMode(RED_LED_PIN,OUTPUT);
    
  if (s_error)
  {
    for(int x=0;x<=5;x++)
    {
      digitalWrite(RED_LED_PIN, HIGH);
      delay(5);
      digitalWrite(RED_LED_PIN, LOW);
      delay(50);     
    }
  }
  else
  {
    // Flash the LED every FLASH_PERIOD seconds to show alive
    if(s_aliveFlashCounter >= FLASH_PERIOD)
    {
      digitalWrite(RED_LED_PIN, HIGH);
      delay(5);
      digitalWrite(RED_LED_PIN, LOW); 
      s_aliveFlashCounter=0;
    }
  }

  pinMode(RED_LED_PIN,INPUT);
}

/***************************************************
 *  Name:        handleCalibration
 *
 *  Returns:     Nothing.
 *
 *  Parameters:  None
 *
 *  Description: Per-second LED flash            
 *
 ***************************************************/
static void handleCalibration()
{
  Serial.println("Calibrate");    
  SERIAL_HandleCalibrationData();
  delay(500);  // Some time to read data
  SD_PrintDataToSerial(); 
  Serial.flush();    // Force out the end of the serial data
}

/***************************************************
 *  Name:        readInputs
 *
 *  Returns:     Nothing.
 *
 *  Parameters:  None.
 *
 *  Description: Read the SD card present and calibrate inputs.           
 *
 ***************************************************/
void readInputs()
{
  s_error = !SD_CardIsPresent();
  s_calibrate_mode = (digitalRead(CALIBRATE_PIN)== HIGH);
}

/***************************************************
 *  Name:        setup
 *
 *  Returns:     Nothing.
 *
 *  Parameters:  None.
 *
 *  Description: Setup for the Arduino.           
 *
 ***************************************************/
void setup()
{
  Serial.begin(115200);
  Wire.begin();

  SD_Setup();
  
  //Set up digital data lines
  pinMode(CALIBRATE_PIN,INPUT_PULLUP);
  
  analogReference(EXTERNAL);  // This should be default, but just to be sure

  // Analog lines
  pinMode(VANE_PIN,INPUT);
  pinMode(VOLTAGE_PIN,INPUT);
  pinMode(CURRENT_1_PIN,INPUT); 

  // Initialise the real time clock (A4 = scl, A5 = sda, 2 = 1Hz clock input)
  RTC_Setup(A4, A5, 2);  
  
  SD_CreateFileForToday();  // Create the corrct filename (from date)

  // Read the reference number from the EEPROM
  char deviceID[2];
  EEPROM_GetDeviceID(deviceID);
  SD_SetDeviceID(deviceID);
  
  // Read in the sample time from EEPROM
  SD_SetSampleTime( EEPROM_GetSampleTime() );
  
  // Read the Current Voltage Offset from the EEROM
  VA_SetCurrentOffset( EEPROM_GetCurrentOffset() );

  VA_SetVoltageDivider(
    EEPROM_GetR1(),
    EEPROM_GetR2()
  );

  // read the current gain value
  VA_SetCurrentGain( EEPROM_GetCurrentGain() );
  
  WIND_SetWindvanePosition( EEPROM_GetWindwavePosition() );
  
  // Interrupt for the 1Hz signal from the RTC
  RTC_EnableInterrupt();

  // Attach interrupts for the pulse counting
  WIND_SetupWindPulseInterrupts();
}

/***************************************************
 *  Name:        main loop
 *
 *  Returns:     Nothing.
 *
 *  Parameters:  None.
 *
 *  Description: Main application loop.
 *
 ***************************************************/
void loop()
{

  readInputs();

  // *********** WIND DIRECTION **************************************  
  // Want to measure the wind direction every second to give good direction analysis
  // This can be checked every second and an average used
  WIND_ConvertWindDirection(analogRead(VANE_PIN));    // Run this every second. It increments the windDirectionArray 
  
  flashLED();

  if(SD_WriteIsPending())
  {  
    ledOn();
    SD_WriteDataToCard();
    // Finish up write routine here:    
    ledOff();
    Serial.flush();    // Force out the end of the serial data
  }
   
  WIND_Debug();
  
  if(s_calibrate_mode)
  {    
    handleCalibration();
  }
  else
  {     
    // This function blocks in sleep until the next RTC interrupt
    SLEEP_SetWakeOnRTCAndSleep();
  }  
}

/* 
 * APP_SecondTick
 * Called by the RTC handler every second
 */
void APP_SecondTick()
{
  s_aliveFlashCounter++;  
}

/* 
 * APP_InDebugMode
 * Used by other modules to check if the application is in debug mode
 */
bool APP_InDebugMode()
{
  return s_debugFlag;
}
