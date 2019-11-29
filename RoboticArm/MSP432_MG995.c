/*
 * MSP432_MG995.c
 *
 *  Created on: Oct 28, 2019
 *      Author: Shawn
 */

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

pthread_t servoThreadHandler;

/*
 * ========================================
 * servoThread
 * ========================================
 * Thread intialises necessary pins and connect the PWM to the MG995
 *
 * Thread intialises and maps the readings obtained from the flex Sensor and MPU6050 to
 * the appropriate input readings necessary for the MG995 Servos to function properly
 * The Arm uses the values to controls how firmly the arm should grip an object
 */
void *servoThread(void *arg0){

    // Period and duty in microseconds
    sleep(1);
    // Set pwmPeriod to 20ms
    uint16_t   pwmPeriod = 20000;
    // Initialize pwm handles
    pwm1 = NULL;
    pwm2 = NULL;
    pwm3 = NULL;
    pwm4 = NULL;
    pwm5 = NULL;
    pwm6 = NULL;

    // Call the PWM_init function from the PWM library
    PWM_init();
    // Initialise PWM params
    PWM_Params_init(&params);
    params.dutyUnits = PWM_DUTY_US;
    params.dutyValue = 0;
    params.periodUnits = PWM_PERIOD_US;
    params.periodValue = pwmPeriod;
    // Open PWM1 handle
    pwm1 = PWM_open(CONFIG_PWM_0, &params);
    if (pwm1 == NULL) {
        /* CONFIG_PWM_0 did not open */
        while (1);
    }
    // Start PWM handle
    PWM_start(pwm1);

    // Open PWM2 handle
    pwm2 = PWM_open(CONFIG_PWM_1, &params);
    if (pwm2 == NULL) {
        /* CONFIG_PWM_1 did not open */
        while (1);
    }
    PWM_start(pwm2);

    // Open PWM3 handle
    pwm3 = PWM_open(CONFIG_PWM_2, &params);
    if (pwm3 == NULL) {
        /* CONFIG_PWM_2 did not open */
        while (1);
    }
    PWM_start(pwm3);

    // Open PWM4 handle
    pwm4 = PWM_open(CONFIG_PWM_3, &params);
    if (pwm4 == NULL) {
        /* CONFIG_PWM_3 did not open */
        while (1);
    }
    PWM_start(pwm4);

    // Open PWM5 handle
    pwm5 = PWM_open(CONFIG_PWM_4, &params);
    if (pwm5 == NULL) {
        /* CONFIG_PWM_4 did not open */
        while (1);
    }
    PWM_start(pwm5);

    // Open PWM6 handle
    pwm6 = PWM_open(CONFIG_PWM_5, &params);
    if (pwm6 == NULL) {
        /* CONFIG_PWM_5 did not open */
        while (1);
    }
    PWM_start(pwm6);

    // Call the setDefaultPosition function to set all servos to the configured default position
    setDefaultPosition();
    printf("Servo Started \n");
    while(1){
        // Get values from bluetooth through UART
        getUpdate();
    }
}
/*
 * setDefaultPosition
 * ---
 * This function uses the PWM_setDuty function from the PWM library
 * to set the duty value for the 6 servos.
 */

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

/*
 * getUpdate
 * ---
 * This function uses the rxField and rxValues obtained from bluetooth
 * The values obtained are used to update the servos values
 */

void getUpdate(){

    sem_wait(&rxSem);

    // Angle X Reading Of MPU6050
    if (strcmp(rxField, "AX") == 0){
        updateServosX(rxValue);
    }
    // Angle Y Reading Of MPU6050
    else if (strcmp(rxField, "AY") == 0){
        updateServosY(rxValue);
    }
    // Flex Sensor Reading
    else if (strcmp(rxField, "FS") == 0){
        updateServosC(rxValue);
    }

    // Angle X Reading Of MPU6050
    if (strcmp(rxField2, "AX") == 0){
        updateServosX(rxValue2);
    }
    // Angle Y Reading Of MPU6050
    else if (strcmp(rxField2, "AY") == 0){
        updateServosY(rxValue2);
    }

    sem_post(&rxSem);
    sleep(1);

}

/*
 * updateServosX
 * ---
 * This function takes in the calculated angle of the x axis from the
 * MPU6050 and translates the angle using a self-created algorithm to 
 * the respective PWM duty for servo 2, 3 and 4.
 * --------------------------
 * Angle_x range [-50 ~ 50]
 * --------------------------
 * Duty2 Range [500 ~ 2500]
 * Duty3 Range [2500 ~ 500]
 * Duty4 Range [500 ~ 2500]
 * 
 */
void updateServosX(float angle_x){
    // Setting upper and lower limits for the angle
    if (angle_x > 50){
        angle_x = 50;
    } else if (angle_x < -50){
        angle_x = -50;
    }
    // Set temp duty variables
    volatile int16_t tempDuty2_4 = 0.0f;
    volatile int16_t tempDuty3 = 0.0f;

    // Translation algorithm to get translated duty value based on angle
    if (angle_x > 0){
        // Duty for servos 2 and 4
        tempDuty2_4 = (int16_t)(2500 - (20 * (50 - angle_x)));
        // Duty for servo 3
        tempDuty3 = (int16_t)(500 + (20 * (50 - angle_x)));
    } else {
        // Duty for servos 2 and 4
        tempDuty2_4 = (int16_t)(2500 - (20 * (fabs(angle_x) + 50)));
        // Duty for servo 3
        tempDuty3 = (int16_t)(500 + (20 * (fabs(angle_x) + 50)));
    }
    // Save the value to the global variables
    duty2 = tempDuty2_4;
    duty3 = tempDuty3;
    duty4 = tempDuty2_4;

    // Update servos position by calling PWM_setDuty
    PWM_setDuty(pwm2, duty2);
    PWM_setDuty(pwm3, duty3);
    PWM_setDuty(pwm4, duty4);
}

/*
 * updateServosY
 * ---
 * This function takes in the calculated angle of the y axis from the
 * MPU6050 and translates the angle using a self-created algorithm to 
 * the respective PWM duty for servo 1.
 * --------------------------
 * Angle_y range [-50 ~ 50]
 * --------------------------
 * Duty1 Range [500 ~ 2500]
 * 
 */
void updateServosY(float angle_y){
    // Setting upper and lower limits for the angle
    if (angle_y > 50){
        angle_y = 50;
    } else if (angle_y < -50){
        angle_y = -50;
    }
    // Set temp duty variable
    volatile int16_t tempDuty = 0.0f;
    // Translation algorithm to get translated duty value based on angle
    if (angle_y > 0){
        // Duty for servos 1
        tempDuty = (int16_t)(2500 - (20 * (angle_y + 50)));
    } else {
        // Duty for servos 1
        tempDuty = (int16_t)(2500 - (20 * (50 - fabs(angle_y))));
    }
    // Save the value to the global variables
    duty1 = tempDuty;

    // Update servo position by calling PWM_setDuty
    PWM_setDuty(pwm1, duty1);
}


/*
 * updateServosC
 * ---
 * This function takes in the values of the flex sensor
 * and translates the angle using a self-created algorithm to
 * the respective PWM duty for servo 6.
 * --------------------------
 * Angle_c range [1000 ~ 4000]
 * --------------------------
 * Duty1 Range [1000 ~ 2200]
 *
 */

void updateServosC(float angle_c){
    /*
     * This version is due to high noise from the flex sensor.
     * If noise is reduced, use the version below to translate the values
     */
    // Setting upper and lower limits for the angle, Default reading from flex senor
   if (angle_c > 5){
       duty6 = 2200;
   } else{
       duty6 = 1000;
   }
    // Update servos position by calling PWM_setDuty
    PWM_setDuty(pwm6, duty6);

    /* ===========================================================
     * This version is if the flex sensor readings are usable.
     * 
     */
    /*
    // Setting upper and lower limits for the angle
    if (angle_c > 4000){
        angle_c = 4000;
    } else if (angle_c < 1000){
        angle_c = 1000;
    }
    // Set temp duty variable
    volatile int16_t tempDuty = 0.0f;
    // Translation algorithm to get translated duty value based on angle
    tempDuty = 2200 - (12 * ((angle_c - 1000)/30));
    // Save the value to the global variables
    duty6 = tempDuty;
    // Update servos position by calling PWM_setDuty
    PWM_setDuty(pwm6, duty6);
    // =============================================================
    */
}
