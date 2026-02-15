/**
 ******************************************************************************
 * @file           : display_bsp.h
 * @brief          : BSP Layer - Hardware abstraction for SSD1309 OLED
 ******************************************************************************
 */

#ifndef DISPLAY_BSP_H
#define DISPLAY_BSP_H

#include "main.h"
#include <stdint.h>

/* Display dimensions */
#define SSD1309_WIDTH   128
#define SSD1309_HEIGHT  64

/* Pin definitions - AJUSTAR SEGÚN TU HARDWARE */
#define SSD1309_CS_PORT     GPIOB
#define SSD1309_CS_PIN      GPIO_PIN_0

#define SSD1309_DC_PORT     GPIOC
#define SSD1309_DC_PIN      GPIO_PIN_5

#define SSD1309_RST_PORT    GPIOC
#define SSD1309_RST_PIN     GPIO_PIN_4

/* External SPI handle */
extern SPI_HandleTypeDef hspi1;

/* Function prototypes */

/**
 * @brief Initialize SSD1309 display
 */
void BSP_SSD1309_Init(void);

/**
 * @brief Hardware reset
 */
void BSP_SSD1309_Reset(void);

/**
 * @brief Send command to SSD1309
 * @param cmd: Command byte
 */
void BSP_SSD1309_SendCommand(uint8_t cmd);

/**
 * @brief Send data to SSD1309
 * @param data: Pointer to data buffer
 * @param size: Number of bytes to send
 */
void BSP_SSD1309_SendData(uint8_t* data, uint16_t size);

/**
 * @brief Update screen with buffer content
 * @param buffer: Pointer to display buffer
 */
void BSP_SSD1309_UpdateScreen(uint8_t* buffer);

/**
 * @brief Clear display buffer
 * @param buffer: Pointer to display buffer
 */
void BSP_SSD1309_Clear(uint8_t* buffer);

#endif /* DISPLAY_BSP_H */
