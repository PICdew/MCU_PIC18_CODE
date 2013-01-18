/**
* GSA0 FW configuration
* @author Joseph FC Tseng@domintech.com.tw
* @version 2012.04.17
*/

#ifndef __FW_CONFIG_GSA0_H__
#define __FW_CONFIG_GSA0_H__

#
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


#endif //__FW_CONFIG_GSA0_H__
