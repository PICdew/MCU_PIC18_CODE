/**
* FW command parser implementation
* @author Joseph FC Tseng (JoseT@domintech.com.tw)
* @version 2011.12.27
* @version 2012.01.05: add "q, config" to query all configurations
* 									 add configurations: "xyzextra", "txyzextra", "datalign", "tfirst"
* @version 2012.01.10: add "mw, start_addr, val_1, val_2, .....", multiwrite command
* @version 2012.02.10: add "q,osc" to query osc frequency 
* @version 2012.02.23: add configurations: "sspm", "pr2"
* @version 2012.03.01: add strlwr(s) before string processing in _hexstrtol(char* s)
* @version 2012.04.17: default_configure[] (error input default) is hard coded with constant to avoid depend on devices
* @version 2012.11.20: c,osc default 1; will return the osc configuration parameter for possible OSC scaling
*/
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "FW_CMD_PARSER_KAI.h"


long _hexstrtol(char* s);
unsigned char _q_parser(char* cmdStr, unsigned char* param);
unsigned char _standby_parser(char* cmdStr, unsigned char* param);
unsigned char _normal_parser(char* cmdStr, unsigned char* param);
unsigned char _r_parser(char* cmdStr, unsigned char* param);
unsigned char _w_parser(char* cmdStr, unsigned char* param);
unsigned char _o_parser(char* cmdStr, unsigned char* param);
unsigned char _ow_parser(char* cmdStr, unsigned char* param);
unsigned char _rt_parser(char* cmdStr, unsigned char* param);
unsigned char _az_parser(char* cmdStr, unsigned char* param);
unsigned char _i_parser(char* cmdStr, unsigned char* param);
unsigned char _c_parser(char* cmdStr, unsigned char* param);
unsigned char _mw_parser(char* cmdStr, unsigned char* param);
unsigned char _change_device(char* cmdStr, unsigned char* param);

const rom far char* ROM_STR_XYZ = (const rom far char*) "xyz";
const rom far char* ROM_STR_F = (const rom far char*) "f";


//FW command and parser definition
const rom unsigned char fw_cmd_num = 13;
const rom far char* ROM_STR_FW_CMD[] = {
	(const rom far char*) "q",
	(const rom far char*) "standby", 
	(const rom far char*) "normal", 
	(const rom far char*) "r",
	(const rom far char*) "w", 
	(const rom far char*) "o", 
	(const rom far char*) "ow", 
	(const rom far char*) "rt", 
	(const rom far char*) "az", 
	(const rom far char*) "i", 
	(const rom far char*) "c",
	(const rom far char*) "mw",
	(const rom far char*) "sw"
	};
unsigned char (*cmdParser[])(char* cmdStr, unsigned char* param) = {
	_q_parser,
	_standby_parser,
	_normal_parser,
	_r_parser,
	_w_parser,
	_o_parser,
	_ow_parser,
	_rt_parser,
	_az_parser,
	_i_parser,
	_c_parser,
	_mw_parser,
	_change_device
	};
const rom unsigned char configure_num = 18;
const rom far char* ROM_STR_CONFIGURE[] = {
	(const rom far char*) "fw",					//0, FW
	(const rom far char*) "osc",        //1, OSC freq
	(const rom far char*) "cfg",				//2, Configuration
	(const rom far char*) "sar",		 		//3, slave addr
	(const rom far char*) "ibr",		 		//4, I2C Baudrate
	(const rom far char*) "dby",				//5, data byte number
	(const rom far char*) "xar",		 		//6, XYZ data start addr
	(const rom far char*) "tar", 				//7, TXYZ data start addr
	(const rom far char*) "edn",				//8, endianness
	(const rom far char*) "xex",				//9, XYZ data extra byte number
	(const rom far char*) "tex",			 	//10,TXYZ data extra byte number
	(const rom far char*) "dal",				//11, data alignment
	(const rom far char*) "tft",				//12, temperature is first or last
	(const rom far char*) "sspm",				//13, SPI sspm
	(const rom far char*) "pr2",				//14, SPI PR2
	(const rom far char*) "ifs",				//15, interface select
	(const rom far char*) "cmd",				//16, command COM PORT
	(const rom far char*) "mon"					//17, monitor COM PORT
	};
const rom unsigned char default_configure[] = {
	0,		//0
	1,		//1
	0,		//2
	0x38,	//3
	4,		//4
	1,		//5
	0,		//6
	0,		//7
	0,		//8
	0,		//9
	0,		//10
	0,		//11
	0,		//12
	1,		//13
	1,		//14
	1,		//15
	2,		//16
	1			//17
};

const rom far char* sepTok = (const rom far char*) ",";
char* charPtr;
long num;
/**
* Parse the command string, and return the results
* @param unsigned char* cmdStr: command string to be parsed
* @param unsigned char* param[0]: parameters to be assigned and returned
* @return CMD_UNKNOWN: no match
* @return CMD_QUERY: "q", query
*							fw: param[0] = 0, fw query
*							osc: param[0] = 1, osc frequency
*							cfg: param[0] = 2, configuration parameter query
*							sar: param[0] = 3, slave address query
*							ibr: param[0] = 4, I2C baud rate speed, kHz
*							dby: param[0] = 5, TXYZ data byte number
*							xar: param[0] = 6, XYZ data start address
*							tar: param[0] = 7, TXYZ data start address
*							edn: param[0] = 8, endianness of the datas
*							xex: param[0] = 9, extra number of byte to read from XYZ data start address
*							tex: param[0] = 10, extra number of byte to read from TXYZ data start address
*							dal: param[0] = 11, TXYZ data alignment
*							tft: param[0] = 12, 1 for txyz, 0 for xyzt
*							sspm: param[0] = 13, SSP master clock mode
*							pr2: param[0] = 14, timer2 period register
*							ifs: param[0] = 15, interface selection
*							cmd: command port selection
*							mon: monitor port selection
* @return CMD_STANDBY: "standby"
* @return CMD_NORMAL: "normal"
* @return CMD_SINGLE_R: "r,dd", dd - address in hex (param[0])
* @return CMD_MULTI_R: "r,dd,n", dd - address in hex (param[0]), n - number of byte to read in decimal (param[1])
* @return CMD_CONT_XYZ: "r,xyz(,nnn)(,fc_h,fc_l", continuous reading, nnn, Hz in decimal, no nnn represent max freq = 1000 (param[0] (10Hz)= 100), 
*							with high pass filter cutoff freq fc_h (param[1]) in milli-Hz, 0 to turn off high pass filter(default)
*							with low pass filter cutoff freq fc_l (param[2]) in Hz, 0 to turn off low pass filter(default)
* @return CMD_SINGLE_W: "w,dd,bb(,mm) ", dd - address in hex (param[0]), bb - byte in hex to be written (param[1]), mm - mask byte in hex (param[2])
* @return CMD_SINGLE_OR: "o,ddd", ddd - addrss in hex (param[0] high byte, param[1] low byte)
* @return CMD_MULTI_OR: "o,ddd,n", ddd - address in hex (param[0] high byte, param[1] low byte), n - number of byte to read in decimal (param[2])
* @return CMD_INTERRUPT: "i", interrupt configuration
*							f,n,addr: data ready on INT(n) configuration; param[0] = n, n in decimal. 
* @return CMD_SINGLE_OW: "ow,ddd,bb", ddd - address in hex (param[0] high byte, param[1] low byte), bb - byte in hex to be written (param[2])
* @return CMD_CONT_XYZT: "rt,xyz(,nnn)(,fc)(,[hl] continuous reading txyz, nnn, Hz in decimal, no nnn represent max freq = 1000 (param[0] (10Hz)= 100),
*							with high pass filtering of cutoff freq fc (param[1])Hz, 0 for none filtering (default), param[2] ='h'(default) or 'l'
* @return CMD_AUTO_NIL: "az,n": autonil command, a auatonil pulse with n*250us width, default n = 20 (5ms pulse width)
* @return CMD_CONFIGURE: "c", configure
*							sladdr(,nnn): configure slave address, nnn in decimal, default 56 (param[1])
* 						i2cbdr(,n): configure I2C baud rate, n in decimal (100kHz), default DEFAULT_I2C_BAUD_RATE_100KHZ (param[1] = n)
* 						databyte(,n): TXYZ data byte number, n in decimal, default DEFAULT_TXYZ_DATA_BYTE_LEN (param[1] = n)
* 						xyzaddr(,nnn): XYZ data start address, nnn in decimal, default DEFAULT_XYZ_DATA_START_ADDR (param[1] = nnn)
* 						txyzaddr(,nnn): TXYZ data start address, nnn in decimal, default DEFAULT_TXYZ_DATA_START_ADDR (param[1] = nnn)
* 						endian(,n): endianness of the datas, n in decimal, default BIG_ENDIAN (param[1] = n)
*							xyzextra(,n): extra number of byte to read from XYZ data start address, n in decimal, default DEFAULT_XYZ_EXTRA_BYTE (param[1] = n)
*							txyzextra(,n): extra number of byte to read from TXYZ data start address, n in decimal, default DEFAULT_TXYZ_EXTRA_BYTE (param[1] = n)
*							datalign(,n): TXYZ data alignment, n in decimal, default DEFAULT_DATA_ALIGNMENT (param[1] = n), 0 for data last, 255 for data first, other for the data shift
*							tfirst(,n): 1 for txyz, 0 for xyzt, n in decimal, default DEFAULT_T_FIRST (param[1] = n)
*							sspm(,n): SPI master clock mode: for n = 0~2, clock = 48MHz / 4^(n+1), for n = 3,  clock = TMR2 output/2 = (48/4)MHz / (PR2 + 1) / 2, default DEFAULT_SSPM (param[1] = n)
*							pr2(,n): timer2 period register: n = 1~255
* @return CMD_MULTI_W: "mw, start_addr, val_1, val_2, .....", multiwrite val_1, val_2, ...., val_n to starting adddress start_addr within one I2C transaction
*							start_addr: register start address in hex, returned by param[0]
*							param[1]: number of bytes to write
*							val_1, val_2, .... val_n: values to be assigned in hex, returned by param[2], param[3], ...., param[n+1]
* @return CMD_CHANGE_DEVICE: "sw" request MCU to change demo board mode
*/
unsigned char parseCommand(char* cmdStr, unsigned char* param){
	
	int index;
	signed char parseId;
	
	//make it the lower case
	strlwr(cmdStr);
	
	//get the command token
	charPtr = strtokpgmram(cmdStr, sepTok);
	
	//Look up the command
	parseId = -1;
	for(index = 0; index < fw_cmd_num; ++index){
		if(strcmppgm2ram(charPtr, ROM_STR_FW_CMD[index]) == 0){
			parseId = index;
			break;
		}
	}

	if(parseId < 0){ //no command parser Id found
		return CMD_UNKNOWN;
	}
	else{ //go the individual command parser
		return (*cmdParser[parseId])(cmdStr, param);
	}
}

unsigned char _q_parser(char* cmdStr, unsigned char* param){
	
	int index;
	signed char parseId;
	
	//get the field
	charPtr = strtokpgmram(NULL, sepTok);
	
	if(charPtr == NULL){ //no field, (q,)
			return CMD_UNKNOWN;
	}
	
	//Look up the command
	parseId = -1;
	for(index = 0; index < configure_num; ++index){
		if(strcmppgm2ram(charPtr, ROM_STR_CONFIGURE[index]) == 0){
			parseId = index;
			break;
		}
	}
	
	if(parseId >= 0){ //query item found
		param[0] = parseId;
		return CMD_QUERY;
	}
	
	return CMD_UNKNOWN;
}

/***********************standby****************************/
unsigned char _standby_parser(char* cmdStr, unsigned char* param){
	return CMD_STANDBY;
}

/***********************normal****************************/
unsigned char _normal_parser(char* cmdStr, unsigned char* param){
	return CMD_NORMAL;
}

unsigned char _r_parser(char* cmdStr, unsigned char* param){
	
	//expect dd or xyz
	charPtr = strtokpgmram(NULL, sepTok);

	if(charPtr == NULL){ //no dd or xyz
		return CMD_UNKNOWN;
	}

	//check if r,xyz(,n)(,fc, h or l)
	if(strcmppgm2ram(charPtr, ROM_STR_XYZ) == 0){
		param[0] = 100; //default 100*10Hz
		//get n if any
		charPtr = strtokpgmram(NULL, sepTok);
		if(charPtr != NULL){
			num = atol(charPtr);
			param[0] = num/10; //to make it fix one byte, Hz->10Hz
		}
		//get fc_h if any
		param[1] = 0;  //default highpass cutoff freq = 0, in milli-Hz, no filtering
		charPtr = strtokpgmram(NULL, sepTok);
		if(charPtr != NULL){
			param[1] = atol(charPtr);
		}
		//get fc_l if any
		param[2] = 0; //default lowpass cutoff freq = 0, in Hz, no filtering
		charPtr = strtokpgmram(NULL, sepTok);
		if(charPtr != NULL){
			param[2] = atol(charPtr);
		}

		return CMD_CONT_XYZ;
	}

	//handling dd
	num = _hexstrtol(charPtr);
	param[0] = num;

	//get n if any
	charPtr = strtokpgmram(NULL, sepTok);
	if(charPtr == NULL){
		return CMD_SINGLE_R; //r,dd
	}
	num = atol(charPtr);
	param[1] = num;
	return CMD_MULTI_R;
}

unsigned char _w_parser(char* cmdStr, unsigned char* param){
		
		//expect dd
		charPtr = strtokpgmram(NULL, sepTok);
		if(charPtr == NULL){ //no dd
			return CMD_UNKNOWN;
		}
		num = _hexstrtol(charPtr);
		param[0] = num;
		
		//expect bb
		charPtr = strtokpgmram(NULL, sepTok);
		if(charPtr == NULL){ //no bb
			return CMD_UNKNOWN;
		}
		num = _hexstrtol(charPtr);
		param[1] = num;
		
		//mm is optional
		charPtr = strtokpgmram(NULL, sepTok);
		if(charPtr == NULL){ //no mm, default
			num = 0xFF;
		}
		else{
			num = _hexstrtol(charPtr);
		}
		
		param[2] = num;
		
		return CMD_SINGLE_W;
}

unsigned char _o_parser(char* cmdStr, unsigned char* param){

		//expect ddd
		charPtr = strtokpgmram(NULL, sepTok);	
		if(charPtr == NULL){ //no ddd
			return CMD_UNKNOWN; //no match
		}
	
		//handling ddd
		num = _hexstrtol(charPtr);
		param[0] = (num>>8 & 0x0F); //high byte
		param[1] = num; //low byte
		
		//get n if any
		charPtr = strtokpgmram(NULL, sepTok);
		if(charPtr == NULL){
			return CMD_SINGLE_OR; //o,ddd
		}
		num = atol(charPtr);
		param[2] = num;
		return CMD_MULTI_OR; //o,ddd,n
		
}

unsigned char _ow_parser(char* cmdStr, unsigned char* param){

	//expect ddd
	charPtr = strtokpgmram(NULL, sepTok);	
	if(charPtr == NULL){ //no ddd
		return CMD_UNKNOWN; //no match
	}
	
	//handling ddd
	num = _hexstrtol(charPtr);
	param[0] = (num>>8 & 0x0F); //high byte
	param[1] = num; //low byte
	
	//expect bb
	charPtr = strtokpgmram(NULL, sepTok);
	if(charPtr == NULL){
		return CMD_UNKNOWN; //no bb, nothing to write
	}
	param[2] = _hexstrtol(charPtr);

	return CMD_SINGLE_OW; //ow,ddd,bb
}

unsigned char _rt_parser(char* cmdStr, unsigned char* param){

	//expect xyz
	charPtr = strtokpgmram(NULL, sepTok);
	if(charPtr == NULL || strcmppgm2ram(charPtr, ROM_STR_XYZ)){ //no field or field not xyz
		return CMD_UNKNOWN;
	}
	
	//expect nnn
	param[0] = 100; //default 100*10Hz
	charPtr = strtokpgmram(NULL, sepTok); //expecting nnn
	if(charPtr != NULL){ //no nnn, use default
		num = atol(charPtr);
		param[0] = num/10; //to make it fix one byte, Hz->10Hz
	}
	//get fc if any
	param[1] = 0;  //default cutoff freq = 0 milli-Hz, no filtering
	charPtr = strtokpgmram(NULL, sepTok);
	if(charPtr != NULL){
		param[1] = atol(charPtr);
	}
	//get h or l if any
	param[2] = 'h'; //default high pass filter
	charPtr = strtokpgmram(NULL, sepTok);
	if(charPtr != NULL && *charPtr == 'l'){
		param[2] = 'l';
	}
		
	return CMD_CONT_XYZT;
}

unsigned char _az_parser(char* cmdStr, unsigned char* param){

	//expect n
	param[0] = 20; //default 20*250us = 5ms

	charPtr = strtokpgmram(NULL, sepTok); //expecting n
	if(charPtr != NULL){ //if no n, use default
		num = atol(charPtr);
		param[0] = num & 0xFF; //only the byte
	}
	
	return CMD_AUTO_NIL;
}

unsigned char _i_parser(char* cmdStr, unsigned char* param){
	
	//expect field
	charPtr = strtokpgmram(NULL, sepTok);
	
	if(charPtr == NULL || strcmppgm2ram(charPtr, ROM_STR_F)){ //no field or field not f
		return CMD_UNKNOWN;
	}
	
	//expect nn
	charPtr = strtokpgmram(NULL, sepTok);
	if(charPtr == NULL){ //no n
		return CMD_UNKNOWN; //no match
	}
	
	//handling nn
	num = atol(charPtr);
	param[0] = num;
	
	return CMD_INTERRUPT;
}

unsigned char _c_parser(char* cmdStr, unsigned char* param){

	int index;
	signed char parseId;	
	
	//get the field
	charPtr = strtokpgmram(NULL, sepTok);
	
	if(charPtr == NULL){ //no field, (c,)
			return CMD_UNKNOWN;
	}
		
	//Look up the configure
	parseId = -1;
	for(index = 0; index < configure_num; ++index){
		if(strcmppgm2ram(charPtr, ROM_STR_CONFIGURE[index]) == 0){
			parseId = index;
			break;
		}
	}
	
	if(parseId > 1){ //configure item found, "fw"(0) is not configurable
		param[0] = parseId;
		
		//handling configure parameter
		charPtr = strtokpgmram(NULL, sepTok);
		
		if(charPtr == NULL){ //no parameter, using default value
			param[1] = default_configure[parseId];
		}
		else{
			num = atol(charPtr);
			param[1] = num;
		}
		
		return CMD_CONFIGURE;
	}	
	
	return CMD_UNKNOWN;
}

unsigned char _mw_parser(char* cmdStr, unsigned char* param){

	int val_num;
	
	//expect start_addr
	charPtr = strtokpgmram(NULL, sepTok);
	if(charPtr == NULL){ //no start_addr
		return CMD_UNKNOWN;
	}
	num = _hexstrtol(charPtr);
	param[0] = num;
	
	//expect val_1, val_2, ...val_n
	val_num = 0;
	while(charPtr = strtokpgmram(NULL, sepTok)){
		num = _hexstrtol(charPtr);
		param[++val_num + 1] = num;
	}
	
	if(val_num == 0){ //no value in
		return CMD_UNKNOWN;
	}
	
	param[1] = val_num; //number of bytes to write
	return CMD_MULTI_W;
}
unsigned char _change_device(char* cmdStr, unsigned char* param){
	return CMD_CHANGE_DEVICE;
}
long _hexstrtol(char* s){

	long m = 0;
	unsigned n =0;

	//to lower string
	strlwr(s);
	
	while(*(s+n)!=NULL){
		if(*(s+n)<='9'&&*(s+n)>='0'){
			m*=16;
			m+=*(s+n)-'0';
			n++;
		}
		else if(*(s+n)<='f'&&*(s+n)>='a'){
			m*=16;
			m+=*(s+n)-'a'+10;
			n++;
		}
		else{
			return m;
		}
	}
	return m;
}