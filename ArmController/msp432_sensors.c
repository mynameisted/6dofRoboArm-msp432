/* POSIX Header files */
#include <stdint.h>
#include <stddef.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>

/* Driver Header files */
#include <ti/drivers/ADC.h>

/* Driver configuration */
#include "ti_drivers_config.h"

/* Additional Modules  */
#include <msp432_bluetooth.h>

#define ADCSAMPLESIZE    (100)

pthread_t flexSensorThreadHandler, pressureSensorThreadHandler;

/*
 * Part of the Arm Controller Code
 * Used to control the grip of the arm
 */
void *flexSensorThread(void *arg0)
{
    sleep(1.5); // Allow Bluetooth To Run Before Starting

    ADC_Handle   adc;
    ADC_Params   params;
    int_fast16_t res;
    uint16_t adcValue;
    uint32_t average = 0;
    uint16_t counter = 0;

    ADC_init();
    ADC_Params_init(&params);               // Initialize an ADC_Params structure to its default values.
    adc = ADC_open(Flex_Sensor, &params);   // Initialize the ADC peripheral

    if (adc == NULL) {                      // If failed to init ADC
        printf("Error: Unable to start Flex Sensor Thread\n");
        while (1);
    }

    printf("Flex Sensor Thread Started\n");

    while(1) {

        res = ADC_convert(adc, &adcValue);  // Get Raw ADC Value
        if (res == ADC_STATUS_SUCCESS) {    // If Raw ADC Value Fetching Was Successful
            average += adcValue;
            counter++;
            if (counter % ADCSAMPLESIZE == 0) { // Increment Until ADCSAMPLESIZE has been reached this is used to smooth out the readings
                BluetoothWriteKeyValue("FS",(float) average/ADCSAMPLESIZE); // Increment Until ADCSAMPLESIZE has been reached this is used to smooth out the readings
                average = 0;
                counter = 0;
            }

       }
    }

    ADC_close(adc);
    return (NULL);
}

/*
 * Part of the Robotic Arm Code
 * Used to send back how firmly the arm is griping an object
 */
void *pressureSensorThread(void *arg0)
{
    sleep(1.5); // Allow Bluetooth To Run Before Starting

    ADC_Handle   adc;
    ADC_Params   params;
    int_fast16_t res;
    uint16_t adcValue;
    uint32_t average = 0;
    uint16_t counter = 0;

    ADC_init();
    ADC_Params_init(&params);                   // Initialize an ADC_Params structure to its default values.
    adc = ADC_open(Pressure_Sensor, &params);   // Initialize the ADC peripheral

    if (adc == NULL) {                          // If failed to init ADC
        printf("Error: Unable to start Pressure Sensor Thread\n");
        while (1);
    }

    printf("Pressure Sensor Thread Started\n");

    while(1) {

        res = ADC_convert(adc, &adcValue);  // Get Raw ADC Value
        if (res == ADC_STATUS_SUCCESS) {    // If Raw ADC Value Fetching Was Successful
            average += adcValue;
            counter++;
            if (counter % ADCSAMPLESIZE == 0) { // Increment Until ADCSAMPLESIZE has been reached this is used to smooth out the readings
                BluetoothWriteKeyValue("PS",(float) average/ADCSAMPLESIZE);
                average = 0;
                counter = 0;
            }
       }
    }

    ADC_close(adc);
    return (NULL);

}

