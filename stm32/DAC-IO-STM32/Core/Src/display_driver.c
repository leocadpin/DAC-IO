/**
 ******************************************************************************
 * @file           : display_driver.c
 * @brief          : Driver Layer implementation
 ******************************************************************************
 */

#include "display_driver.h"
#include "display_font.h"
#include "display_bsp.h"
#include <string.h>
#include <stdlib.h>

/* Private variables */
static uint8_t display_buffer[SSD1309_WIDTH * SSD1309_HEIGHT / 8];
static bool is_initialized = false;


/* Private function prototypes */
static const uint8_t* get_font_data(char c, font_size_t font);

/**
 * @brief Initialize display driver
 */
bool Display_Init(void)
{
    /* Initialize BSP layer */
    BSP_SSD1309_Init();

    /* Clear buffer */
    Display_Clear();

    /* Update display */
    Display_Update();

    is_initialized = true;

    return true;
}

/**
 * @brief Clear display
 */
void Display_Clear(void)
{
    BSP_SSD1309_Clear(display_buffer);
}

/**
 * @brief Update display
 */
void Display_Update(void)
{
    if (!is_initialized) {
        return;
    }

    BSP_SSD1309_UpdateScreen(display_buffer);
}

/**
 * @brief Draw a pixel
 */
void Display_DrawPixel(uint8_t x, uint8_t y, color_t color)
{
    if (x >= DISPLAY_WIDTH || y >= DISPLAY_HEIGHT) {
        return;
    }

    uint16_t index = x + (y / 8) * DISPLAY_WIDTH;

    if (color == COLOR_WHITE) {
        display_buffer[index] |= (1 << (y % 8));
    } else {
        display_buffer[index] &= ~(1 << (y % 8));
    }
}

/**
 * @brief Draw a line (Bresenham's algorithm)
 */
void Display_DrawLine(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, color_t color)
{
    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);
    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx - dy;

    while (1) {
        Display_DrawPixel(x0, y0, color);

        if (x0 == x1 && y0 == y1) {
            break;
        }

        int e2 = 2 * err;

        if (e2 > -dy) {
            err -= dy;
            x0 += sx;
        }

        if (e2 < dx) {
            err += dx;
            y0 += sy;
        }
    }
}

/**
 * @brief Draw a rectangle
 */
void Display_DrawRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, color_t color)
{
    Display_DrawLine(x, y, x + w - 1, y, color);             // Top
    Display_DrawLine(x, y + h - 1, x + w - 1, y + h - 1, color); // Bottom
    Display_DrawLine(x, y, x, y + h - 1, color);             // Left
    Display_DrawLine(x + w - 1, y, x + w - 1, y + h - 1, color); // Right
}

/**
 * @brief Fill a rectangle
 */
void Display_FillRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, color_t color)
{
    for (uint8_t i = 0; i < h; i++) {
        Display_DrawLine(x, y + i, x + w - 1, y + i, color);
    }
}

/**
 * @brief Draw a circle (Midpoint circle algorithm)
 */
void Display_DrawCircle(uint8_t x0, uint8_t y0, uint8_t r, color_t color)
{
    int x = r;
    int y = 0;
    int err = 0;

    while (x >= y) {
        Display_DrawPixel(x0 + x, y0 + y, color);
        Display_DrawPixel(x0 + y, y0 + x, color);
        Display_DrawPixel(x0 - y, y0 + x, color);
        Display_DrawPixel(x0 - x, y0 + y, color);
        Display_DrawPixel(x0 - x, y0 - y, color);
        Display_DrawPixel(x0 - y, y0 - x, color);
        Display_DrawPixel(x0 + y, y0 - x, color);
        Display_DrawPixel(x0 + x, y0 - y, color);

        y += 1;
        err += 1 + 2 * y;

        if (2 * (err - x) + 1 > 0) {
            x -= 1;
            err += 1 - 2 * x;
        }
    }
}

/**
 * @brief Draw a character
 */
void Display_DrawChar(uint8_t x, uint8_t y, char c, font_size_t font, color_t color)
{
    const uint8_t *font_data = get_font_data(c, font);

    if (font_data == NULL) {
        return;
    }

    uint8_t font_width = 6;  // For 6x8 font
    uint8_t font_height = 8;

    for (uint8_t i = 0; i < font_width; i++) {
        uint8_t line = font_data[i];
        for (uint8_t j = 0; j < font_height; j++) {
            if (line & 0x01) {
                Display_DrawPixel(x + i, y + j, color);
            }
            line >>= 1;
        }
    }
}

/**
 * @brief Draw a string
 */
void Display_DrawString(uint8_t x, uint8_t y, const char *str, font_size_t font, color_t color)
{
    uint8_t font_width = 6;  // For 6x8 font
    uint8_t cursor_x = x;

    while (*str) {
        if (cursor_x + font_width > DISPLAY_WIDTH) {
            break;  // Out of screen
        }

        Display_DrawChar(cursor_x, y, *str, font, color);
        cursor_x += font_width;
        str++;
    }
}

/**
 * @brief Get display buffer pointer
 */
uint8_t* Display_GetBuffer(void)
{
    return display_buffer;
}

/**
 * @brief Get font data for character
 */
static const uint8_t* get_font_data(char c, font_size_t font)
{
    // Only printable ASCII characters supported
    if (c < 32 || c > 126)
    {
        c = 32;  // Space as fallback
    }

    return font_6x8[c - 32];
}

