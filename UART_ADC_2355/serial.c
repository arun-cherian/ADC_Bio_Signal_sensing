/*
 * serial.c
 *
 *  Created on: 20-Oct-2023
 *      Author: Arun
 */
#define UCBRS_0x49 0x49

#include <msp430.h>
#include <stdio.h>
#include <stdint.h>
#include <my.h>

// Function Prototypes
void send2(unsigned int);
void send(unsigned int);
void convert(void);
void initializeUART(void);
void configureClocks(void);

// Variables
//uint16_t count_bytes;
int s =0;
int kindi = 0;
uint8_t A[5];


#include <msp430.h>


void initializeUART() {
    UCA0CTLW0 |= UCSWRST;             // Put eUSCI in reset
    UCA0CTLW0 |= UCSSEL__SMCLK;       // Select SMCLK (24 MHz)

    // Set UCBRx for 921600 baud
    UCA0BRW = 26;                     // Set UCBRx (integer part)
    UCA0MCTLW = 0;                    // No oversampling, no fractional part correction
    UCA0CTLW0 &= ~UCSWRST;            // Release from reset

    PM5CTL0 &= ~LOCKLPM5; // Disable the GPIO power-on default high-impedance mode

    // Configure P4.2 and P4.3 for UART TXD/RXD (UCA1)
    P4SEL0 |= BIT2 | BIT3;
    P4SEL1 &= ~(BIT2 | BIT3);

    // Configure P4.2 and P4.3 for UART TXD/RXD (UCA1)
    P1SEL0 |= BIT6 | BIT7;
    P1SEL1 &= ~(BIT6 | BIT7);
    UCA0IE |= UCTXCPTIE;
    UCA0IFG|=UCTXCPTIFG;
    __enable_interrupt();
}

void send(unsigned int f){
    //if(s==0){
    A[0]=f&0x3F;
    A[1]=(f&0xFC0)>>6;
    UCA0IE|= UCTXCPTIE;
    UCA0IFG|=UCTXCPTIFG;
    //}
}

void send2(unsigned int f){
    while(!(s==0));
    ADCIE&= ~ADCIE0;
    A[0]=f&0x3F;
    A[1]=(f&0xFC0)>>6;
    A[2]=(f&0x3F000)>>12;
    //A[3]=(f&0xFC0000)>>18;
    UCA0IE|= UCTXCPTIE;
    count_bytes=3;
    UCA0IFG|=UCTXCPTIFG;
}

/*void send(unsigned int f) {
    kindi = f;
    convert();
//    memcpy(B, A, 5 * sizeof(int));
    UCA0IE |= UCTXCPTIE;      // Enable transmit complete interrupt
   // UCA1IFG|=UCTXCPTIFG;

}

void convert() {
    // Clear the buffer A
    int v;
    for (v = 0; v < 5; v++) {
        A[v] = '\0';
    }

    // Handle zero case
    if (kindi == 0) {
        A[4] = '0';
    } else {
        int m = 0;
        if (kindi < 0) {
            A[4] = '-';
            kindi = -kindi;
            m = 1;
        }
        int n = m;

        // Convert number to character representation
        while (kindi > 0) {
            int digit = kindi % 10;
            A[n] = digit + '0';
            kindi /= 10;
            n++;
        }
    }
}
*/


#pragma vector=EUSCI_A0_VECTOR
__interrupt void UART(void) {

    if(s<=count_bytes-1){
        A[s]|=(s<<6);
        UCA0TXBUF = A[s]; // Send lower byte
        s++;}
        else{
        UCA0IE &= ~UCTXCPTIE;
        ADCIE|= ADCIE0;
        s=0;
        }
  UCA0IFG &= ~UCTXCPTIFG; // Clear transmit complete flag
}




