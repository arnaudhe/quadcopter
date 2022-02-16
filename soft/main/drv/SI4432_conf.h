#pragma once

#include <drv/SI4432_defs.h>

#define SI4432_MODEM_SOURCE         SI4432_MODULATION_SOURCE_FIFO
#define SI4432_MODEM_TYPE           SI4432_MODULATION_TYPE_FSK
#define SI4432_MODEM_ENCODING       SI4432_DATA_MANCHESTER_ENABLED | SI4432_DATA_MANCHESTER_INVERSION | SI4432_DATA_MANCHESTER_POLARITY
#define SI4432_MODEM_FREQ           433.907
#define SI4432_MODEM_DEVIATION      30000
#define SI4432_MODEM_DATARATE       4800
#define SI4432_MODEM_TX_POWER       SI4432_TX_POWER_20dBm

#define SI4432_PH_PATH              SI4432_PH_PATH_TXRX     // active tx and rx packet handling
#define SI4432_PH_PREAMBLE_LENGTH   8                       // emit 8 * 4 bits of preamble
#define SI4432_PH_PREAMBLE_DETECT   5                       // validate 5 * 4 bits of preamble
#define SI4432_PH_SYNC_LENGTH       SI4432_PH_SYNC_2        // Use 2 bytes of sync word
#define SI4432_PH_SYNC_BYTES        "\x5C\x22"              // Use 0x5C 0x22
#define SI4432_PH_HEADER_LENGTH     SI4432_PH_HEADER_OFF
#define SI4432_PH_CRC               SI4432_PH_CRC_OFF
#define SI4432_PH_CRC_TYPE          SI4432_PH_CRC_TYPE_CITT

#define SI4432_IRQ_ENABLE_1         (SI4432_IRQ1_PACKET_SENT | SI4432_IRQ1_VALID_PACKET_RECEIVED)
#define SI4432_IRQ_ENABLE_2         (SI4432_IRQ2_SYNC_WORD_DETECT | SI4432_IRQ2_VALID_PREAMBLE_DETECT)

/* GPIO0 AND GPIO1 are used to drive antenna switch */
#define SI4432_GPIO0_CONFIG         0x12  // TX State (output)
#define SI4432_GPIO1_CONFIG         0x15  // RX State (output)
#define SI4432_GPIO2_CONFIG         0x0f  // data clock
