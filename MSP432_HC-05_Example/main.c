#include "msp.h"
#include "bluetooth.h"

/*
 *  Example echos back incoming data
 *  Please extract bluetooth.c and bluetooth.h to use lib
 *
 *  Methods
 *  1. UART2_init()             , must be called before any other method to start bluetooth module
 *  2. EUSCIA2_IRQHandler(void) , must be edit in the lib to handle incoming data
 *  3. bluetoothWrite("string") , call method to transmit Data
 */
void main(void) {

    UART2_init(); // Initialize Bluetooth Module
    SCB->SCR |= (SCB_SCR_SLEEPDEEP_Msk);
    SCB->SCR |= SCB_SCR_SLEEPONEXIT_Msk;        // Do not wake up on exit from ISR
    __DSB();                                    // Ensures SLEEPONEXIT takes effect immediately
    __sleep();                                  // Go to LPM4
    __no_operation();                           // (Timer) For debugger

}

