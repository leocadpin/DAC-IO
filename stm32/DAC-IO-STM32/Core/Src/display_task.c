/**
 ******************************************************************************
 * @file           : display_task.c
 * @brief          : Display task implementation (adapted to HAL-BSP-Driver-Task)
 ******************************************************************************
 */

#include "display_task.h"
#include "display_driver.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include <string.h>
#include <math.h>

/* Private defines */
#define DISPLAY_QUEUE_LENGTH    5
#define DISPLAY_TASK_STACK      512
#define DISPLAY_TASK_PRIORITY   (tskIDLE_PRIORITY + 1)

/* Private variables */
static QueueHandle_t displayQueue = NULL;
static TaskHandle_t displayTaskHandle = NULL;
static ui_state_t uiState = UI_STATE_IDLE;
static float doorAngle = 0.0f;
static const float doorStep = 0.05f;
static uint8_t animationFrame = 0;
static DoorDirection_t door_direction =  DOOR_CLOSING;

/* Private function prototypes */
static void DisplayTask(void *argument);
static void UI_DrawDoorAnimation(DoorDirection_t direction);
static void UI_DrawIdleScreen(void);

/**
 * @brief Initialize display task
 */
void DisplayTask_Init(void)
{
    /* Create queue */
    displayQueue = xQueueCreate(DISPLAY_QUEUE_LENGTH, sizeof(display_event_t));

    if (displayQueue == NULL) {
        return;
    }

    /* Create task */
    BaseType_t result = xTaskCreate(
        DisplayTask,
        "DisplayTask",
        DISPLAY_TASK_STACK,
        NULL,
        DISPLAY_TASK_PRIORITY,
        &displayTaskHandle
    );

    if (result != pdPASS) {
        vQueueDelete(displayQueue);
        displayQueue = NULL;
    }
}

/**
 * @brief Send event to display task
 */
void DisplayTask_Send(display_event_t event)
{
    if (displayQueue != NULL) {
        xQueueSend(displayQueue, &event, 0);
    }
}

/**
 * @brief Get current UI state
 */
ui_state_t DisplayTask_GetState(void)
{
    return uiState;
}

/**
 * @brief Display task main function
 */
static void DisplayTask(void *argument)
{
    display_event_t event;

    /* Initialize display driver (which initializes BSP) */
    Display_Init();

    /* Show initial screen */
    Display_Clear();
    Display_DrawString(10, 25, "System", FONT_6X8, COLOR_WHITE);
    Display_DrawString(15, 35, "Init...", FONT_6X8, COLOR_WHITE);
    Display_Update();

    vTaskDelay(pdMS_TO_TICKS(2000));

    /* Show idle screen */
    UI_DrawIdleScreen();

    /* Main loop */
    for (;;)
    {
        /* Check for events with timeout */
        if (xQueueReceive(displayQueue, &event, pdMS_TO_TICKS(50)) == pdPASS)
        {
            /* Process event */
            switch (event)
            {
                case DISPLAY_EVENT_FINGER_OK:
                    Display_Clear();
                    Display_DrawString(5, 20, "Waiting for", FONT_6X8, COLOR_WHITE);
                    Display_DrawString(35, 35, "confirmation", FONT_6X8, COLOR_WHITE);
                    Display_Update();
                    uiState = UI_STATE_FINGERPRINT;
                    break;

                case DISPLAY_EVENT_FINGER_FAIL:
                    Display_Clear();
                    Display_DrawString(5, 20, "Fingerprint", FONT_6X8, COLOR_WHITE);
                    Display_DrawString(25, 35, "FAIL", FONT_6X8, COLOR_WHITE);
                    Display_Update();
                    uiState = UI_STATE_FINGERPRINT;
                    vTaskDelay(pdMS_TO_TICKS(2000));
                    UI_DrawIdleScreen();
                    uiState = UI_STATE_IDLE;
                    break;

                case DISPLAY_EVENT_DOOR_OPEN:
                    uiState = UI_STATE_DOOR_ANIMATION;
                    doorAngle = 0.0f;
                    animationFrame = 0;
                    door_direction = DOOR_OPENING;
                    break;

                case DISPLAY_EVENT_DOOR_CLOSED:
                    uiState = UI_STATE_DOOR_ANIMATION;
                    doorAngle = 1.57f;   // puerta completamente abierta
                    animationFrame = 0;
                    door_direction = DOOR_CLOSING;
                    break;

                case DISPLAY_EVENT_ERROR:
                    Display_Clear();
                    Display_DrawString(20, 25, "System", FONT_6X8, COLOR_WHITE);
                    Display_DrawString(20, 35, "Error", FONT_6X8, COLOR_WHITE);
                    Display_Update();
                    uiState = UI_STATE_ERROR;
                    break;

                case DISPLAY_EVENT_IDLE:
                    UI_DrawIdleScreen();
                    uiState = UI_STATE_IDLE;
                    break;

                default:
                    break;
            }
        }

        /* Update animations */
        switch (uiState)
        {
            case UI_STATE_DOOR_ANIMATION:
                UI_DrawDoorAnimation(door_direction);
                break;

            default:
                break;
        }
    }
}



static void UI_DrawDoorAnimation(DoorDirection_t direction)
{
    const int hingeX = 20;
    const int hingeY = 15;
    const float doorWidth  = 30.0f;
    const float doorHeight = 40.0f;
    const float isoFactor = 0.5f;
    const float maxAngle = 1.57f;  // ~90 grados

    Display_Clear();

    float c = cosf(doorAngle);
    float s = sinf(doorAngle);

    /* Door 3D points (relative to hinge) */

    float x1 = doorWidth;
    float x2 = doorWidth;


    /* Draw frame (marco fijo) */
    Display_DrawRect(hingeX - 2, hingeY - 2,
                     doorWidth + 4, doorHeight + 4,
                     COLOR_WHITE);

    /* Rotate around Y axis */
    float xr1 = x1 * c;
    float zr1 = x1 * s;
    float xr2 = x2 * c;
    float zr2 = x2 * s;

    /* Project isometric */
    int px0 = hingeX;
    int py0 = hingeY;
    int px1 = hingeX + (int)(xr1 - zr1 * isoFactor);
    int py1 = hingeY + (int)(0 + zr1 * isoFactor);
    int px2 = hingeX + (int)(xr2 - zr2 * isoFactor);
    int py2 = hingeY + (int)(doorHeight + zr2 * isoFactor);
    int px3 = hingeX;
    int py3 = hingeY + (int)doorHeight;

    if (doorAngle < 1.0f)
    {
        Display_DrawCircle(px1 - 4,
                           py1 + doorHeight / 2,
                           2,
                           COLOR_WHITE);
    }

    /* Draw door quad */
    Display_DrawLine(px0, py0, px1, py1, COLOR_WHITE);
    Display_DrawLine(px1, py1, px2, py2, COLOR_WHITE);
    Display_DrawLine(px2, py2, px3, py3, COLOR_WHITE);
    Display_DrawLine(px3, py3, px0, py0, COLOR_WHITE);

    /* Draw inner depth edge */
    if (doorAngle > 0.05f)
    {
        Display_DrawLine(px1, py1, px2, py2, COLOR_WHITE);
    }

    /* Display text based on direction */
    if (direction == DOOR_OPENING)
    {
        Display_DrawString(60, 25, "Opening", FONT_6X8, COLOR_WHITE);
    }
    else
    {
        Display_DrawString(60, 25, "Closing", FONT_6X8, COLOR_WHITE);
    }

    Display_Update();

    /* Update angle based on direction */
    if (direction == DOOR_OPENING)
    {
        doorAngle += doorStep;
        if (doorAngle >= maxAngle)
        {
            doorAngle = maxAngle;
            uiState = UI_STATE_IDLE;
            UI_DrawIdleScreen();
        }
    }
    else  // DOOR_CLOSING
    {
        doorAngle -= doorStep;
        if (doorAngle <= 0.0f)
        {
            doorAngle = 0.0f;
            uiState = UI_STATE_IDLE;
            UI_DrawIdleScreen();
        }
    }

    vTaskDelay(pdMS_TO_TICKS(30));
}



//static void UI_DrawDoorAnimation(void)
//{
//    const int hingeX = 30;
//    const int hingeY = 10;
//
//    const int doorWidth  = 30;
//    const int doorHeight = 40;
//
//    Display_Clear();
//
//    /* Draw frame (marco fijo) */
//    Display_DrawRect(hingeX - 2, hingeY - 2,
//                     doorWidth + 4, doorHeight + 4,
//                     COLOR_WHITE);
//
//    /* Calculate visible width using projection */
//    float projection = cosf(doorAngle);
//
//    if (projection < 0.0f)
//        projection = 0.0f;
//
//    int visibleWidth = (int)(doorWidth * projection);
//
//    /* Draw door (as shrinking rectangle) */
//    Display_DrawRect(hingeX,
//                     hingeY,
//                     visibleWidth,
//                     doorHeight,
//                     COLOR_WHITE);
//
//    /* Draw knob (optional, queda muy bien visualmente) */
//    if (visibleWidth > 5)
//    {
//        Display_DrawCircle(hingeX + visibleWidth - 4,
//                           hingeY + doorHeight / 2,
//                           2,
//                           COLOR_WHITE);
//    }
//
//    /* Draw status text */
//    Display_DrawString(75, 25, "Opening", FONT_6X8, COLOR_WHITE);
//
//    Display_Update();
//
//    /* Update angle */
//    doorAngle += doorStep;
//
//    if (doorAngle >= 1.57f)  // 90 degrees
//    {
//        Display_Clear();
//        Display_DrawString(30, 25, "Door", FONT_6X8, COLOR_WHITE);
//        Display_DrawString(30, 35, "Open", FONT_6X8, COLOR_WHITE);
//        Display_Update();
//
//        vTaskDelay(pdMS_TO_TICKS(1000));
//
//        doorAngle = 0.0f;
//        uiState = UI_STATE_IDLE;
//        UI_DrawIdleScreen();
//    }
//
//    vTaskDelay(pdMS_TO_TICKS(30));
//}

/**
 * @brief Draw idle screen
 */
static void UI_DrawIdleScreen(void)
{
    Display_Clear();

    /* Title */
    Display_DrawString(20, 10, "DAC-IO", FONT_6X8, COLOR_WHITE);
    Display_DrawString(15, 20, "LEO CADAVID", FONT_6X8, COLOR_WHITE);

    /* Status */
    Display_DrawString(30, 45, "Ready", FONT_6X8, COLOR_WHITE);

    Display_Update();
}
