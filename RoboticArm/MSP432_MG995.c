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

/* Self-created Drivers */
#include "MSP432_MG995.h"
#include "msp432_bluetooth.h"
/*
 *  ======== mainThread ========
 *
 */

pthread_t servoThreadHandler;

void *servoThread(void *arg0){
    /* Period and duty in microseconds */

    sleep(1);

    uint16_t   pwmPeriod = 20000;

    pwm1 = NULL;
    pwm2 = NULL;
    pwm3 = NULL;
    pwm4 = NULL;
    pwm5 = NULL;
    pwm6 = NULL;

    /* Call driver init functions. */

    PWM_init();

    PWM_Params_init(&params);
    params.dutyUnits = PWM_DUTY_US;
    params.dutyValue = 0;
    params.periodUnits = PWM_PERIOD_US;
    params.periodValue = pwmPeriod;

    pwm1 = PWM_open(CONFIG_PWM_0, &params);
    if (pwm1 == NULL) {
        /* CONFIG_PWM_0 did not open */
        while (1);
    }
    PWM_start(pwm1);

    pwm2 = PWM_open(CONFIG_PWM_1, &params);
    if (pwm2 == NULL) {
        /* CONFIG_PWM_1 did not open */
        while (1);
    }
    PWM_start(pwm2);

    pwm3 = PWM_open(CONFIG_PWM_2, &params);
    if (pwm3 == NULL) {
        /* CONFIG_PWM_2 did not open */
        while (1);
    }
    PWM_start(pwm3);

    pwm4 = PWM_open(CONFIG_PWM_3, &params);
    if (pwm4 == NULL) {
        /* CONFIG_PWM_3 did not open */
        while (1);
    }
    PWM_start(pwm4);

    pwm5 = PWM_open(CONFIG_PWM_4, &params);
    if (pwm5 == NULL) {
        /* CONFIG_PWM_4 did not open */
        while (1);
    }
    PWM_start(pwm5);

    pwm6 = PWM_open(CONFIG_PWM_5, &params);
    if (pwm6 == NULL) {
        /* CONFIG_PWM_5 did not open */
        while (1);
    }

    PWM_start(pwm6);

    setDefaultPosition();
    printf("Servo Started \n");
    while(1){
        getUpdate();
    }
}

void setDefaultPosition(void){
    PWM_setDuty(pwm1, SERVO1_DEFAULT);
    PWM_setDuty(pwm2, SERVO2_DEFAULT);
    PWM_setDuty(pwm3, SERVO3_DEFAULT);
    PWM_setDuty(pwm4, SERVO4_DEFAULT);
    PWM_setDuty(pwm5, SERVO5_DEFAULT);
    PWM_setDuty(pwm6, SERVO6_DEFAULT);

    // Release tension for servo6 (Claw) to avoid strain and over-heating
    //PWM_setDuty(pwm6, SERVO_RELEASE);
}


void getUpdate(){

    //printf("SO: %s %f \n",rxField, rxValue);

    sem_wait(&rxSem);
    //printf("SO: %s %f %s %f\n",rxField,rxValue,rxField2,rxValue2);


    if (strcmp(rxField, "AX") == 0){
        updateServosX(rxValue);
    }
    else if (strcmp(rxField, "AY") == 0){
        updateServosY(rxValue);
    }
    else if (strcmp(rxField, "FS") == 0){
        updateServosC(rxValue);
    }

    if (strcmp(rxField2, "AX") == 0){
        updateServosX(rxValue2);
    }
    else if (strcmp(rxField2, "AY") == 0){
        updateServosY(rxValue2);
    }

    sem_post(&rxSem);
    sleep(1);

}

void updateServosX(float angle_x){
    // Setting limits for the angle
    if (angle_x > 50){
        angle_x = 50;
    } else if (angle_x < -50){
        angle_x = -50;
    }
    // Servo 2
    // Set temp duty variable
    volatile int16_t tempDuty2_4 = 0.0f;
    volatile int16_t tempDuty3 = 0.0f;
    // Translation algorithm to control the servos
    if (angle_x > 0){
        tempDuty2_4 = (int16_t)(2500 - (20 * (50 - angle_x)));
        tempDuty3 = (int16_t)(500 + (20 * (50 - angle_x)));
    } else {
        tempDuty2_4 = (int16_t)(2500 - (20 * (fabs(angle_x) + 50)));
        tempDuty3 = (int16_t)(500 + (20 * (fabs(angle_x) + 50)));
    }
    // Set duty to variable
    duty2 = tempDuty2_4;
    duty3 = tempDuty3;
    duty4 = tempDuty2_4;
    // Update servo position

    //printf("duty2 %d\n",duty2);
    //printf("duty3 %d\n",duty3);
    //printf("duty4 %d\n",duty4);

    PWM_setDuty(pwm2, duty2);
    PWM_setDuty(pwm3, duty3);
    PWM_setDuty(pwm4, duty4);
}
void updateServosY(float angle_y){
    // Setting limits for the angle
    if (angle_y > 50){
        angle_y = 50;
    } else if (angle_y < -50){
        angle_y = -50;
    }
    // Set temp duty variable
    volatile int16_t tempDuty = 0.0f;
    // Translation algorithm to control the servos
    if (angle_y > 0){
        tempDuty = (int16_t)(2500 - (20 * (angle_y + 50)));
    } else {
        tempDuty = (int16_t)(2500 - (20 * (50 - fabs(angle_y))));
    }
    // Set duty to variable
    duty1 = tempDuty;
    // Update servo position

    //printf("duty1 %d\n",duty1);

    PWM_setDuty(pwm1, duty1);
}

void updateServosC(float angle_c){
    // Setting limits for the angle
   if (angle_c > 5){
       duty6 = 2200;
   } else{
       duty6 = 1000;
   }

    PWM_setDuty(pwm6, 2200);
    // 1000, 1600, 2200 (Open, Loose grip, tight grip)

    //PWM_setDuty(pwm6, duty6);


    /*
    if (angle_c > 4000){
        angle_c = 4000;
    } else if (angle_c < 1000){
        angle_c = 1000;
    }
    // Set temp duty variable
    volatile int16_t tempDuty = 0.0f;
    // Translation algorithm to control the servos
    tempDuty = 2200 - (12 * ((angle_c - 1000)/30));
    // Set duty to variable
    duty6 = tempDuty;
    // Update servo position
    printf("duty6 %d\n",duty6);
    PWM_setDuty(pwm6, duty6);
    */
}
