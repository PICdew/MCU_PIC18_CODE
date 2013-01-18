/**
* Microchip APP001Rev.3 20080825 Hardware interface implementation
* @author Joseph FC Tseng@domintech.com.tw
* @version 2009.11.23.00
*/
#include "app001_HW_definition.h"
#include "app001_PWM.h"

void init_HW(void){

	// Setup LED
	TRISD = 0; 			//digital output for LED control
	//initilize RC2/P1A PWM
	INIT_PWM1(0x0F);
	SET_PWM1_PERIOD(0xFF, 3);
	SET_PWM1_DUTY_CYCLE(0x3FF/2);
	//pushbutton & SPI/I2C interface
	TRISB0 = 1;  //digital input from SW2
	TRISA4 = 1;  //digital input from SW3
	ADCON1 = 0b00011101;
	//         ||||||||
	//         ||||------- PCFG3:0=1101, AN0:1=analog, all others digital, including RB0/AN12, and RA5/AN4
	//         |||-------- VCFG0 = 1, voltage reference configuration bit, => VREF+ (RA3/AN3/VREF+)
	//         ||--------- VCFG1 = 0, voltage reference configuration bit, => VSS
	//         xx

	//initialize hint
	for(int i = 0; i < 2; i++){
		LED_BYTE = 0xFF;

		#ifndef BUZZER_MUTE_ON
		TURN_ON_PWM1;
		#endif
		
		for(int j = 0; j<15;j++) __delay_ms(40);
		LED_BYTE = 0;
		
		#ifndef BUZZER_MUTE_ON
		TURN_OFF_PWM1;
		#endif
		
		for(int j = 0; j<10;j++) __delay_ms(40);
	}
}