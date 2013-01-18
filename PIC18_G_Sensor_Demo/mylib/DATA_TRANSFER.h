#ifndef __DATA_TRANSFER_H__
#define __DATA_TRANSFER_H__

//I2C
#define SDA LATCbits.LATC4
#define SCL LATCbits.LATC3
//SPI
#define CS LATAbits.LATA5
#define CE LATBbits.LATB3
#define SDO LATCbits.LATC5
#define CLK LATCbits.LATC3
#define SCKH LATCbits.LATC3 = 1;
#define SCKL LATCbits.LATC3 = 0;
#define SPIE SSP1CON1bits.SSPEN
#define MWAIT _asm NOP _endasm

extern void I2C_Init(void);
extern void I2C_Read(unsigned char, unsigned char, char*, unsigned char);
extern void I2C_Write(unsigned char, unsigned char, char*, unsigned char);

extern void SPI_Init(void);
extern void SPI_Read(unsigned short, char*, unsigned char);
extern void SPI_Write(unsigned short, char*, unsigned char);

