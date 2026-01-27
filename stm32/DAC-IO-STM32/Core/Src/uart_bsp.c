/*
 * uart_bsp.c
 *
 *  Created on: Jan 23, 2026
 *      Author: leo
 */



#include "uart_bsp.h"
#include "stm32f4xx_hal.h"

/* Este handle lo crea CubeMX */
extern UART_HandleTypeDef huart2;

void uart_bsp_init(void)
{
    /* Nada que hacer aquí de momento.
       El init real lo hace CubeMX */
}

int uart_bsp_tx(const uint8_t *data, size_t len, uint32_t timeout_ms)
{
    if (HAL_UART_Transmit(&huart2, (uint8_t *)data, len, timeout_ms) != HAL_OK)
        return -1;

    return 0;
}

int uart_bsp_rx(uint8_t *data, size_t len, uint32_t timeout_ms)
{
    if (HAL_UART_Receive(&huart2, data, len, timeout_ms) != HAL_OK)
        return -1;

    return 0;
}

