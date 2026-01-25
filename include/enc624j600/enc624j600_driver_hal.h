#pragma once

#include <stdint.h>

/**
 *  @brief Microcontroller-specific function that performs a full-duplex SPI transfer.
 * 
 *  Clocks out one byte of data on MOSI while simultaneously
 *  clocking in data on MISO. Data is transmitted and received MSbit first, LSbit last.
 *  
 *  @note The ENC624J600 supports only SPI Mode 0 (CPOL = 0, CPHA = 0).
 *        The maximum supported SPI clock frequency is 14 Mhz.
 * 
 *  @param data Byte to be transmitted on MOSI.
 * 
 *  @return Byte received on MISO.
 */
extern uint8_t enc624j600_hal_spi_transfer(uint8_t data);

/**
 *  @brief Microcontroller-specific function that asserts the ENC624J600 chip select.
 *
 *  This function enforces the minimum required chip select setup time of
 *  50 ns before returning.
 */
extern void enc624j600_hal_cs_assert(void);

/**
 *  @brief Microcontroller-specific function that deasserts the ENC624J600 chip select.
 * 
 *  This function enforces the minimum required chip select disable time of 
 *  20 ns before returning.
 */
extern void enc624j600_hal_cs_deassert(void);

/**
 *	@brief Enter a critical section.
 *
 *	This function provides mutual exclusion for shared resources used by the
 *	ENC624J600 driver. The implementation may disable interrupts to prevent
 *	concurrent access while the critical section is active.
 */
extern void enc624j600_enter_critical(void);

/**
 *	@brief Exit a critical section.
 *
 *	This function releases the protection acquired by
 *	enc624j600_enter_critical() and restores the previous execution state.
 */
extern void enc624j600_exit_critical(void);

/**
 *  @brief Microcontroller-specific function that delays the execution for specific number of microseconds.
 * 
 *  @param us Time in microseconds to delay the execution.
 */
extern void enc624j600_hal_delay(uint16_t us);