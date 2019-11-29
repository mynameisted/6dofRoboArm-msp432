/*
 * msp432_mpu6050.h
 *
 *  Created on: Oct 26, 2019
 *      Author: Ted
 */


#ifndef MSP432_MPU6050_H_
#define MSP432_MPU6050_H_

#include <pthread.h>
#include <semaphore.h>
#include <stdint.h>
#include <stdio.h>
#include <ti/drivers/I2C.h>

/*
 * Function Prototype Declarations (RTOS Threading)
 *
 */

extern pthread_t mpu6050ThreadHandler;
extern void *mpu6050Thread(void *arg0);

/*
 * Definitions
 *
 */

// Define constants for MPU6050 register address
#define MPU6050_ADDR 0x68
#define MPU6050_POWER_MGMT_1 0x6B
#define MPU6050_WHOAMI 0x75
#define MPU6050_GYRO 0x43
#define MPU6050_ACCEL 0x3B


/*
 * Function Prototype Declarations (General Helpers)
 */
void delayMs(int n);
int16_t merge8bitTo16bit(uint8_t val1, uint8_t val2);



/*
 * Function Prototype Declarations (I2C)
 *
 */
void i2cWrite(I2C_Handle i2c, uint8_t slaveRegAddr, uint8_t value);
int8_t* i2cRead(I2C_Handle i2c, uint8_t regAddr, uint8_t readCount);
void initialiseI2C(int i2cBitRate);
void closeI2C();

/*
 * Function Prototype Declarations (MPU6050)
 *
 */
void wakeMPU6050();
void setRegister(uint8_t regAddr, uint8_t value);
int8_t getRegisterValue(uint8_t regAddr);
int16_t* get3Axis(uint8_t regAddr);
int16_t* getGyro();
int16_t* getAccel();
int16_t* getGyroAccel();
void calibrate_sensors();


#endif /* MSP432_MPU6050_H_ */
