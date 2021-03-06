/********************************************************************
 FileName:      main.c
 Dependencies:  See INCLUDES section
 Processor:     PIC18, PIC24, dsPIC, and PIC32 USB Microcontrollers
 Hardware:      This demo is natively intended to be used on Microchip USB demo
                boards supported by the MCHPFSUSB stack.  See release notes for
                support matrix.  This demo can be modified for use on other 
                hardware platforms.
 Complier:      Microchip C18 (for PIC18), XC16 (for PIC24/dsPIC), XC32 (for PIC32)
 Company:       Microchip Technology, Inc.

 Software License Agreement:

 The software supplied herewith by Microchip Technology Incorporated
 (the "Company") for its PIC?Microcontroller is intended and
 supplied to you, the Company's customer, for use solely and
 exclusively on Microchip PIC Microcontroller products. The
 software is owned by the Company and/or its supplier, and is
 protected under applicable copyright laws. All rights are reserved.
 Any use in violation of the foregoing restrictions may subject the
 user to criminal sanctions under applicable laws, as well as to
 civil liability for the breach of the terms and conditions of this
 license.

 THIS SOFTWARE IS PROVIDED IN AN "AS IS" CONDITION. NO WARRANTIES,
 WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED
 TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT,
 IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.

********************************************************************
 File Description:

 Change History:
  Rev   Description
  ----  -----------------------------------------
  1.0   Initial release

  2.1   Updated for simplicity and to use common
        coding style

  2.6a  Added button debouncing using Start-of-Frame packets

  2.7   Updated demo to place the PIC24F devices into sleep when the
        USB is in suspend.  

  2.7b  Improvements to USBCBSendResume(), to make it easier to use.
  2.9f  Adding new part support
********************************************************************/

/** INCLUDES *******************************************************/
#include "./USB/usb.h"
#include "./USB/usb_function_cdc.h"

#include "HardwareProfile.h"

/** CONFIGURATION **************************************************/
#if defined(PICDEM_FS_USB)      // Configuration bits for PICDEM FS USB Demo Board (based on PIC18F4550)
        #pragma config PLLDIV   = 5         // (20 MHz crystal on PICDEM FS USB board)
        #if (USB_SPEED_OPTION == USB_FULL_SPEED)
            #pragma config CPUDIV   = OSC1_PLL2  
        #else
            #pragma config CPUDIV   = OSC3_PLL4   
        #endif
        #pragma config USBDIV   = 2         // Clock source from 96MHz PLL/2
        #pragma config FOSC     = HSPLL_HS
        #pragma config FCMEN    = OFF
        #pragma config IESO     = OFF
        #pragma config PWRT     = OFF
        #pragma config BOR      = ON
        #pragma config BORV     = 3
        #pragma config VREGEN   = ON      //USB Voltage Regulator
        #pragma config WDT      = OFF
        #pragma config WDTPS    = 32768
        #pragma config MCLRE    = ON
        #pragma config LPT1OSC  = OFF
        #pragma config PBADEN   = OFF
//      #pragma config CCP2MX   = ON
        #pragma config STVREN   = ON
        #pragma config LVP      = OFF
//      #pragma config ICPRT    = OFF       // Dedicated In-Circuit Debug/Programming
        #pragma config XINST    = OFF       // Extended Instruction Set
        #pragma config CP0      = OFF
        #pragma config CP1      = OFF
//      #pragma config CP2      = OFF
//      #pragma config CP3      = OFF
        #pragma config CPB      = OFF
//      #pragma config CPD      = OFF
        #pragma config WRT0     = OFF
        #pragma config WRT1     = OFF
//      #pragma config WRT2     = OFF
//      #pragma config WRT3     = OFF
        #pragma config WRTB     = OFF       // Boot Block Write Protection
        #pragma config WRTC     = OFF
//      #pragma config WRTD     = OFF
        #pragma config EBTR0    = OFF
        #pragma config EBTR1    = OFF
//      #pragma config EBTR2    = OFF
//      #pragma config EBTR3    = OFF
        #pragma config EBTRB    = OFF

#elif defined(PICDEM_FS_USB_K50)
        #pragma config PLLSEL   = PLL3X     // 3X PLL multiplier selected
        #pragma config CFGPLLEN = OFF       // PLL turned on during execution
        #pragma config CPUDIV   = NOCLKDIV  // 1:1 mode (for 48MHz CPU)
        #pragma config LS48MHZ  = SYS48X8   // Clock div / 8 in Low Speed USB mode
        #pragma config FOSC     = INTOSCIO  // HFINTOSC selected at powerup, no clock out
        #pragma config PCLKEN   = OFF       // Primary oscillator driver
        #pragma config FCMEN    = OFF       // Fail safe clock monitor
        #pragma config IESO     = OFF       // Internal/external switchover (two speed startup)
        #pragma config nPWRTEN  = OFF       // Power up timer
        #pragma config BOREN    = SBORDIS   // BOR enabled
        #pragma config nLPBOR   = ON        // Low Power BOR
        #pragma config WDTEN    = SWON      // Watchdog Timer controlled by SWDTEN
        #pragma config WDTPS    = 32768     // WDT postscalar
        #pragma config PBADEN   = OFF       // Port B Digital/Analog Powerup Behavior
        #pragma config SDOMX    = RC7       // SDO function location
        #pragma config LVP      = OFF       // Low voltage programming
        #pragma config MCLRE    = ON        // MCLR function enabled (RE3 disabled)
        #pragma config STVREN   = ON        // Stack overflow reset
        //#pragma config ICPRT  = OFF       // Dedicated ICPORT program/debug pins enable
        #pragma config XINST    = OFF       // Extended instruction set

#elif defined(PIC18F87J50_PIM)				// Configuration bits for PIC18F87J50 FS USB Plug-In Module board
        #pragma config XINST    = OFF   	// Extended instruction set
        #pragma config STVREN   = ON      	// Stack overflow reset
        #pragma config PLLDIV   = 3         // (12 MHz crystal used on this board)
        #pragma config WDTEN    = OFF      	// Watch Dog Timer (WDT)
        #pragma config CP0      = OFF      	// Code protect
        #pragma config CPUDIV   = OSC1      // OSC1 = divide by 1 mode
        #pragma config IESO     = OFF      	// Internal External (clock) Switchover
        #pragma config FCMEN    = OFF      	// Fail Safe Clock Monitor
        #pragma config FOSC     = HSPLL     // Firmware must also set OSCTUNE<PLLEN> to start PLL!
        #pragma config WDTPS    = 32768
//      #pragma config WAIT     = OFF      	// Commented choices are
//      #pragma config BW       = 16      	// only available on the
//      #pragma config MODE     = MM      	// 80 pin devices in the 
//      #pragma config EASHFT   = OFF      	// family.
        #pragma config MSSPMSK  = MSK5
//      #pragma config PMPMX    = DEFAULT
//      #pragma config ECCPMX   = DEFAULT
        #pragma config CCP2MX   = DEFAULT   
        
// Configuration bits for PIC18F97J94 PIM and PIC18F87J94 PIM
#elif defined(PIC18F97J94_PIM) || defined(PIC18F87J94_PIM)
        #pragma config STVREN   = ON      	// Stack overflow reset
        #pragma config XINST    = OFF   	// Extended instruction set
        #pragma config BOREN    = ON        // BOR Enabled
        #pragma config BORV     = 0         // BOR Set to "2.0V" nominal setting
        #pragma config CP0      = OFF      	// Code protect disabled
        #pragma config FOSC     = FRCPLL    // Firmware should also enable active clock tuning for this setting
        #pragma config SOSCSEL  = LOW       // SOSC circuit configured for crystal driver mode
        #pragma config CLKOEN   = OFF       // Disable clock output on RA6
        #pragma config IESO     = OFF      	// Internal External (clock) Switchover
        #pragma config PLLDIV   = NODIV     // 4 MHz input (from 8MHz FRC / 2) provided to PLL circuit
        #pragma config POSCMD   = NONE      // Primary osc disabled, using FRC
        #pragma config FSCM     = CSECMD    // Clock switching enabled, fail safe clock monitor disabled
        #pragma config WPDIS    = WPDIS     // Program memory not write protected
        #pragma config WPCFG    = WPCFGDIS  // Config word page of program memory not write protected
        #pragma config IOL1WAY  = OFF       // IOLOCK can be set/cleared as needed with unlock sequence
        #pragma config LS48MHZ  = SYSX2     // Low Speed USB clock divider
        #pragma config WDTCLK   = LPRC      // WDT always uses INTOSC/LPRC oscillator
        #pragma config WDTEN    = ON        // WDT disabled; SWDTEN can control WDT
        #pragma config WINDIS   = WDTSTD    // Normal non-window mode WDT.
        #pragma config VBTBOR   = OFF       // VBAT BOR disabled
      
#elif defined(PIC18F46J50_PIM) || defined(PIC18F_STARTER_KIT_1) || defined(PIC18F47J53_PIM)
     #pragma config WDTEN = OFF          //WDT disabled (enabled by SWDTEN bit)
     #pragma config PLLDIV = 3           //Divide by 3 (12 MHz oscillator input)
     #pragma config STVREN = ON          //stack overflow/underflow reset enabled
     #pragma config XINST = OFF          //Extended instruction set disabled
     #pragma config CPUDIV = OSC1        //No CPU system clock divide
     #pragma config CP0 = OFF            //Program memory is not code-protected
     #pragma config OSC = HSPLL          //HS oscillator, PLL enabled, HSPLL used by USB
     //#pragma config T1DIG = ON           //Sec Osc clock source may be selected
     //#pragma config LPT1OSC = OFF        //high power Timer1 mode
     #pragma config FCMEN = OFF          //Fail-Safe Clock Monitor disabled
     #pragma config IESO = OFF           //Two-Speed Start-up disabled
     #pragma config WDTPS = 32768        //1:32768
     #pragma config DSWDTOSC = INTOSCREF //DSWDT uses INTOSC/INTRC as clock
     #pragma config RTCOSC = T1OSCREF    //RTCC uses T1OSC/T1CKI as clock
     #pragma config DSBOREN = OFF        //Zero-Power BOR disabled in Deep Sleep
     #pragma config DSWDTEN = OFF        //Disabled
     #pragma config DSWDTPS = 8192       //1:8,192 (8.5 seconds)
     #pragma config IOL1WAY = OFF        //IOLOCK bit can be set and cleared
     #pragma config MSSP7B_EN = MSK7     //7 Bit address masking
     #pragma config WPFP = PAGE_1        //Write Protect Program Flash Page 0
     #pragma config WPEND = PAGE_0       //Start protection at page 0
     #pragma config WPCFG = OFF          //Write/Erase last page protect Disabled
     #pragma config WPDIS = OFF          //WPFP[5:0], WPEND, and WPCFG bits ignored  
#elif defined(LOW_PIN_COUNT_USB_DEVELOPMENT_KIT)
        #pragma config CPUDIV = NOCLKDIV
        #pragma config USBDIV = OFF
        #pragma config FOSC   = HS
        #pragma config PLLEN  = ON
        #pragma config FCMEN  = OFF
        #pragma config IESO   = OFF
        #pragma config PWRTEN = OFF
        #pragma config BOREN  = OFF
        #pragma config BORV   = 30
        #pragma config WDTEN  = OFF
        #pragma config WDTPS  = 32768
        #pragma config MCLRE  = OFF
        #pragma config HFOFST = OFF
        #pragma config STVREN = ON
        #pragma config LVP    = OFF
        #pragma config XINST  = OFF
        #pragma config BBSIZ  = OFF
        #pragma config CP0    = OFF
        #pragma config CP1    = OFF
        #pragma config CPB    = OFF
        #pragma config WRT0   = OFF
        #pragma config WRT1   = OFF
        #pragma config WRTB   = OFF
        #pragma config WRTC   = OFF
        #pragma config EBTR0  = OFF
        #pragma config EBTR1  = OFF
        #pragma config EBTRB  = OFF       

#elif	defined(PIC16F1_LPC_USB_DEVELOPMENT_KIT)
    // PIC 16F1459 fuse configuration:
    // Config word 1 (Oscillator configuration)
    // 20Mhz crystal input scaled to 48Mhz and configured for USB operation
    #if defined (USE_INTERNAL_OSC)
        __CONFIG(FOSC_INTOSC & WDTE_OFF & PWRTE_OFF & MCLRE_OFF & CP_OFF & BOREN_OFF & CLKOUTEN_ON & IESO_OFF & FCMEN_OFF);
        __CONFIG(WRT_OFF & CPUDIV_NOCLKDIV & USBLSCLK_48MHz & PLLMULT_3x & PLLEN_ENABLED & STVREN_ON &  BORV_LO & LPBOR_OFF & LVP_OFF);
    #else
        __CONFIG(FOSC_HS & WDTE_OFF & PWRTE_OFF & MCLRE_OFF & CP_OFF & BOREN_OFF & CLKOUTEN_ON & IESO_OFF & FCMEN_OFF);
        __CONFIG(WRT_OFF & CPUDIV_NOCLKDIV & USBLSCLK_48MHz & PLLMULT_4x & PLLEN_ENABLED & STVREN_ON &  BORV_LO & LPBOR_OFF & LVP_OFF);
    #endif

#elif defined(EXPLORER_16)
    #if defined(__PIC24FJ256GB110__)
        _CONFIG1( JTAGEN_OFF & GCP_OFF & GWRP_OFF & FWDTEN_OFF & ICS_PGx2) 
        _CONFIG2( PLL_96MHZ_ON & IESO_OFF & FCKSM_CSDCMD & OSCIOFNC_ON & POSCMOD_HS & FNOSC_PRIPLL & PLLDIV_DIV2 & IOL1WAY_ON)
    #elif defined(__PIC24FJ256GB210__)
        _CONFIG1(FWDTEN_OFF & ICS_PGx2 & GWRP_OFF & GCP_OFF & JTAGEN_OFF)
        _CONFIG2(POSCMOD_HS & IOL1WAY_ON & OSCIOFNC_ON & FCKSM_CSDCMD & FNOSC_PRIPLL & PLL96MHZ_ON & PLLDIV_DIV2 & IESO_OFF)
    #elif defined(__PIC24FJ64GB004__)
        _CONFIG1(WDTPS_PS1 & FWPSA_PR32 & WINDIS_OFF & FWDTEN_OFF & ICS_PGx1 & GWRP_OFF & GCP_OFF & JTAGEN_OFF)
        _CONFIG2(POSCMOD_HS & I2C1SEL_PRI & IOL1WAY_OFF & OSCIOFNC_ON & FCKSM_CSDCMD & FNOSC_PRIPLL & PLL96MHZ_ON & PLLDIV_DIV2 & IESO_OFF)
        _CONFIG3(WPFP_WPFP0 & SOSCSEL_SOSC & WUTSEL_LEG & WPDIS_WPDIS & WPCFG_WPCFGDIS & WPEND_WPENDMEM)
        _CONFIG4(DSWDTPS_DSWDTPS3 & DSWDTOSC_LPRC & RTCOSC_SOSC & DSBOREN_OFF & DSWDTEN_OFF)
    #elif defined(__32MX460F512L__) || defined(__32MX795F512L__)
        #pragma config UPLLEN   = ON        // USB PLL Enabled
        #pragma config FPLLMUL  = MUL_15        // PLL Multiplier
        #pragma config UPLLIDIV = DIV_2         // USB PLL Input Divider
        #pragma config FPLLIDIV = DIV_2         // PLL Input Divider
        #pragma config FPLLODIV = DIV_1         // PLL Output Divider
        #pragma config FPBDIV   = DIV_1         // Peripheral Clock divisor
        #pragma config FWDTEN   = OFF           // Watchdog Timer
        #pragma config WDTPS    = PS1           // Watchdog Timer Postscale
        #pragma config FCKSM    = CSDCMD        // Clock Switching & Fail Safe Clock Monitor
        #pragma config OSCIOFNC = OFF           // CLKO Enable
        #pragma config POSCMOD  = HS            // Primary Oscillator
        #pragma config IESO     = OFF           // Internal/External Switch-over
        #pragma config FSOSCEN  = OFF           // Secondary Oscillator Enable (KLO was off)
        #pragma config FNOSC    = PRIPLL        // Oscillator Selection
        #pragma config CP       = OFF           // Code Protect
        #pragma config BWP      = OFF           // Boot Flash Write Protect
        #pragma config PWP      = OFF           // Program Flash Write Protect
        #pragma config ICESEL   = ICS_PGx2      // ICE/ICD Comm Channel Select
    #elif defined(__dsPIC33EP512MU810__)|| defined(__PIC24EP512GU810__)
        _FOSCSEL(FNOSC_FRC);
        _FOSC(FCKSM_CSECMD & OSCIOFNC_OFF & POSCMD_XT);
        _FWDT(FWDTEN_OFF);
    #else
        #error No hardware board defined, see "HardwareProfile.h" and __FILE__
    #endif
#elif defined(PIC24F_STARTER_KIT)
    _CONFIG1( JTAGEN_OFF & GCP_OFF & GWRP_OFF & FWDTEN_OFF & ICS_PGx2) 
    _CONFIG2( PLL_96MHZ_ON & IESO_OFF & FCKSM_CSDCMD & OSCIOFNC_ON & POSCMOD_HS & FNOSC_PRIPLL & PLLDIV_DIV3 & IOL1WAY_ON)
#elif defined(PIC24FJ256DA210_DEV_BOARD)
    _CONFIG1(FWDTEN_OFF & ICS_PGx2 & GWRP_OFF & GCP_OFF & JTAGEN_OFF)
    _CONFIG2(POSCMOD_HS & IOL1WAY_ON & OSCIOFNC_ON & FCKSM_CSDCMD & FNOSC_PRIPLL & PLL96MHZ_ON & PLLDIV_DIV2 & IESO_OFF)
#elif defined(PIC32_USB_STARTER_KIT)
    #pragma config UPLLEN   = ON        // USB PLL Enabled
    #pragma config FPLLMUL  = MUL_15        // PLL Multiplier
    #pragma config UPLLIDIV = DIV_2         // USB PLL Input Divider
    #pragma config FPLLIDIV = DIV_2         // PLL Input Divider
    #pragma config FPLLODIV = DIV_1         // PLL Output Divider
    #pragma config FPBDIV   = DIV_1         // Peripheral Clock divisor
    #pragma config FWDTEN   = OFF           // Watchdog Timer
    #pragma config WDTPS    = PS1           // Watchdog Timer Postscale
    #pragma config FCKSM    = CSDCMD        // Clock Switching & Fail Safe Clock Monitor
    #pragma config OSCIOFNC = OFF           // CLKO Enable
    #pragma config POSCMOD  = HS            // Primary Oscillator
    #pragma config IESO     = OFF           // Internal/External Switch-over
    #pragma config FSOSCEN  = OFF           // Secondary Oscillator Enable (KLO was off)
    #pragma config FNOSC    = PRIPLL        // Oscillator Selection
    #pragma config CP       = OFF           // Code Protect
    #pragma config BWP      = OFF           // Boot Flash Write Protect
    #pragma config PWP      = OFF           // Program Flash Write Protect
    #pragma config ICESEL   = ICS_PGx2      // ICE/ICD Comm Channel Select
#elif defined(DSPIC33E_USB_STARTER_KIT)
    _FOSCSEL(FNOSC_FRC);
    _FOSC(FCKSM_CSECMD & OSCIOFNC_OFF & POSCMD_XT);
    _FWDT(FWDTEN_OFF);
#elif defined(PIC24FJ64GB502_MICROSTICK)
    _CONFIG1(WDTPS_PS1 & FWPSA_PR32 & WINDIS_OFF & FWDTEN_OFF & ICS_PGx1 & GWRP_OFF & GCP_OFF & JTAGEN_OFF)
    _CONFIG2(I2C1SEL_PRI & IOL1WAY_OFF & FCKSM_CSDCMD & FNOSC_PRIPLL & PLL96MHZ_ON & PLLDIV_DIV2 & IESO_OFF)
    _CONFIG3(WPFP_WPFP0 & SOSCSEL_SOSC & WUTSEL_LEG & WPDIS_WPDIS & WPCFG_WPCFGDIS & WPEND_WPENDMEM)
    _CONFIG4(DSWDTPS_DSWDTPS3 & DSWDTOSC_LPRC & RTCOSC_SOSC & DSBOREN_OFF & DSWDTEN_OFF)
#else
    #error No hardware board defined, see "HardwareProfile.h" and __FILE__
#endif

/** I N C L U D E S **********************************************************/

#include "GenericTypeDefs.h"
#include "Compiler.h"
#include "timers.h"
#include "delays.h"
#include "USB/usb_config.h"
#include "USB/usb_device.h"
#include "USB/usb.h"
#include "./mylib/HW_DEFINITION.h"
#include "./mylib/DATA_TRANSFER.h"
#include "./mylib/FW_CMD_PARSER_KAI.h"

/** V A R I A B L E S ********************************************************/
#if defined(__18CXX)
    #pragma udata
#endif
/***G L O B E L V A R I A B L E***********************************************/
const rom far char* ROM_STR_CODE_TYPE[] = {
	(const rom far char*) "D03/8\n",   //6
	(const rom far char*) "D05/6/7\n", //8
	(const rom far char*) "D10\n"      //4
};
const rom far char* ROM_STR_CR = (const rom far char*) "\r";
const rom far char* ROM_STR_NL = (const rom far char*) "\n";
const rom far char* ROM_STR_COMMA = (const rom far char*) ",";
const rom far char* ROM_STR_DATA_FORMAT = (const rom far char*) "%02X";

union{
	struct{
		unsigned int i2cSlaveAddr;
		unsigned int i2cBaudRate_kHz;
		unsigned int txyzDataByteLen;
		unsigned int xyzDataStartAddr;
		unsigned int txyzDataStartAddr;	
		unsigned int dataEndianness;
		unsigned int xyzDataExtraByte;
		unsigned int txyzDataExtraByte;
		unsigned int dataAlignment;
		unsigned int tFirst;
		unsigned int sspm;
		unsigned int pr2;
		unsigned int ifs;
		unsigned int commandPort;
		unsigned int monitorPort;
	};
	unsigned int v[15];
} mConfig = {
	DEFAULT_I2C_SLAVE_ADDR,
	100L*DEFAULT_I2C_BAUD_RATE_100KHZ,
	DEFAULT_TXYZ_DATA_BYTE_LEN,
	DEFAULT_XYZ_DATA_START_ADDR,
	DEFAULT_TXYZ_DATA_START_ADDR,
	DEFAULT_DATA_ENDIANNESS,
	DEFAULT_XYZ_EXTRA_BYTE,
	DEFAULT_TXYZ_EXTRA_BYTE,
	DEFAULT_DATA_ALIGNMENT,
	DEFAULT_T_FIRST,
	DEFAULT_SSPM,
	DEFAULT_PR2,
	DEFAULT_IFS,
	DEFAULT_CMD,
	DEFAULT_MON	};

unsigned char led_flag = 0, RxFlag = 0, led_blink_status_flag = 0;
unsigned char xyz_output_time_count = 0, periodXYZoutFlag = 0, dataReadyFlag = 0, T_outFlag = 0;
unsigned char monitorINT1Flag = 0, monitorINT2Flag = 0;
unsigned char ASIC_TYPE;
unsigned char sw2_pressed_flag = 0, sw2_long_pressed_flag=0, sw2_short_pressed_flag=0;//2013_0114 added by York
unsigned short int sw2_counter = 0, sw2_long_pressed_count = 0;
char USB_In_Buffer[64];
char USB_Out_Buffer[64];

BOOL stringPrinted;
volatile BOOL buttonPressed;
volatile BYTE buttonCount;

/** P R I V A T E  P R O T O T Y P E S ***************************************/
static void InitializeSystem(void);
void USBDeviceTasks(void);
void YourHighPriorityISRCode();
void YourLowPriorityISRCode();
void USBCBSendResume(void);
void BlinkUSBStatus(void);
void UserInit(void);
unsigned char ProcessIO(unsigned char*);
void DEMO_C802(unsigned char, unsigned char*);
void DEMO_C803(unsigned char, unsigned char*);
void DEMO_D10(unsigned char, unsigned char*);
void mFreqT0cki(long*);
void EraseUserFlash(void);
void WriteUserFlash(void);
void ReadAsicType(void);
void sw2_push_check(void);
void sw2_function(void);
/** VECTOR REMAPPING ***********************************************/
#if defined(__18CXX)
	//On PIC18 devices, addresses 0x00, 0x08, and 0x18 are used for
	//the reset, high priority interrupt, and low priority interrupt
	//vectors.  However, the current Microchip USB bootloader 
	//examples are intended to occupy addresses 0x00-0x7FF or
	//0x00-0xFFF depending on which bootloader is used.  Therefore,
	//the bootloader code remaps these vectors to new locations
	//as indicated below.  This remapping is only necessary if you
	//wish to program the hex file generated from this project with
	//the USB bootloader.  If no bootloader is used, edit the
	//usb_config.h file and comment out the following defines:
	//#define PROGRAMMABLE_WITH_USB_HID_BOOTLOADER
	//#define PROGRAMMABLE_WITH_USB_LEGACY_CUSTOM_CLASS_BOOTLOADER
	
	#if defined(PROGRAMMABLE_WITH_USB_HID_BOOTLOADER)
		#define REMAPPED_RESET_VECTOR_ADDRESS			0x1000
		#define REMAPPED_HIGH_INTERRUPT_VECTOR_ADDRESS	0x1008
		#define REMAPPED_LOW_INTERRUPT_VECTOR_ADDRESS	0x1018
	#elif defined(PROGRAMMABLE_WITH_USB_MCHPUSB_BOOTLOADER)	
		#define REMAPPED_RESET_VECTOR_ADDRESS			0x800
		#define REMAPPED_HIGH_INTERRUPT_VECTOR_ADDRESS	0x808
		#define REMAPPED_LOW_INTERRUPT_VECTOR_ADDRESS	0x818
	#else	
		#define REMAPPED_RESET_VECTOR_ADDRESS			0x00
		#define REMAPPED_HIGH_INTERRUPT_VECTOR_ADDRESS	0x08
		#define REMAPPED_LOW_INTERRUPT_VECTOR_ADDRESS	0x18
	#endif
	
	#if defined(PROGRAMMABLE_WITH_USB_HID_BOOTLOADER)||defined(PROGRAMMABLE_WITH_USB_MCHPUSB_BOOTLOADER)
	extern void _startup (void);        // See c018i.c in your C18 compiler dir
	#pragma code REMAPPED_RESET_VECTOR = REMAPPED_RESET_VECTOR_ADDRESS
	void _reset (void)
	{
	    _asm goto _startup _endasm
	}
	#endif
	#pragma code REMAPPED_HIGH_INTERRUPT_VECTOR = REMAPPED_HIGH_INTERRUPT_VECTOR_ADDRESS
	void Remapped_High_ISR (void)
	{
	     _asm goto YourHighPriorityISRCode _endasm
	}
	#pragma code REMAPPED_LOW_INTERRUPT_VECTOR = REMAPPED_LOW_INTERRUPT_VECTOR_ADDRESS
	void Remapped_Low_ISR (void)
	{
	     _asm goto YourLowPriorityISRCode _endasm
	}
	
	#if defined(PROGRAMMABLE_WITH_USB_HID_BOOTLOADER)||defined(PROGRAMMABLE_WITH_USB_MCHPUSB_BOOTLOADER)
	//Note: If this project is built while one of the bootloaders has
	//been defined, but then the output hex file is not programmed with
	//the bootloader, addresses 0x08 and 0x18 would end up programmed with 0xFFFF.
	//As a result, if an actual interrupt was enabled and occured, the PC would jump
	//to 0x08 (or 0x18) and would begin executing "0xFFFF" (unprogrammed space).  This
	//executes as nop instructions, but the PC would eventually reach the REMAPPED_RESET_VECTOR_ADDRESS
	//(0x1000 or 0x800, depending upon bootloader), and would execute the "goto _startup".  This
	//would effective reset the application.
	
	//To fix this situation, we should always deliberately place a 
	//"goto REMAPPED_HIGH_INTERRUPT_VECTOR_ADDRESS" at address 0x08, and a
	//"goto REMAPPED_LOW_INTERRUPT_VECTOR_ADDRESS" at address 0x18.  When the output
	//hex file of this project is programmed with the bootloader, these sections do not
	//get bootloaded (as they overlap the bootloader space).  If the output hex file is not
	//programmed using the bootloader, then the below goto instructions do get programmed,
	//and the hex file still works like normal.  The below section is only required to fix this
	//scenario.
	#pragma code HIGH_INTERRUPT_VECTOR = 0x08
	void High_ISR (void)
	{
	     _asm goto REMAPPED_HIGH_INTERRUPT_VECTOR_ADDRESS _endasm
	}
	#pragma code LOW_INTERRUPT_VECTOR = 0x18
	void Low_ISR (void)
	{
	     _asm goto REMAPPED_LOW_INTERRUPT_VECTOR_ADDRESS _endasm
	}
	#endif	//end of "#if defined(PROGRAMMABLE_WITH_USB_HID_BOOTLOADER)||defined(PROGRAMMABLE_WITH_USB_LEGACY_CUSTOM_CLASS_BOOTLOADER)"

	#pragma code
	
	
	//These are your actual interrupt handling routines.
	#pragma interrupt YourHighPriorityISRCode
	void YourHighPriorityISRCode()
	{
	static unsigned int led_cnt = 0;	//delay counter
	static unsigned int periodXYZ_cnt = 0; //period output XYZ counter

		if(PIR3bits.TMR4IF == 1){
		PIR3bits.TMR4IF = 0; //clear interrupt flag
/*
2013_0113 added by York
*/
		sw2_counter = sw2_counter+sw2_pressed_flag;
		sw2_long_pressed_count = sw2_long_pressed_count+sw2_long_pressed_flag;
// 2013_0111 added end here.
		//periodic XYZ output
		if(periodXYZoutFlag == 1 && ++periodXYZ_cnt >= xyz_output_time_count){
			periodXYZ_cnt = 0;		// reset counter
			dataReadyFlag = 1;	// raise the flag
			}
		}
	
		if(INTCON3bits.INT1F == 1){//INT1
			INTCON3bits.INT1F = 0; //clear the interrupt flag
			dataReadyFlag = monitorINT1Flag; //raise the flag only when requested
		}
		if(INTCON3bits.INT2F == 1){ //INT2
			INTCON3bits.INT2F = 0; //clear the interrupt flag
			dataReadyFlag = monitorINT2Flag; //raise the flag only when requested	
		}

  		#if defined(USB_INTERRUPT)
	  	  USBDeviceTasks();
  	 	#endif

}	//This return will be a "retfie fast", since this is in a #pragma interrupt section 

	#pragma interruptlow YourLowPriorityISRCode
	void YourLowPriorityISRCode()
	{
		//Check which interrupt flag caused the interrupt.
		//Service the interrupt
		//Clear the interrupt flag
		//Etc.
	
	}	//This return will be a "retfie", since this is in a #pragma interruptlow section 
#endif

/** DECLARATIONS ***************************************************/
#pragma code
/******************************************************************************
 * Function:        void main(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Main program entry point.
 *
 * Note:            None
 *****************************************************************************/
#if defined(__18CXX)
void main(void)
#else
int main(void)
#endif
{   
	unsigned char param[64];
	unsigned char cmd_parser;
	char lock;
	ReadAsicType(); //To read the ASIC revision from flash memory. It will be 0xFF if the flash memory has been erased. 2013_0104 by York
	switch(ASIC_TYPE) //special cases ONLY
	{
		// For the function of erase flash memory.
		case 0xFF: 	ASIC_TYPE = 0x07;
		break;
		//For the device to use SPI interface only.
		case 0x08: mConfig.ifs = INTERFACE_SPI;
		break;
		default:
		break;
	}
	InitializeSystem();
	cmd_parser = 0;
	LATD = ASIC_TYPE;// To show the device to LED
    while(1)
    {
		cmd_parser = ProcessIO(param);
		if (ASIC_TYPE == 0x07)
		{
			DEMO_C803(cmd_parser, param);
		} else if (ASIC_TYPE == 0x08)
		{
			DEMO_C802(cmd_parser, param);
		} else if (ASIC_TYPE == 0x0A)
		{
			DEMO_D10(cmd_parser, param);
		}
        #if defined(USB_INTERRUPT)
            if(USB_BUS_SENSE && (USBGetDeviceState() == DETACHED_STATE))
            {
                USBDeviceAttach();
            }
        #endif
		sw2_function();//2013_0114 added by York
		// Application-specific tasks.
		// Application related code may be added here, or in the ProcessIO() function.    
    }//end while
}//end main
/*******************************************************************/
unsigned char ProcessIO(unsigned char* param)
{   
    BYTE numBytesRead;
	BYTE i;
	unsigned char Command;
	Command = 0;
    //Blink the LEDs according to the USB device status
	BlinkUSBStatus();
    // User Application USB tasks
    if((USBDeviceState < CONFIGURED_STATE)||(USBSuspendControl==1)) return 0;

    if(USBUSARTIsTxTrfReady())
    {
		numBytesRead = getsUSBUSART(USB_Out_Buffer,20);
		if(numBytesRead != 0)
		{
			periodXYZoutFlag = 0; //stop the periodic XYZout monitoring
			T_outFlag = 0; //reset the Tout flag
			dataReadyFlag = 0; //reset the flag
			monitorINT1Flag = 0;  //stop monitor INT
			monitorINT2Flag = 0;  //stop monitor INT
			INTCON3bits.INT1E = 0;  //disable INT1
			INTCON3bits.INT2E = 0; //disable INT2
			USB_Out_Buffer[numBytesRead - 1] = '\0';
			Command = parseCommand(USB_Out_Buffer, param);
			RxFlag = 1;
		}
	}
    CDCTxService();
	return(Command);
}		//end ProcessIO

/*******************************************************************/
void DEMO_C803(unsigned char Command, unsigned char * param){
	unsigned char SlaveAddr;
	char i;
	SlaveAddr = 0x38;
	if(RxFlag == 1)
	{
	switch(Command)
	{
		case 1://q,fw
		{
			strcpypgm2ram(USB_In_Buffer,ROM_STR_CODE_TYPE[1]);
			putUSBUSART(USB_In_Buffer,8);
		}
		break;
		case 4: //r,dd
		{
			I2C_Read(SlaveAddr,param[0], USB_In_Buffer, 1);
			putUSBUSART(USB_In_Buffer,1);
		}
		break;
		case 5: //r,dd,n
		{
			I2C_Read(SlaveAddr,param[0], USB_In_Buffer, param[1]);
			putUSBUSART(USB_In_Buffer,param[1]);
		}
		break;
		case 6:	
			if(param[0] <= 0 || param[0] > 100){ //not support freq range
				periodXYZoutFlag = 0; //turn off the periodic output flag
			}
			else{ //turn on the periodic output flag
				periodXYZoutFlag = 1;
					//xyz_output_time_count = 400/param[0]; //every count = 250us
				xyz_output_time_count = 100/param[0]; //every count = 1ms					
			}
		break;
		case 7: //w,dd,nn,mm
		{
			I2C_Read(SlaveAddr,param[0], USB_In_Buffer, 1);
			USB_In_Buffer[1] = (USB_In_Buffer[0] & ~param[2]) | (param[1] & param[2]);
			I2C_Write(SlaveAddr,param[0],&USB_In_Buffer[1] , 1);
		}
		break;
		case 9: //0,ddd,n
		{
			G_SENSOR_OTP_ENABLE_ON;
			i = 10;
			while(i--);
			I2C_Read(SlaveAddr,param[1], USB_In_Buffer, param[2]);
			G_SENSOR_OTP_ENABLE_OFF;
			putUSBUSART(USB_In_Buffer, param[2]);
		}
		break;
		case 10:
		{
			if( param[0] == 1){
				TRISBbits.TRISB1 = 1;  //enable interrupt on change
				INTCON2bits.INTEDG1 = 1;
				INTCON3bits.INT1E = 1;  //enable eht INT external interrupt

				//start monitor INT
	  			monitorINT1Flag = 1;
			}
			else if( param[0] == 2){
			}
				TRISBbits.TRISB2 = 1;  //enable interrupt on change
				INTCON2bits.INTEDG2 = 1;
				INTCON3bits.INT2E = 1;  //enable eht INT external interrupt

	  			monitorINT2Flag = 1;
			}
		break;
		case 11: //ow,ddd,bb
		{
			G_SENSOR_OTP_ENABLE_ON;
			i = 10;
			while(i--);
			I2C_Write(SlaveAddr,param[1], param[2], 1);
			G_SENSOR_OTP_ENABLE_OFF;
		}
		break;
		case 12: //r,xyzT
		{
			if(param[0] <= 0 || param[0] > 100){
				periodXYZoutFlag = 0;
			} else {
				periodXYZoutFlag = 1;
				xyz_output_time_count = 100/param[0];	
			}
			T_outFlag = 1;
		}
		break;
		case 13: //AutoNil
		{
			INTCONbits.GIE = 0;
			Command = AZ_SCALE_FACTOR * param[0];
			G_SENSOR_AZ_PIN_ON;
			for(i = 0; i < Command; i--){}
			G_SENSOR_AZ_PIN_OFF;
			INTCONbits.GIE = 1; // 2013_0107 added by Kuan. Bug fixed.
		}
		break;
 		case CMD_MULTI_W:
 		{
		    I2C_Write(SlaveAddr,param[0],&param[2] , param[1]);
 		    putUSBUSART(&param[1], 1);   
 		}
 		break;
		case 16: //change device
		{
			ASIC_TYPE = 0x08;
			LATD = ASIC_TYPE;
			INTCON3bits.INT1E = 0;  
			INTCON3bits.INT2E = 0;
	  		monitorINT1Flag = 0;
	  		monitorINT2Flag = 0;
			dataReadyFlag = 0;
			EraseUserFlash();
			WriteUserFlash();
			SPI_Init();
		}
		default:
		break;
	}
	RxFlag = 0;
	}
	if(dataReadyFlag){ //suspend once RxFlag is set //if(sendXYZoutFlag){
		USB_In_Buffer[0] = 0xFF;
		USB_In_Buffer[1] = 0xFF;
		LATD = 0x00;
		if(T_outFlag)
		{
			I2C_Read(SlaveAddr,0x40, &USB_In_Buffer[2], 4);
			putUSBUSART(USB_In_Buffer,6);
		} else {
			I2C_Read(SlaveAddr,0x41, &USB_In_Buffer[2], 3);
			putUSBUSART(USB_In_Buffer,5);
		}
		dataReadyFlag = 0;
		LATD = 0x01;
	}
}
/*******************************************************************/
void DEMO_C802(unsigned char Command, unsigned char * param){
	unsigned short regAddr;
	char i;
	regAddr = 0x0000;
	if(RxFlag == 1)
	{
	switch(Command)
	{
		case 1://q,fw
		{
			#if defined (DEBUG_COMMAND)
			LATD = 1;
			#endif
			strcpypgm2ram(USB_In_Buffer,ROM_STR_CODE_TYPE[0]);
			putUSBUSART(USB_In_Buffer,6);
		}
		break;
		case 2: //standby
			#if defined (DEBUG_COMMAND)
			LATD = 2;
			#endif
			G_SENSOR_CE_OFF;
		break;
		case 3: //normal
			#if defined (DEBUG_COMMAND)
			LATD = 3;
			#endif
			G_SENSOR_CE_ON;
		break;
		case 4: //r,dd
		{
			#if defined (DEBUG_COMMAND)
			LATD = 4;
			#endif
			regAddr |= param[0];
			SPI_Read(regAddr, USB_In_Buffer, 1);
			putUSBUSART(USB_In_Buffer,1);
		}
		break;
		case 5: //r,dd,n
		{
			#if defined (DEBUG_COMMAND)
			LATD = 5;
			#endif
			regAddr |= param[0];
			SPI_Read(regAddr, USB_In_Buffer, param[1]);
			putUSBUSART(USB_In_Buffer,param[1]);
		}
		break;
		case 6:
			#if defined (DEBUG_COMMAND)
			LATD = 6;
			#endif
			if(param[0] <= 0 || param[0] > 100){ //not support freq range
				periodXYZoutFlag = 0; //turn off the periodic output flag
			}
			else{ //turn on the periodic output flag
				periodXYZoutFlag = 1;
				//xyz_output_time_count = 400/param[0]; //every count = 250us
				xyz_output_time_count = 100/param[0]; //every count = 1ms						
			}
		break;
		case 7: //w,dd,nn,mm
		{
			#if defined (DEBUG_COMMAND)
			LATD = 7;
			#endif
			regAddr |= param[0];
			SPI_Read(regAddr, USB_In_Buffer,1);
			USB_In_Buffer[1] = (USB_In_Buffer[0] & ~param[2]) | (param[1] & param[2]);
			SPI_Write(regAddr,&USB_In_Buffer[1] , 1);
		}
		break; 
		case 8: //o,ddd
		{
			#if defined (DEBUG_COMMAND)
			LATD = 8;
			#endif
			regAddr = 0x0028 | param[0];
  			regAddr <<= 8;
			regAddr |= param[1];
	 		SPI_Read(regAddr, USB_In_Buffer, 1);
			putUSBUSART(USB_In_Buffer,1);
		}
		break;
		case 9: //o,ddd,n
		{
			#if defined (DEBUG_COMMAND)
			LATD = 9;
			#endif
		  	regAddr = 0x0028 | param[0];
  			regAddr <<= 8;
			regAddr |= param[1];
	 		SPI_Read(regAddr, USB_In_Buffer, param[2]);
			putUSBUSART(USB_In_Buffer, param[2]);
		}
		break;
		case 10:
		{
			#if defined (DEBUG_COMMAND)
			LATD = 10;
			#endif
			 putUSBUSART(param[0], 1);
		}
		break;
		case 11: //ow,ddd,bb
		{
			#if defined (DEBUG_COMMAND)
			LATD = 11;
			#endif
		  	regAddr = 0x0028 | param[0];
  			regAddr <<= 8;
			regAddr |= param[1];
			SPI_Write(regAddr, param[2], 1);
		}
		break;
		case 12:
		{
			#if defined (DEBUG_COMMAND)
			LATD = 12;
			#endif
			if(param[0] <= 0 || param[0] > 100){
				periodXYZoutFlag = 0;
			} else {
				periodXYZoutFlag = 1;
				xyz_output_time_count = 100/param[0];	
			}
			T_outFlag = 1;
		}
		break; // 2013_0107 added by Kuan. Bug fixed.
 		case CMD_MULTI_W:
  		{
			regAddr |= param[0];
  	      	SPI_Write(regAddr,&param[2] , param[1]);
 		}
 		break;
		case 16:
		{
			#if defined (DEBUG_COMMAND)
			LATD = 16;
			#endif
			ASIC_TYPE = 0x0A;
			LATD = ASIC_TYPE;
			EraseUserFlash();
			WriteUserFlash();
			mConfig.ifs = INTERFACE_I2C; //update ifs for D10 demo
			I2C_Init();
		}
		default:
		break;
	}
	RxFlag = 0;
	}

	if(dataReadyFlag){ //suspend once RxFlag is set //if(sendXYZoutFlag){
		USB_In_Buffer[0] = 0xFF;
		USB_In_Buffer[1] = 0xFF;
		if(T_outFlag)
		{
			SPI_Read(0x0000,&USB_In_Buffer[2], 8);
			putUSBUSART(USB_In_Buffer,10);
		} else {
			SPI_Read(0x0002,&USB_In_Buffer[2], 6);
			putUSBUSART(USB_In_Buffer,8);
		}
		dataReadyFlag = 0;
	}
}
/*******************************************************************/
void DEMO_D10(unsigned char Command, unsigned char * param){
	 unsigned char SlaveAddr;
	 unsigned long counter;
	 char cmdDataOutStrLen;
	 unsigned char tmp[5];
	 char i;
	 SlaveAddr = 0x30;
	 if (RxFlag == 1)
	 {
		 switch(Command)
		 {
		  case CMD_QUERY:
		  {
			  // deleted by York 2013_0102// LATD = param[0];
			   if(param[0] == 0){ //fw
			    strcpypgm2ram(USB_In_Buffer, ROM_STR_CODE_TYPE[2]); //return to user
			   }
			   else if(param[0] == 1){ //osc
			    mFreqT0cki((long*)&counter);
			    ltoa(counter, USB_In_Buffer);
			    strcatpgm2ram(USB_In_Buffer, ROM_STR_NL);
			    strcatpgm2ram(USB_In_Buffer, ROM_STR_CR);
			   }
			   else if(param[0] == 2){ //config
			    USB_In_Buffer[0] = '\0';
			    for(i = param[0] + 1; i < configure_num; i++){
			     strcatpgm2ram(USB_In_Buffer, ROM_STR_CR);
			     strcatpgm2ram(USB_In_Buffer, ROM_STR_CONFIGURE[i]);
			     strcatpgm2ram(USB_In_Buffer, ROM_STR_COMMA);
			     strcat(USB_In_Buffer, itoa(mConfig.v[i - param[0] - 1], (char*) tmp));
			    }
			   }
			   else{
			    itoa(mConfig.v[param[0] - 3], USB_In_Buffer);  //return to user
			   }
			   
			   cmdDataOutStrLen = strlen(USB_In_Buffer);
			   putUSBUSART(USB_In_Buffer, cmdDataOutStrLen);
		  }
		  break;
		  case CMD_STANDBY:
		  {
		   		G_SENSOR_CE_OFF;
		  }
		  case CMD_NORMAL:
		  {
		   		G_SENSOR_CE_ON;
		  }
		  break;
		  case CMD_SINGLE_R: //r,dd
		  {
			   I2C_Read(SlaveAddr,param[0], USB_In_Buffer, 1);
			   putUSBUSART(USB_In_Buffer,1);
		  }
		  break;
		  case CMD_MULTI_R: //r,dd,n
		  {
			   I2C_Read(SlaveAddr,param[0], USB_In_Buffer, param[1]);
			   putUSBUSART(USB_In_Buffer,param[1]);
		  }
		  break;
		  case CMD_CONT_XYZ: 
			   if(param[0] <= 0 || param[0] > 100){ //not support freq range
			    periodXYZoutFlag = 0; //turn off the periodic output flag
			   }
			   else{ //turn on the periodic output flag
			    periodXYZoutFlag = 1;
			     //xyz_output_time_count = 400/param[0]; //every count = 250us
			    xyz_output_time_count = 100/param[0]; //every count = 1ms     
			   }
		  break;
		  case CMD_SINGLE_W: //w,dd,nn,mm
		  {
			   I2C_Read(SlaveAddr,param[0], USB_In_Buffer, 1);
			   USB_In_Buffer[1] = (USB_In_Buffer[0] & ~param[2]) | (param[1] & param[2]);
			   I2C_Write(SlaveAddr,param[0],&USB_In_Buffer[1] , 1);
			   putUSBUSART(1, 1);
		  }
		  break;
		  case CMD_MULTI_OR: //0,ddd,n
		  {
			   G_SENSOR_OTP_ENABLE_ON;
			   i = 10;
			   while(i--);
			   I2C_Read(SlaveAddr,param[1], USB_In_Buffer, param[2]);
			   G_SENSOR_OTP_ENABLE_OFF;
			   putUSBUSART(USB_In_Buffer, param[2]);
		  }
		  break;
		  case CMD_INTERRUPT:
		  {
			   T_outFlag = 0;
			   switch (param[0])
			   {
			    case 3:
			    {
			     T_outFlag = 2;
			    }
			    case 1:
			    {
			     TRISBbits.TRISB1 = 1; 
			     INTCON2bits.INTEDG1 = 1;
			     INTCON3bits.INT1E = 1; 
			       monitorINT1Flag = 1;
			    }
			    break;
			    case 4:
			    {
			     T_outFlag = 2;
			    }
			    case 2:
			    {
			     TRISBbits.TRISB2 = 1;
			     INTCON2bits.INTEDG2 = 1;
			     INTCON3bits.INT2E = 1; 
			       monitorINT2Flag = 1;
			    }
			    break; 
			   }
		  }
		  break;
		  case CMD_SINGLE_OW: //ow,ddd,bb
		  {
			   G_SENSOR_OTP_ENABLE_ON;
			   i = 10;
			   while(i--);
			   I2C_Write(SlaveAddr, param[1], param[2], 1);
			   G_SENSOR_OTP_ENABLE_OFF;
		  }
		  break;
		  case CMD_CONT_XYZT: //r,xyzT
		  {
			   if(param[0] <= 0 || param[0] > 100){
			    periodXYZoutFlag = 0;
			   } else {
			    periodXYZoutFlag = 1;
			    xyz_output_time_count = 100/param[0]; 
			   }
			   T_outFlag = 1;
		  }
		  break;
		  case CMD_CONFIGURE:
		  {
			   if(param[0] == 4){//baud rate: special case to take care
			    mConfig.i2cBaudRate_kHz = param[1]*100L;
			   }
			   else{
			    mConfig.v[param[0] - 3] = param[1];
			   }
			   
			   if(param[0] == 3 || param[0] == 4 || param[0] == 13 || param[0] == 14 || param[0] == 15){ //sar, ibr, sspm, pr2, ifs configure
			    UserInit();
			   }
		  }
		  break;
		  case CMD_MULTI_W:
		  {
			   I2C_Write(SlaveAddr,param[0],&param[2] , param[1]);
			   putUSBUSART(&param[1], 1);
		   
		  }
		  break;
		  case CMD_CHANGE_DEVICE: //change device
		  {
			   ASIC_TYPE = 0x07;
			   LATD = ASIC_TYPE;
			   INTCON3bits.INT1E = 0;  
			   INTCON3bits.INT2E = 0;
			   monitorINT1Flag = 0;
			   monitorINT2Flag = 0;
			   dataReadyFlag = 0;
			   EraseUserFlash();
			   WriteUserFlash();
			   I2C_Init();
		  }
		  default:
		  break;
	 }
	 RxFlag = 0;
	 }
	 if(dataReadyFlag){ //suspend once RxFlag is set //if(sendXYZoutFlag){
	  INTCONbits.GIE = 0;
	  dataReadyFlag = 0;
	  if(T_outFlag == 0){ //only xyz
	   tmp[0] = mConfig.xyzDataStartAddr;
	   tmp[3] = 3*mConfig.txyzDataByteLen;
	   tmp[1] = tmp[3] + mConfig.xyzDataExtraByte;
	  }
	  else{//txyz
	   tmp[0] = mConfig.txyzDataStartAddr;
	   tmp[3] = 4*mConfig.txyzDataByteLen;
	   tmp[1] = tmp[3] + mConfig.txyzDataExtraByte;
	  }
	  I2C_Read(SlaveAddr,tmp[0], param, tmp[1]);
	       
	  if(mConfig.dataAlignment == 0){ //data last
	   if(T_outFlag == 0){ //xyz
	    tmp[2] = mConfig.xyzDataExtraByte;
	   } 
	   else{ //txyz
	    tmp[2] = mConfig.txyzDataExtraByte; 
	   }
	  }
	  else if(mConfig.dataAlignment == 0xFF){ //data first
	   tmp[2] = 0;
	  }
	  else{
	   tmp[2] = mConfig.dataAlignment;
	  }
	     
	  if(mConfig.txyzDataByteLen == 2 && mConfig.dataEndianness == LITTLE_ENDIAN){ //output as the big endian format (MSByte first)
	   for(i = 0; i < tmp[3]; i+=2){
	    USB_In_Buffer[i + 2] = param[i + tmp[2] + 1];
	    USB_In_Buffer[i + 2 + 1] = param[i + tmp[2]];
	   }    
	  }
	  else{ //data format match output format of the big endian (MSByte first)
	   for(i = 0; i < tmp[3]; i++){
	    USB_In_Buffer[i + 2] = param[i + tmp[2]];
	    }
	  }
	   
	  cmdDataOutStrLen = tmp[3] + 2;
	   
	  if(T_outFlag && mConfig.tFirst == 0){ //The output should be TXYZ, but the data is XYZT, extra effort here
	      
	    //shift right by mConfig.txyzDataByteLen
	   for(i = 0; i < cmdDataOutStrLen; i++){
	    USB_In_Buffer[cmdDataOutStrLen - 1 - i + mConfig.txyzDataByteLen] = USB_In_Buffer[cmdDataOutStrLen - 1 - i];
	   }
	   
	    //shift T to front
	   for(i = 0; i < mConfig.txyzDataByteLen; i++){
	    USB_In_Buffer[2 + i] = USB_In_Buffer[cmdDataOutStrLen + i];
	   }
	  }
	   
	  //The first two bytes always be 0xFF
	  USB_In_Buffer[0] = '\xFF';
	  USB_In_Buffer[1] = '\xFF';
	   
	  INTCONbits.GIE = 1;  // global interrupt disable
	  putUSBUSART(USB_In_Buffer, cmdDataOutStrLen);
	 }
}
/********************************************************************/
void mFreqT0cki(long* valPtr){

	//Start the counter
	WriteTimer0(0);
	OpenTimer0(TIMER_INT_OFF & T0_16BIT & T0_SOURCE_EXT & T0_EDGE_RISE & T0_PS_1_1);
	//OpenTimer0(TIMER_INT_OFF & T0_16BIT & T0_SOURCE_EXT & T0_EDGE_RISE & T0_PS_1_16);
	//OpenTimer0(TIMER_INT_OFF & T0_16BIT & T0_SOURCE_EXT & T0_EDGE_RISE & T0_PS_1_64);
	
	
	//delay 200ms
	mDelay_10ms(20);
	//mDelay_10ms(1);
	
	//stop the counter first
	CloseTimer0();
	
	//read the counter
	*valPtr = (5L*ReadTimer0());
	//*valPtr = (64*5L*ReadTimer0());
	//*valPtr = (ReadTimer0());
}
/********************************************************************
 * Function:        void BlinkUSBStatus(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        BlinkUSBStatus turns on and off LEDs 
 *                  corresponding to the USB device state.
 *
 * Note:            mLED macros can be found in HardwareProfile.h
 *                  USBDeviceState is declared and updated in
 *                  usb_device.c.
 *******************************************************************/
void BlinkUSBStatus(void)
{
    static WORD led_count=0;
    
    if(led_count == 0)led_count = 10000U;
    led_count--;

    #define mLED_Both_Off()         {mLED_7_Off();mLED_8_Off();}
    #define mLED_Both_On()          {mLED_7_On();mLED_8_On();}
    #define mLED_Only_7_On()        {mLED_7_On();mLED_8_Off();}
    #define mLED_Only_8_On()        {mLED_7_Off();mLED_8_On();}

    if(USBSuspendControl == 1)
    {
        if(led_count==0)
        {
            mLED_7_Toggle();
            if(mGetLED_7())
            {
                mLED_8_On();
            }
            else
            {
                mLED_8_Off();
            }
        }//end if
    }
    else
    {
        if(USBDeviceState == DETACHED_STATE)
        {
            mLED_Both_Off();
        }
        else if(USBDeviceState == ATTACHED_STATE)
        {
            mLED_Both_On();
        }
        else if(USBDeviceState == POWERED_STATE)
        {
            mLED_Only_7_On();
        }
        else if(USBDeviceState == DEFAULT_STATE)
        {
            mLED_Only_8_On();
        }
        else if(USBDeviceState == ADDRESS_STATE)
        {
            if(led_count == 0)
            {
                mLED_7_Toggle();
                mLED_8_Off();
            }//end if
        }
        else if(USBDeviceState == CONFIGURED_STATE)
        {
            if(led_count==0)
            {
                mLED_7_Toggle();
                if(mGetLED_7())
                {
                    mLED_8_Off();
                }
                else
                {
                    mLED_8_On();
                }
            }//end if
        }//end if(...)
    }//end if(UCONbits.SUSPND...)

}//end BlinkUSBStatus

/********************************************************************
 * Function:        static void InitializeSystem(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        InitializeSystem is a centralize initialization
 *                  routine. All required USB initialization routines
 *                  are called from here.
 *
 *                  User application initialization routine should
 *                  also be called from here.                  
 *
 * Note:            None
 *******************************************************************/
static void InitializeSystem(void)
{
#if defined(_PIC14E)
        ANSELA = 0x00;
        ANSELB = 0x00;
        ANSELC = 0x00;
        TRISA  = 0x00;
        TRISB  = 0x00;
        TRISC  = 0x00;
        OSCTUNE = 0;
#if defined (USE_INTERNAL_OSC)
        OSCCON = 0x7C;   // PLL enabled, 3x, 16MHz internal osc, SCS external
        OSCCONbits.SPLLMULT = 1;   // 1=3x, 0=4x
        ACTCON = 0x90;   // Clock recovery on, Clock Recovery enabled; SOF packet
#else
        OSCCON = 0x3C;   // PLL enabled, 3x, 16MHz internal osc, SCS external
        OSCCONbits.SPLLMULT = 0;   // 1=3x, 0=4x
        ACTCON = 0x00;   // Clock recovery off, Clock Recovery enabled; SOF packet
#endif
#endif
    #if (defined(__18CXX) & !defined(PIC18F87J50_PIM) & !defined(PIC18F97J94_FAMILY))
        ADCON1 |= 0x0F;                 // Default all pins to digital
    #elif defined(__C30__) || defined __XC16__
    	#if defined(__PIC24FJ256DA210__) || defined(__PIC24FJ256GB210__)
    		ANSA = 0x0000;
    		ANSB = 0x0000;
    		ANSC = 0x0000;
    		ANSD = 0x0000;
    		ANSE = 0x0000;
    		ANSF = 0x0000;
    		ANSG = 0x0000;
        #elif defined(__dsPIC33EP512MU810__) || defined (__PIC24EP512GU810__)
        	ANSELA = 0x0000;
    		ANSELB = 0x0000;
    		ANSELC = 0x0000;
    		ANSELD = 0x0000;
    		ANSELE = 0x0000;
    		ANSELG = 0x0000;
            
            // The dsPIC33EP512MU810 features Peripheral Pin
            // select. The following statements map UART2 to 
            // device pins which would connect to the the 
            // RX232 transciever on the Explorer 16 board.

             RPINR19 = 0;
             RPINR19 = 0x64;
             RPOR9bits.RP101R = 0x3;

        #else
            AD1PCFGL = 0xFFFF;
        #endif
    #elif defined(__C32__)
        AD1PCFG = 0xFFFF;
    #endif

    #if defined(PIC18F87J50_PIM) || defined(PIC18F46J50_PIM) || defined(PIC18F_STARTER_KIT_1) || defined(PIC18F47J53_PIM)
	//On the PIC18F87J50 Family of USB microcontrollers, the PLL will not power up and be enabled
	//by default, even if a PLL enabled oscillator configuration is selected (such as HS+PLL).
	//This allows the device to power up at a lower initial operating frequency, which can be
	//advantageous when powered from a source which is not gauranteed to be adequate for 48MHz
	//operation.  On these devices, user firmware needs to manually set the OSCTUNE<PLLEN> bit to
	//power up the PLL.
    {
        unsigned int pll_startup_counter = 600;
        OSCTUNEbits.PLLEN = 1;  //Enable the PLL and wait 2+ms until the PLL locks before enabling USB module
        while(pll_startup_counter--);
    }
    //Device switches over automatically to PLL output after PLL is locked and ready.
    #endif

    #if defined(PIC18F97J94_FAMILY)
        //Configure I/O pins for digital input mode.
        ANCON1 = 0xFF;
        ANCON2 = 0xFF;
        ANCON3 = 0xFF;
        #if(USB_SPEED_OPTION == USB_FULL_SPEED)
            //Enable INTOSC active clock tuning if full speed
            ACTCON = 0x90; //Enable active clock self tuning for USB operation
            while(OSCCON2bits.LOCK == 0);   //Make sure PLL is locked/frequency is compatible
                                            //with USB operation (ex: if using two speed 
                                            //startup or otherwise performing clock switching)
        #endif
    #endif
    
    #if defined(PIC18F45K50_FAMILY)
        //Configure oscillator settings for clock settings compatible with USB 
        //operation.  Note: Proper settings depends on USB speed (full or low).
        #if(USB_SPEED_OPTION == USB_FULL_SPEED)
            OSCTUNE = 0x80; //3X PLL ratio mode selected
            OSCCON = 0x70;  //Switch to 16MHz HFINTOSC
            OSCCON2 = 0x10; //Enable PLL, SOSC, PRI OSC drivers turned off
            while(OSCCON2bits.PLLRDY != 1);   //Wait for PLL lock
            *((unsigned char*)0xFB5) = 0x90;  //Enable active clock tuning for USB operation
        #endif
        //Configure all I/O pins for digital mode (except RA0/AN0 which has POT on demo board)
        ANSELA = 0x01;
        ANSELB = 0x00;
        ANSELC = 0x00;
        ANSELD = 0x00;
        ANSELE = 0x00;
    #endif
    
    #if defined(__32MX460F512L__)|| defined(__32MX795F512L__)
    // Configure the PIC32 core for the best performance
    // at the operating frequency. The operating frequency is already set to 
    // 60MHz through Device Config Registers
    SYSTEMConfigPerformance(60000000);
	#endif

    #if defined(__dsPIC33EP512MU810__) || defined (__PIC24EP512GU810__)

    // Configure the device PLL to obtain 60 MIPS operation. The crystal
    // frequency is 8MHz. Divide 8MHz by 2, multiply by 60 and divide by
    // 2. This results in Fosc of 120MHz. The CPU clock frequency is
    // Fcy = Fosc/2 = 60MHz. Wait for the Primary PLL to lock and then
    // configure the auxilliary PLL to provide 48MHz needed for USB 
    // Operation.

	PLLFBD = 38;				/* M  = 60	*/
	CLKDIVbits.PLLPOST = 0;		/* N1 = 2	*/
	CLKDIVbits.PLLPRE = 0;		/* N2 = 2	*/
	OSCTUN = 0;			

    /*	Initiate Clock Switch to Primary
     *	Oscillator with PLL (NOSC= 0x3)*/
	
    __builtin_write_OSCCONH(0x03);		
	__builtin_write_OSCCONL(0x01);
	
	
	while (OSCCONbits.COSC != 0x3);       

    // Configuring the auxiliary PLL, since the primary
    // oscillator provides the source clock to the auxiliary
    // PLL, the auxiliary oscillator is disabled. Note that
    // the AUX PLL is enabled. The input 8MHz clock is divided
    // by 2, multiplied by 24 and then divided by 2. Wait till 
    // the AUX PLL locks.

    ACLKCON3 = 0x24C1;   
    ACLKDIV3 = 0x7;
    
    
    ACLKCON3bits.ENAPLL = 1;
    while(ACLKCON3bits.APLLCK != 1); 

    #endif

    #if defined(PIC18F87J50_PIM)
	//Configure all I/O pins to use digital input buffers.  The PIC18F87J50 Family devices
	//use the ANCONx registers to control this, which is different from other devices which
	//use the ADCON1 register for this purpose.
    WDTCONbits.ADSHR = 1;			// Select alternate SFR location to access ANCONx registers
    ANCON0 = 0xFF;                  // Default all pins to digital
    ANCON1 = 0xFF;                  // Default all pins to digital
    WDTCONbits.ADSHR = 0;			// Select normal SFR locations
    #endif

    #if defined(PIC18F46J50_PIM) || defined(PIC18F_STARTER_KIT_1) || defined(PIC18F47J53_PIM)
	//Configure all I/O pins to use digital input buffers.  The PIC18F87J50 Family devices
	//use the ANCONx registers to control this, which is different from other devices which
	//use the ADCON1 register for this purpose.
    ANCON0 = 0x7F;                  // All pins to digital (except AN7: temp sensor)
    ANCON1 = 0xBF;                  // Default all pins to digital.  Bandgap on.

    #endif
    
   #if defined(PIC24FJ64GB004_PIM) || defined(PIC24FJ256DA210_DEV_BOARD)
	//On the PIC24FJ64GB004 Family of USB microcontrollers, the PLL will not power up and be enabled
	//by default, even if a PLL enabled oscillator configuration is selected (such as HS+PLL).
	//This allows the device to power up at a lower initial operating frequency, which can be
	//advantageous when powered from a source which is not gauranteed to be adequate for 32MHz
	//operation.  On these devices, user firmware needs to manually set the CLKDIV<PLLEN> bit to
	//power up the PLL.
    {
        unsigned int pll_startup_counter = 600;
        CLKDIVbits.PLLEN = 1;
        while(pll_startup_counter--);
    }

    //Device switches over automatically to PLL output after PLL is locked and ready.
    #endif


//	The USB specifications require that USB peripheral devices must never source
//	current onto the Vbus pin.  Additionally, USB peripherals should not source
//	current on D+ or D- when the host/hub is not actively powering the Vbus line.
//	When designing a self powered (as opposed to bus powered) USB peripheral
//	device, the firmware should make sure not to turn on the USB module and D+
//	or D- pull up resistor unless Vbus is actively powered.  Therefore, the
//	firmware needs some means to detect when Vbus is being powered by the host.
//	A 5V tolerant I/O pin can be connected to Vbus (through a resistor), and
// 	can be used to detect when Vbus is high (host actively powering), or low
//	(host is shut down or otherwise not supplying power).  The USB firmware
// 	can then periodically poll this I/O pin to know when it is okay to turn on
//	the USB module/D+/D- pull up resistor.  When designing a purely bus powered
//	peripheral device, it is not possible to source current on D+ or D- when the
//	host is not actively providing power on Vbus. Therefore, implementing this
//	bus sense feature is optional.  This firmware can be made to use this bus
//	sense feature by making sure "USE_USB_BUS_SENSE_IO" has been defined in the
//	HardwareProfile.h file.    
    #if defined(USE_USB_BUS_SENSE_IO)
    tris_usb_bus_sense = INPUT_PIN; // See HardwareProfile.h
    #endif
    
//	If the host PC sends a GetStatus (device) request, the firmware must respond
//	and let the host know if the USB peripheral device is currently bus powered
//	or self powered.  See chapter 9 in the official USB specifications for details
//	regarding this request.  If the peripheral device is capable of being both
//	self and bus powered, it should not return a hard coded value for this request.
//	Instead, firmware should check if it is currently self or bus powered, and
//	respond accordingly.  If the hardware has been configured like demonstrated
//	on the PICDEM FS USB Demo Board, an I/O pin can be polled to determine the
//	currently selected power source.  On the PICDEM FS USB Demo Board, "RA2" 
//	is used for	this purpose.  If using this feature, make sure "USE_SELF_POWER_SENSE_IO"
//	has been defined in HardwareProfile - (platform).h, and that an appropriate I/O pin 
//  has been mapped	to it.
    #if defined(USE_SELF_POWER_SENSE_IO)
    tris_self_power = INPUT_PIN;	// See HardwareProfile.h
    #endif

   
    UserInit();

    USBDeviceInit();	//usb_device.c.  Initializes USB module SFRs and firmware
    					//variables to known states.
}//end InitializeSystem



/******************************************************************************
 * Function:        void UserInit(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This routine should take care of all of the demo code
 *                  initialization that is required.
 *
 * Note:            
 *
 *****************************************************************************/
void UserInit(void)
{
	MCU_init();
	if (mConfig.ifs)
	{
		I2C_Init();
		SSP1ADD = mFOSC_MHZ*250/(mConfig.i2cBaudRate_kHz) - 1; 
	} else {
		SPI_Init();
		SSP1CON1bits.SSPM = mConfig.sspm;
	}
	//Timer 2 output
	PR2 = mConfig.pr2;
	TMR2 = 0;
	OpenTimer2(TIMER_INT_OFF & T2_PS_1_1); //output freq: (48/4)MHz / (PR2 + 1) =>(2,4), (3, 3)
	
	//Timer 4
	PR4 = 187;
	TMR4 = 0;
	OpenTimer4(TIMER_INT_ON & T4_PS_1_4 & T4_POST_1_16); //tick every 1ms

}//end UserInit



// ******************************************************************************************************
// ************** USB Callback Functions ****************************************************************
// ******************************************************************************************************
// The USB firmware stack will call the callback functions USBCBxxx() in response to certain USB related
// events.  For example, if the host PC is powering down, it will stop sending out Start of Frame (SOF)
// packets to your device.  In response to this, all USB devices are supposed to decrease their power
// consumption from the USB Vbus to <2.5mA* each.  The USB module detects this condition (which according
// to the USB specifications is 3+ms of no bus activity/SOF packets) and then calls the USBCBSuspend()
// function.  You should modify these callback functions to take appropriate actions for each of these
// conditions.  For example, in the USBCBSuspend(), you may wish to add code that will decrease power
// consumption from Vbus to <2.5mA (such as by clock switching, turning off LEDs, putting the
// microcontroller to sleep, etc.).  Then, in the USBCBWakeFromSuspend() function, you may then wish to
// add code that undoes the power saving things done in the USBCBSuspend() function.

// The USBCBSendResume() function is special, in that the USB stack will not automatically call this
// function.  This function is meant to be called from the application firmware instead.  See the
// additional comments near the function.

// Note *: The "usb_20.pdf" specs indicate 500uA or 2.5mA, depending upon device classification. However,
// the USB-IF has officially issued an ECN (engineering change notice) changing this to 2.5mA for all 
// devices.  Make sure to re-download the latest specifications to get all of the newest ECNs.

/******************************************************************************
 * Function:        void USBCBSuspend(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Call back that is invoked when a USB suspend is detected
 *
 * Note:            None
 *****************************************************************************/
void USBCBSuspend(void)
{
	//Example power saving code.  Insert appropriate code here for the desired
	//application behavior.  If the microcontroller will be put to sleep, a
	//process similar to that shown below may be used:
	
	//ConfigureIOPinsForLowPower();
	//SaveStateOfAllInterruptEnableBits();
	//DisableAllInterruptEnableBits();
	//EnableOnlyTheInterruptsWhichWillBeUsedToWakeTheMicro();	//should enable at least USBActivityIF as a wake source
	//Sleep();
	//RestoreStateOfAllPreviouslySavedInterruptEnableBits();	//Preferrably, this should be done in the USBCBWakeFromSuspend() function instead.
	//RestoreIOPinsToNormal();									//Preferrably, this should be done in the USBCBWakeFromSuspend() function instead.

	//IMPORTANT NOTE: Do not clear the USBActivityIF (ACTVIF) bit here.  This bit is 
	//cleared inside the usb_device.c file.  Clearing USBActivityIF here will cause 
	//things to not work as intended.	
	

    #if defined(__C30__) || defined __XC16__
        USBSleepOnSuspend();
    #endif
}

/******************************************************************************
 * Function:        void USBCBWakeFromSuspend(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        The host may put USB peripheral devices in low power
 *					suspend mode (by "sending" 3+ms of idle).  Once in suspend
 *					mode, the host may wake the device back up by sending non-
 *					idle state signalling.
 *					
 *					This call back is invoked when a wakeup from USB suspend 
 *					is detected.
 *
 * Note:            None
 *****************************************************************************/
void USBCBWakeFromSuspend(void)
{
	// If clock switching or other power savings measures were taken when
	// executing the USBCBSuspend() function, now would be a good time to
	// switch back to normal full power run mode conditions.  The host allows
	// 10+ milliseconds of wakeup time, after which the device must be 
	// fully back to normal, and capable of receiving and processing USB
	// packets.  In order to do this, the USB module must receive proper
	// clocking (IE: 48MHz clock must be available to SIE for full speed USB
	// operation).  
	// Make sure the selected oscillator settings are consistent with USB 
    // operation before returning from this function.
}

/********************************************************************
 * Function:        void USBCB_SOF_Handler(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        The USB host sends out a SOF packet to full-speed
 *                  devices every 1 ms. This interrupt may be useful
 *                  for isochronous pipes. End designers should
 *                  implement callback routine as necessary.
 *
 * Note:            None
 *******************************************************************/
void USBCB_SOF_Handler(void)
{
    // No need to clear UIRbits.SOFIF to 0 here.
    // Callback caller is already doing that.

    //This is reverse logic since the pushbutton is active low
    if(buttonPressed == sw2)
    {
        if(buttonCount != 0)
        {
            buttonCount--;
        }
        else
        {
            //This is reverse logic since the pushbutton is active low
            buttonPressed = !sw2;

            //Wait 100ms before the next press can be generated
            buttonCount = 100;
        }
    }
    else
    {
        if(buttonCount != 0)
        {
            buttonCount--;
        }
    }
}

/*******************************************************************
 * Function:        void USBCBErrorHandler(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        The purpose of this callback is mainly for
 *                  debugging during development. Check UEIR to see
 *                  which error causes the interrupt.
 *
 * Note:            None
 *******************************************************************/
void USBCBErrorHandler(void)
{
    // No need to clear UEIR to 0 here.
    // Callback caller is already doing that.

	// Typically, user firmware does not need to do anything special
	// if a USB error occurs.  For example, if the host sends an OUT
	// packet to your device, but the packet gets corrupted (ex:
	// because of a bad connection, or the user unplugs the
	// USB cable during the transmission) this will typically set
	// one or more USB error interrupt flags.  Nothing specific
	// needs to be done however, since the SIE will automatically
	// send a "NAK" packet to the host.  In response to this, the
	// host will normally retry to send the packet again, and no
	// data loss occurs.  The system will typically recover
	// automatically, without the need for application firmware
	// intervention.
	
	// Nevertheless, this callback function is provided, such as
	// for debugging purposes.
}


/*******************************************************************
 * Function:        void USBCBCheckOtherReq(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        When SETUP packets arrive from the host, some
 * 					firmware must process the request and respond
 *					appropriately to fulfill the request.  Some of
 *					the SETUP packets will be for standard
 *					USB "chapter 9" (as in, fulfilling chapter 9 of
 *					the official USB specifications) requests, while
 *					others may be specific to the USB device class
 *					that is being implemented.  For example, a HID
 *					class device needs to be able to respond to
 *					"GET REPORT" type of requests.  This
 *					is not a standard USB chapter 9 request, and 
 *					therefore not handled by usb_device.c.  Instead
 *					this request should be handled by class specific 
 *					firmware, such as that contained in usb_function_hid.c.
 *
 * Note:            None
 *******************************************************************/
void USBCBCheckOtherReq(void)
{
    USBCheckCDCRequest();
}//end


/*******************************************************************
 * Function:        void USBCBStdSetDscHandler(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        The USBCBStdSetDscHandler() callback function is
 *					called when a SETUP, bRequest: SET_DESCRIPTOR request
 *					arrives.  Typically SET_DESCRIPTOR requests are
 *					not used in most applications, and it is
 *					optional to support this type of request.
 *
 * Note:            None
 *******************************************************************/
void USBCBStdSetDscHandler(void)
{
    // Must claim session ownership if supporting this request
}//end


/*******************************************************************
 * Function:        void USBCBInitEP(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This function is called when the device becomes
 *                  initialized, which occurs after the host sends a
 * 					SET_CONFIGURATION (wValue not = 0) request.  This 
 *					callback function should initialize the endpoints 
 *					for the device's usage according to the current 
 *					configuration.
 *
 * Note:            None
 *******************************************************************/
void USBCBInitEP(void)
{
    //Enable the CDC data endpoints
    CDCInitEP();
}

/********************************************************************
 * Function:        void USBCBSendResume(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        The USB specifications allow some types of USB
 * 					peripheral devices to wake up a host PC (such
 *					as if it is in a low power suspend to RAM state).
 *					This can be a very useful feature in some
 *					USB applications, such as an Infrared remote
 *					control	receiver.  If a user presses the "power"
 *					button on a remote control, it is nice that the
 *					IR receiver can detect this signalling, and then
 *					send a USB "command" to the PC to wake up.
 *					
 *					The USBCBSendResume() "callback" function is used
 *					to send this special USB signalling which wakes 
 *					up the PC.  This function may be called by
 *					application firmware to wake up the PC.  This
 *					function will only be able to wake up the host if
 *                  all of the below are true:
 *					
 *					1.  The USB driver used on the host PC supports
 *						the remote wakeup capability.
 *					2.  The USB configuration descriptor indicates
 *						the device is remote wakeup capable in the
 *						bmAttributes field.
 *					3.  The USB host PC is currently sleeping,
 *						and has previously sent your device a SET 
 *						FEATURE setup packet which "armed" the
 *						remote wakeup capability.   
 *
 *                  If the host has not armed the device to perform remote wakeup,
 *                  then this function will return without actually performing a
 *                  remote wakeup sequence.  This is the required behavior, 
 *                  as a USB device that has not been armed to perform remote 
 *                  wakeup must not drive remote wakeup signalling onto the bus;
 *                  doing so will cause USB compliance testing failure.
 *                  
 *					This callback should send a RESUME signal that
 *                  has the period of 1-15ms.
 *
 * Note:            This function does nothing and returns quickly, if the USB
 *                  bus and host are not in a suspended condition, or are 
 *                  otherwise not in a remote wakeup ready state.  Therefore, it
 *                  is safe to optionally call this function regularly, ex: 
 *                  anytime application stimulus occurs, as the function will
 *                  have no effect, until the bus really is in a state ready
 *                  to accept remote wakeup. 
 *
 *                  When this function executes, it may perform clock switching,
 *                  depending upon the application specific code in 
 *                  USBCBWakeFromSuspend().  This is needed, since the USB
 *                  bus will no longer be suspended by the time this function
 *                  returns.  Therefore, the USB module will need to be ready
 *                  to receive traffic from the host.
 *
 *                  The modifiable section in this routine may be changed
 *                  to meet the application needs. Current implementation
 *                  temporary blocks other functions from executing for a
 *                  period of ~3-15 ms depending on the core frequency.
 *
 *                  According to USB 2.0 specification section 7.1.7.7,
 *                  "The remote wakeup device must hold the resume signaling
 *                  for at least 1 ms but for no more than 15 ms."
 *                  The idea here is to use a delay counter loop, using a
 *                  common value that would work over a wide range of core
 *                  frequencies.
 *                  That value selected is 1800. See table below:
 *                  ==========================================================
 *                  Core Freq(MHz)      MIP         RESUME Signal Period (ms)
 *                  ==========================================================
 *                      48              12          1.05
 *                       4              1           12.6
 *                  ==========================================================
 *                  * These timing could be incorrect when using code
 *                    optimization or extended instruction mode,
 *                    or when having other interrupts enabled.
 *                    Make sure to verify using the MPLAB SIM's Stopwatch
 *                    and verify the actual signal on an oscilloscope.
 *******************************************************************/
void USBCBSendResume(void)
{
    static WORD delay_count;
    
    //First verify that the host has armed us to perform remote wakeup.
    //It does this by sending a SET_FEATURE request to enable remote wakeup,
    //usually just before the host goes to standby mode (note: it will only
    //send this SET_FEATURE request if the configuration descriptor declares
    //the device as remote wakeup capable, AND, if the feature is enabled
    //on the host (ex: on Windows based hosts, in the device manager 
    //properties page for the USB device, power management tab, the 
    //"Allow this device to bring the computer out of standby." checkbox 
    //should be checked).
    if(USBGetRemoteWakeupStatus() == TRUE) 
    {
        //Verify that the USB bus is in fact suspended, before we send
        //remote wakeup signalling.
        if(USBIsBusSuspended() == TRUE)
        {
            USBMaskInterrupts();
            
            //Clock switch to settings consistent with normal USB operation.
            USBCBWakeFromSuspend();
            USBSuspendControl = 0; 
            USBBusIsSuspended = FALSE;  //So we don't execute this code again, 
                                        //until a new suspend condition is detected.

            //Section 7.1.7.7 of the USB 2.0 specifications indicates a USB
            //device must continuously see 5ms+ of idle on the bus, before it sends
            //remote wakeup signalling.  One way to be certain that this parameter
            //gets met, is to add a 2ms+ blocking delay here (2ms plus at 
            //least 3ms from bus idle to USBIsBusSuspended() == TRUE, yeilds
            //5ms+ total delay since start of idle).
            delay_count = 3600U;        
            do
            {
                delay_count--;
            }while(delay_count);
            
            //Now drive the resume K-state signalling onto the USB bus.
            USBResumeControl = 1;       // Start RESUME signaling
            delay_count = 1800U;        // Set RESUME line for 1-13 ms
            do
            {
                delay_count--;
            }while(delay_count);
            USBResumeControl = 0;       //Finished driving resume signalling

            USBUnmaskInterrupts();
        }
    }
}


/*******************************************************************
 * Function:        void USBCBEP0DataReceived(void)
 *
 * PreCondition:    ENABLE_EP0_DATA_RECEIVED_CALLBACK must be
 *                  defined already (in usb_config.h)
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This function is called whenever a EP0 data
 *                  packet is received.  This gives the user (and
 *                  thus the various class examples a way to get
 *                  data that is received via the control endpoint.
 *                  This function needs to be used in conjunction
 *                  with the USBCBCheckOtherReq() function since 
 *                  the USBCBCheckOtherReq() function is the apps
 *                  method for getting the initial control transfer
 *                  before the data arrives.
 *
 * Note:            None
 *******************************************************************/
#if defined(ENABLE_EP0_DATA_RECEIVED_CALLBACK)
void USBCBEP0DataReceived(void)
{
}
#endif

/*******************************************************************
 * Function:        BOOL USER_USB_CALLBACK_EVENT_HANDLER(
 *                        int event, void *pdata, WORD size)
 *
 * PreCondition:    None
 *
 * Input:           int event - the type of event
 *                  void *pdata - pointer to the event data
 *                  WORD size - size of the event data
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This function is called from the USB stack to
 *                  notify a user application that a USB event
 *                  occured.  This callback is in interrupt context
 *                  when the USB_INTERRUPT option is selected.
 *
 * Note:            None
 *******************************************************************/
BOOL USER_USB_CALLBACK_EVENT_HANDLER(int event, void *pdata, WORD size)
{
    switch( event )
    {
        case EVENT_TRANSFER:
            //Add application specific callback task or callback function here if desired.
            break;
        case EVENT_SOF:
            USBCB_SOF_Handler();
            break;
        case EVENT_SUSPEND:
            USBCBSuspend();
            break;
        case EVENT_RESUME:
            USBCBWakeFromSuspend();
            break;
        case EVENT_CONFIGURED: 
            USBCBInitEP();
            break;
        case EVENT_SET_DESCRIPTOR:
            USBCBStdSetDscHandler();
            break;
        case EVENT_EP0_REQUEST:
            USBCBCheckOtherReq();
            break;
        case EVENT_BUS_ERROR:
            USBCBErrorHandler();
            break;
        case EVENT_TRANSFER_TERMINATED:
            //Add application specific callback task or callback function here if desired.
            //The EVENT_TRANSFER_TERMINATED event occurs when the host performs a CLEAR
            //FEATURE (endpoint halt) request on an application endpoint which was 
            //previously armed (UOWN was = 1).  Here would be a good place to:
            //1.  Determine which endpoint the transaction that just got terminated was 
            //      on, by checking the handle value in the *pdata.
            //2.  Re-arm the endpoint if desired (typically would be the case for OUT 
            //      endpoints).
            break;
        default:
            break;
    }      
    return TRUE; 
}

/*******************************************************************
 * Function:        Flash memory erase, write, and read.
 *         
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    0x01F800 - 0x01FBFF
 *
 * Overview:        
 *
 * Note:            2013_0102 added by York
 *******************************************************************/
void EraseUserFlash(void) // To erase flash address from 0x01F800 - 0x1FBFF.
{
//Really want this: TBLPTR = ((unsigned short long)ErasePageTracker << 10); but compiler not very efficient at this, so instead do this:
	TBLPTRL = 0x00;
	TBLPTRH = 0xF8;
	TBLPTRU = 0x01;
	EECON1 = 0b00010100;
	INTCONbits.GIE = 0;	//Make certain interrupts disabled for unlock process.
	_asm
	MOVLW 0x55
	MOVWF EECON2, 0
	MOVLW 0xAA
	MOVWF EECON2, 0
	BSF EECON1, 1, 0
	_endasm

	EECON1bits.WREN = 0;  //Good practice now to clear the WREN bit, as further protection against any future accidental activation of self write/erase operations.
	INTCONbits.GIE = 1;
	ClrWdt();
}	

void WriteUserFlash(void) // To write 2 bytes only to flash address 0x01F800
{
_asm
		MOVLW 0x01
		MOVWF TBLPTRU, 0
		MOVLW 0xF8
		MOVWF TBLPTRH, 0
		MOVLW 0x00
		MOVWF TBLPTRL, 0
_endasm
		TABLAT = ASIC_TYPE;
_asm
		TBLWTPOSTINC
		MOVLW 0x00
		MOVWF TABLAT, 0
		TBLWT
program_flash:
		BSF EECON1, 5, 0
		BSF EECON1, 2, 0
		BCF INTCON, 7, 0
		MOVLW 0x55
		MOVWF EECON2, 0
		MOVLW 0xAA
		MOVWF EECON2, 0
		BSF EECON1, 1, 0	
		BSF INTCON, 7, 0
		BCF EECON1, 2, 0
		BCF EECON1, 5, 0
_endasm
		ClrWdt();
}

void ReadAsicType(void) // To read 2 bytes only from flash address 0x01F800
{
		short OriginalTBLPTR;
		OriginalTBLPTR = TBLPTR; //To save the TBLPTR before read flash by unknow reason since it will cause USB init fail.
		TBLPTR = 0x1f800;
_asm
		tblrd
_endasm
		TBLPTR = OriginalTBLPTR;
		ASIC_TYPE = TABLAT;
}
/*******************************************************************
 * Function:        Push button #2 check.
 *         
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          sw2_long_pressed_flag, sw2_short_pressed_flag
 *
 * Side Effects:    None
 *
 * Overview:        
 *
 * Note:            The sw2_long_pressed_flag and sw2_short_pressed_flag will be set to "1" once the sw2 is pressed over 100ms.
 *                  if sw2 is pressed over 100ms and less than 1000ms then sw2_short_pressed_flag = 1
 *                  if sw2 is pressed over 1000ms then sw2_long_pressed_flag = 1
 *                  The function uses Timer4 for 1ms counter.
 *                  2013_0114 added by York
 *******************************************************************/
void sw2_push_check(void)
{
	if (!sw2)   //sw2_counter = sw2_counter+sw2_pressed_flag every 1ms
	{
		if (sw2_counter >= 1000) 
		{
		sw2_long_pressed_flag = 1;
		sw2_pressed_flag=0;//To stop count up sw2_counter.
		}
		else 
		sw2_pressed_flag=1;//To count up sw2_counter.
	}
	else 
	{
		if (sw2_counter>=100 && sw2_counter <1000)sw2_short_pressed_flag = 1;
		sw2_counter=0;
		sw2_pressed_flag=0;
	}
}
/*******************************************************************
 * Function:        Push button #2 function.
 *         
 *
 * PreCondition:    None
 *
 * Input:           sw2_long_pressed_flag, sw2_short_pressed_flag
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        
 *
 * Note:            ASIC type will be changed if sw2 is pressed by 1000ms.
 *                  After release sw2 button from 1000ms pressed then short 
 *                  press sw2 to switch the ASIC type in 5000ms.
 *                  2013_0115 added by York
 *******************************************************************/
void sw2_function(void)
{
static WORD sw2_function_count=0;
static char sw2_function_flag=0;
static char asic_type_changed_flag=0;
sw2_push_check();
	if (sw2_long_pressed_flag) //To switch the ASIC TYPE with sw2 short pressed.
	{
		if (sw2_function_count==0)//Blink the LED of ASIC TYPE.
		{
		sw2_function_count=5000U;
		sw2_function_flag=!sw2_function_flag;
			if (sw2_function_flag) LATD=ASIC_TYPE;
			else LATD=0x00;
		}
		sw2_function_count--;//Blink LED of ASIC here.
		if (sw2_long_pressed_count >= 5000) // After 5000ms then finish the ASIC TYPE switch function.
		{
		sw2_long_pressed_flag = 0;
		sw2_long_pressed_count = 0;
			if (asic_type_changed_flag)//If ASIC TYPE has been changed then update flash memory.
			{
			EraseUserFlash();
			WriteUserFlash();
			}
		LATD=ASIC_TYPE;
		}
	}	
	if (sw2_short_pressed_flag) 
	{
		if (sw2_long_pressed_flag)//ASIC TYPE changes here if sw2 has been long pressed.
		{
		sw2_long_pressed_count=0;
		switch(ASIC_TYPE)
			{
			case 0x07:
			ASIC_TYPE = 0x08;
			INTCON3bits.INT1E = 0;  
			INTCON3bits.INT2E = 0;
	  		monitorINT1Flag = 0;
	  		monitorINT2Flag = 0;
			dataReadyFlag = 0;
			SPI_Init();
			break;
			case 0x08:
			ASIC_TYPE = 0x0A;
			mConfig.ifs = INTERFACE_I2C; //update ifs for D10 demo
			I2C_Init();
			break;
			case 0x0A:
			ASIC_TYPE = 0x07;
   			INTCON3bits.INT1E = 0;  
   			INTCON3bits.INT2E = 0;
   			monitorINT1Flag = 0;
   			monitorINT2Flag = 0;
   			dataReadyFlag = 0;
			I2C_Init();
			break;
			default:
			break;
			}
		asic_type_changed_flag = 1;
		}
		else asic_type_changed_flag = 0;
	sw2_short_pressed_flag = 0;
	}
}
/** EOF main.c *************************************************/

