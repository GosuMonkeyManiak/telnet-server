/*******************************************************************************
  Main Source File

  Company:
    Microchip Technology Inc.

  File Name:
    main.c

  Summary:
    This file contains the "main" function for a project.

  Description:
    This file contains the "main" function for a project.  The
    "main" function calls the "SYS_Initialize" function to initialize the state
    machines of all modules in the system
 *******************************************************************************/

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stdio.h>
#include <stddef.h>                     // Defines NULL
#include <stdbool.h>                    // Defines true
#include <stdlib.h>                     // Defines EXIT_FAILURE
#include "definitions.h"                // SYS function prototypes

#include "FreeRTOS.h"
#include "task.h"
#include "enc624j600/enc624j600_driver.h"

void my_first_task(void *parameter);
void my_second_task(void *parameter);

// *****************************************************************************
// *****************************************************************************
// Section: Main Entry Point
// *****************************************************************************
// *****************************************************************************

int main ( void )
{
    /* Initialize all modules */
    SYS_Initialize ( NULL );
    
    UART_SERIAL_SETUP uart_setup;
    uart_setup.baudRate = 9600;
    uart_setup.parity = UART_PARITY_NONE;
    uart_setup.dataWidth = UART_DATA_8_BIT;
    uart_setup.stopBits = UART_STOP_1_BIT;
    
    UART1_SerialSetup(&uart_setup, 0);
    
    SPI_TRANSFER_SETUP spi_setup;
    spi_setup.clockFrequency = 10000000;
    spi_setup.clockPhase = SPI_CLOCK_PHASE_LEADING_EDGE;
    spi_setup.clockPolarity = SPI_CLOCK_POLARITY_IDLE_LOW;
    spi_setup.dataBits = SPI_DATA_BITS_8;
    
    SPI2_TransferSetup(&spi_setup, 0);
    
    
    TRISDbits.TRISD0 = 0;
    TRISDbits.TRISD1 = 0;
    TRISDbits.TRISD2 = 0;
    
    TRISFbits.TRISF12 = 0;
    GPIO_PinSet(GPIO_PIN_RF12);
    
    
    // SYSCLK, PBCLK = 76Mhz
    // task priorities 0 - 4
    
    BaseType_t returned;
    
    returned = xTaskCreate(
            my_first_task,
            "first",
            configMINIMAL_STACK_SIZE,
            NULL,
            1,
            NULL);
    
    if (returned != pdPASS) {
        for (;;) {
            
        }
    }
    
    returned = xTaskCreate(
            my_second_task,
            "second",
            configMINIMAL_STACK_SIZE,
            NULL,
            2,
            NULL);
    
    if (returned != pdPASS) {
        for (;;) {
            
        }
    }
    
    vTaskStartScheduler();

    while ( true )
    {
        /* Maintain state machines of all polled MPLAB Harmony modules. */
        SYS_Tasks ( );
    }

    /* Execution should not come here during normal operation */

    return ( EXIT_FAILURE );
}

void my_first_task(void *parameter) {
    
    // toggle LED 2 every two seconds
    
    TickType_t ticks = xTaskGetTickCount();
    
    for (;;) {
        
        if ((xTaskGetTickCount() - ticks) > pdMS_TO_TICKS(500)) {
            
            GPIO_PinToggle(GPIO_PIN_RD1);
            
            ticks = xTaskGetTickCount();
        }
    }
}

uint8_t data[100];

void my_second_task(void *parameter) {
    
    // toggle LED 3
    // then sleep for 5 seconds
    
    enc624j600_init();
    
    uint8_t destination_address[6];
    uint8_t source_address[6];
    uint16_t length_protocol = 0;
    uint16_t data_length = 0;
    
    int i;
    
    for (;;) {
        
        GPIO_PinToggle(GPIO_PIN_RD2);
        
        if (enc624j600_receive(destination_address, source_address, &length_protocol, data, &data_length) == 0) {
            
            printf("Destination: ");
            for(i = 0; i < 6; i++) {
                
                if (i == 5) {
                    printf("%X\r\n", destination_address[i]);
                    continue;
                }
                
                printf("%X:", destination_address[i]);
            }
            
            printf("Source: ");
            for(i = 0; i < 6; i++) {
                
                if (i == 5) {
                    printf("%X\r\n", source_address[i]);
                    continue;
                }
                
                printf("%X:", source_address[i]);
            }
            
            printf("Length/Protocol: 0x%X\r\n", length_protocol);
            
            for (i = 0; i < data_length; i++) {
                printf("%c ", data[i]);
            }
            
            printf("\r\n\r\n");
            
        } else {
            printf("Nothing!\r\n");
        }
        
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}

void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName) {
    for (;;) {
        
    }
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

void enc624j600_hal_delay(uint8_t us) {
    TickType_t ticks = xTaskGetTickCount();
    
    while ((xTaskGetTickCount() - ticks) < pdMS_TO_TICKS(us)) {
        
    }
    
}

/*******************************************************************************
 End of File
*/

