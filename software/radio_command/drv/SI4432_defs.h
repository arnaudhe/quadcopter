#pragma once

typedef enum {
    SI4432_PH_PATH_OFF     = 0b00000000,
    SI4432_PH_PATH_RX_ONLY = 0b10000000,
    SI4432_PH_PATH_TX_ONLY = 0b00001000,
    SI4432_PH_PATH_TXRX = SI4432_PH_PATH_RX_ONLY | SI4432_PH_PATH_TX_ONLY
} SI4432_PH_PATH;

typedef enum {
    SI4432_PH_CRC_OFF = 0b00000000,
    SI4432_PH_CRC_ON  = 0b00000100,
    SI4432_PH_CRC_DATA_ONLY = SI4432_PH_CRC_ON | 0b00100000
} SI4432_PH_CRC;

typedef enum {
    SI4432_PH_CRC_TYPE_CITT       = 0b00000000,
    SI4432_PH_CRC_TYPE_IBM16      = 0b00000001,
    SI4432_PH_CRC_TYPE_IEC16      = 0b00000010,
    SI4432_PH_CRC_TYPE_BIACHEVA   = 0b00000011
} SI4432_PH_CRC_TYPE;

typedef enum {
    SI4432_PH_HEADER_OFF  = 0b00000000,
    SI4432_PH_HEADER_1    = 0b00010000,
    SI4432_PH_HEADER_2    = 0b00100000,
    SI4432_PH_HEADER_3    = 0b00110000,
    SI4432_PH_HEADER_4    = 0b01000000
} SI4432_PH_HEADER_LENGTH;

typedef enum {
    SI4432_PH_SYNC_1    = 0b00000000,
    SI4432_PH_SYNC_2    = 0b00000010,
    SI4432_PH_SYNC_3    = 0b00000100,
    SI4432_PH_SYNC_4    = 0b00000110
} SI4432_PH_SYNC_LENGTH;

/*
    Output power in Si4432 corresponds to +20dBm
*/

typedef enum {
    SI4432_TX_POWER_1dBm    = 0b000,
    SI4432_TX_POWER_2dBm    = 0b001,
    SI4432_TX_POWER_5dBm    = 0b010,
    SI4432_TX_POWER_8dBm    = 0b011,
    SI4432_TX_POWER_11dBm   = 0b100,
    SI4432_TX_POWER_14dBm   = 0b101,
    SI4432_TX_POWER_17dBm   = 0b110,
    SI4432_TX_POWER_20dBm   = 0b111,
} SI4432_TX_POWER;

/*
    Tx settings. Default values:
    * data rate < 30 kbps
    * No TX data clock (because we use FIFO mode)
    * Frequency deviation < 160 kHz
*/

#define SI4432_DATA_WHITENING_ENABLED     0b00000001
#define SI4432_DATA_MANCHESTER_ENABLED    0b00000010
#define SI4432_DATA_MANCHESTER_INVERSION  0b00000100
#define SI4432_DATA_MANCHESTER_POLARITY   0b00001000

#define SI4432_OPERATION_MODE_READY       0b00000001
#define SI4432_OPERATION_MODE_TUNE        0b00000010
#define SI4432_OPERATION_MODE_RX          0b00000100
#define SI4432_OPERATION_MODE_TX          0b00001000
#define SI4432_OPERATION_MODE_RESET       0b10000000

#define SI4432_STARTUP_TIME               20

//Force recalibration
#define SI4432_FORCE_RACALIBRATION_VAL    0b00011110

typedef enum {
    SI4432_MODULATION_SOURCE_GPIO    = 0b00000000,
    SI4432_MODULATION_SOURCE_SDI     = 0b00010000,
    SI4432_MODULATION_SOURCE_FIFO    = 0b00100000,
    SI4432_MODULATION_SOURCE_PN9     = 0b00110000
} SI4432_MODULATION_SOURCE;

typedef enum {
    SI4432_MODULATION_TYPE_UNMODULATED    = 0b00000000,
    SI4432_MODULATION_TYPE_OOK            = 0b00000001,
    SI4432_MODULATION_TYPE_FSK            = 0b00000010,
    SI4432_MODULATION_TYPE_GFSK           = 0b00000011
} SI4432_MODULATION_TYPE;

#define SI4432_TX_DATARATE_SCALE               0b00100000

#define SI4432_IRQ1_FIFO_UNDER_OVERFLOW_ERROR  0b10000000
#define SI4432_IRQ1_TX_FIFO_ALMOST_FULL        0b01000000
#define SI4432_IRQ1_TX_FIFO_ALMOST_EMPTY       0b00100000
#define SI4432_IRQ1_RX_FIFO_ALMOST_FULL        0b00010000
#define SI4432_IRQ1_EXTERNAL_INTERRUPT         0b00001000
#define SI4432_IRQ1_PACKET_SENT                0b00000100
#define SI4432_IRQ1_VALID_PACKET_RECEIVED      0b00000010
#define SI4432_IRQ1_CRC_ERROR                  0b00000001

#define SI4432_IRQ2_SYNC_WORD_DETECT           0b10000000
#define SI4432_IRQ2_VALID_PREAMBLE_DETECT      0b01000000
#define SI4432_IRQ2_INVALID_PREAMBLE_DETECT    0b00100000
#define SI4432_IRQ2_RSSI                       0b00010000
#define SI4432_IRQ2_WAKE_UP_TIMER              0b00001000
#define SI4432_IRQ2_LOW_BATTERY_DETECT         0b00000100
#define SI4432_IRQ2_CHIP_READY                 0b00000010
#define SI4432_IRQ2_POWER_ON_RESET             0b00000001

/* Registers */

#define SI4432_REG_DEV_TYPE                    0x00
#define SI4432_REG_DEV_VERSION                 0x01
#define SI4432_REG_DEV_STATUS                  0x02

#define SI4432_REG_IRQ_STATUS_1                0x03
#define SI4432_REG_IRQ_STATUS_2                0x04
#define SI4432_REG_IRQ_ENABLE_1                0x05
#define SI4432_REG_IRQ_ENABLE_2                0x06
#define SI4432_REG_STATE                       0x07
#define SI4432_REG_OPERATION_CONTROL           0x08

#define SI4432_REG_CRYSTAL_LOAD_CAPACITANCE    0x09

#define SI4432_REG_GPIO0_CONF                  0x0B
#define SI4432_REG_GPIO1_CONF                  0x0C
#define SI4432_REG_GPIO2_CONF                  0x0D
#define SI4432_REG_IOPORT_CONF                 0x0E

#define SI4432_REG_IF_FILTER_BW                0x1C
#define SI4432_REG_AFC_LOOP_GEARSHIFT_OVERRIDE 0x1D
#define SI4432_REG_AFC_TIMING_CONTROL          0x1E
#define SI4432_REG_CLOCK_RECOVERY_GEARSHIFT    0x1F
#define SI4432_REG_CLOCK_RECOVERY_OVERSAMPLING 0x20
#define SI4432_REG_CLOCK_RECOVERY_OFFSET2      0x21
#define SI4432_REG_CLOCK_RECOVERY_OFFSET1      0x22
#define SI4432_REG_CLOCK_RECOVERY_OFFSET0      0x23
#define SI4432_REG_CLOCK_RECOVERY_TIMING_GAIN1 0x24
#define SI4432_REG_CLOCK_RECOVERY_TIMING_GAIN0 0x25
#define SI4432_REG_RSSI                        0x26
#define SI4432_REG_RSSI_THRESHOLD              0x27

#define SI4432_REG_AFC_LIMITER                 0x2A
#define SI4432_REG_AFC_CORRECTION_READ         0x2B

#define SI4432_REG_DATA_ACCESS_CONTROL         0x30
#define SI4432_REG_EZMAC_STATUS                0x31
#define SI4432_REG_HEADER_CONTROL1             0x32
#define SI4432_REG_HEADER_CONTROL2             0x33
#define SI4432_REG_PREAMBLE_LENGTH             0x34
#define SI4432_REG_PREAMBLE_DETECTION          0x35
#define SI4432_REG_SYNC_WORD3                  0x36
#define SI4432_REG_SYNC_WORD2                  0x37
#define SI4432_REG_SYNC_WORD1                  0x38
#define SI4432_REG_SYNC_WORD0                  0x39
#define SI4432_REG_TRANSMIT_HEADER3            0x3A
#define SI4432_REG_TRANSMIT_HEADER2            0x3B
#define SI4432_REG_TRANSMIT_HEADER1            0x3C
#define SI4432_REG_TRANSMIT_HEADER0            0x3D

#define SI4432_REG_TX_PKT_LEN                  0x3E

#define SI4432_REG_CHECK_HEADER3               0x3F
#define SI4432_REG_CHECK_HEADER2               0x40
#define SI4432_REG_CHECK_HEADER1               0x41
#define SI4432_REG_CHECK_HEADER0               0x42

#define SI4432_REG_RECEIVED_HEADER3            0x47
#define SI4432_REG_RECEIVED_HEADER2            0x48
#define SI4432_REG_RECEIVED_HEADER1            0x49
#define SI4432_REG_RECEIVED_HEADER0            0x4A

#define SI4432_REG_RX_PKT_LENGTH               0x4B

#define SI4432_REG_CHARGEPUMP_OVERRIDE         0x58
#define SI4432_REG_DIVIDER_CURRENT_TRIM        0x59
#define SI4432_REG_VCO_CURRENT_TRIM            0x5A

#define SI4432_REG_AGC_OVERRIDE                0x69

#define SI4432_REG_TX_POWER                    0x6D
#define SI4432_REG_TX_DATARATE1                0x6E
#define SI4432_REG_TX_DATARATE0                0x6F

#define SI4432_REG_MODULATION_MODE1            0x70
#define SI4432_REG_MODULATION_MODE2            0x71

#define SI4432_REG_FREQ_DEVIATION              0x72
#define SI4432_REG_FREQ_OFFSET1                0x73
#define SI4432_REG_FREQ_OFFSET2                0x74
#define SI4432_REG_FREQ_BAND_SELECT            0x75
#define SI4432_REG_FREQCARRIER_H               0x76
#define SI4432_REG_FREQCARRIER_L               0x77

#define SI4432_REG_FREQCHANNEL                 0x79
#define SI4432_REG_CHANNEL_STEPSIZE            0x7A

#define SI4432_REG_FIFO                        0x7F
