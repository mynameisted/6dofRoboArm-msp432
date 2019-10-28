/*
 * mps432_mpu6050.c
 *
 *  Created on: Oct 26, 2019
 *      Author: Ted
 */
#include "msp.h"
#include <msp432_mpu6050.h>
#include "ti_drivers_config.h"
#include <ti/drivers/I2C.h>
#include <stdio.h>


I2C_Handle      i2c;
I2C_Params      i2cParams;

I2C_Transaction i2cTransaction;         // Initialise I2C Transaction

uint8_t txBuffer[10];                   // Initialise I2C transmitting buffer for MPU6050
static uint8_t rxBuffer[20];            // Initialise I2C receiving buffer for MPU6050

uint8_t fs_sel = 3; //Gyro split
uint8_t afs_sel = 3; //accel split


/*
 * General Helper Functions
 *
 */
void delayMs(int n) {
    // Setting 1 cycle of Timer32 to be 1ms
    int i;
    TIMER32_1->LOAD = 3000 - 1;                  // Load timer with the number of ticks of the clock per millisecond (Clock rate/1000 e.g. 3Mhz/3 --> 3000)
    TIMER32_1->CONTROL = 0xC2;                   // Set Timer32 config: no prescaler, periodic wrapping mode, disable interrupt, 32-bit timer

    // Repeat the 1ms cycle of Timer32 n times
    for(i = 0; i < n; i++) {                     // Loop n number of times
        while((TIMER32_1->RIS & 1) == 0);        // Polling wait for RAW_IFG flag to be set when Timer32 finishes counting
            TIMER32_1->INTCLR = 0;               // Clear the RAW_IFG flag to 0
    }
}

int16_t merge8bitTo16bit(uint8_t val1, uint8_t val2) {
    return (int16_t) (((uint16_t) val1 << 8) | ((uint16_t) val2));
}


/*
 * I2C Functions
 *
 */

void i2cWrite(I2C_Handle i2c, uint8_t slaveRegAddr, uint8_t value)
{

    // Set local boolean to check I2C transfer completion
    bool retVal = 0;
    // Set txBuffer Register Address
    txBuffer[0] = slaveRegAddr;
    // Set txBuffer data to write
    txBuffer[1] = value;
    i2cTransaction.slaveAddress = MPU6050_ADDR;
    i2cTransaction.writeBuf = txBuffer;
    i2cTransaction.writeCount = 2;
    i2cTransaction.readBuf = rxBuffer;
    i2cTransaction.readCount = 0;
    /* Re-try writing to slave till I2C_transfer returns true */
    do {
        retVal = I2C_transfer(i2c, &i2cTransaction);
    } while(!retVal);
}

uint8_t* i2cRead(I2C_Handle i2c, uint8_t regAddr, uint8_t readCount)
{
    // Set local boolean to check I2C transfer completion
    bool retVal = 0;
    // Set transfer
    txBuffer[0] = regAddr; //register addr
    i2cTransaction.slaveAddress = MPU6050_ADDR;
    i2cTransaction.writeBuf = txBuffer;
    i2cTransaction.writeCount = 1;
    i2cTransaction.readBuf = rxBuffer;
    i2cTransaction.readCount = readCount;

    /* Re-try writing to slave till I2C_transfer returns true */
    do {
        retVal = I2C_transfer(i2c, &i2cTransaction);
    } while(!retVal);
    // Return pointer to the receiving buffer for reading
    return rxBuffer;
}


//=============================================================================================

/*
 * MPU6050 Functions
 *
 */

void setRegister(uint8_t regAddr, uint8_t value)
{
    i2cWrite(i2c, regAddr, value);
}

uint8_t getRegister(uint8_t regAddr)
{
    return i2cRead(i2c, regAddr, 1)[0];
}

int16_t* get3AxisData(uint8_t regAddr)
{
    uint8_t* tempBuffer = i2cRead(i2c, regAddr, 6);
    static int16_t axisData[3];
    axisData[0] = merge8bitTo16bit(tempBuffer[0], tempBuffer[1]);
    axisData[1] = merge8bitTo16bit(tempBuffer[2], tempBuffer[3]);
    axisData[2] = merge8bitTo16bit(tempBuffer[4], tempBuffer[5]);
    return axisData;
}
int16_t* getGyro()
{
    return get3AxisData(MPU6050_GYRO);
}

int16_t* getAccel()
{
    return get3AxisData(MPU6050_ACCEL);
}

void initialiseI2C(int i2cBitRate){
    //Init and set params for I2C
    I2C_init();
    I2C_Params_init(&i2cParams);
    if(i2cBitRate == 100){
        /*!< I2C Standard-mode. Up to 100 kbit/s. */
        i2cParams.bitRate = I2C_100kHz;
        //printf("I2C Bit rate set to 100kHz!\n");
    } else if (i2cBitRate == 400){
        /*!< I2C Fast-mode. Up to 400 kbit/s. */
        i2cParams.bitRate = I2C_400kHz;
        printf("I2C Bit rate set to 400kHz!\n");
    } else if (i2cBitRate == 1000){
        /*!< I2C Fast-mode Plus. Up to 1Mbit/s. */
        i2cParams.bitRate = I2C_1000kHz;
        printf("I2C Bit rate set to 1000kHz!\n");
    } else if (i2cBitRate == 3330){
        /*!< I2C High-speed mode. Up to 3.4Mbit/s. */
        i2cParams.bitRate = I2C_3330kHz;
        printf("I2C Bit rate set to 3330kHz!\n");
    } else if (i2cBitRate == 3400){
        /*!< I2C High-speed mode. Up to 3.4Mbit/s. */
        i2cParams.bitRate = I2C_3400kHz;
        printf("I2C Bit rate set to 3400kHz!\n");
    } else {
        // Use standard mode with bit rate 100kHz
        i2cParams.bitRate = I2C_100kHz;
        printf("Unknown bit rate entered, I2C Bit rate set to 100kHz!\n");
    }
}
void closeI2C(){
    /* Deinitialise I2C */
    I2C_close(i2c);
    printf("I2C closed!\n");

}
void wakeMPU6050(){
    //Wake up MPU
    printf("Waking up MPU6050\n");

    setRegister(MPU6050_POWER_MGMT_1, 0x40); //reset to default values
    delayMs(100);
    setRegister(MPU6050_POWER_MGMT_1, 0x00); //clear sleep bit
    delayMs(100);
    setRegister(MPU6050_POWER_MGMT_1, 0x01); //use internal gyro clock

    printf("MPU6050 Awake, WHOAMI = %x\n\n", getRegister(MPU6050_WHOAMI));
}
void startMPU6050()
{

    initialiseI2C(100);
    i2c = I2C_open(0, &i2cParams);
    P1OUT |= BIT0;


    printf("Power cycling MPU prior to reading of data for reliability\n");
    P6DIR = 0;
    P6OUT = 0;
    delayMs(5000);
    P1OUT &= ~BIT0;
    P6DIR = 1;
    P6OUT = 1;
    delayMs(5000);
    P1OUT |= BIT0;
    printf("Power cycle complete.\n");

    wakeMPU6050();

}
/*
static int16_t* gyroOffset;
void calibrateMPU6050(){
    printf("Calibrating MPU6050, please hold the device steady to initialise base offset...");
    delayMs(10000);
    *gyroOffset = getAccel();
    printf("\n\nGyro Offset in calibration XYZ = %d %d %d\n", gyroOffset[0], gyroOffset[1], gyroOffset[2]);
    printf("Calibration completed.\n");

    //return gyroOffset;
}
getGyroData(){
    printf("Calibrated Offset XYZ = %d %d %d\n", gyroOffset[0], gyroOffset[1], gyroOffset[2]);
    int16_t* rawGyro = getAccel();
    printf("rawGyro XYZ = %d %d %d\n", rawGyro[0], rawGyro[1], rawGyro[2]);
    rawGyro[0] -= gyroOffset[0];
    rawGyro[1] -= gyroOffset[1];
    rawGyro[2] -= gyroOffset[2];
    printf("Gyro with offset XYZ = %d %d %d\n\n\n", rawGyro[0], rawGyro[1], rawGyro[2]);
}
*/

//=============================================================================================

