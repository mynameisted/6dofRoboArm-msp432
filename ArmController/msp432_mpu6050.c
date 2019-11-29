/*
 * msp432_mpu6050.c
 *
 *  Created on: Oct 26, 2019
 *      Author: Ted
 */

#include "msp.h"
#include <msp432_bluetooth.h>
#include <msp432_mpu6050.h>
#include "ti_drivers_config.h"
#include <ti/drivers/I2C.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <Math.h>

// Define constants for division of raw MPU6050 values
#define GYRO_DIV 131
#define ACCEL_DIV 16384

#define SAMPLE_DELAY_MS 100

pthread_t mpu6050ThreadHandler;

/*
 * I2C Variables
 *
 */
I2C_Handle      i2c;
I2C_Params      i2cParams;
// Initialise I2C transaction
I2C_Transaction i2cTransaction;
// Initialise I2C transmitting buffer for MPU6050
uint8_t txBuffer[10];
// Initialise I2C receiving buffer for MPU6050
static uint8_t rxBuffer[20];



/*
 *  MPU6050 Variables
 *
 */
volatile int16_t* gyroAccel;
// Raw gyro values
volatile int16_t gyro_x;
volatile int16_t gyro_y;
volatile int16_t gyro_z;
// Raw acceleration values
volatile int16_t accel_x;
volatile int16_t accel_y;
volatile int16_t accel_z;

// Baseline variables to store calibration values
int16_t base_x_accel;
int16_t base_y_accel;
int16_t base_z_accel;
int16_t base_x_gyro;
int16_t base_y_gyro;
int16_t base_z_gyro;

// Variables for translating raw values to angle values
float RADIANS_TO_DEGREES = 180/3.14159f;
volatile float accel_angle_y;
volatile float accel_angle_x;
float dt = SAMPLE_DELAY_MS/1000.0f;
volatile float gyro_angle_x;
volatile float gyro_angle_y;
volatile float gyro_angle_z;
volatile float unfiltered_gyro_angle_x;
volatile float unfiltered_gyro_angle_y;
volatile float unfiltered_gyro_angle_z;
float alpha = 0.91f;
volatile float angle_x;
volatile float angle_y;
volatile float angle_z;

// Store last values
volatile float prev_x_gyro = 0.0f;
volatile float prev_y_gyro = 0.0f;
volatile float prev_z_gyro = 0.0f;
volatile float prev_x_angle = 0.0f;
volatile float prev_y_angle = 0.0f;
volatile float prev_z_angle = 0.0f;


/*
 * =======================================
 * General Helper Functions
 * =======================================
 *
 */


/*
 * delayMs
 * ---
 * @input integer n
 *
 * This function uses timer32 to cause a delay, with input value n
 * being the delay in milliseconds.
 * E.g. delayMs(1000) = 1000ms = 1s
 *
 */
void delayMs(int n) {
    // Setting 1 cycle of Timer32 to be 1ms
    int i;
    TIMER32_1->LOAD = 48000 - 1;                  // Load timer with the number of ticks of the clock per millisecond (Clock rate/1000 e.g. 48Mhz/1000 --> 48000)
    TIMER32_1->CONTROL = 0xC2;                   // Set Timer32 config: no prescaler, periodic wrapping mode, disable interrupt, 32-bit timer

    // Repeat the 1ms cycle of Timer32 n times
    for(i = 0; i < n; i++) {                     // Loop n number of times
        while((TIMER32_1->RIS & 1) == 0);        // Polling wait for RAW_IFG flag to be set when Timer32 finishes counting
            TIMER32_1->INTCLR = 0;               // Clear the RAW_IFG flag to 0
    }
}

/*
 * merge8bitTo16bit
 * ---
 * @input uint8_t value1, uint8_t value2
 * @output uint16_t
 *
 * This function uses takes two 8 bit values and returns them as
 * a 16 bit value. Value 1 will be the most significant 8 bits.
 * E.g. merge8bitTo16bit(0x01, 0x11) = 0x0111
 *
 */

int16_t merge8bitTo16bit(uint8_t val1, uint8_t val2) {
    return (int16_t) (((uint16_t) val1 << 8) | ((uint16_t) val2));
}


/*
 * I2C Functions
 *
 */

/*
 * i2cWrite
 * ---
 * @input I2C_Handle, uint8_t registerAddress, uint8_t valueToWrite
 *
 * This function uses the I2C library to initiate an I2C write transaction
 * to the MPU6050 register address with the value given in the parameters.
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


/*
 * i2cRead
 * ---
 * @input I2C_Handle, uint8_t registerAddress, uint8_t readCount
 * @return int8_t pointer to the values read from the register
 * This function uses the I2C library to initiate an I2C read transaction
 * to the MPU6050 register address with readCount number of times.
 *
 */

int8_t* i2cRead(I2C_Handle i2c, uint8_t regAddr, uint8_t readCount)
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



/*
 * ========================================
 * MPU6050 Functions
 * ========================================
 *
 */

/*
 * setRegister
 * ---
 * @input uint8_t registerAddress, uint8_t valueToWrite
 *
 * This function uses the i2cWrite function to write a value
 * to the MPU register address given in the parameters.
 *
 */
void setRegister(uint8_t regAddr, uint8_t value)
{
    i2cWrite(i2c, regAddr, value);
}

/*
 * getRegister
 * ---
 * @input uint8_t registerAddress
 *
 * This function uses the i2cRead function to read the value
 * from the MPU register address given in the parameters.
 *
 */

int8_t getRegister(uint8_t regAddr)
{
    return i2cRead(i2c, regAddr, 1)[0];
}


/*
 * get3AxisData
 * ---
 * @input uint8_t registerAddress
 * @output int16_t* pointer to the retrieved values
 * This function uses the i2cRead function to read the 3 values (Gyro or Accel)
 * from the MPU register address given in the parameters. The values are then merged
 * into 16 bit values, and the pointer to the 16bit values are returned.
 *
 */

int16_t* get3AxisData(uint8_t regAddr)
{
    int8_t* tempBuffer = i2cRead(i2c, regAddr, 6);
    static int16_t axisData[3];
    axisData[0] = merge8bitTo16bit(tempBuffer[0], tempBuffer[1]);
    axisData[1] = merge8bitTo16bit(tempBuffer[2], tempBuffer[3]);
    axisData[2] = merge8bitTo16bit(tempBuffer[4], tempBuffer[5]);
    return axisData;
}

/*
 * getGyroAccel()
 * ---
 * @output int16_t* pointer to the retrieved values
 * This function uses the i2cRead function to read 6 values from the accel and gyro
 * registers in the MPU6050. The values are then merged
 * into 16 bit values, and the pointer to the 16bit values are returned.
 *
 */

int16_t* getGyroAccel()
{
    int8_t* tempBufferG = i2cRead(i2c, MPU6050_GYRO, 6);
    int8_t* tempBufferA = i2cRead(i2c, MPU6050_ACCEL, 6);
    static int16_t axisData[6];
    axisData[0] = merge8bitTo16bit(tempBufferG[0], tempBufferG[1]);
    axisData[1] = merge8bitTo16bit(tempBufferG[2], tempBufferG[3]);
    axisData[2] = merge8bitTo16bit(tempBufferG[4], tempBufferG[5]);
    axisData[3] = merge8bitTo16bit(tempBufferA[0], tempBufferA[1]);
    axisData[4] = merge8bitTo16bit(tempBufferA[2], tempBufferA[3]);
    axisData[5] = merge8bitTo16bit(tempBufferA[4], tempBufferA[5]);
    return axisData;
}

/*
 * getGyro
 * ---
 * @output int16_t* pointer to the retrieved gyro values
 * This function uses the get3AxisData function to read the Gyro values
 * from the MPU6050 gyro register address and returns a pointer to the value.
 *
 */

int16_t* getGyro()
{
    return get3AxisData(MPU6050_GYRO);
}


/*
 * getAccel
 * ---
 * @output int16_t* pointer to the retrieved accel values
 * This function uses the get3AxisData function to read the Accel values
 * from the MPU6050 accel register address and returns a pointer to the value
 *
 */

int16_t* getAccel()
{
    return get3AxisData(MPU6050_ACCEL);
}


/*
 * initialiseI2C
 * ---
 * @input int i2cBitRate
 * This function uses the I2C library to initialise the I2C connection.
 * The passed value in the parameter will set the I2C bit rate accordingly.
 * If any invalid bit rate is set, 100kHz will be used automatically.
 * Available Values:
 *      100kHz (default), 400kHz, 1000kHz, 3300kHz, 3400kHz
 *
 */

void initialiseI2C(int i2cBitRate){
    //Init and set params for I2C
    printf("Initialising I2C\n");
    I2C_init();
    I2C_Params_init(&i2cParams);
    if(i2cBitRate == 100){
        /*!< I2C Standard-mode. Up to 100 kbit/s. */
        i2cParams.bitRate = I2C_100kHz;
        printf("I2C Bit rate set to 100kHz!\n");
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

/*
 * closeI2C
 * ---
 * This function uses the I2C library to close the I2C connection.
 *
 */

void closeI2C(){
    /* Deinitialise I2C */
    I2C_close(i2c);
    printf("I2C closed!\n");

}

/*
 * wakeMPU6050
 * ---
 * @output int16_t* pointer to the retrieved values
 * This function wakes up the MPU6050 as it is in sleep mode by
 * default on power on.
 *
 */

void wakeMPU6050(){
    //Wake up MPU
    printf("Waking up MPU6050\n");
    P2OUT |= BIT0;
    // Reset the power management register to default value
    setRegister(MPU6050_POWER_MGMT_1, 0x40);
    delayMs(100);
    // Clear the sleep bit
    setRegister(MPU6050_POWER_MGMT_1, 0x00); //clear sleep bit
    delayMs(100);
    // Set the MPU6050 to use internal gyro clock
    setRegister(MPU6050_POWER_MGMT_1, 0x01);
    // Print the WHOAMI register to verify that the MPU6050 is awake
    printf("MPU6050 Awake, WHOAMI = %x\n\n", getRegister(MPU6050_WHOAMI));
}

/*
 * calibrate_sensors
 * ---
 * This function averages 10 readings of the MPU6050 gyro and accel
 * values to get a baseline reading to be used in the calculation of
 * the angle values. The MSP432 LED2 will be yellow during calibration.
 *
 */

// The sensor should be motionless on a horizontal surface
//  while calibration is happening
void calibrate_sensors() {
    // Set MSP432 LED2 to YELLOW to indicate calibration in progress
    P2OUT |= BIT1 + BIT0;

    int16_t                 x_accel = 0;
    int16_t                 y_accel = 0;
    int16_t                 z_accel = 0;
    int16_t                 x_gyro = 0;
    int16_t                 y_gyro = 0;
    int16_t                 z_gyro = 0;
    static int16_t* tempData;

    printf("Starting Calibration\n");
    // Discard first reading
    getGyroAccel();
    // Read and average the raw values from the IMU
    tempData = getGyroAccel();
    x_gyro += tempData[0];
    y_gyro += tempData[1];
    z_gyro += tempData[2];
    x_accel += tempData[3];
    y_accel += tempData[4];
    z_accel += tempData[5];
    delayMs(100);
    tempData = getGyroAccel();
    x_gyro += tempData[0];
    y_gyro += tempData[1];
    z_gyro += tempData[2];
    x_accel += tempData[3];
    y_accel += tempData[4];
    z_accel += tempData[5];
    delayMs(100);
    tempData = getGyroAccel();
    x_gyro += tempData[0];
    y_gyro += tempData[1];
    z_gyro += tempData[2];
    x_accel += tempData[3];
    y_accel += tempData[4];
    z_accel += tempData[5];
    delayMs(100);
    tempData = getGyroAccel();
    x_gyro += tempData[0];
    y_gyro += tempData[1];
    z_gyro += tempData[2];
    x_accel += tempData[3];
    y_accel += tempData[4];
    z_accel += tempData[5];
    delayMs(100);
    tempData = getGyroAccel();
    x_gyro += tempData[0];
    y_gyro += tempData[1];
    z_gyro += tempData[2];
    x_accel += tempData[3];
    y_accel += tempData[4];
    z_accel += tempData[5];
    delayMs(100);
    tempData = getGyroAccel();
    x_gyro += tempData[0];
    y_gyro += tempData[1];
    z_gyro += tempData[2];
    x_accel += tempData[3];
    y_accel += tempData[4];
    z_accel += tempData[5];
    delayMs(100);
    tempData = getGyroAccel();
    x_gyro += tempData[0];
    y_gyro += tempData[1];
    z_gyro += tempData[2];
    x_accel += tempData[3];
    y_accel += tempData[4];
    z_accel += tempData[5];
    delayMs(100);
    tempData = getGyroAccel();
    x_gyro += tempData[0];
    y_gyro += tempData[1];
    z_gyro += tempData[2];
    x_accel += tempData[3];
    y_accel += tempData[4];
    z_accel += tempData[5];
    delayMs(100);
    tempData = getGyroAccel();
    x_gyro += tempData[0];
    y_gyro += tempData[1];
    z_gyro += tempData[2];
    x_accel += tempData[3];
    y_accel += tempData[4];
    z_accel += tempData[5];
    delayMs(100);
    tempData = getGyroAccel();
    x_gyro += tempData[0];
    y_gyro += tempData[1];
    z_gyro += tempData[2];
    x_accel += tempData[3];
    y_accel += tempData[4];
    z_accel += tempData[5];
    delayMs(100);
    // Divide to get the average value
    x_accel /= 10;
    y_accel /= 10;
    z_accel /= 10;
    x_gyro /= 10;
    y_gyro /= 10;
    z_gyro /= 10;

    // Store the calibration values globally
    base_x_accel = x_accel;
    base_y_accel = y_accel;
    base_z_accel = z_accel;
    base_x_gyro = x_gyro;
    base_y_gyro = y_gyro;
    base_z_gyro = z_gyro;

    // Save the averaged reading as the last used gyro reading
    prev_x_gyro = x_gyro;
    prev_y_gyro = y_gyro;
    prev_z_gyro = z_gyro;


    printf("Finished Calibration\n");
}

/* 
 * ========================================
 * mpu6050Thread
 * ========================================
 * This thread intialises necessary pins, opens the I2C connection to the
 * MPU6050 and wakes up the MPU6050. It will then calibrate the sensor to get
 * an average baseline gyro and accel reading, and start retrieving the sensor values in a
 * continuous loop. A complementary filter is used to calculate the angles from the raw values.
 * The accel_angle_x and accel_angle_y values are then sent through bluetooth using UART.
 *
 */

void *mpu6050Thread(void *arg0)
{
    // Set LED1 direction (Heartbeat)
    P1DIR |= BIT0;
    P1OUT &= ~BIT0;
    // Set LED directions for LED2 (Calibration status)
    P2DIR |= (BIT0 + BIT1 + BIT2);
    P2OUT &= ~(BIT0 + BIT1 + BIT2);
    // Set P6.4 and P6.5 for I2C communication (SCL and SDA)
    P6REN |= 0x30;
    P6OUT |= 0x30;
    // Set LED1 to RED to indicate starting of MPU thread
    P1OUT |= BIT0;
    // Initiaise I2C with bit rate of 100kHz
    initialiseI2C(100);
    // Open I2C connection
    i2c = I2C_open(0, &i2cParams);
    // Wake MPU6050 up
    wakeMPU6050();
    // Calibrate MPU6050 base values
    calibrate_sensors();
    P2OUT &= ~BIT0;
    printf("Starting readings...\n");

    while(1){
        // Toggle LED1 as heartbeat
        P1OUT ^= BIT0;
        gyroAccel = getGyroAccel();
        // Convert gyro values to degrees/sec
        gyro_x = (gyroAccel[0] - base_x_gyro) / GYRO_DIV;
        gyro_y = (gyroAccel[1] - base_y_gyro) / GYRO_DIV;
        gyro_z = (gyroAccel[2] - base_z_gyro) / GYRO_DIV;
        // Get raw acceleration values
        accel_x = gyroAccel[3];
        accel_y = gyroAccel[4];
        accel_z = gyroAccel[5];
        // Calculate accel angle
        accel_angle_y = atan(-1*accel_x/sqrt(pow(accel_y,2) + pow(accel_z,2)))*RADIANS_TO_DEGREES;
        accel_angle_x = atan(accel_y/sqrt(pow(accel_x,2) + pow(accel_z,2)))*RADIANS_TO_DEGREES;
        // Compute the (filtered) gyro angles
        gyro_angle_x = gyro_x*dt + prev_x_gyro;
        gyro_angle_y = gyro_y*dt + prev_y_gyro;
        gyro_angle_z = gyro_z*dt + prev_z_gyro;
        // Compute the drifting gyro angles
        unfiltered_gyro_angle_x = gyro_x*dt + prev_x_gyro;
        unfiltered_gyro_angle_y = gyro_y*dt + prev_y_gyro;
        unfiltered_gyro_angle_z = gyro_z*dt + prev_z_gyro;
        // Apply the complementary filter to figure out the change in angle
        // Alpha depends on the sampling rate...
        angle_x = alpha*gyro_angle_x + (1.0f - alpha)*accel_angle_x;
        angle_y = alpha*gyro_angle_y + (1.0f - alpha)*accel_angle_y;
        angle_z = gyro_angle_z;  //Accelerometer doesn't give z-angle
        // Save angle values
        prev_x_angle = angle_x;
        prev_y_angle = angle_y;
        prev_z_angle = angle_z;
        // Save unfiltered gyro data
        prev_x_gyro = unfiltered_gyro_angle_x;
        prev_y_gyro = unfiltered_gyro_angle_y;
        prev_z_gyro = unfiltered_gyro_angle_z;
        //delayMs(500);
        BluetoothWriteKeyValue2("AX",accel_angle_x,"AY",accel_angle_y);
        //sprintf(buffer, "GX%.4f\nGY%.4f\nGZ%.4f\n", gyro_angle_x, gyro_angle_y, gyro_angle_z);
        //printf(buffer);
        //BluetoothWrite(buffer);
        //sprintf(buffer, "AX%fAY%fAZ%f\n", accel_angle_x, accel_angle_y, accel_z);
        //BluetoothWrite(buffer);

    }
}




