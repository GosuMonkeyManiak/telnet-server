#include <stdio.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include "definitions.h"

#include "lwip/tcpip.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/errno.h"

#include "ethernet/ethernet_if.h"
#include "enc624j600/enc624j600_driver.h"
#include "hal/enc624j600_hal.h"
#include "hdw_time/hdw_time.h"
#include "lwip/timeouts.h"
#include "lwip/init.h"

int main ( void )
{
    /* Initialize all modules */
    SYS_Initialize ( NULL );
    
	__builtin_disable_interrupts();
	
    UART_SERIAL_SETUP uart_setup;
    uart_setup.baudRate = 9600;
    uart_setup.parity = UART_PARITY_NONE;
    uart_setup.dataWidth = UART_DATA_8_BIT;
    uart_setup.stopBits = UART_STOP_1_BIT;
    UART1_SerialSetup(&uart_setup, 0);
    
	enc624j600_hal_init();
	
	TMR1_CallbackRegister(hdw_1ms_isr);
	
	lwip_init();
	
	// status led
	TRISDbits.TRISD1 = 0;
	
	__builtin_enable_interrupts();

	TMR1_Start(); // after start will generate interrupt
	
	enc624j600_config cfg;
	cfg.mac_address = NULL;
	cfg.mac_huge_frame = 0;
	cfg.mac_loopback = 0;
	cfg.phy_loppback = 0;
	
	enc624j600_driver_init(&cfg);
	
	ethernet_if_init();
	
	uint32_t time = hdw_time();
	
    while ( true )
    {
		if ((hdw_time() - time) > 1000) {
			
			GPIO_PinToggle(GPIO_PIN_RD1);
			
			time = hdw_time();
		}
		
		enc624j600_pump();
		
		ethernet_if_pump();
		
		sys_check_timeouts();
    }

    /* Execution should not come here during normal operation */
    return ( EXIT_FAILURE );
}
