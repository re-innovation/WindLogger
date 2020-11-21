# WindLoggerVersion3 - Arduino Code

  See www.re-innovation.co.uk for information and construction details

  The DataDuino is an Arduino based SD card datalogger.
  A PCF8563 Real Time Clock is used to timestamp the data.

  The logger is based on the Arduino Uno (ATMEGA328P).
  This board option should be selected in the Arduino IDE for programming.

## Features
  Counts pulses from a sensor (such as a anemometer or flow sensor)
  These are pulses are averaged into a wind speed.
  
  We also keep a record of the maximum in the averaging period.
  For turbulence measurement 
  
  Each logger has a reference number (user adjustable).
  
  Data is written to a .csv file created on an SD card.
  
  A new file is created each day. If file already present then data is appended.
  The file name is created from the current date in the format DXXXXXX.csv, where DXXXXXX is the date in the format YYMMDD. 
  
  Data is stored with a human readable header line at the start of the file.

## Using the code

  The code is a standard Arduino application. You can place the repository inside your Arduino sketches directory. 

  In order to build the software, additional arduino libraries are required.
  
  ### Logger configuration

  In app.h, you can set which fields are recorded.
  Each field has a READ define (for example READ_TEMPERATURE).
  To enable recording of this field, set this define to 1.
  To disable recording of this field, set this define to 0.
  
  ### Adding new fields

  To add a new field to the logger software (for example pressure):
  1. Create a define in app.h that will enable/disable the new field (for example READ_PRESSURE)
  2. Create the module .cpp and .h files. In the .h file, define the headers, e.g.

  ```
  #if READ_PRESSURE == 1
  #define PRESSURE_HEADERS "Pressure mb, "
  #else
  #define PRESSURE_HEADERS ""
  #endif
  ```

  3. In sd.cpp:
    1. Add the header entry to the s_pstr_headers string in the desired position. Be sure to add a trailing slash as per the exisiting entries.
    2. Find the write_configurable_fields function.
      * Add the necessary lines of code to write the new data string to the accumulator.
      * Ensure you surround the new code with ifdef...endif for the field.
      * Ensure you include the line to add a comma before the data.
      For example:

  ```
  #if READ_PRESSURE == 1
  accum->writeChar(comma);
  PRESS_WritePressureToBuffer(accum);
  #endif

  ```


### Required libraries:
  ####[https://github.com/GreyGnome/EnableInterrupt](EnableInterrupt by Mike Schwager)

  On Arduino versions > 1.6.2 this library can be installed via the Library Manager. Search for 'EnableInterrupt' in the manager.

  The WindLogger software is built with EnableInterrupt v0.9.4. Later versions may not be supported.

  ####[https://github.com/greiman/SdFat](SfFat by William Greiman)

  Download the repository from GitHub and copy the 'SdFat' folder to your local Arduino libraries folder.

  ####[http://playground.arduino.cc/Main/RTC-PCF8563](RTC-PCF8563 by Joe Robertson)

  Follow the 'Download, install and import' instructions on that page.

  #### Libraries in this repository

  As an alternative to download the libraries individually, this repository contains suitable versions of each library. Unzip "3rd-party-libraries.zip" into your local Arduino libraries folder.

## Calibrate Mode

  In calibrate mode you can adjust the parameters of the device using serial commands.
  These parameters are stored in device EEPROM so they will be saved if the logger is turned off.
  
  The serial commands are:

  "T??????E"
  
  This will change the time to HHMMSS
  
  "D??????E"
  
  This will change the date to DDMMYY
  
  "S?????E"
  
  This will change the sample period to ????? seconds. Set to 00001 for 1 second data, set to 03600 for 1 hour data.
  The minimum is 1 second data. The maximum is 99999 seconds
  
  "R??E"
  
  This will change the logger reference to ??. 
  
  "OE"
  
  This will take the current reading and write it to the current offset.
  
  "V1???E" &  "V2???E"
  
  These select the two resistor values from the resistor potential divider.
  Values are in k Ohm.
  
  R1 is the top resistor, R2 is the lower resistor.
  
  "I???E"
  
  This sets the current gain value, in mV/A

  "W1E" or "W0E" 
  
  "W1E" sets the windwave potentiometer to be on the HIGH side of the potential divider.
  "W0E" sets the windwave potentiometer to be on the LOW side of the potential divider.

## Pin Assignments
  
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
  
