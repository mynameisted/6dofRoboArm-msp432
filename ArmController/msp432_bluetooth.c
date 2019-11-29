/*
 * msp432_bluetooth.c
 *
 *  Created on: 16 Nov 2019
 *      Author: Xing Long
 */
/* Includes */

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

pthread_t bluetoothThreadHandler;

sem_t txSem;    // Semaphore to ensure each thread only access the bluetooth transmit function and variables one at a time
sem_t rxSem;    // Semaphore to ensure each thread only access received data variables from one at a time

char uartTXBuffer[50];  // Buffer for handling transmission of data
char uartRXBuffer[50];  // Buffer for handling incoming data
UART_Handle uart;


/* Incoming data that has been parsed will be placed in these variables
 * E.g FS,50,FG,12 into rxField = FS, rxValue = 50, rxField2 = FG, rxValue = 12
 */
char rxField[3];
float rxValue;
char rxField2[3];
float rxValue2;

void *bluetoothThread(void *arg0)
{
      int bytesReceived = 0;    // Length Of incoming string
      UART_Params uartParams;   // UART Parameters

      UART_init();              // Init UART

      UART_Params_init(&uartParams);                  // Init UART with the following settings
      uartParams.writeDataMode = UART_DATA_BINARY;    // Data will not be examined by the system and returned when newline is inserted as part of the message
      uartParams.readDataMode = UART_DATA_BINARY ;    // Data will not be examined by the system and returned when newline is inserted as part of the message
      uartParams.readReturnMode = UART_RETURN_FULL;   // Unblock/callback when buffer is full.
      uartParams.readTimeout = 800;                   // Timeout when time has reached 0.8ms
      uartParams.readEcho = UART_ECHO_OFF;            // Prevent echo the chars back to the sender
      uartParams.baudRate = 9600;                     // Baud Rate is set to 9600

      uart = UART_open(Bluetooth_UART, &uartParams);

      sem_init(&txSem, 0, 1);   //Init Transmit Semaphore
      sem_init(&rxSem, 0, 1);   //Init Receive Semaphore

      printf("Bluetooth Ready\n");
      while (1) {

          bytesReceived = UART_read(uart, uartRXBuffer, 50);
          if(bytesReceived != 0) {  // If there is incoming data
              BluetoothReceived();  // Function to handle incoming string
          }
      }
}


/*
 * Parse associative array in csv format
 * E.g FS,50 into rxField = FS, rxValue = 50
 */
void BluetoothReceived(){

    sem_wait(&rxSem);
    printf("BTR:%s\n",uartRXBuffer);

    char *token = strtok(uartRXBuffer, ",");
    int count = 0;

    while (token != NULL)   // Loops every substring where ',' is the delimiter
     {
         if(count == 0){
            sprintf(rxField,"%s",token);
         }
         else if(count == 1){
            rxValue = atof(token);
         }
         else if(count == 2){
            sprintf(rxField2,"%s",token);
         }
         else if(count == 3){
            rxValue2 = atof(token);
         }
        count ++;
        token = strtok(NULL, ",");// Go to next substring where ',' is the delimiter
     }

    sem_post(&rxSem);


}

/*
 * Simulates a associative array in csv format
 * E.g FS,50
 */
void BluetoothWriteKeyValue(char key[], double value){
    sem_wait(&txSem);                                       // Obtain semaphore to prevent other threads from accessing uartTXBuffer
    sprintf(uartTXBuffer, "%s,%.2f", key, value);           // Place Key values in csv format and output string in uartTXBuffer
    UART_write(uart, uartTXBuffer, strlen(uartTXBuffer));   // Transmit uartTXBuffer Over Bluetooth
    printf("BTS:%s\n", uartTXBuffer);
    sleep(1);                                               // Give Recipient Time to Process Data Before Senting the next over
    sem_post(&txSem);
}

/*
 * Simulates a associative array with two key values in csv format
 * E.g FS,50,PS,90
 */
void BluetoothWriteKeyValue2(char key[], double value, char key2[], double value2){
    sem_wait(&txSem);                                                   // Obtain semaphore to prevent other threads from accessing uartTXBuffer
    sprintf(uartTXBuffer,"%s,%.2f,%s,%.2f",key, value, key2, value2);   // Place Key values in csv format and output string in uartTXBuffer
    UART_write(uart, uartTXBuffer, strlen(uartTXBuffer));               // Transmit uartTXBuffer Over Bluetooth
    printf("BTS:%s\n", uartTXBuffer);
    sleep(1);                                                           // Give Recipient Time to Process Data Before Senting the next over
    sem_post(&txSem);
}


