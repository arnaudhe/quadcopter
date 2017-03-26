#pragma once

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