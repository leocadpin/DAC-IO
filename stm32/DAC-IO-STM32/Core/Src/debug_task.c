#include "debug_task.h"

#define CAN_RX_BUFFER_SIZE 32

static void ConsumerTask(void *arg)
{
    fingerprint_event_t evt;


    static can_bsp_msg_t can_rx_buffer[CAN_RX_BUFFER_SIZE];
    static uint8_t can_rx_index = 0;
    for (;;)
    {
        can_bsp_msg_t msg;
        QueueHandle_t rx_queue = CAN_App_GetRxQueue();

        if (xQueueReceive(CAN_App_GetRxQueue(), &msg, portMAX_DELAY))
        {
            if (msg.id == 0x200)
            {
                // comando remoto
            }
        }
//        if (xQueueReceive(FingerprintTask_GetQueue(), &evt, portMAX_DELAY))
//        {
//            if (evt.status == AS608_MATCH) {
//                // Do something with evt.id
//            }
//            else {
//                // no match
//            }
//        }
    }
}

void ConsumerTask_Init(void)
{
    xTaskCreate(ConsumerTask, "consumer", 256, NULL, tskIDLE_PRIORITY + 1, NULL);
}
