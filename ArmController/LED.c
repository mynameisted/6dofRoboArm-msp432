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

void *ledThread(void *arg0){

    /* Period and duty in microseconds */
       uint16_t   pwmPeriod = 1000;
       uint16_t   duty = 0;
       PWM_Handle pwm1 = NULL;
       PWM_Params params;

       /* Call driver init functions. */
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

           sem_wait(&rxSem);    // Obtain Rxbuffer semaphore to read values from bluetooth Rxbuffer

           if (strcmp(rxField, "PS") == 0){
               duty = (uint16_t) rxValue;
           }

           if(duty > pwmPeriod){    // Prevent duty from exceeding pwm period
               duty = pwmPeriod;
           }
           //printf("LED:%d\n",duty);
           PWM_setDuty(pwm1, duty);

           sem_post(&rxSem);    // Release Rxbuffer semaphore
           sleep(0.5);          // Put thread to sleep to prevent Thread from utilizing too much of cpu resource
       }
}

