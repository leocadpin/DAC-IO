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
    FP_STATE_ERROR,
	FP_STATE_ENROLL
} fp_state_t;

typedef enum {
    ENROLL_GET_IMAGE_1,
    ENROLL_CONVERT_1,
    ENROLL_WAIT_RELEASE,
    ENROLL_GET_IMAGE_2,
    ENROLL_CONVERT_2,
    ENROLL_CREATE_MODEL,
    ENROLL_STORE,
    ENROLL_DONE,
    ENROLL_FAIL
} enroll_state_t;


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
            if (enroll_requested)
            {
                enroll_requested = 0;
                state = FP_STATE_ENROLL;
            }
            else
            {
				if (AS608_GetImage() == AS608_OK)
					state = FP_STATE_CONVERT;
				else
					vTaskDelay(pdMS_TO_TICKS(10000));
            }

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

        case FP_STATE_ENROLL:
        {
            static enroll_state_t enroll_state = ENROLL_GET_IMAGE_1;
            static uint16_t enroll_id = 0xFFFF;

            switch (enroll_state)
            {
            case ENROLL_GET_IMAGE_1:
                if (AS608_GetImage() == AS608_OK)
                    enroll_state = ENROLL_CONVERT_1;
                else
                    vTaskDelay(pdMS_TO_TICKS(5000));
                break;

            case ENROLL_CONVERT_1:
                if (AS608_Img2Tz(1) == AS608_OK)
                    enroll_state = ENROLL_WAIT_RELEASE;
                else
                    enroll_state = ENROLL_FAIL;
                break;

            case ENROLL_WAIT_RELEASE:
                if (AS608_GetImage() == AS608_NO_FINGER)
                    enroll_state = ENROLL_GET_IMAGE_2;
                else
                    vTaskDelay(pdMS_TO_TICKS(5000));
                break;

            case ENROLL_GET_IMAGE_2:
                if (AS608_GetImage() == AS608_OK)
                    enroll_state = ENROLL_CONVERT_2;
                else
                    vTaskDelay(pdMS_TO_TICKS(5000));
                break;

            case ENROLL_CONVERT_2:
                if (AS608_Img2Tz(2) == AS608_OK)
                    enroll_state = ENROLL_CREATE_MODEL;
                else
                    enroll_state = ENROLL_FAIL;
                break;

            case ENROLL_CREATE_MODEL:
                if (AS608_RegModel() == AS608_OK)
                {
                    enroll_id = AS608_FindFreeID(300);
                    if (enroll_id != 0xFFFF)
                        enroll_state = ENROLL_STORE;
                    else
                        enroll_state = ENROLL_FAIL;
                }
                else
                    enroll_state = ENROLL_FAIL;
                break;

            case ENROLL_STORE:
                if (AS608_StoreChar(1, enroll_id) == AS608_OK)
                    enroll_state = ENROLL_DONE;
                else
                    enroll_state = ENROLL_FAIL;
                break;

            case ENROLL_DONE:
            {
                fingerprint_event_t evt = {
                    .status = AS608_OK,
                    .id = enroll_id
                };
                xQueueSend(fp_queue, &evt, 0);


                enroll_state = ENROLL_GET_IMAGE_1;
                state = FP_STATE_IDLE;
                break;
            }

            case ENROLL_FAIL:
            default:

                enroll_state = ENROLL_GET_IMAGE_1;
                state = FP_STATE_ERROR;
                break;
            }

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

void Fingerprint_RequestEnroll(void)
{
    enroll_requested = 1;
}

