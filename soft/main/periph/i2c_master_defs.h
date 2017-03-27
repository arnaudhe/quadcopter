#pragma once

#define WRITE_BIT         I2C_MASTER_WRITE  /*!< I2C master write */
#define READ_BIT          I2C_MASTER_READ   /*!< I2C master read */
#define ACK_CHECK_EN      0x1               /*!< I2C master will check ack from slave*/
#define ACK_CHECK_DIS     0x0               /*!< I2C master will not check ack from slave */
#define ACK_VAL           0x0               /*!< I2C ack value */
#define NACK_VAL          0x1               /*!< I2C nack value */

#define I2C_MASTER_TX_BUF_DISABLE     0                 /*!< I2C master do not need buffer */
#define I2C_MASTER_RX_BUF_DISABLE     0                 /*!< I2C master do not need buffer */

typedef struct
{
    int sda_io_num;
    int scl_io_num;
}i2c_io_mapping_t;

typedef struct
{
   i2c_io_mapping_t mapping;
   int              frequency;
}i2c_master_conf_t;