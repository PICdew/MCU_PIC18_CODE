#include "./HW_DEFINITION.h"

void MCU_init()
{
	//LED PORT SETTING
	TRISD = 0x00;
	LATD = 0x00;
// 2013_0110 added for push button by York
	TRISGbits.TRISG0=0;
	LATGbits.LATG0=0;
// 2013_0110 added end.
	//SPI MODE apply for all series
	G_SENSOR_CE_INIT;
	G_SENSOR_CE_ON;

	//I2C MODE for 05 to 07
	G_SENSOR_OTP_ENABLE_INIT;
	G_SENSOR_OTP_ENABLE_OFF;

	G_SENSOR_AZ_PIN_INIT;
	G_SENSOR_AZ_PIN_OFF;

	//Interrupts
	//RB1/INT1
	TRISBbits.TRISB1 = 1;	//RB1 as input
	INTCON2bits.INTEDG1 = 1;	//external interrupt 1 edge select to interrupt on rising edge
	//RB2/INT2 input
	TRISBbits.TRISB2 = 1;	//RB2 as input
	INTCON2bits.INTEDG2 = 1;	//external interrupt 2 edge select to interrupt on rising edge
	//INTCON2bits.INTEDG2 = 0;	//external interrupt 2 edge select to interrupt on falling edge
	INTCON2bits.RBPU = 1;    //RB input has internal weak pullup dis-enabled.
	//INTCON2bits.RBPU = 0;    //RB input has internal weak pullup enabled.

	INTCONbits.GIEH = 1;								//turn on the global interrupt enable
	INTCONbits.GIEL = 1;
}