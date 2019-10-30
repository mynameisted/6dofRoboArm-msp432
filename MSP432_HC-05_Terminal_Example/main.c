#include <Hardware/CS_Driver.h>
#include <Devices/MSPIO.h>

#define BUFFER_SIZE    128
/*Data Buffer*/
char Buffer[BUFFER_SIZE];

/*
 * Author: Xing Long
 * Credits to amartinezacosta, https://github.com/amartinezacosta/MSP432-HC-05 for his UART lib to connect to the terminal
 *
 * Allows input via terminal(tera term) to configure the bluetooth modules as master and slave
 *
 * Instruction
 * 1. Run Code,
 * 2. Run Tera Term
 * 3. Enter the debuger port to the MSP432
 * 4. Power Off MSP432
 * 5. Hold button on HC05
 * 6. Power On MSP432
 * 7. Enter the following commands into tera term
 *
 * Command For Slave
 * AT+ADDR? (Get Address), 98d3,51,fd9017
 *
 * Command For Master
 * AT+ROLE1 (Config to Master)
 * AT+CMODE=0 (Connect to one)
 * AT+BIND=98d3,51,fd9017 (Connect Master to Slave)
 *
 * To verfiy if configuration is correct
 * AT+ROLE? (1 = Master, 0 = Slave)
 * AT+CMODE? (Show mode 0 = One, 1 = many)
 * AT+BIND? (Show address to bind)
 */


/* UART Configuration Parameter
 * Parameters are configured using the calulated provided by texas instruments
 * http://software-dl.ti.com/msp430/msp430_public_sw/mcu/msp430/MSP430BaudRateConverter/index.html
 * This settings configures the terminal to accept 9600 baud rate
 */


eUSCI_UART_Config UART0Config =
{
     EUSCI_A_UART_CLOCKSOURCE_SMCLK,
     156,
     4,
     0,
     EUSCI_A_UART_NO_PARITY,
     EUSCI_A_UART_LSB_FIRST,
     EUSCI_A_UART_ONE_STOP_BIT,
     EUSCI_A_UART_MODE,
     EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION
};

/* UART Configuration Parameter
 * Parameters are configured using the calulated provided by texas instruments
 * http://software-dl.ti.com/msp430/msp430_public_sw/mcu/msp430/MSP430BaudRateConverter/index.html
 * This settings configures the HC0-05 to accept 384000 baud rate, which is the default settings when entering command mode
 */
eUSCI_UART_Config UART2Config =
{
     EUSCI_A_UART_CLOCKSOURCE_SMCLK,
     39,
     1,
     0,
     EUSCI_A_UART_NO_PARITY,
     EUSCI_A_UART_LSB_FIRST,
     EUSCI_A_UART_ONE_STOP_BIT,
     EUSCI_A_UART_MODE,
     EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION
};

void main(void){
    MAP_WDT_A_holdTimer();

    /*MSP432 running at 24 MHz*/
    CS_Init();


    UART_Init(EUSCI_A0_BASE, UART0Config);      /*Initialize UART A0 As Loopback for terminial*/
    UART_Init(EUSCI_A2_BASE, UART2Config);      /*Initialize UART A2 As for Bluetooth device*/

    MAP_Interrupt_enableMaster();

    while(1)
       {
           /*Get data from MSP432 PC UART*/
           MSPrintf(EUSCI_A0_BASE, "Enter data to be sent to HC-05: ");
           MSPgets(EUSCI_A0_BASE, Buffer, BUFFER_SIZE);
           MSPrintf(EUSCI_A0_BASE, "\r\n");

           /*Send data to HC-05*/
           MSPrintf(EUSCI_A2_BASE, Buffer);

           /*Get data from HC-05*/
           MSPrintf(EUSCI_A0_BASE, "Waiting for HC-05 response...\r\n");
           MSPgets(EUSCI_A2_BASE, Buffer, BUFFER_SIZE);

           /*Send data to serial terminal*/
           MSPrintf(EUSCI_A0_BASE, "Data received from HC-05: %s\r\n", Buffer);
       }

}
