/*
 * display_bsp.c
 *
 *  Created on: Feb 13, 2026
 *      Author: leo
 */

#include "display_bsp.h"

/* ========= FUNCIONES INTERNAS ========= */

static void SSD1309_Select(void)
{
    HAL_GPIO_WritePin(SSD1309_CS_PORT, SSD1309_CS_PIN, GPIO_PIN_RESET);
}

static void SSD1309_Unselect(void)
{
    HAL_GPIO_WritePin(SSD1309_CS_PORT, SSD1309_CS_PIN, GPIO_PIN_SET);
}

/* ========= RESET ========= */

void BSP_SSD1309_Reset(void)
{
    HAL_GPIO_WritePin(SSD1309_RST_PORT, SSD1309_RST_PIN, GPIO_PIN_RESET);
    HAL_Delay(10);
    HAL_GPIO_WritePin(SSD1309_RST_PORT, SSD1309_RST_PIN, GPIO_PIN_SET);
    HAL_Delay(10);
}

/* ========= ENVÍO COMANDO ========= */

void BSP_SSD1309_SendCommand(uint8_t cmd)
{
    HAL_GPIO_WritePin(SSD1309_DC_PORT, SSD1309_DC_PIN, GPIO_PIN_RESET);
    SSD1309_Select();
    HAL_SPI_Transmit(&hspi1, &cmd, 1, HAL_MAX_DELAY);
    SSD1309_Unselect();
}

/* ========= ENVÍO DATOS ========= */

void BSP_SSD1309_SendData(uint8_t* data, uint16_t size)
{
    HAL_GPIO_WritePin(SSD1309_DC_PORT, SSD1309_DC_PIN, GPIO_PIN_SET);
    SSD1309_Select();
    HAL_SPI_Transmit(&hspi1, data, size, HAL_MAX_DELAY);
    SSD1309_Unselect();
}

/* ========= INICIALIZACIÓN ========= */

void BSP_SSD1309_Init(void)
{
    BSP_SSD1309_Reset();

    BSP_SSD1309_SendCommand(0xAE); // Display OFF

    BSP_SSD1309_SendCommand(0xD5);
    BSP_SSD1309_SendCommand(0x80);

    BSP_SSD1309_SendCommand(0xA8);
    BSP_SSD1309_SendCommand(0x3F); // 64 líneas

    BSP_SSD1309_SendCommand(0xD3);
    BSP_SSD1309_SendCommand(0x00);

    BSP_SSD1309_SendCommand(0x40);

    BSP_SSD1309_SendCommand(0x8D);
    BSP_SSD1309_SendCommand(0x14);

    BSP_SSD1309_SendCommand(0x20);
    BSP_SSD1309_SendCommand(0x00); // Horizontal addressing mode

    BSP_SSD1309_SendCommand(0xA1);
    BSP_SSD1309_SendCommand(0xC8);

    BSP_SSD1309_SendCommand(0xDA);
    BSP_SSD1309_SendCommand(0x12);

    BSP_SSD1309_SendCommand(0x81);
    BSP_SSD1309_SendCommand(0xCF);

    BSP_SSD1309_SendCommand(0xD9);
    BSP_SSD1309_SendCommand(0xF1);

    BSP_SSD1309_SendCommand(0xDB);
    BSP_SSD1309_SendCommand(0x40);

    BSP_SSD1309_SendCommand(0xA4);
    BSP_SSD1309_SendCommand(0xA6);

    BSP_SSD1309_SendCommand(0xAF); // Display ON
}

/* ========= ACTUALIZAR PANTALLA ========= */

void BSP_SSD1309_UpdateScreen(uint8_t* buffer)
{
    BSP_SSD1309_SendCommand(0x21); // Column addr
    BSP_SSD1309_SendCommand(0);
    BSP_SSD1309_SendCommand(127);

    BSP_SSD1309_SendCommand(0x22); // Page addr
    BSP_SSD1309_SendCommand(0);
    BSP_SSD1309_SendCommand(7);

    BSP_SSD1309_SendData(buffer, SSD1309_WIDTH * SSD1309_HEIGHT / 8);
}

/* ========= LIMPIAR BUFFER ========= */

void BSP_SSD1309_Clear(uint8_t* buffer)
{
    for (uint16_t i = 0; i < (SSD1309_WIDTH * SSD1309_HEIGHT / 8); i++)
    {
        buffer[i] = 0x00;
    }
}



