/**
 ******************************************************************************
 * @file           : display_task.h
 * @brief          : Display task header for OLED management
 ******************************************************************************
 */

#ifndef DISPLAY_TASK_H
#define DISPLAY_TASK_H

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include <stdint.h>

/* Display events */
typedef enum {
    DISPLAY_EVENT_FINGER_OK = 0,
    DISPLAY_EVENT_FINGER_FAIL,
    DISPLAY_EVENT_DOOR_OPEN,
    DISPLAY_EVENT_DOOR_CLOSED,
    DISPLAY_EVENT_ERROR,
    DISPLAY_EVENT_IDLE
} display_event_t;

/* UI states */
typedef enum {
    UI_STATE_IDLE = 0,
    UI_STATE_DOOR_ANIMATION,
    UI_STATE_FINGERPRINT,
    UI_STATE_ERROR
} ui_state_t;

typedef enum {
    DOOR_OPENING,
    DOOR_CLOSING
} DoorDirection_t;

/* Function prototypes */

/**
 * @brief Initialize display task
 */
void DisplayTask_Init(void);

/**
 * @brief Send event to display task
 * @param event: Event to send
 */
void DisplayTask_Send(display_event_t event);

/**
 * @brief Get current UI state
 * @retval Current UI state
 */
ui_state_t DisplayTask_GetState(void);

#endif /* DISPLAY_TASK_H */
