#ifndef __HW_DEFINITION_H__
#define __HW_DEFINITION_H__

#include "Compiler.h"
#define DELAY_TIME_COUNT 1000

#define G_SENSOR_CE_INIT TRISBbits.TRISB3 = 0;
#define G_SENSOR_CE_ON LATBbits.LATB3 = 1;
#define G_SENSOR_CE_OFF LATBbits.LATB3 = 0;

//OTP enable is connected to RA4
#define G_SENSOR_OTP_ENABLE_INIT TRISBbits.TRISB5 = 0;
#define G_SENSOR_OTP_ENABLE_ON LATBbits.LATB5 = 1;
#define G_SENSOR_OTP_ENABLE_OFF LATBbits.LATB5 = 0;

//AZ pin is connected to RC5
#define G_SENSOR_AZ_PIN_INIT TRISBbits.TRISB4 = 0;
#define G_SENSOR_AZ_PIN_ON LATBbits.LATB4 = 1;
#define G_SENSOR_AZ_PIN_OFF LATBbits.LATB4 = 0;
#define AZ_SCALE_FACTOR 11 

//Timer
#define INIT_TIMER0_TICK_1US() TMR0H:TMR0L=0; T0CON = 0b11000101;
//                                               ||||||||
//                                               |||||---- T0PS2:0 = 001, 1:4 prescale value
//                                               ||||----- PSA = 0, Timer 0 prescaler is assigned.
//                                               |||------ T0SE = 0, Timer0 source edge select increment on low to high transition on T0CKI pin
//                                               ||------- T0CS = 0, internal instruction cycle clock
//                                               |-------- T08BIT = 1, Timer0 is configured as an 8-bit Timer0
//                                               --------- TMR0ON = 1, Timer0 is enabled

/*
*	TMR0IF = 0; => clear the timer0 interrupt flag if any
*	TMR0IE = 1; => set the timer0 interrupt enable
*	rememer to turn on the global interrupt enable if any by GIE = 1;
*/
#define TURN_ON_TIMER0_INT() \
	INTCONbits.TMR0IF = 0; \
	INTCONbits.TMR0IE = 1;

#define mFOSC_MHZ 48
#define mDelay_10us(val) Delay10TCYx(val*(mFOSC_MHZ/4))  	//10*?*4/mFOSC_MHZ MHz = val*10, val range 1~256/12
#define mDelay_250us(val) Delay1KTCYx(val*(mFOSC_MHZ/16)) // 1000*?*4/mFOSC_MHZ = val*250, val range 1~256/3
#define mDelay_1ms(val) Delay1KTCYx(val*(mFOSC_MHZ/4))   	// 1000*?*4/mFOSC_MHZ MHz/1000 = val, val range 1~256/12
#define mDelay_2P5ms(val) Delay10KTCYx(val*(mFOSC_MHZ/16)) 	// 10000*?*4/mFOSC_MHZ MHz/1000 = val*10, val range 1~256/3
#define mDelay_10ms(val) Delay10KTCYx(val*(mFOSC_MHZ/4)) 	// 10000*?*4/mFOSC_MHZ MHz/1000 = val*10, val range 1~256/12
#define mDelay_500ms mDelay_10ms(20);mDelay_10ms(20);mDelay_10ms(10);
#define mDelay_1s mDelay_500ms;mDelay_500ms;

#define DEFAULT_I2C_SLAVE_ADDR 				0x30
#define DEFAULT_I2C_BAUD_RATE_100KHZ	I2C_HIGH_SPEED_4_100KHZ
#define DEFAULT_TXYZ_DATA_BYTE_LEN 		2
#define DEFAULT_XYZ_DATA_START_ADDR 	0x12
#define DEFAULT_TXYZ_DATA_START_ADDR 	0x12
#define DEFAULT_DATA_ENDIANNESS				LITTLE_ENDIAN   			 		//least significant ... most significant
#define DEFAULT_XYZ_EXTRA_BYTE				2													//STADR and TILT
#define DEFAULT_TXYZ_EXTRA_BYTE				2													//STADR and TILT
#define DEFAULT_DATA_ALIGNMENT				2													//offset by 2
#define DEFAULT_T_FIRST								0													//XYZT
#define DEFAULT_SSPM									1
#define DEFAULT_PR2										1
#define DEFAULT_IFS										INTERFACE_I2C
#define DEFAULT_CMD										CDC_COM_PORT
#define DEFAULT_MON										UR_COM_PORT

#define I2C_HIGH_SPEED_4_100KHZ		 		4
#define I2C_STANDARD_SPEED_1_100KHZ		1
#define BIG_ENDIAN 										0						//MSByte first at Low address byte
#define LITTLE_ENDIAN 								1						//LSByte first at Low address byte
#define NULL_COM_PORT									0
#define UR_COM_PORT										1
#define CDC_COM_PORT									2
#define INTERFACE_SPI									0
#define INTERFACE_I2C									1

extern void MCU_init(void);
