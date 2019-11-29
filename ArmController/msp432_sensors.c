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

// Sample size of each sensor used to average out the readings
#define ADCSAMPLESIZE    (100)

pthread_t flexSensorThreadHandler, pressureSensorThreadHandler;

/*
 * ========================================
 * flexSensorThread
 * ========================================
 * Thread to get values from the flex sensor and transmit the data over bluetooth to the Arm
 * The Arm uses the values to controls how firmly the arm should grip an object
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

    // Initialize an ADC_Params structure to its default values.
    ADC_Params_init(&params);

    // Initialize the ADC peripheral
    adc = ADC_open(Flex_Sensor, &params);

    // If failed to init ADC
    if (adc == NULL) {
        printf("Error: Unable to start Flex Sensor Thread\n");
        while (1);
    }

    printf("Flex Sensor Thread Started\n");

    while(1) {

        // Get Raw ADC Value
        res = ADC_convert(adc, &adcValue);

        // If Raw ADC Value Fetching Was Successful
        if (res == ADC_STATUS_SUCCESS) {
            average += adcValue;
            counter++;

            // Increment Until ADCSAMPLESIZE has been reached this is used to smooth out the readings
            if (counter % ADCSAMPLESIZE == 0) {

                // Increment Until ADCSAMPLESIZE has been reached this is used to smooth out the readings
                BluetoothWriteKeyValue("FS",(float) average/ADCSAMPLESIZE);
                average = 0;
                counter = 0;
            }

       }
    }

    ADC_close(adc);
    return (NULL);
}


/*
 * ========================================
 * pressureSensorThread
 * ========================================
 * Thread to get values from the pressure sensor and send data and transits the data over bluetooth to the Arm Controller
 * The data is used in the arm controller to identifies how firmly the arm is griping an object by the brightness of the green led on the msp432
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

    // Initialize an ADC_Params structure to its default values.
    ADC_Params_init(&params);

    // Initialize the ADC peripheral
    adc = ADC_open(Pressure_Sensor, &params);

    // If failed to init ADC
    if (adc == NULL) {
        printf("Error: Unable to start Pressure Sensor Thread\n");
        while (1);
    }

    printf("Pressure Sensor Thread Started\n");

    while(1) {

        // Get Raw ADC Value
        res = ADC_convert(adc, &adcValue);

        // If Raw ADC Value Fetching Was Successful
        if (res == ADC_STATUS_SUCCESS) {
            average += adcValue;
            counter++;

            // Increment Until ADCSAMPLESIZE has been reached this is used to smooth out the readings
            if (counter % ADCSAMPLESIZE == 0) {
                BluetoothWriteKeyValue("PS",(float) average/ADCSAMPLESIZE);
                average = 0;
                counter = 0;
            }
       }
    }

    ADC_close(adc);
    return (NULL);

}
