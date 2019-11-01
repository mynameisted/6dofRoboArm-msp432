#include "msp.h"
#include "mg995_driver.h"
/**
 *
 * ==============================
 *              MG995
 * ==============================
 *
 * REFO Clock rate of 128Khz
 * - PWM_PERIOD Value of 2559 (2660) in CCR0 to generate
 *   a 50Hz / 20ms period.
 *
 *  Servo Angle   |     Pulse     |  Duty Cycle Value
 *  ---------------------------------------------------
 *  0 degrees     =     0.5ms     =   PWM_PERIOD - 64
 *  45 degrees    =     1.0ms     =   PWM_PERIOD - 128
 *  90 degrees    =     1.5ms     =   PWM_PERIOD - 192
 *  135 degrees   =     2.0ms     =   PWM_PERIOD - 256
 *  180 degrees   =     2.5ms     =   PWM_PERIOD - 320
 *
 *
 *  Formula to translate 0 to X degree to Software PWM value
 *  ((256 / (X degree range of servo)) * degree to turn to) + CCR_OFFSET
 *
 *  E.g. 0 to 180 degrees to Software PWM value
 *  ((256 / (180)) * degree to turn to) + 64
 *
 */

void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer
    SPWM_init();


    // Set up interrupt on P1.1 and P1.4 for testing purposes
    P1DIR &= ~BIT1;                              // Set Px.y direction as input
    P1SEL0 &= ~BIT1;                             // Set Px.y to 0 to use as GPIO Simple I/O
    P1SEL1 &= ~BIT1;                             // Set Px.y to 0 to as GPIO Simple I/O
    P1REN |= BIT1;                               // Enable internal resistor
    P1OUT |= BIT1;                               // Enable Px.y PULL-UP resistor (0 = pull down, 1 = pull up)
    P1IES |= BIT1;                               // Set interrupt edge (0 = high-low/falling edge transition, 1 = low-high/rising edge transition)
    P1IE |= BIT1;                                // Enable interrupt for Px.y
    P1IFG &= ~BIT1;                              // Clear interrupt flag for Px.y

    P1DIR &= ~BIT4;                              // Set Px.y direction as input
    P1SEL0 &= ~BIT4;                             // Set Px.y to 0 to use as GPIO Simple I/O
    P1SEL1 &= ~BIT4;                             // Set Px.y to 0 to as GPIO Simple I/O
    P1REN |= BIT4;                               // Enable internal resistor
    P1OUT |= BIT4;                               // Enable Px.y PULL-UP resistor (0 = pull down, 1 = pull up)
    P1IES |= BIT4;                               // Set interrupt edge (0 = high-low/falling edge transition, 1 = low-high/rising edge transition)
    P1IE |= BIT4;                                // Enable interrupt for Px.y
    P1IFG &= ~BIT4;                              // Clear interrupt flag for Px.y

    // Enable Port 1 interrupt in NVIC
    NVIC->ISER[1] = 1 << ((PORT1_IRQn) & 31);    // Enable Port x interrupt on the NVIC


}

void PORT1_IRQHandler(void)                     // Interrupt request handler for port x
{
    uint16_t s1Angle = getServoDegree(1);
    if((P1->IFG & BIT1) == BIT1)                          // If Px.y interrupt flag is set (there is a pending interrupt)
    {
        if (s1Angle < 180){
            s1Angle += 10;
        }
        P1IFG &= ~BIT1;                         // Clear the interrupt flag for Px.y at the end of the IRQ
    }
    else if((P1->IFG & BIT4) == BIT4)                          // If Px.y interrupt flag is set (there is a pending interrupt)
    {
        if (s1Angle > 0){
            s1Angle -= 10;
        }
        P1IFG &= ~BIT4;                         // Clear the interrupt flag for Px.y at the end of the IRQ
    }
    setServoDegree(1, s1Angle);
}
