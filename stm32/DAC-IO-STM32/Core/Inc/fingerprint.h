/*
 * fingerprint.h
 *
 *  Created on: Jan 22, 2026
 *      Author: leo
 */

#ifndef INC_FINGERPRINT_H_
#define INC_FINGERPRINT_H_

#pragma once
#include <stdint.h>
#include "uart_bsp.h"

typedef enum {
    AS608_OK,
    AS608_NO_FINGER,
    AS608_MATCH,
    AS608_NO_MATCH,
    AS608_ERROR,
} as608_status_t;

void AS608_Init(void);

as608_status_t AS608_GetImage(void);
as608_status_t AS608_Img2Tz(uint8_t buffer);
as608_status_t AS608_Verify(uint16_t id);
as608_status_t AS608_Search(uint16_t *id);
uint16_t AS608_FindFreeID(uint16_t max_id);
as608_status_t AS608_RegModel(void);
as608_status_t AS608_StoreChar(uint8_t buffer, uint16_t page_id);
//as608_status_t AS608


#define AS608_START_CODE   0xEF01
#define AS608_ADDR_DEFAULT 0xFFFFFFFF

#define AS608_CMD_GET_IMAGE    0x01
#define AS608_CMD_IMG2TZ       0x02
#define AS608_CMD_VERIFY       0x03
#define AS608_CMD_SEARCH       0x04
#define AS608_CMD_REG_MODEL    0x05
#define AS608_CMD_STORE_CHAR   0x06

#define AS608_CMD_READ_INDEX   0x1F




#define AS608_TX_TIMEOUT_MS   10
#define AS608_RX_TIMEOUT_MS  3000

#define AS608_ID_MIN     1
#define AS608_ID_MAX     127


#endif /* INC_FINGERPRINT_H_ */
