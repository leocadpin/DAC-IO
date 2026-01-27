/*
 * can_bsp.c
 *
 *  Created on: Jan 26, 2026
 *      Author: leo
 */
#include "can_bsp.h"

extern CAN_HandleTypeDef hcan1;  // viene de can.c (MX_CAN1_Init)
extern CAN_HandleTypeDef hcan2;  // viene de can.c (MX_CAN1_Init)

void CAN_BSP_Init(void)
{
    CAN_FilterTypeDef filter = {0};

	 filter.FilterBank = 0;
     filter.FilterMode = CAN_FILTERMODE_IDMASK;
     filter.FilterScale = CAN_FILTERSCALE_32BIT;
     filter.FilterIdHigh = 0x0000;
     filter.FilterIdLow = 0x0000;
     filter.FilterMaskIdHigh = 0x0000;
     filter.FilterMaskIdLow = 0x0000;
     filter.FilterFIFOAssignment = CAN_FILTER_FIFO0;
     //filter.FilterFIFOAssignment = CAN_RX_FIFO0;
     filter.FilterActivation = ENABLE;
     filter.SlaveStartFilterBank = 14;
     HAL_CAN_ConfigFilter(&hcan1, &filter);
     filter.FilterBank = 14;
     HAL_CAN_ConfigFilter(&hcan1, &filter);
     // Arrancar CAN
	HAL_CAN_Start(&hcan1);
	HAL_CAN_Start(&hcan2);
    // Activar interrupciones RX
    HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING);
    HAL_CAN_ActivateNotification(&hcan2, CAN_IT_RX_FIFO0_MSG_PENDING);

}


can_bsp_status_t CAN_BSP_Send(uint32_t std_id,const uint8_t *data, uint8_t len)
{
    CAN_TxHeaderTypeDef txHeader;
    uint32_t txMailbox;

    if (len > 8) return CAN_BSP_ERROR;

    txHeader.StdId = std_id;
    txHeader.ExtId = 0;
    txHeader.IDE = CAN_ID_STD;
    txHeader.RTR = CAN_RTR_DATA;
    txHeader.DLC = len;
    if (HAL_CAN_GetTxMailboxesFreeLevel(&hcan1) == 0)
        return CAN_BSP_BUSY;
    if (HAL_CAN_AddTxMessage(&hcan1, &txHeader, data, &txMailbox) != HAL_OK)
        return CAN_BSP_BUSY; // cola llena

    return CAN_BSP_OK;
}

__weak void CAN_BSP_RxCallback(const can_bsp_msg_t *msg)
{
    // vacío por defecto
}


void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
    CAN_RxHeaderTypeDef hdr;
    can_bsp_msg_t msg;

    HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &hdr, msg.data);

    msg.id  = hdr.StdId;
    msg.dlc = hdr.DLC;



    	CAN_BSP_RxCallback(&msg);
        if (hcan->Instance == CAN2 && hdr.StdId == 0x123){

        }

}

//void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
//{
//  while (HAL_CAN_GetRxFifoFillLevel(hcan, CAN_RX_FIFO0) > 0)
//  {
//    HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &rxHeader, rxData);
//
//    if (hcan->Instance == CAN2 && rxHeader.StdId == 0x123)
//    {
//      CAN_TxHeaderTypeDef replyHeader = {
//        .StdId = 0x321,
//        .IDE = CAN_ID_STD,
//        .RTR = CAN_RTR_DATA,
//        .DLC = 8,
//    	.TransmitGlobalTime = DISABLE
//      };
//
//            rxLength = rxHeader.DLC;        // Cuántos bytes llegaron
//            for(uint32_t i=0; i<rxLength; i++)
//                rxBuffer[i] = rxData[i];    // Guardamos los datos recibidos
//
//      uint32_t mb;
//      uint8_t reply[8] = {'H','i',' ','C','A','N',' ','1'};
//      HAL_CAN_AddTxMessage(&hcan2, &replyHeader, reply, &mb);
//    }
//
//    if (hcan->Instance == CAN1 && rxHeader.StdId == 0x321)
//    {
//        for(uint32_t i=0; i<rxLength; i++)
//            rxBuffer2[i] = rxData[i];    // Guardamos los datos recibidos
//    }
//  }
//}
