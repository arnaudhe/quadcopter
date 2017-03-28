#pragma once

#include <drv/HMC5883L_defs.h>

#define HMC5883L_ADDRESS             (0x1E)

#define HMC5883L_MAG_SENSIVITY_SEL   HMC5883L_RANGE_1_9GA
#define HMC5883L_MAG_SENSIVITY       1.22f        ///< milliGauss per digit

#define HMC5883L_WAKEUP_DELAY_ms     10
#define HMC5883L_CONFIG_DELAY_ms     6