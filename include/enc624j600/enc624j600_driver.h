#pragma once

/**
 *	@enum enc624j600_transmit_result
 *	@brief Returned values by enc624j600_transmit().
 *
 *	This enumeration defines the possible outcomes of an Ethernet
 *	frame transmission attempt.
 */
typedef enum {
    ENC_TRANSMIT_SUCCEEDED = 0,			/**< Frame transmitted successfully */
    ENC_TRANSMIT_FRAME_IS_TOO_SMALL,	/**< Frame length is less than 8 bytes */
    ENC_TRANSMIT_FRAME_EXCEED_MTU,		/**< Frame length exceeds 1514 bytes */
	ENC_FLOW_CONTROL_ACTIVE,			/**< Peer node has been paused the transmission temporary, so the frame wasn't sent */
    ENC_TRANSMIT_FAILED					/**< Frame transmission failed or any of the pointers are NULL */
} enc624j600_transmit_result;

/*
 *	@enum enc624j600_receive_result
 *	@brief Returned values by enc624j600_receive().
 * 
 *	This enumeration defines the possible outcomes of an Ethernet
 *	frame reception attempt.
 */
typedef enum {
	ENC_RECEIVE_SUCCEEDED = 0,	/**< Frame received successfully */
	ENC_RECEIVE_FAILED			/**< Any of the pointers are NULL */
} enc624j600_receive_result;

typedef struct {
	uint8_t *mac_address;			/**< Pointer to a 6-byte custom MAC address or NULL to use the preprogrammed MAC address */
	
	uint8_t mac_huge_frame : 1;		/**< Enable/Disalbe (1/0) If enable frame of any size will be allowed to be transmitted and received.
									*	 If disable frame bigger than 1518 bytes (whole frame) will be aborted when transmitted or received. */
	
	uint8_t mac_loopback : 1;		/**< Enable/Disable (1/0) transmitted frames are loopback inside the MAC before reaching the PHY */
	
	uint8_t phy_loppback : 1;		/**< Enable/Disable (1/0) PHY loopback of frames */
	
} enc624j600_config;


/**
 *	@brief Initializes the ENC624J600 driver.
 *
 *	This function indirectly uses the delay function provided by the HAL
 *	layer and must be called after the timer used for delays is configured
 *	and started.
 *
 *	@param config Pointer to the driver configuration structure.
 */
extern void enc624j600_driver_init(enc624j600_config *config);

/**
 *	@brief Signals the driver that an IRQ has occurred.
 *
 *	This function must be invoked from the ISR associated with the
 *	ENC624J600 interrupt line.
 */
extern void enc624j600_sig_driver_for_irq(void);

/**
 *	@brief Obtains the link status of the chip.
 * 
 *	@return
 *		1 - link is present  
 *		0 - link is down
 */
extern uint8_t enc624j600_link_status(void);

/**
 *	@brief Indicates whether a frame is pending in the chip SRAM.
 *
 *	@return
 *		1 - a frame is pending  
 *      0 - no frame is pending
 */
extern uint8_t enc624j600_pending_frame(void);

/**
 *	@brief Processes driver tasks.
 *
 *	This function must be called repeatedly from the main loop.
 */
extern void enc624j600_pump(void);

/**
 *	@brief Transmits an Ethernet frame.
 *
 *	This function transmits a single Ethernet frame. The payload is sent
 *	exactly as it appears in memory.
 * 
 *	@note The link must be present before calling this function.
 *       Use enc624j600_link_status() to check the link status.
 *
 *	@pre frame != NULL
 *
 *	@param frame  Pointer to the complete Ethernet frame (headers + payload).
 *	@param length Length of the frame in bytes (headers + payload).
 *
 *	@return enc624j600_transmit_result
 *		See @ref enc624j600_transmit_result for possible return values.
 */
extern enc624j600_transmit_result enc624j600_transmit(uint8_t *frame, uint16_t length);

/**
 *	@brief Receives an Ethernet frame.
 *
 *	@pre frame_buffer != NULL
 *	@pre received_bytes != NULL
 * 
 *	@note There must be a pending frame in the chip SRAM before calling this function.
 *			Use enc624j600_pending_frame() to check for a pending frame.
 *
 *	@param frame_buffer   Pointer to a buffer in which the frame will be stored.
 *	@param received_bytes Pointer to a variable where the received frame length
 *							will be written.
 *
 *	@return enc624j600_receive_result
 *		See @ref enc624j600_receive_result for possible return values.
 */
extern enc624j600_receive_result enc624j600_receive(uint8_t *frame_buffer, uint16_t *received_bytes);