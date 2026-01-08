#pragma once

/**
 *	@enum enc624j600_transmit_result
 *	@brief Result codes returned by enc624j600_transmit().
 *
 *	This enumeration defines the possible outcomes of an Ethernet
 *	frame transmission attempt.
 */
typedef enum {
    ENC_TRANSMIT_SUCCEEDED = 0,		/**< Frame transmitted successfully */
    ENC_TRANSMIT_DATA_IS_TOO_SMALL,	/**< Payload length is less than 8 bytes */
    ENC_TRANSMIT_DATA_EXCEED_MTU,	/**< Payload length exceeds 1500 bytes */
    ENC_TRANSMIT_FAILED				/**< Frame transmission failed or any of the pointers are NULL */
} enc624j600_transmit_result;


extern void enc624j600_init(void);

/**
 *	@brief Performs transmission of an Ethernet frame.
 *
 *	This function constructs and transmits an Ethernet frame using the
 *	provided destination MAC address, Length/Type field, and payload data.
 *	
 *	The payload data is transmitted exactly as it appears in memory.
 *	Byte order is preserved; the first byte in the buffer is transmitted
 *	first on the wire. No byte reordering or endianness conversion is
 *	performed by this function.
 * 
 *	@pre destination_mac != NULL
 *	@pre length_type != NULL
 *	@pre data != NULL
 *
 *	@param destination_mac Pointer to a 6-byte destination MAC address.
 *	@param length_type Pointer to a 2-byte Length/Type field.
 *					   Values <= 1500 indicate payload length (IEEE 802.3),
 *					   values >= 1536 indicate EtherType (Ethernet II).
 *	@param data Pointer to the payload data to be transmitted.
 *              Payload size must be at least 8 bytes and no more
 *              than 1500 bytes.
 *	@param length Payload length in bytes.
 *
 *	@return enc624j600_transmit_result
 *		See @ref enc624j600_transmit_result for possible return values.
 */
extern enc624j600_transmit_result enc624j600_transmit(const uint8_t *destination_mac, const uint8_t *length_type, const uint8_t *data, size_t length);


extern int enc624j600_receive(uint8_t *destination_mac, uint8_t *source_mac, uint16_t *length_protocol, uint8_t *buffer, uint16_t *received_bytes);