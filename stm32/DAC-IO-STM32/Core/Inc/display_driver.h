/**
 ******************************************************************************
 * @file           : display_driver.h
 * @brief          : Driver Layer - Graphics and display control
 ******************************************************************************
 */

#ifndef DISPLAY_DRIVER_H
#define DISPLAY_DRIVER_H

#include <stdint.h>
#include <stdbool.h>

/* Display dimensions */
#define DISPLAY_WIDTH   128
#define DISPLAY_HEIGHT  64

/* Color definitions */
typedef enum {
    COLOR_BLACK = 0,
    COLOR_WHITE = 1
} color_t;

/* Font sizes */
typedef enum {
    FONT_6X8 = 0,
    FONT_8X16,
    FONT_12X16
} font_size_t;

/* Function prototypes */

/**
 * @brief Initialize display driver
 * @retval true if successful
 */
bool Display_Init(void);

/**
 * @brief Clear display
 */
void Display_Clear(void);

/**
 * @brief Update display from internal buffer
 */
void Display_Update(void);

/**
 * @brief Draw a pixel
 * @param x: X coordinate (0-127)
 * @param y: Y coordinate (0-63)
 * @param color: Pixel color
 */
void Display_DrawPixel(uint8_t x, uint8_t y, color_t color);

/**
 * @brief Draw a line
 * @param x0, y0: Start coordinates
 * @param x1, y1: End coordinates
 * @param color: Line color
 */
void Display_DrawLine(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, color_t color);

/**
 * @brief Draw a rectangle
 * @param x, y: Top-left corner
 * @param w, h: Width and height
 * @param color: Rectangle color
 */
void Display_DrawRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, color_t color);

/**
 * @brief Fill a rectangle
 * @param x, y: Top-left corner
 * @param w, h: Width and height
 * @param color: Fill color
 */
void Display_FillRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, color_t color);

/**
 * @brief Draw a circle
 * @param x, y: Center coordinates
 * @param r: Radius
 * @param color: Circle color
 */
void Display_DrawCircle(uint8_t x, uint8_t y, uint8_t r, color_t color);

/**
 * @brief Draw a character
 * @param x, y: Top-left corner
 * @param c: Character to draw
 * @param font: Font size
 * @param color: Character color
 */
void Display_DrawChar(uint8_t x, uint8_t y, char c, font_size_t font, color_t color);

/**
 * @brief Draw a string
 * @param x, y: Top-left corner
 * @param str: String to draw
 * @param font: Font size
 * @param color: Text color
 */
void Display_DrawString(uint8_t x, uint8_t y, const char *str, font_size_t font, color_t color);

/**
 * @brief Get pointer to display buffer (for direct manipulation)
 * @retval Pointer to buffer
 */
uint8_t* Display_GetBuffer(void);

#endif /* DISPLAY_DRIVER_H */
