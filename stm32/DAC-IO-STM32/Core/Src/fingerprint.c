/*
 * fingerprint.c
 *
 *  Created on: Jan 22, 2026
 *      Author: leo
 */

#include "fingerprint.h"
#include "uart_bsp.h"
#include <string.h>

#define AS608_PKT_COMMAND  0x01
#define AS608_PKT_ACK      0x07

#define AS608_ACK_OK        0x00
#define AS608_ACK_NOFINGER  0x02
#define AS608_ACK_NOMATCH   0x09




static as608_status_t as608_parse_ack(uint8_t *rx);
static int as608_send_cmd(uint8_t instruction,
                          const uint8_t *params,
                          uint16_t param_len,
                          uint8_t *rx,
                          uint16_t rx_len);

as608_status_t AS608_GetImage(void)
{
    uint8_t rx[12];

    if (as608_send_cmd(AS608_CMD_GET_IMAGE, NULL, 0, rx, sizeof(rx)) < 0)
        return AS608_ERROR;

    return as608_parse_ack(rx);
}

as608_status_t AS608_Verify(uint16_t id)
{
    uint8_t rx[12];
    uint8_t params[3] = {
        0x01,              // Buffer 1
        id >> 8,
        id & 0xFF
    };

    if(as608_send_cmd(AS608_CMD_VERIFY, params, 3, rx, sizeof(rx)) < 0)
    	return AS608_ERROR;


    return as608_parse_ack(rx);
}

as608_status_t AS608_Search(uint16_t *id)
{
    uint8_t rx[16];
    uint8_t params[5] = {
        0x01,   // Buffer 1
        0x00, 0x00,   // Start page
        0x00, 0xA3    // Page count (163)
    };

    if (as608_send_cmd(AS608_CMD_SEARCH, params, 5, rx, sizeof(rx)) < 0)
        return AS608_ERROR;

    if (rx[9] != AS608_ACK_OK)
        return as608_parse_ack(rx);

    *id = (rx[10] << 8) | rx[11];

    return AS608_MATCH;
}

as608_status_t AS608_Img2Tz(uint8_t buffer)
{
    uint8_t rx[12];
    uint8_t params[1] = { buffer };

    if (as608_send_cmd(AS608_CMD_IMG2TZ, params, 1, rx, sizeof(rx)) < 0)
        return AS608_ERROR;

    return as608_parse_ack(rx);
}




static as608_status_t as608_parse_ack(uint8_t *rx)
{
    uint8_t confirm = rx[9];

    switch (confirm)
    {
    case AS608_ACK_OK:
        return AS608_OK;

    case AS608_ACK_NOFINGER:
        return AS608_NO_FINGER;

    case AS608_ACK_NOMATCH:
        return AS608_NO_MATCH;

    default:
        return AS608_ERROR;
    }
}


static int as608_send_cmd(uint8_t instruction,
                          const uint8_t *params,
                          uint16_t param_len,
                          uint8_t *rx,
                          uint16_t rx_len)
{
    uint8_t tx[32];
    uint16_t i = 0;
    uint16_t checksum = 0;

    // Start code
    tx[i++] = 0xEF;
    tx[i++] = 0x01;

    // Address
    tx[i++] = 0xFF;
    tx[i++] = 0xFF;
    tx[i++] = 0xFF;
    tx[i++] = 0xFF;

    // Packet ID
    tx[i++] = AS608_PKT_COMMAND;
    checksum += AS608_PKT_COMMAND;

    // Length = instr + params + checksum(2)
    uint16_t length = param_len + 3;
    tx[i++] = length >> 8;
    tx[i++] = length & 0xFF;
    checksum += (length >> 8) + (length & 0xFF);

    // Instruction
    tx[i++] = instruction;
    checksum += instruction;

    // Params
    for (uint16_t p = 0; p < param_len; p++)
    {
        tx[i++] = params[p];
        checksum += params[p];
    }

    // Checksum
    tx[i++] = checksum >> 8;
    tx[i++] = checksum & 0xFF;

    // TX / RX
    if (uart_bsp_tx(tx, i, AS608_TX_TIMEOUT_MS) != 0)
        return -1;

    if (uart_bsp_rx(rx, rx_len, AS608_RX_TIMEOUT_MS) != 0)
        return -1;

    return 0;
}


uint16_t AS608_FindFreeID(uint16_t max_id)
{
    uint8_t rx[44]; // suficiente para ACK + tabla
    uint8_t params[1];

    uint16_t max_pages = (max_id + 255) / 256;

    for (uint8_t page = 0; page < max_pages; page++)
    {
        params[0] = page;

        if (as608_send_cmd(AS608_CMD_READ_INDEX, params, 1, rx, sizeof(rx)) < 0)
            return 0xFFFF;

        if (rx[9] != AS608_ACK_OK)
            return 0xFFFF;

        // Tabla empieza en rx[10]
        for (uint8_t byte = 0; byte < 32; byte++)
        {
            uint8_t mask = rx[10 + byte];

            if (mask != 0xFF)
            {
                for (uint8_t bit = 0; bit < 8; bit++)
                {
                    if ((mask & (1 << bit)) == 0)
                    {
                        uint16_t id = page * 256 + byte * 8 + bit;
                        if (id < max_id)
                            return id;
                    }
                }
            }
        }
    }

    return 0xFFFF; // memoria llena
}

as608_status_t AS608_RegModel(void)
{
    uint8_t rx[12];

    if (as608_send_cmd(AS608_CMD_REG_MODEL, NULL, 0, rx, sizeof(rx)) < 0)
        return AS608_ERROR;

    return as608_parse_ack(rx);
}

as608_status_t AS608_StoreChar(uint8_t buffer, uint16_t page_id)
{
    uint8_t rx[12];
    uint8_t params[3];

    params[0] = buffer;
    params[1] = page_id >> 8;
    params[2] = page_id & 0xFF;

    if (as608_send_cmd(AS608_CMD_STORE_CHAR, params, 3, rx, sizeof(rx)) < 0)
        return AS608_ERROR;

    return as608_parse_ack(rx);
}


