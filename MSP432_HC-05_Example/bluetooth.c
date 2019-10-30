#include "bluetooth.h"

char RXBuffer[1];
/* Author: Xing Long
 * Bluetooth Library
 * Current example echos incoming data
 */

// Initialize Bluetooth At UART2 P3.2, P3.3
void UART2_init(void) {

    P3->SEL0 |= (BIT2 + BIT3);                  /* Set P3.2 and P3.3 to UART mode */
    P3->SEL1 &= ~(BIT2 + BIT3);                 /* Set P3.2 and P3.3 to UART mode */

    EUSCI_A2->CTLW0 |= 1;                       /* put in reset mode for config */
    EUSCI_A2->MCTLW = 0;                        /* disable oversampling */
    EUSCI_A2->CTLW0 = 0x0081;                   /* 1 stop bit, no parity, SMCLK, 8-bit data */
    EUSCI_A2->BRW = 312;                         /* 3,000,000 / 9600 = 312 */ /*Command Mode 3,000,000 / 38400 = 78 */
    EUSCI_A2->CTLW0 &= ~1;                      /* take UART out of reset mode */

    EUSCI_A2->IE |= UCRXIE;                     /* enable UART receive interupt */
    NVIC->ISER[0] |= 1 << ((EUSCIA2_IRQn) & 31);/* enable UART receive interupt on register */
    __enable_irq();                             /* enable global interupt */

    printf("%s \n","Bluetooth Initialized");
}

// Request Bluetooth To Transmit String
void bluetoothWrite(char *msg) {

    int i;
    for (i = 0; i < strlen(msg); i++) {
        while (!(EUSCI_A2->IFG & 0x02)) {}      /* wait for transmit buffer empty */
        EUSCI_A2->TXBUF = msg[i];               /* send a char */
    }

}



// Received Data From External Bluetooth Device
// Edit this to handle incoming request
void EUSCIA2_IRQHandler(void) {


    if (UCA2IFG & UCRXIFG)           // Check if interupt is based on Incoming Data
    {
        RXBuffer[0] = UCA2RXBUF;     // Convert into char
        bluetoothWrite(RXBuffer);    // Send received char back to sender
        UCA2IFG &= ~UCRXIFG;         // Reset interrupt back to 0
    }
}


