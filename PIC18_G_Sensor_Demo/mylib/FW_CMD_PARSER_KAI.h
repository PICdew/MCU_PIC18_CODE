#ifndef __FW_CMD_PARSER_KAI_H__
#define __FW_CMD_PARSER_KAI_H__

extern unsigned char parseCommand(char* cmdStr, unsigned char* param);
enum {
	CMD_UNKNOWN, 		// return 0
	CMD_QUERY,	  		// return 1
	CMD_STANDBY,  		// return 2
	CMD_NORMAL,   		// return 3
	CMD_SINGLE_R, 		// return 4
	CMD_MULTI_R, 		// return 5
	CMD_CONT_XYZ, 		// return 6
	CMD_SINGLE_W, 		// return 7
	CMD_SINGLE_OR,		// return 8
	CMD_MULTI_OR, 		// return 9
	CMD_INTERRUPT,		// return 10
	CMD_SINGLE_OW,		// return 11
	CMD_CONT_XYZT,		// return 12
	CMD_AUTO_NIL, 		// return 13
	CMD_CONFIGURE,		// return 14
	CMD_MULTI_W,   		// return 15
	CMD_CHANGE_DEVICE 	// return 16
	};
	
extern const rom unsigned char configure_num;
extern const rom far char* ROM_STR_CONFIGURE[];

#endif