/**
* Microchip APP001Rev.3 20080825 Timer control macro definition
* @author Joseph FC Tseng@domintech.com.tw
* @version 2009.12.08.00
*/

#ifndef __APP001_TIMER_H__
#define __APP001_TIMER_H__

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
	
#endif