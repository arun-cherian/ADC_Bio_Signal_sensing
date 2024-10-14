#include <msp430fr2355.h>
#include <my.h>
#include <stdio.h>
#include <stdint.h>
#include <mpu6050.h>

// Global variable
int count_bytes=0;
unsigned int result=0;
int currentState = 0;  // Used to track the current step in the sequence
int count=0;
unsigned int bit_24=0;
uint8_t data_main[5];

uint8_t bit=1;
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
    initialize_I2C(1);
    initialize_adc();

    ADCCTL0 |= ADCENC;    // Enable ADC conversion
    ADCCTL0 |= ADCSC;     // Start ADC conversion
    ADCIE|= ADCIE0;


    UCA0IE &= ~UCTXCPTIE;


    while (1) {
        //writeByte(0x00,2);
        //while(bit==0);
        if(P1IN&BIT2){
        exept=1;
        bit_24=0;


        readByte(0x12, 1); // Read the second byte
        bit_24 |= data;     // Combine the second byte
        //data_main[1]=data;
        bit_24 <<= 8;       // Shift left by 8 bits to make room for the next byte

        readByte(0x13, 1); // Read the third byte
        bit_24 |= data;     // Combine the third byte
        bit_24 <<= 8;
        //data_main[2]=data;
        //readByte(0x01,1);
        bit_24 |= count;
        send2(bit_24);

        exept=0;
            }
        }


    return 0;
}


#pragma vector =ADC_VECTOR
__interrupt void ADC_ISR(void){
        //if(UCA0IE & UCTXCPTIE);
        //UCA0IFG &= ~UCTXCPTIFG;
        count_bytes=2;
        send(ADCMEM0);

}
#pragma vector=PORT1_VECTOR
__interrupt void PORT1_ISR(void){
    if(P1IFG & BIT2){
        P1IFG&=~BIT2;
    }
}
