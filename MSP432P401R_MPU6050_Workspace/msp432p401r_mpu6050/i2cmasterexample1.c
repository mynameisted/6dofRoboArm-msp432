/*
 * Copyright (c) 2016-2019, Texas Instruments Incorporated
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
 *    ======== i2cmasterexample1.c ========
 */
#include <stdint.h>
#include <stddef.h>
#include <unistd.h>

#include <stdio.h>
#include "msp432_mpu6050.h"
#include "msp.h"
#define GYRO_DIV 131
#define ACCEL_DIV 16384.0

/*
 *  ======== mainThread ========
 */
void *mainThread(void *arg0)
{
    volatile int16_t gyroX;
    volatile int16_t gyroY;
    volatile int16_t gyroZ;
    volatile long double accelX;
    volatile long double accelY;
    volatile long double accelZ;

    printf("\n== Start of thread ==\n\n");
    P1DIR |= BIT0;
    P1OUT &= ~BIT0;
    P6REN |= 0x30;
    P6OUT |= 0x30;
    printf("Startup MPU6050\n\n");
    startMPU6050();
    //delayMs(5000);
    //calibrateMPU6050();
    printf("==================================\n");
    printf("Reading MPU6050 Data...\n\n");
        while(1)
        {
            P1OUT ^= BIT0;
            //getGyroData();
            // Gyro Data
            int16_t* gyro = getGyro();
            gyroX = gyro[0] / GYRO_DIV;
            gyroY = gyro[1] / GYRO_DIV;
            gyroZ = gyro[2] / GYRO_DIV;
            //printf("[Gyro]  X = %-4d, Y =  %-4d, Z = %-4d\n", gyro[0]/131, gyro[1]/131, gyro[2]/131);
            //Accel data
            int16_t* accel = getAccel();
            accelX = accel[0] / ACCEL_DIV;
            accelY = accel[1] / ACCEL_DIV;
            accelZ = accel[2] / ACCEL_DIV;
            //printf("\n[Accel] X = %-4d, Y = %-4d, Z = %-4d\n", accel[0]/16384, accel[1]/2048, accel[2]/2048);
            printf("----------------------------------------\n");
            delayMs(1000);
            //delayMs(11900);
        }
    return (0);
}
