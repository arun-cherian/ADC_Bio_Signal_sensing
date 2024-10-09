#include <msp430fr2355.h>
#include <my.h>
#include <stdio.h>
#include <stdint.h>
#include <mpu6050.h>

// Global variable
unsigned int result=0;
uint8_t bit;
int currentState = 3;  // Used to track the current step in the sequence

// Function Prototypes
void configureClocks(void);
void initializeUART(void);
void set(void);

void configureClocks(void) {
    // Stop the watchdog timer
    WDTCTL = WDTPW | WDTHOLD;
    __bis_SR_register(SCG0 | SCG1);  // Set SCG0 and SCG1 bits in the Status Register

    // Set DCO to 24 MHz
    CSCTL1 |= DCORSEL_7; // Set DCO frequency to 24 MHz
    CSCTL2 &= ~FLLD_7;  // Clear FLL divider
    CSCTL2 |= 731; // Based on refclock 32 kHz
    CSCTL3 &= ~SELREF_3; // Select reference clock as 32 kHz
    CSCTL3 &= ~FLLREFDIV_7; // Set reference divider as 1
    __bic_SR_register(SCG0 | SCG1);  // Clear SCG0 and SCG1 bits in the Status Register

    // Optional: Wait for the clock to stabilize
    while (CSCTL7 & (FLLUNLOCK0 | FLLUNLOCK1)); // Wait until both FLLUNLOCK0 and FLLUNLOCK1 are 0 (FLL locked)
}

int main(void) {
    WDTCTL = WDTPW | WDTHOLD;  // Stop watchdog timer
    initializeUART();
    configureClocks();

    __enable_interrupt(); // Enable global interrupts*/
    // Configure ADC registers
    ADCCTL0 &= ~ADCSHT;   // Clear sample-and-hold bits
    ADCCTL0 |= ADCSHT_2;  // Set sample-and-hold time to 16 ADC clock cycles
    ADCCTL0 |= ADCON;     // Turn on ADC
    ADCCTL0 |= ADCMSC;
    ADCCTL1 |= ADCCONSEQ_2;     // Set ADC to repeat single channel mode

    ADCCTL1 |= ADCSHP;    // Use sampling timer
    ADCCTL1 |= ADCSSEL_2; // Select SMCLK as clock source
    ADCCTL1 |= ADCDIV_7;

    ADCCTL2 &= ~ADCRES;   // Clear resolution bits
    ADCCTL2 |= ADCRES_2;  // Set ADC resolution to 10-bit
    ADCCTL2 |= ADCPDIV_1; //DIVIDE BY 4

    ADCMCTL0 &= ~ADCINCH;        // Clear channel selection bits
    ADCMCTL0 |= ADCINCH_1;       // Select input channel A2
    // Configure the pin for analog input (P5.2 -> A12)
    P1SEL1 |= BIT1;
    P1SEL0 |= BIT1;
    P1DIR&=~BIT2;
    P1SEL1&=~BIT2;
    P1SEL0&=~BIT2;
    //P1IE|=BIT2;


    ADCCTL0 |= ADCENC;    // Enable ADC conversion
    ADCCTL0 |= ADCSC;     // Start ADC conversion
    ADCIE|= ADCIE0;

    initialize_I2C(1);
    initialize_adc();
    UCA0IE &= ~UCTXCPTIE;


    while (1) {
        //writeByte(0x00,2);
        switch (currentState) {
            case 0:
                exept=1;
                r = 0;
                readByte(0x11, 1); // Read the first byte
                currentState = 1;  // Move to the next state
                break;

            case 1:
                r = 1;
                readByte(0x12, 1); // Read the second byte
                currentState = 2;  // Move to the next state
                break;

            case 2:
                r = 2;
                readByte(0x13, 1); // Read the third byte
                currentState = 3;  // Reset to initial state
                exept=0;
                // You can add further actions here, like processing the data or sending it
                break;
            default: break;
        }
    }


    return 0;
}


#pragma vector =ADC_VECTOR
__interrupt void ADC_ISR(void){
    if(UCA0IE & UCTXCPTIE);
        UCA0TXBUF = ADCMEM0_L;
        while(!(UCA0IFG & UCTXCPTIFG));
        UCA0TXBUF = ADCMEM0_H;
        UCA0IFG &= ~UCTXCPTIFG;



}
#pragma vector=PORT1_VECTOR
__interrupt void PORT1_ISR(void){
    if(P1IFG & BIT2){
        currentState=0;
        result=Data_in[0];
        result=(result<<8)|Data_in[1];
        result=(result<<8)|Data_in[2];
        P1IFG&=~BIT2;
        send(result);
    }
}
