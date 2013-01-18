#include "i2c.h"
#include "./DATA_TRANSFER.h"


unsigned char getsI2C_KAI( unsigned char *, unsigned char );
void I2C_Init(){
	//PORT STATE
	TRISCbits.TRISC4 = 1; //I
	TRISCbits.TRISC3 = 1; //I
	TRISCbits.TRISC5 = 0;
	LATCbits.LATC5 = 0;
	//I2C SETTING
	SSP1STATbits.SMP = 0;
	SSP1CON1 = 0x28;
	SSP1ADD = 0x18;
	SSP1CON2 = 0x00;
	PIR1bits.SSP1IF = 0x00;
}
void I2C_Read(unsigned char deviceAddr, unsigned char regAddr, char* RX_buffer, unsigned char Num_Bytes){
	 IdleI2C();
	 StartI2C();
	 WriteI2C(deviceAddr);
	 IdleI2C();
	 WriteI2C(regAddr);
	 IdleI2C();
	 RestartI2C();
	 WriteI2C(deviceAddr | 0x01);
	 IdleI2C();
	 getsI2C((unsigned char*)RX_buffer, Num_Bytes);
	 NotAckI2C();
	 StopI2C();
}
void I2C_Write(unsigned char deviceAddr, unsigned char regAddr, char* TX_buffer, unsigned char Num_Bytes){
	IdleI2C();
	StartI2C();
	WriteI2C(deviceAddr);
	IdleI2C();
	WriteI2C(regAddr);
	IdleI2C();
	while(Num_Bytes--)
	{
		WriteI2C(*TX_buffer);
		IdleI2C();
		TX_buffer++;
	}
	StopI2C();
}

void SPI_Init(){

	//PORT STATE
   	TRISCbits.TRISC5 = 0; //O
   	TRISCbits.TRISC4 = 1; //I
  	TRISCbits.TRISC3 = 0; //O
   	TRISBbits.TRISB3 = 0; //O
  	TRISAbits.TRISA5 = 0; //O
	TRISFbits.TRISF7 = 1; //I
	//PORT INIT
	LATAbits.LATA5 = 0; //CS
  	LATBbits.LATB3 = 1; //CE
   	LATCbits.LATC5 = 1; //SDO
   	LATCbits.LATC3 = 1; //CLK
	//SPI SETTING
	SSP1STAT = 0b10000000;
	SSP1CON1 = 0b00010000;
    SSP1CON1bits.WCOL = 0;
    PIR1bits.SSPIF = 0;
}
void SPI_Read(unsigned short regAddr, char* RX_buffer, unsigned char Num_Bytes){
	char i;
	regAddr |= 0x2000;
 	SCKH;
 	MWAIT;
	CS = 1;
	MWAIT;
	MWAIT;
  	for (i = 0; i < 14; i ++){
      if (regAddr & 0x2000){
        SDO = 1;
      } else {
        SDO = 0;
      }
      regAddr <<= 1;
      SCKL;
      MWAIT;
      MWAIT;
      SCKH;
      MWAIT;
      MWAIT;
  	}
	SPIE = 1;
    SSP1CON1bits.WCOL = 0;
    PIR1bits.SSPIF = 0;
	while (Num_Bytes--)
	{
      SSP1BUF = 0xFF;
	  while(SSP1STATbits.BF == 0);
	  *RX_buffer = SSP1BUF;
	  RX_buffer++;
	}
	SPIE = 0;
    SCKH;
 	MWAIT;
 	MWAIT;
 	CS = 0; 
 	SDO = 1;
}
void SPI_Write(unsigned short regAddr, char* RX_buffer, unsigned char Num_Bytes){
	char data;
	char i;
	SCKH;
 	MWAIT;
 	CS = 1;
 	MWAIT;
  	MWAIT;
 	for (i = 0; i < 14; i ++){
    	if (regAddr & 0x2000){
     	  SDO = 1;
    	} else {
      	  SDO = 0;
   	 	}
   	 	regAddr <<= 1;
  		SCKL;
   		MWAIT;
  	 	MWAIT;
  	 	SCKH;
   		MWAIT;
   	 	MWAIT;
  	}
	SCKL;
	while(Num_Bytes--){
	data = *RX_buffer;
	RX_buffer++;
	for (i = 0; i < 8; i++){
	     if (data & 0x80){
		   SDO = 1;
		 } else {
		   SDO = 0;
		 }
		 data <<= 1;
		 SCKH;
         MWAIT;
         MWAIT;
		 MWAIT;
		 SCKL;
		}
	}
    SCKH;
    MWAIT;
    MWAIT;
    CS = 0; 
    SDO = 1;
}

