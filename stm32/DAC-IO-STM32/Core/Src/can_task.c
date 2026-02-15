/*
 * can_task.c
 *
 *  Created on: Jan 26, 2026
 *      Author: leo
 */


#include "can_task.h"
#include "can_bsp.h"
#include "fingerprint_task.h"
#include "motor_task.h"

uint8_t txData[8];
Motor_Message_t motor_msg;

// Declaración de la variable global de la cola
static QueueHandle_t can_rx_queue = NULL;

static void CANTask(void *arg)
{
    fingerprint_event_t evt;

    for (;;)
    {
        if (xQueueReceive(FingerprintTask_GetQueue(), &evt, portMAX_DELAY))
        {
            txData[0] = (evt.status == AS608_MATCH) ? 1 : 0;
            txData[1] = (evt.status == AS608_MATCH) ? (evt.id >> 8) & 0xFF : 0;
            txData[2] = (evt.status == AS608_MATCH) ? evt.id & 0xFF : 0;
            CAN_BSP_Send(0x123, txData, 3);

            // Debug: indicador visual de envío CAN
            HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_12);  // LED naranja

            vTaskDelay(pdMS_TO_TICKS(100));
        }
    }
}

static void CAN_RxTask(void *arg)
{
    can_bsp_msg_t msg;

    // DEBUG: Togglea LED al inicio para confirmar que la tarea arrancó
    HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_14);  // LED azul
    vTaskDelay(pdMS_TO_TICKS(500));
    HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_14);

    for (;;)
    {
        // DEBUG: Togglea LED antes de esperar
        HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_15);  // LED verde

        if (xQueueReceive(can_rx_queue, &msg, portMAX_DELAY))
        {


            // Verificar si el mensaje es de confirmación desde la RPi
            if (msg.id == 0x124)
            {
                // Enviar confirmación a la tarea de fingerprint
                QueueHandle_t fp_confirm_queue = FingerprintTask_GetConfirmQueue();
                if (fp_confirm_queue != NULL)
                {
                    uint8_t confirm = 1;
                    xQueueSend(fp_confirm_queue, &confirm, 0);
                }
            }
        }
    }
}

void CANTask_Init(void)
{

    can_rx_queue = xQueueCreate(8, sizeof(can_bsp_msg_t));


    if (can_rx_queue == NULL)
    {
        // Error: no se pudo crear la cola

        while(1);  // Quedarse aquí para debug
    }

    xTaskCreate(CANTask, "CAN", 256, NULL, tskIDLE_PRIORITY + 2, NULL);
    xTaskCreate(CAN_RxTask, "CANRX", 256, NULL, tskIDLE_PRIORITY + 1, NULL);
}

void CAN_BSP_RxCallback(const can_bsp_msg_t *msg)
{
    BaseType_t hpw = pdFALSE;

    if (can_rx_queue != NULL)
    {
        xQueueSendFromISR(can_rx_queue, msg, &hpw);

        // Usar la macro correcta
        portYIELD_FROM_ISR(hpw);
    }
}

QueueHandle_t CAN_App_GetRxQueue(void)
{
    return can_rx_queue;
}
