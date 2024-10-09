#include <mpu6050.h>
#include <msp430.h> 
#include<stdint.h>

char flag=1;
void readByte(char reg, char len){//only works for 1 byte
   if(flag|exept){
    flag=0;
    t=0;
    UCB1CTLW0|=UCSWRST;
    UCB1TBCNT= 1;
    UCB1CTLW0&=~UCSWRST;
    UCB1IE|=UCTXIE0;
    UCB1IE|=UCRXIE0;
    UCB1IE|=UCSTPIE;
    UCB1CTLW0|=UCTR;
    Data_out[0]=reg;
    UCB1CTLW0|=UCTXSTT;
    while(flag==0);
    flag=0;
    UCB1CTLW0|=UCSWRST;
    UCB1TBCNT= 1;
    UCB1CTLW0&=~UCSWRST;
    UCB1IE|=UCTXIE0;
    UCB1IE|=UCRXIE0;
    UCB1IE|=UCSTPIE;
    UCB1CTLW0&=~UCTR;
    UCB1CTLW0|=UCTXSTT;
    while(flag==0);
    flag=0;
    }
}
void writeByte(char reg,char data){
    t=0;r=0,flag=0;
    UCB1CTLW0|=UCSWRST;
    UCB1TBCNT= 2;
    UCB1CTLW0&=~UCSWRST;
    UCB1IE|=UCTXIE0;
    UCB1IE|=UCRXIE0;
    UCB1IE|=UCSTPIE;
    UCB1CTLW0|=UCTR;
    Data_out[0]=reg;
    Data_out[1]=data;
    UCB1CTLW0|=UCTXSTT;
    while(flag==0);
    flag=0;
}

void initialize_I2C(char byte_count){
    UCB1CTLW0|=UCSWRST;
    UCB1CTLW0|=UCSSEL_3;
    UCB1BRW=60;

    UCB1CTLW0|=UCMODE_3|UCSYNC|UCCLTO_3;
    UCB1CTLW0|=UCMST;
    UCB1I2CSA=0x2A;
    UCB1CTLW1|=UCASTP_2;
    UCB1TBCNT= byte_count;
    UCB1IE|=UCTXIE0;
    UCB1IE|=UCRXIE0;
    UCB1IE|=UCSTPIE;

    P4SEL1&=~BIT6&~BIT7;
    P4SEL0|=BIT6|BIT7;

    PM5CTL0&=~LOCKLPM5;
    UCB1CTLW0&=~UCSWRST;
}

void initialize_adc(){
    exept=1;
    char x;
    writeByte(0x00,1);
    writeByte(0x00,0);
    readByte(0x00,1);
    x=Data_in[0];
    x|=0b10000110;
    writeByte(0x00,x); //power up digital


    readByte(0x01,1);
    x=Data_in[0];
    x&=0b00100000;
    writeByte(0x01,x);//vldo 3.3v

    readByte(0x02,1);
    x=Data_in[0];
    x|=0b01111000;
    writeByte(0x02,x);//320 sps
    exept=0;
}

#pragma vector= USCI_B1_VECTOR
__interrupt void MPU650(void){
    switch(UCB1IV){
    case 0x16:               //receive
        Data_in[r]= UCB1RXBUF;
        r++ ;
        UCB1IFG&=~UCRXIFG0;
        break;
    case 0x18:
        UCB1TXBUF=Data_out[t];
        t++;
        UCB1IFG&=~UCTXIFG0;
        break;
    case 0x08:
        flag=1;
        UCB1IFG&=~UCSTPIFG;
        break;
    default:
        break;
    }
}

