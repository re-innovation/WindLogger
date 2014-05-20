Here are the open-source files for a Wind Speed Logger.
Based upon the DataDuino (an Arduino based SD card datalogger).


The Wind Logger was designed by Matt Little of www.re-innovation.co.uk
Contact: info@re-innovation.co.uk

Please see: www.re-innovation.co.uk for more details.

The items in this repository include:

	The PCB files were created in KiCAD PCB design software

	Example code has been written for the Arduino IDE.

	The CAD design for fixings


Overview of the design:

  A PCF8563 Realt Time Clock is used to timestamp the data.
  
  Pin D3 is set up to cound pulses from a sensor (such as a anemometer or flow sensor)
  Pins D7,D8,D9 are set up to record digital information (0 or 1)
  Pins A0 to A3 are set up to record analogue information (0 to 1024)
  
  Each logger has a reference (user adjustable from 00-99).
  
  Data is written to a .csv file created on an SD card.
  A new file is created each day. If file alreay present then data is appended.
  The file name is created from the reference number and the date in the format:
  RXXDXXXX.csv, where RXX is the reference number and DXXXX is the date in the format DDMM. 
  
  Data is stored with human readable headers:
  "Reference, Time, Date, Pulses, Temp1, Temp2, Temp3, Temp4, D7,D8,D9,A0,A1,A2,A3"
  
  You can adjust the parameters of the device using serial commands. These parameters are stored in EEPROM.
  These are:
  R??E
  This will change the reference number to ??
  T??????E
  This will change the time to HHMMSS
  D??????E
  This will change the date to DDMMYY
  S?????E
  This will change the sample period to ????? seconds. Set to 00001 for 1 second data, set to 03600 for 1 hour data.
  The minimum is 1 second data. The maximum is 99999 seconds

 

Modified:
10/5/14	GitHub Repository created - Matt Little
 
