/*
 * Copyright (c) 2015-2019, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *  ======== pwmled2.c ========
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


#define SERVO1_DEFAULT 1300
#define SERVO2_DEFAULT 1500
#define SERVO3_DEFAULT 1000
#define SERVO4_DEFAULT 1200
#define SERVO5_DEFAULT 1500
#define SERVO6_DEFAULT 1000

#define SERVO_RELEASE 0
/*
 *  ======== mainThread ========
 *
 *
 * All servo direction bearing is from the point of view of the robotic arm.
 * (stand behind with base protruding away from you)
 *
 * For all Servos, set duty cycle to 0 to release all servo force.
 * *** Any servo that is holding parts up WILL FALL DOWN! ***
 *
 */
//                           Min, Mid,  Max
//                          ------------------
     volatile uint16_t   duty1;    // 500, 1300, 2500  (Right, Mid, Left)
     volatile uint16_t   duty2;    // 500, 1500, 2500  (Down, Mid, Up)
     volatile uint16_t   duty3;    // 2500, 1500, 500  (Down, Mid, Up)
     volatile uint16_t   duty4;    // 500, 1200, 2500  (Down, Straight, Up)
     volatile uint16_t   duty5;    // 500, 1500, 2500  (Anti-c/w, Straight, C/w || Rotate left, straight, right)
     volatile uint16_t   duty6;    // 1000, 1600, 2200 (Open, Loose grip, tight grip)
     volatile uint16_t   duty0;    // Deactivate servo (all tension force will be released)

     PWM_Handle pwm1;
     PWM_Handle pwm2;
     PWM_Handle pwm3;
     PWM_Handle pwm4;
     PWM_Handle pwm5;
     PWM_Handle pwm6;
     PWM_Params params;


     pthread_t servoThreadHandler;
     void *servoThread(void *arg0);

void startMG995();
void setDefaultPosition();
void updateServos(float angle_x,float angle_y);
void updateServosX(float angle_x);
void updateServosY(float angle_y);
void updateServosC(float angle_c);
#endif /* MSP432_MG995_H_ */

