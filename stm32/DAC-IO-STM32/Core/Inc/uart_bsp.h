/*
 * uart_bsp.h
 *
 *  Created on: Jan 23, 2026
 *      Author: leo
 */

#ifndef INC_BSP_UART_BSP_H_
#define INC_BSP_UART_BSP_H_

#pragma once

#include <stdint.h>
#include <stddef.h>

/* Inicialización del BSP UART */
void uart_bsp_init(void);

/* Envío bloqueante */
int uart_bsp_tx(const uint8_t *data, size_t len, uint32_t timeout_ms);

/* Recepción bloqueante */
int uart_bsp_rx(uint8_t *data, size_t len, uint32_t timeout_ms);



#endif /* INC_BSP_UART_BSP_H_ */
