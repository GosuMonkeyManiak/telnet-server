#include "definitions.h"

#include "hal/enc624j600_hal.h"
#include "enc624j600/enc624j600_driver_hal.h"
#include "enc624j600/enc624j600_driver.h"
#include "hdw_time/hdw_time.h"

static void external_int2(void) {
	enc624j600_sig_driver_for_irq();
}

void enc624j600_hal_init(void) {
	SPI_TRANSFER_SETUP spi_setup;
	
    spi_setup.clockFrequency = 10000000;
    spi_setup.clockPhase = SPI_CLOCK_PHASE_LEADING_EDGE;
    spi_setup.clockPolarity = SPI_CLOCK_POLARITY_IDLE_LOW;
    spi_setup.dataBits = SPI_DATA_BITS_8;
    
    SPI2_TransferSetup(&spi_setup, 0);
	
	// Chip select
	TRISFbits.TRISF12 = 0;
    GPIO_PinSet(GPIO_PIN_RF12);
	
	// INT pin
	EVIC_ExternalInterruptCallbackRegister(EXTERNAL_INT_2, external_int2);
	EVIC_ExternalInterruptEnable(EXTERNAL_INT_2);
}

uint8_t enc624j600_hal_spi_transfer(uint8_t data) {
    
    uint8_t receive = 0;
    
    SPI2_WriteRead(&data, 1, &receive, 1);
    
    return receive;
}

void enc624j600_hal_cs_assert(void) {
    
    // NOP 13 ns
    
    GPIO_PinClear(GPIO_PIN_RF12);
    
    Nop();
    Nop();
    Nop();
    Nop();
}

void enc624j600_hal_cs_deassert(void) {
    
    GPIO_PinSet(GPIO_PIN_RF12);
    
    Nop();
    Nop();
}

// WATCH HOW TO USE IT NOT IN CRITICAL SECTION
void enc624j600_hal_delay(uint16_t us) {
    uint32_t time = hdw_time();
	
	while ((hdw_time() - time) < us) {
		
	}
}

void enc624j600_enter_critical(void) {
	__builtin_disable_interrupts();
}

void enc624j600_exit_critical(void) {
	__builtin_enable_interrupts();
}