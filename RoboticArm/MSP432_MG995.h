/*
 * MSP432_MG995.h
 *
 *  Created on: Oct 28, 2019
 *      Author: Shawn
 */

#ifndef MSP432_MG995_H_
#define MSP432_MG995_H_

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

/*
 * Definitions
 *
 */
// Define constants for the default servo PWM duty values
#define SERVO1_DEFAULT 1300
#define SERVO2_DEFAULT 1500
#define SERVO3_DEFAULT 1000
#define SERVO4_DEFAULT 1200
#define SERVO5_DEFAULT 1500
#define SERVO6_DEFAULT 1000
#define SERVO_RELEASE 0
/*
 *
 * All servo direction bearing is from the point of view of the robotic arm.
 * (stand behind with base protruding away from you)
 *
 * For all Servos, set duty cycle to 0 to release all servo force.
 * *** Any servo that is holding parts up WILL FALL DOWN! ***
 * 
 *                          PWM Table of Values
 * ===========================================================================
 * Servo #   |   duty1   |   Min   |   Mid   |    Max   |   Orientation
 * --------------------------------------------------------------------
 * Servo 1   |   duty1   |   500   |   1300  |   2500   | Right, Mid, Left
 * Servo 2   |   duty2   |   500   |   1500  |   2500   | Down, Mid, Up
 * Servo 3   |   duty3   |   2500  |   1500  |   500    | Down, Mid, Up
 * Servo 4   |   duty4   |   500   |   1200  |   2500   | Down, Straight, Up
 * Servo 5   |   duty5   |   500   |   1500  |   2500   | Anti-c/w, Straight, C/w
 * Servo 6   |   duty6   |   1000  |   1600  |   2200   | Open, loose grip, tight grip
 * 
 */
// Duty variables for each servo
volatile uint16_t   duty1;
volatile uint16_t   duty2;
volatile uint16_t   duty3;
volatile uint16_t   duty4;
volatile uint16_t   duty5;
volatile uint16_t   duty6;

// PwM handles for each servo
PWM_Handle pwm1;
PWM_Handle pwm2;
PWM_Handle pwm3;
PWM_Handle pwm4;
PWM_Handle pwm5;
PWM_Handle pwm6;
// PWM params to configure PWM
PWM_Params params;

/*
 * Function Prototype Declarations (RTOS Threading)
 *
 */
pthread_t servoThreadHandler;
void *servoThread(void *arg0);

/*
 * Function Prototype Declarations (MG995)
 *
 */
void startMG995();
void setDefaultPosition();
void updateServos(float angle_x,float angle_y);
void updateServosX(float angle_x);
void updateServosY(float angle_y);
void updateServosC(float angle_c);
#endif /* MSP432_MG995_H_ */

