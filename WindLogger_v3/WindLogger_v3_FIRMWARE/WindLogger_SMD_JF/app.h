#ifndef _APP_H_
#define _APP_H_

/*
 * Defines and typedefs
 */

// If READ_WINDSPEED is 1, the windspeed will be read and included in serial data
#define READ_WINDSPEED 1

// If READ_WIND_DIRECTION is 1, the windspeed will be read and included in serial data
#define READ_WIND_DIRECTION 1

// If READ_TEMPERATURE is 1, the temperature will be read and included in serial data
#define READ_TEMPERATURE 0

// If READ_IRRADIANCE is 1, the irradiance will be read and included in serial data
#define READ_IRRADIANCE 1

// If READ_EXTERNAL_VOLTS is 1, the external voltage will be read and included in serial data
#define READ_EXTERNAL_VOLTS 0

// If READ_EXTERNAL_AMPS is 1, the external current will be read and included in serial data
#define READ_EXTERNAL_AMPS 0

/*
 * Application functions
 */

void APP_SecondTick();
bool APP_InDebugMode();

#endif
