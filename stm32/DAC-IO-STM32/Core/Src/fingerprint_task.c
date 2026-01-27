/*
 * fingerprint_task.c
 *
 *  Created on: Jan 25, 2026
 *      Author: leo
 */


#include "fingerprint_task.h"


typedef enum {
    FP_STATE_IDLE,
    FP_STATE_WAIT_FINGER,
    FP_STATE_CONVERT,
    FP_STATE_SEARCH,
    FP_STATE_MATCH,
    FP_STATE_NO_MATCH,
    FP_STATE_ERROR
} fp_state_t;

static QueueHandle_t fp_queue;
static TaskHandle_t fp_task_handle;



QueueHandle_t FingerprintTask_GetQueue(void)
{
    return fp_queue;
}

static void FingerprintTask(void *arg)
{
    fp_state_t state = FP_STATE_IDLE;
    uint16_t id;

    for (;;)
    {
        switch (state)
        {
        case FP_STATE_IDLE:
            state = FP_STATE_WAIT_FINGER;
            break;

        case FP_STATE_WAIT_FINGER:
            if (AS608_GetImage() == AS608_OK)
                state = FP_STATE_CONVERT;
            else
                vTaskDelay(pdMS_TO_TICKS(100));
            break;

        case FP_STATE_CONVERT:
            if (AS608_Img2Tz(1) == AS608_OK)
                state = FP_STATE_SEARCH;
            else
                state = FP_STATE_ERROR;
            break;

        case FP_STATE_SEARCH:
            if (AS608_Search(&id) == AS608_MATCH)
                state = FP_STATE_MATCH;
            else
                state = FP_STATE_NO_MATCH;
            break;

        case FP_STATE_MATCH:
        {
            fingerprint_event_t evt = {
                .status = AS608_MATCH,
                .id = id
            };
            xQueueSend(fp_queue, &evt, 0);
            vTaskDelay(pdMS_TO_TICKS(500));
            state = FP_STATE_IDLE;
            break;
        }

        case FP_STATE_NO_MATCH: {
            fingerprint_event_t evt = {
                .status = AS608_NO_MATCH,
                .id = 0
            };
            xQueueSend(fp_queue, &evt, 0);
            vTaskDelay(pdMS_TO_TICKS(500));
            state = FP_STATE_IDLE;
            break;
        }

        case FP_STATE_ERROR:
        default:
            vTaskDelay(pdMS_TO_TICKS(300));
            state = FP_STATE_IDLE;
            break;
        }
    }
}

void FingerprintTask_Init(void)
{
    fp_queue = xQueueCreate(4, sizeof(fingerprint_event_t));

    xTaskCreate(
        FingerprintTask,
        "fingerprint",
        512,
        NULL,
        tskIDLE_PRIORITY + 2,
        &fp_task_handle
    );
}
