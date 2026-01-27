/*
 * fingerprint_task.h
 *
 *  Created on: Jan 25, 2026
 *      Author: leo
 */

#ifndef SRC_FINGERPRINT_TASK_H_
#define SRC_FINGERPRINT_TASK_H_

#pragma once

#include "FreeRTOS.h"
#include "queue.h"

#include "fingerprint.h"

typedef struct {
    as608_status_t status;
    uint16_t id;
} fingerprint_event_t;

void FingerprintTask_Init(void);
QueueHandle_t FingerprintTask_GetQueue(void);




#endif /* SRC_FINGERPRINT_TASK_H_ */
