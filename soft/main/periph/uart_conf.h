#pragma once

#include <driver/uart.h>

uart_config_t UART_CONF[UART_NUM_MAX] = {
    [UART_NUM_0] = {
        .baud_rate              = 115200,
        .data_bits              = UART_DATA_8_BITS,
        .parity                 = UART_PARITY_DISABLE,
        .stop_bits              = UART_STOP_BITS_1,
        .flow_ctrl              = UART_HW_FLOWCTRL_DISABLE,
        .rx_flow_ctrl_thresh    = 0,
        .use_ref_tick           = true
    },
    [UART_NUM_1] = {
        .baud_rate              = 115200,
        .data_bits              = UART_DATA_8_BITS,
        .parity                 = UART_PARITY_DISABLE,
        .stop_bits              = UART_STOP_BITS_1,
        .flow_ctrl              = UART_HW_FLOWCTRL_DISABLE,
        .rx_flow_ctrl_thresh    = 0,
        .use_ref_tick           = true
    },
    [UART_NUM_2] = {
        .baud_rate              = 9600,
        .data_bits              = UART_DATA_8_BITS,
        .parity                 = UART_PARITY_DISABLE,
        .stop_bits              = UART_STOP_BITS_1,
        .flow_ctrl              = UART_HW_FLOWCTRL_DISABLE,
        .rx_flow_ctrl_thresh    = 0,
        .use_ref_tick           = true
    }
};

typedef struct
{
    int tx;
    int rx;
    int rts;
    int cts;
}uart_pin_config_t;

uart_pin_config_t UART_PIN_CONF[UART_NUM_MAX] = {
    [UART_NUM_0] = {
        .tx  = UART_PIN_NO_CHANGE,
        .rx  = UART_PIN_NO_CHANGE,
        .rts = UART_PIN_NO_CHANGE,
        .cts = UART_PIN_NO_CHANGE
    },
    [UART_NUM_1] = {
        .tx  = 17,
        .rx  = 16,
        .rts = UART_PIN_NO_CHANGE,
        .cts = UART_PIN_NO_CHANGE
    },
    [UART_NUM_2] = {
        .tx  = 23,
        .rx  = 22,
        .rts = UART_PIN_NO_CHANGE,
        .cts = UART_PIN_NO_CHANGE
    }
};