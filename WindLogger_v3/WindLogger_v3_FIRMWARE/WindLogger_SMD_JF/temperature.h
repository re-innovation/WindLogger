#ifndef _TEMPERATURE_H_
#define _TEMPERATURE_H_

#if READ_TEMPERATURE == 1
#define TEMPERATURE_HEADERS "Temp C, "
#else
#define TEMPERATURE_HEADERS ""
#endif

void TEMP_WriteTemperatureToBuffer(FixedLengthAccumulator * accum);

#endif
