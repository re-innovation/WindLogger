#ifndef _EXTERNAL_VOLTS_AMPS_H_
#define _EXTERNAL_VOLTS_AMPS_H_

// Defines
#define VOLTAGE_PIN A2  // The external voltage with a potential divider (680k // 46k)
#define CURRENT_1_PIN A3  // Current from a hall effect sensor

#if READ_EXTERNAL_AMPS == 1
#define EXTERNAL_AMPS_HEADERS "Current, "
#else
#define EXTERNAL_AMPS_HEADERS ""
#endif

#if READ_EXTERNAL_VOLTS == 1
#define EXTERNAL_VOLTS_HEADERS "Ext V, "
#else
#define EXTERNAL_VOLTS_HEADERS ""
#endif

// Public Functions
void VA_SetCurrentOffset(int newOffset);
void VA_SetCurrentGain(int newGain);

void VA_SetVoltageDivider(uint16_t newR1, uint16_t newR2);

void VA_StoreNewCurrentOffset(void);
void VA_StoreNewResistor1(int value);
void VA_StoreNewResistor2(int value);
void VA_StoreNewCurrentGain(int value);

void VA_UpdateExternalVoltage(void);
void VA_UpdateExternalCurrent(void);

void VA_WriteExternalVoltageToBuffer(FixedLengthAccumulator * accum);
void VA_WriteExternalCurrentToBuffer(FixedLengthAccumulator * accum);

#endif
