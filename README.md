# Low Cost Data Logger for Wind Speed Parameters
This repository has the open-source files for a Data Logger for Wind Speed and other small wind turbine parameters.

So far there have been two different versions of this unit:
* Version 1 was released in 2014. This was basic add on shield for the 'DataDuino' (an Arduino based SD card datalogger from www.re-innovation.co.uk)
* Version 2 was released on 2016. This was a surface mount design with additional functions such as solar PV charging.

The Wind Logger was originally designed by Matt Little of www.re-innovation.co.uk
Contact: info@re-innovation.co.uk

Please see: www.re-innovation.co.uk and search for 'wind datalogger' for more details.
Some more details are available here:
https://www.re-innovation.co.uk/docs/wind-datalogger/

The folders in this repository include:
	
* WindLogger_PCB
The PCB files were created in KiCAD PCB design software
These are in Windlogger_PCB
There are the KICAD project files and the GERBER files for both the DataDuino and the Windlogger Shield	

* WindLogger_FIRMWARE
Example code has been written for uploading via the Arduino IDE.

* WindLogger_FIXINGS
The CAD design for fixing system for an anemometer and a wind vane.
These is a work in progress. Originally draw using CorelDraw - I hope to update/convert this soon.
	
* WindLogger_INSTRUCTIONS
These are the instructions in a number of formats and languages.

## Version 1 - 2014
Overview of the design:
  A PCF8563 Real Time Clock is used to timestamp the data.

  Pin D3 is set up to cound pulses from a sensor (such as a anemometer or flow sensor)
 
  Pins D7,D8,D9 are set up to record digital information (0 or 1)
  
  Pins A0 to A3 are set up to record analogue information (0 to 1024)
  
  Each logger has a reference (user adjustable from 00-99).
  
  Data is written to a .csv file created on an SD card.
  
  A new file is created each day. If file alreay present then data is appended.
  
  The file name is created from the reference number and the date in the format:
  
  RXXDXXXX.csv, where RXX is the reference number and DXXXX is the date in the format DDMM. 
  
  Data is stored with human readable headers. Check the Arduino code for these.
  
  You can adjust the parameters of the device using serial commands. These parameters are stored in EEPROM.
  These are:
  * R??E - This will change the reference number to ??
  * T??????E - This will change the time to HHMMSS
  * D??????E - This will change the date to DDMMYY
  * S?????E - This will change the sample period to ????? seconds. Set to 00001 for 1 second data, set to 03600 for 1 hour data. (The minimum is 1 second data. The maximum is 99999 seconds)

## Version 3 - 2016
 
This newer version mainly covers a new circuit board which was designed as a surface mount design. The Code on this board has additional changes, but not too different functionality.

## Version 4 - 2020

This is work in progress. I'm trying to create a better PCB design which will also have WiFi and GPRS/2G connectivity for your data.

The wind speed logging unit is a plug in board onto a data-logging main unit.


##  Data Processing
These are programs to take the data on the SD card and produce nice graphs with it.
This is a work in progress.
https://github.com/re-innovation/CSVviewer

 
## Changes to do!

* PCF8563 - not accurate enough. Use DS3231 with temperature compensation.
* Add data upload via WiFi
* Add data upload via GPRS

