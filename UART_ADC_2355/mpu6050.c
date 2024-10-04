#include <mpu6050.h>
#include <msp430.h> 
#include<stdint.h>

char flag=1;
void readByte(char reg, char len){
   if(flag|exept){
    flag=0;
    t=0;r=0;
    UCB0CTLW0|=UCSWRST;
    UCB0TBCNT= 1;
    UCB0CTLW0&=~UCSWRST;
    UCB0IE|=UCTXIE0;
    UCB0IE|=UCRXIE0;
    UCB0IE|=UCSTPIFG;
   UCB0CTLW0|=UCTR;
   Data_out[0]=reg;
   UCB0CTLW0|=UCTXSTT;
   while(flag==0);
   flag=0;

   UCB0CTLW0|=UCSWRST;
   UCB0TBCNT= len;
   UCB0CTLW0&=~UCSWRST;
   UCB0IE|=UCTXIE0;
   UCB0IE|=UCRXIE0;
   UCB0IE|=UCSTPIFG;
   UCB0CTLW0&=~UCTR;
   UCB0CTLW0|=UCTXSTT;

    }
}
void writeByte(char reg,char data){
    t=0;r=0,flag=0;
    UCB0CTLW0|=UCSWRST;
    UCB0TBCNT= 2;
    UCB0CTLW0&=~UCSWRST;
    UCB0IE|=UCTXIE0;
    UCB0IE|=UCRXIE0;
    UCB0IE|=UCSTPIFG;
   UCB0CTLW0|=UCTR;
   Data_out[0]=reg;
   Data_out[1]=data;
   UCB0CTLW0|=UCTXSTT;
   while(flag==0);

}



void initialize_I2C(char byte_count){
    UCB0CTLW0|=UCSWRST;
    UCB0CTLW0|=UCSSEL_3;
    UCB0BRW=60;

    UCB0CTLW0|=UCMODE_3|UCSYNC;
    UCB0CTLW0|=UCMST;
    UCB0I2CSA=0x2A;
    UCB0CTLW1|=UCASTP_2;
    UCB0TBCNT= byte_count;

    P1SEL1&=~BIT3&~BIT2;
    P1SEL0|=BIT3|BIT2;

    PM5CTL0&=~LOCKLPM5;
    UCB0CTLW0&=~UCSWRST;
}


void initialize_adc(){
    exept=1;
    char x;
    readByte(0x00,1);
    while(flag==0);
    x=Data_in[0];
    x|=0b00000010;
    writeByte(0x00,x); //power up digital
    x|=0b00000100;
    writeByte(0x00,x);//power up analog

    readByte(0x01,1);
    while(flag==0);
    x=Data_in[0];
    x&=0b00100000;
    writeByte(0x01,x);//vldo 3.3v


    readByte(0x02,1);
    while(flag==0);
    x=Data_in[0];
    x|=0b01111000;
    writeByte(0x02,x);//320 sps
    exept=0;
}



#pragma vector= USCI_B0_VECTOR
__interrupt void MPU650(void){
switch(UCB0IV){
case 0x16:               //receive
    Data_in[r]= UCB0RXBUF;
    r++ ;
    break;
case 0x18:
    UCB0TXBUF=Data_out[t];
    t++;
    break;
case 0x08:
    flag=1;
    UCB0IFG&=~UCSTPIFG;
    break;
default:
    break;


    }

}
