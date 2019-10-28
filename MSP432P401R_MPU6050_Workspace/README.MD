Components:
--
1x MSP432P401R Launchpad (Texas Instruments - SimpleLink Ultra-Low-Power 32-Bit ARM Cortex-M4F MCU With Precision ADC, 256KB Flash and 64KB RAM)

1x Invensense MPU-6050 Six-Axis (Gyro + Accelerometer) MEMS MotionTracking™ Device

Instructions:
          To launch this program, open the entire folder as a workspace in CCS studio.

Circuit Schematics (MSP432P401R - MPU6050)
-----
P6.0 <--> VCC

GND <--> GND

P6.5 <--> SCL

P6.4 <--> SDA

*There is no need to use 10k pull up resistor for I2C as the internal pullup resistor is used in the MSP432P401R GPIO pins.
