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

// Semaphore to ensure each thread only access the bluetooth transmit function and variables one at a time
sem_t txSem;
// Semaphore to ensure each thread only access received data variables from one at a time
sem_t rxSem;


// Buffer for handling transmission of data
char uartTXBuffer[50];
// Buffer for handling incoming data
char uartRXBuffer[50];
UART_Handle uart;


/* Incoming data that has been parsed will be placed in these variables
 * E.g FS,50,FG,12 into rxField = FS, rxValue = 50, rxField2 = FG, rxValue = 12
 */
char rxField[3];
float rxValue;
char rxField2[3];
float rxValue2;

/*
 * ========================================
 * bluetoothThread
 * ========================================
 * This thread intialises necessary pins, opens the UART connection to the
 * HC-05. The thread handles incoming data as a associative array in csv format and
 * puts the values into rxField and rxValues variables
 *
 */
void *bluetoothThread(void *arg0)
{
    // Length Of incoming string
      int bytesReceived = 0;
      // UART Parameters
      UART_Params uartParams;

      // Init UART
      UART_init();

      // Init UART with the following settings
      UART_Params_init(&uartParams);
      // Data will not be examined by the system and returned when newline is inserted as part of the message
      uartParams.writeDataMode = UART_DATA_BINARY;
      // Data will not be examined by the system and returned when newline is inserted as part of the message
      uartParams.readDataMode = UART_DATA_BINARY ;
      // Unblock/callback when buffer is full.
      uartParams.readReturnMode = UART_RETURN_FULL;
      // Timeout when time has reached 0.8ms
      uartParams.readTimeout = 800;
      // Prevent echo the chars back to the sender
      uartParams.readEcho = UART_ECHO_OFF;
      // Baud Rate is set to 9600
      uartParams.baudRate = 9600;

      uart = UART_open(Bluetooth_UART, &uartParams);

      //Init Transmit Semaphore
      sem_init(&txSem, 0, 1);
      //Init Receive Semaphore
      sem_init(&rxSem, 0, 1);

      printf("Bluetooth Ready\n");
      while (1) {

          bytesReceived = UART_read(uart, uartRXBuffer, 50);
          // If there is incoming data
          if(bytesReceived != 0) {
              // Function to handle incoming string
              BluetoothReceived();
          }
      }
}


/*
 * BluetoothWriteKeyValue
 * ---
 * Parse incoming associative array in csv format and place the values into rxField and rxValue variables
 * E.g FS,50 (Incoming String) into rxField = FS, rxValue = 50
 */
void BluetoothReceived(){

    sem_wait(&rxSem);
    printf("BTR:%s\n",uartRXBuffer);

    char *token = strtok(uartRXBuffer, ",");
    int count = 0;

    // Loops every substring where ',' is the delimiter
    while (token != NULL)
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
        // Go to next substring where ',' is the delimiter
        token = strtok(NULL, ",");
     }

    sem_post(&rxSem);


}

/*
 * BluetoothWriteKeyValue
 * ---
 * @input char key, double value
 *
 * This function put the inputed values into a associative array in csv format
 * The values are then sent over bluetooth
 */
void BluetoothWriteKeyValue(char key[], double value){

    // Obtain semaphore to prevent other threads from accessing uartTXBuffer
    sem_wait(&txSem);

    // Place Key values in csv format and output string in uartTXBuffer
    sprintf(uartTXBuffer, "%s,%.2f", key, value);

    // Transmit uartTXBuffer Over Bluetooth
    UART_write(uart, uartTXBuffer, strlen(uartTXBuffer));
    printf("BTS:%s\n", uartTXBuffer);

    // Give Recipient 1s to Process Data Before Senting the next over
    sleep(1);

    // Release semaphore txSem
    sem_post(&txSem);
}

/*
 * BluetoothWriteKeyValue2
 * ---
 * @input char key, double value, char key2[], double value2
 *
 * This function put the inputed values into a associative array in csv format
 * The values are then sent over bluetooth as a string
 * E.g FS,50,PS,90
 */
void BluetoothWriteKeyValue2(char key[], double value, char key2[], double value2){

    // Obtain txSem semaphore to prevent other threads from accessing uartTXBuffer
    sem_wait(&txSem);
    // Place Key values in csv format and output string in uartTXBuffer
    sprintf(uartTXBuffer,"%s,%.2f,%s,%.2f",key, value, key2, value2);
    // Transmit uartTXBuffer Over Bluetooth
    UART_write(uart, uartTXBuffer, strlen(uartTXBuffer));
    printf("BTS:%s\n", uartTXBuffer);

    // Give Recipient 1s to Process Data Before Senting the next over
    sleep(1);

    // Release semaphore txSem
    sem_post(&txSem);
}


