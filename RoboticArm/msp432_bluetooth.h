/*
 * msp432_bluetooth.h
 *
 *  Created on: 16 Nov 2019
 *      Author: Xing Long
 */

/* POSIX Header Files */
#include <unistd.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <semaphore.h>

/* Driver Header Files */
#include <ti/drivers/UART.h>
#include "ti_drivers_config.h"

#ifndef MSP432_BLUETOOTH_H_
#define MSP432_BLUETOOTH_H_

extern pthread_t bluetoothThreadHandler;

extern sem_t txSem; // Semaphore to ensure each thread only access the bluetooth transmit function and variables one at a time
extern sem_t rxSem; // Semaphore to ensure each thread only access received data variables from one at a time

extern char uartTXBuffer[50];   // Buffer for handling transmission of data
extern char uartRXBuffer[50];   // Buffer for handling incoming data
extern UART_Handle uart;


/*
 * Incoming data that has been parsed will be placed in these variables
 * E.g FS,50,FG,12 into rxField = FS, rxValue = 50, rxField2 = FG, rxValue = 12
 */
extern char rxField[3];
extern float rxValue;
extern char rxField2[3];
extern float rxValue2;

extern void *bluetoothThread(void *arg0);

#endif /* MSP432_BLUETOOTH_H_ */
