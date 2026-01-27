/*
 * can_task.c
 *
 *  Created on: Jan 26, 2026
 *      Author: leo
 */


#include "can_task.h"
#include "can_bsp.h"
#include "fingerprint_task.h"

static void CANTask(void *arg)
{
    fingerprint_event_t evt;
    uint8_t txData[3];
    can_bsp_msg_t msg;
    for (;;)
    {
        if (xQueueReceive(FingerprintTask_GetQueue(), &evt, portMAX_DELAY))
        {
            txData[0] = (evt.status == AS608_MATCH) ? 1 : 0;
            txData[1] = (evt.status == AS608_MATCH) ? (evt.id >> 8) & 0xFF : 0;
            txData[2] = (evt.status == AS608_MATCH) ? evt.id & 0xFF : 0;

            CAN_BSP_Send(0x123, txData, 3);
        }

					//
					//            txData[0] = 0;
					//            txData[1] = 0xF2;
					//            txData[2] = 0;
					//
					//            CAN_BSP_Send(0x123, txData, 3);
					//            vTaskDelay(pdMS_TO_TICKS(100));
					//        if (xQueueReceive(CAN_App_GetRxQueue(), &msg, portMAX_DELAY))
					//        {
					//            // Ejemplo:
					//            if (msg.id == 0x200)
					//            {
					//                // comando remoto
					//            }
					//        }
    }
}

static void CAN_RxTask(void *arg)
{
    can_bsp_msg_t msg;

    for (;;)
    {
        if (xQueueReceive(CAN_App_GetRxQueue(), &msg, portMAX_DELAY))
        {
            if (msg.id == 0x200)
            {
                // comando remoto
            }
        }
    }
}

void CANTask_Init(void)
{
	can_rx_queue = xQueueCreate(8, sizeof(can_bsp_msg_t));
	xTaskCreate(CANTask, "CAN", 256, NULL, tskIDLE_PRIORITY + 2, NULL);
	xTaskCreate(CAN_RxTask, "CANRX", 256, NULL, tskIDLE_PRIORITY + 1, NULL);

}

void CAN_BSP_RxCallback(const can_bsp_msg_t *msg)
{
    BaseType_t hpw = pdFALSE;
    if (can_rx_queue != NULL)
    {
    	xQueueSendFromISR(can_rx_queue, msg, &hpw);
    	portYIELD_FROM_ISR(hpw);
    }


}

QueueHandle_t CAN_App_GetRxQueue(void)
{
    return can_rx_queue;
}
