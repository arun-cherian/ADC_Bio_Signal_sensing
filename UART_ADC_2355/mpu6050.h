/*
 * mpu6050.h
 *
 *  Created on: 24-Oct-2023
 *      Author: Arun
 */
#ifndef ADC_24_H_
#define ADC_24_H_
void readByte(char,char);
void writeByte(char,char);
void initialize_I2C(char);
void initialize_adc();

int Data_out[3];
unsigned int data;
char t,r;
char exept;




#endif /* ADC_24_H_ */
