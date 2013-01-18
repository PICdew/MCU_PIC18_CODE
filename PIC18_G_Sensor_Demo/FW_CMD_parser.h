/**
* FW command parser header file
* @author FC Tseng (JoseT@domintech.com.tw)
* @version 2009.01.04.00: Digital_SPI_R1.3
* @version 2009.01.05.10: Digital_SPI_R1.3 release
* @version 2010.01.25.00: R1.4 kick off
* @version 2010.03.26.00: R1.41
* @version 2010.05.07.00: add parallel output function
* @version 2010.06.30: FW2.0, based on FW1.41, add "por", power on reset
* @version 2010.11.16: CS803 add az,n for auto zero control
*/

#ifndef __FW_CMD_PARSER_H__
#define __FW_CMD_PARSER_H__

extern unsigned char parseCommand(char* cmdStr, unsigned char* param);
enum {
	CMD_UNKNOWN,
	CMD_QUERY,
	CMD_STANDBY,
	CMD_NORMAL,
	CMD_SINGLE_R,
	CMD_MULTI_R,
	CMD_CONT_XYZ,
	CMD_SINGLE_W,
	CMD_SINGLE_OR,
	CMD_MULTI_OR,
	CMD_INTERRUPT,
	CMD_SINGLE_OW,
	CMD_CONT_XYZT,
	CMD_AUTO_NIL,
	CMD_CONFIGURE,
	CMD_MULTI_W
	};
	
extern const rom unsigned char configure_num;
extern const rom far char* ROM_STR_CONFIGURE[];
extern unsigned char parseCommand_SPI_R1_3(char cmdStr[], unsigned char* paramOne, unsigned char* paramTwo, unsigned char* paramThree);


#endif