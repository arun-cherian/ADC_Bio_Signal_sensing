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


char Data_in[10],Data_out[10];
char t,r;
char exept;




#endif /* ADC_24_H_ */
