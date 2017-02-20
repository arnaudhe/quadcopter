#ifndef HMC5883L_DEFS_H
#define HMC5883L_DEFS_H

#define HMC5883L_ADDRESS              (0x1E)
#define HMC5883L_REG_CONFIG_A         (0x00)
#define HMC5883L_REG_CONFIG_B         (0x01)
#define HMC5883L_REG_MODE             (0x02)
#define HMC5883L_REG_OUT_X_M          (0x03)
#define HMC5883L_REG_OUT_X_L          (0x04)
#define HMC5883L_REG_OUT_Z_M          (0x05)
#define HMC5883L_REG_OUT_Z_L          (0x06)
#define HMC5883L_REG_OUT_Y_M          (0x07)
#define HMC5883L_REG_OUT_Y_L          (0x08)
#define HMC5883L_REG_STATUS           (0x09)
#define HMC5883L_REG_IDENT_A          (0x0A)
#define HMC5883L_REG_IDENT_B          (0x0B)
#define HMC5883L_REG_IDENT_C          (0x0C)

#define HMC5885L_IDENT_A              (0x48)
#define HMC5885L_IDENT_B              (0x34)
#define HMC5885L_IDENT_C              (0x33)

typedef enum
{
    HMC5883L_SAMPLES_8     = 0x60,
    HMC5883L_SAMPLES_4     = 0x40,
    HMC5883L_SAMPLES_2     = 0x20,
    HMC5883L_SAMPLES_1     = 0x00
} hmc5883l_samples_t;

typedef enum
{
    HMC5883L_DATARATE_75HZ       = 0x18,
    HMC5883L_DATARATE_30HZ       = 0x14,
    HMC5883L_DATARATE_15HZ       = 0x10,
    HMC5883L_DATARATE_7_5HZ      = 0x0c,
    HMC5883L_DATARATE_3HZ        = 0x08,
    HMC5883L_DATARATE_1_5HZ      = 0x04,
    HMC5883L_DATARATE_0_75_HZ    = 0x00
} hmc5883l_dataRate_t;

typedef enum
{
    HMC5883L_RANGE_8_1GA     = 0xe0,
    HMC5883L_RANGE_5_6GA     = 0xc0,
    HMC5883L_RANGE_4_7GA     = 0xa0,
    HMC5883L_RANGE_4GA       = 0x80,
    HMC5883L_RANGE_2_5GA     = 0x60,
    HMC5883L_RANGE_1_9GA     = 0x40,
    HMC5883L_RANGE_1_3GA     = 0x20,
    HMC5883L_RANGE_0_88GA    = 0x00
} hmc5883l_range_t;

typedef enum
{
    HMC5883L_IDLE          = 0x02,
    HMC5883L_SINGLE        = 0x01,
    HMC5883L_CONTINOUS     = 0x00
} hmc5883l_mode_t;

#endif