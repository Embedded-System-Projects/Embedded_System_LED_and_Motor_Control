#include <xc.h>
#include "motor_control.h"

#define _XTAL_FREQ 20000000  // Define the crystal frequency as 20 MHz

// Define motor control pins
#define MOTOR_PIN1 PORTBbits.RB0
#define MOTOR_PIN2 PORTBbits.RB1

// Define push-button pins
#define S1 PORTBbits.RB3
#define S2 PORTBbits.RB4
#define S3 PORTBbits.RB5

void initPort() {
    TRISBbits.TRISB3 = 1; // Button 1 input
    TRISBbits.TRISB4 = 1; // Button 2 input
    TRISBbits.TRISB5 = 1; // Button 3 input

    TRISBbits.TRISB0 = 0; // Motor direction pin 1
    TRISBbits.TRISB1 = 0; // Motor direction pin 2
    TRISCbits.TRISC2 = 0; // PWM output pin
}

void initPwm(uint32_t freq) {
    uint16_t PR2_Value = (_XTAL_FREQ / (freq * 4)) - 1;
    if (PR2_Value > 255) {
        PR2_Value = 255; // Ensure PR2 fits in 8-bit register
    }
    PR2 = PR2_Value;      // Set PR2 for desired frequency
    T2CON = 0x04;         // Timer2 ON, prescaler = 1
    CCP1CON = 0x0C;       // CCP1 in PWM mode
}

void startPwm() {
    TMR2 = 0;             // Clear Timer2
    T2CONbits.TMR2ON = 1; // Enable Timer2
}

void setPWMDutyCycle(uint16_t dutyCycle) {
    if (dutyCycle > 1023) {
        dutyCycle = 1023;  // Ensure duty cycle is within 10-bit range
    }
    CCPR1L = dutyCycle >> 2;              // Set upper 8 bits
    CCP1CON = (CCP1CON & 0xCF) | ((dutyCycle & 0x03) << 4); // Set LSBs
}

void controlMotor() {
    if (S1 == 1) {  // Button 1 pressed
        setPWMDutyCycle(256);  // 25% duty cycle
        MOTOR_PIN1 = 1;
        MOTOR_PIN2 = 0;        // Motor forward
    } else if (S2 == 1) {  // Button 2 pressed
        setPWMDutyCycle(1023);  // 100% duty cycle
        MOTOR_PIN1 = 1;
        MOTOR_PIN2 = 0;        // Motor forward faster
    } else if (S3 == 1) {  // Button 3 pressed
        setPWMDutyCycle(1023);  // 100% duty cycle
        MOTOR_PIN1 = 0;
        MOTOR_PIN2 = 1;        // Motor backward faster
    } else {
        setPWMDutyCycle(0);    // Stop motor
        MOTOR_PIN1 = 0;
        MOTOR_PIN2 = 0;
    }
}
