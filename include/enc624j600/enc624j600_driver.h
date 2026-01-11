#pragma once

/**
 *	@enum enc624j600_transmit_result
 *	@brief Returned values by enc624j600_transmit().
 *
 *	This enumeration defines the possible outcomes of an Ethernet
 *	frame transmission attempt.
 */
typedef enum {
    ENC_TRANSMIT_SUCCEEDED = 0,		/**< Frame transmitted successfully */
    ENC_TRANSMIT_DATA_IS_TOO_SMALL,	/**< Payload length is less than 8 bytes */
    ENC_TRANSMIT_DATA_EXCEED_MTU,	/**< Payload length exceeds 1500 bytes */
	ENC_FLOW_CONTROL_ACTIVE,		/**< Peer node has been paused the transmission temporary, so the frame wasn't sent */
    ENC_TRANSMIT_FAILED				/**< Frame transmission failed or any of the pointers are NULL */
} enc624j600_transmit_result;

/*
 *	@enum enc624j600_receive_result
 *	@brief Returned values by enc624j600_receive().
 * 
 *	This enumeration defines the possible outcomes of an Ethernet
 *	frame reception attempt.
 */
typedef enum {
	ENC_RECEIVE_SUCCEEDED = 0,
	ENC_RECEIVE_NO_PENDING_FRAME,
	ENC_RECEIVE_FAILED
} enc624j600_receive_result;

typedef struct {
	uint8_t *mac_address;				/**< Pointer to a 6-byte custom MAC address or NULL to use the preprogrammed MAC address */
	
	uint8_t mac_huge_frame : 1;		/**< Enable/Disalbe (1/0) If enable frame of any size will be allowed to be transmitted and received.
									*	 If disable frame bigger than 1518 bytes (whole frame) will be aborted when transmitted or received. */
	
	uint8_t mac_loopback : 1;		/**< Enable/Disable (1/0) transmitted frames are loopback inside the MAC before reaching the PHY */
	
	uint8_t phy_loppback : 1;		/**< Enable/Disable (1/0) PHY loopback of frames */
	
} enc624j600_config;


/**
 * 
 *	@param config
 */
extern void enc624j600_init(enc624j600_config *config);

/**
 *	@brief Transmits an Ethernet frame.
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
extern enc624j600_transmit_result enc624j600_transmit(uint8_t *destination_mac, uint8_t *length_type, uint8_t *data, uint16_t length);

/**
 *	@brief Receives an Ethernet frame.
 *
 *	This function receives an Ethernet frame and extracts the destination
 *	MAC address, source MAC address, Length/Type field, and payload data.
 *
 *	@pre destination_mac != NULL
 *	@pre source_mac != NULL
 *	@pre length_type != NULL
 *	@pre buffer != NULL
 *	@pre received_bytes != NULL
 * 
 *	@param destination_mac Pointer to a 6-byte buffer where the destination
 *						   MAC address will be stored.
 *	@param source_mac Pointer to a 6-byte buffer where the source
 *                    MAC address will be stored.
 *	@param length_type Pointer to a 2-byte buffer where the Length/Type
 *                     field will be stored.
 *	@param buffer Pointer to a buffer where the received payload
 *				  data will be written. The buffer must be at least
 *				  1500 bytes in length.
 *	@param received_bytes Pointer to a variable where the number of payload
 *                        bytes received will be stored.
 *
 *	@return enc624j600_receive_result
 *		See @ref enc624j600_receive_result for possible return values.
 */
extern enc624j600_receive_result enc624j600_receive(uint8_t *destination_mac, uint8_t *source_mac, uint8_t *length_type, uint8_t *buffer, uint16_t *received_bytes);