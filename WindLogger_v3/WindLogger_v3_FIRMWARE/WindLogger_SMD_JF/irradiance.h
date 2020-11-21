#ifndef _IRRADIANCE_H_
#define _IRRADIANCE_H_

#if READ_IRRADIANCE == 1
#define IRRADIANCE_HEADERS "Irradiance Wm-2, "
#else
#define IRRADIANCE_HEADERS ""
#endif

void IRR_WriteIrradianceToBuffer(FixedLengthAccumulator * accum);

#endif
