#ifndef _WIND_H_
#define _WIND_H_

// Defines
#define VANE_PIN A0      // The wind vane with a 10k pullup or pulldown
#define ANEMOMETER1 3  //   This is digital pin the pulse is attached to
#define ANEMOMETER2 5  //   This is digital pin the pulse is attached to

#if READ_WINDSPEED == 1
#define WINDSPEED_HEADERS "Wind 1, Wind 2, "
#else
#define WINDSPEED_HEADERS ""
#endif

#if READ_WIND_DIRECTION == 1
#define WIND_DIRECTION_HEADERS "Direction, "
#else
#define WIND_DIRECTION_HEADERS ""
#endif

// Public Functions

void WIND_SetupWindPulseInterrupts();

void WIND_SetWindvanePosition(bool windwave_is_at_top_of_divider);

void WIND_ConvertWindDirection(int reading);
void WIND_AnalyseWindDirection();

void WIND_WritePulseCountToBuffer(uint8_t counter, FixedLengthAccumulator * accum);
void WIND_WriteDirectionToBuffer(FixedLengthAccumulator * accum);

long WIND_GetLivePulseCount(uint8_t counter);

void WIND_StoreWindPulseCounts();
void WIND_Debug();

#endif