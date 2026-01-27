/*
 * can_task.h
 *
 *  Created on: Jan 26, 2026
 *      Author: leo
 */

#ifndef INC_CAN_TASK_H_
#define INC_CAN_TASK_H_


#pragma once
#include "FreeRTOS.h"
#include "queue.h"
#include "can_bsp.h"

void CANTask_Init(void);
QueueHandle_t CAN_App_GetRxQueue(void);
static QueueHandle_t can_rx_queue;

#endif /* INC_CAN_TASK_H_ */
