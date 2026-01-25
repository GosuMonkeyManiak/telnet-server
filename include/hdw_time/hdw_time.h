#pragma once

#include <stdint.h>

/**
 *	@brief Indicates that 1 ms has elapsed.
 *
 *	This function must be called from an interrupt service routine
 *	that handles a hardware timer with a 1 ms period.
 */
extern void hdw_1ms_isr(void);

/**
 *	@brief Returns the current hardware time in milliseconds.
 * 
 *	@return Current hardware time in milliseconds.
 */
extern uint32_t hdw_time(void);