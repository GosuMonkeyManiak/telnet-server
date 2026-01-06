#pragma once

typedef enum {
    ENC_TRANSMIT_SUCCEEDED,
    ENC_TRANSMIT_DATA_IS_TOO_SMALL,
    ENC_TRANSMIT_DATA_EXCEED_MTU,
    ENC_TRANSMIT_FAILED
} enc624j600_transmit_result;


extern void enc624j600_init(void);

/**
 *  @brief
 * 
 *  @param destination_mac
 *  @param length_protocol
 *  @param data
 *  @param length
 * 
 *  @return 
 */
extern enc624j600_transmit_result enc624j600_transmit(uint8_t *destination_mac, uint16_t *length_protocol, uint8_t *data, size_t length);

extern int enc624j600_receive(uint8_t *destination_mac, uint8_t *source_mac, uint16_t *length_protocol, uint8_t *buffer, uint16_t *received_bytes);