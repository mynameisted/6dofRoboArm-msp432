/* POSIX Header files */
#include <unistd.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

/* Driver Header files */
#include <ti/drivers/PWM.h>

/* Driver configuration */
#include "ti_drivers_config.h"

/* Additional Modules */
#include "msp432_bluetooth.h"

pthread_t ledThreadHandler;


/*
 * ========================================
 * ledThread
 * ========================================
 * This thread intialises necessary pins to handle the Green LED on the msp432
 * The thread obtains data from the pressure sensor which translate the values into the brightness of the LED
 *
 */
void *ledThread(void *arg0){

       //Period and duty in microseconds
       uint16_t   pwmPeriod = 1000;
       uint16_t   duty = 0;
       PWM_Handle pwm1 = NULL;
       PWM_Params params;

       //Call driver init functions.
       PWM_init();

       PWM_Params_init(&params);
       params.dutyUnits = PWM_DUTY_US;
       params.dutyValue = 0;
       params.periodUnits = PWM_PERIOD_US;
       params.periodValue = pwmPeriod;
       pwm1 = PWM_open(CONFIG_PWM_6, &params);
       if (pwm1 == NULL) {
           while (1);
       }

       PWM_start(pwm1);

       printf("LED Thread Started\n");
       while (1) {

           // Obtain Rxbuffer semaphore to read values from bluetooth Rxbuffer
           sem_wait(&rxSem);

           if (strcmp(rxField, "PS") == 0){
               duty = (uint16_t) rxValue;
           }

           // Prevent duty from exceeding pwm period
           if(duty > pwmPeriod){
               duty = pwmPeriod;
           }

           // Set PWM Duty Of Green LED
           PWM_setDuty(pwm1, duty);

           // Release Rxbuffer semaphore
           sem_post(&rxSem);
           // Put thread to sleep to prevent Thread from utilizing too much of cpu resource
           sleep(0.5);
       }
}

