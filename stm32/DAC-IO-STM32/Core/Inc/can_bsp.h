/*
 * can_bsp.h
 *
 *  Created on: Jan 26, 2026
 *      Author: leo
 */

#ifndef INC_CAN_BSP_H_
#define INC_CAN_BSP_H_

#include <stdint.h>
#include "stm32f4xx_hal.h"

typedef enum {
    CAN_BSP_OK,
    CAN_BSP_ERROR,
    CAN_BSP_BUSY
} can_bsp_status_t;

typedef struct {
    uint32_t id;
    uint8_t  dlc;
    uint8_t  data[8];
} can_bsp_msg_t;

void CAN_BSP_Init(void);


can_bsp_status_t CAN_BSP_Send(uint32_t std_id,
                              const uint8_t *data,
                              uint8_t len);

/* Callback débil (hook) */
void CAN_BSP_RxCallback(const can_bsp_msg_t *msg);

#endif

