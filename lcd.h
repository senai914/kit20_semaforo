#ifndef __LCD_H
#define __LCD_H
#include <xc.h>
/* PIC18 XLCD peripheral routines.
 *
 *   Notes:
 *      - These libraries routines are written to support the
 *        Hitachi HD44780 LCD controller.
 *      - The user must define the following items:
 *          - The LCD interface type (4- or 8-bits)
 *          - If 4-bit mode
 *              - whether using the upper or lower nibble
 *          - The data port
 *              - The tris register for data port
 *              - The control signal ports and pins
 *              - The control signal port tris and pins
 *          - The user must provide three delay routines:
 *              - DelayFor18TCY() provides a 18 Tcy delay
 *              - DelayPORXLCD() provides at least 15ms delay
 *              - DelayXLCD() provides at least 5ms delay
 */

/* Interface type 8-bit or 4-bit
 * For 8-bit operation uncomment the #define BIT8
 */
/* #define LCD_BIT8 */

/* When in 4-bit interface define if the data is in the upper
 * or lower nibble.  For lower nibble, comment the #define UPPER
 */

#define LCD_UPPER

/* DATA_PORT defines the port to which the LCD data lines are connected */
 #define LCD_DATA_PORT      		PORTB
 #define LCD_TRIS_DATA_PORT 		TRISB

/* CTRL_PORT defines the port where the control lines are connected.
 * These are just samples, change to match your application.
 */

 #define LCD_RS_PIN   PORTBbits.RB2   		/* PORT for RS */
 #define LCD_TRIS_RS  TRISBbits.TRISB2    	/* TRIS for RS */

 #define LCD_E_PIN    PORTBbits.RB3  		/* PORT for D  */
 #define LCD_TRIS_E   TRISBbits.TRISB3    	/* TRIS for E  */

/* Display ON/OFF Control defines */
#define LCD_DON         0b00001111  /* Display on      */
#define LCD_DOFF        0b00001011  /* Display off     */
#define LCD_CURSOR_ON   0b00001111  /* Cursor on       */
#define LCD_CURSOR_OFF  0b00001101  /* Cursor off      */
#define LCD_BLINK_ON    0b00001111  /* Cursor Blink    */
#define LCD_BLINK_OFF   0b00001110  /* Cursor No Blink */

/* Cursor or Display Shift defines */
#define LCD_SHIFT_CUR_LEFT    0b00000100  /* Cursor shifts to the left   */
#define LCD_SHIFT_CUR_RIGHT   0b00000101  /* Cursor shifts to the right  */
#define LCD_SHIFT_DISP_LEFT   0b00000110  /* Display shifts to the left  */
#define LCD_SHIFT_DISP_RIGHT  0b00000111  /* Display shifts to the right */

/* Function Set defines */
#define LCD_FOUR_BIT   0b00101100  /* 4-bit Interface               */
#define LCD_EIGHT_BIT  0b00111100  /* 8-bit Interface               */
#define LCD_LINE_5X7   0b00110000  /* 5x7 characters, single line   */
#define LCD_LINE_5X10  0b00110100  /* 5x10 characters               */
#define LCD_LINES_5X7  0b00111000  /* 5x7 characters, multiple line */

#ifdef _OMNI_CODE_
#define PARAM_SCLASS
#else
#define PARAM_SCLASS auto
#endif

#ifndef MEM_MODEL
#ifdef _OMNI_CODE_
#define MEM_MODEL
#else
#define MEM_MODEL far  /* Change this to near for small memory model */
#endif
#endif

/* OpenXLCD
 * Configures I/O pins for external LCD
 */
void InicializaLCD(PARAM_SCLASS unsigned char);

/* SetDDRamAddr
 * Sets the display data address
 */
void PosicaoLCD(PARAM_SCLASS unsigned char);

/* BusyXLCD
 * Returns the busy status of the LCD
 */
unsigned char AguardarProcLCD(void);

/* ReadAddrXLCD
 * Reads the current address
 */
unsigned char LeEnderecoLCD(void);

/* ReadDataXLCD
 * Reads a byte of data
 */
char LeDadoLCD(void);

/* WriteCmdXLCD
 * Writes a command to the LCD
 */
void EnviaComandoLCD(PARAM_SCLASS unsigned char);

/* WriteDataXLCD
 * Writes a data byte to the LCD
 */
void EnviaDadoLCD(PARAM_SCLASS char);

/* putcXLCD
 * A putc is a write
 */
#define putcXLCD WriteDataXLCD

/* putsXLCD
 * Writes a string of characters to the LCD
 */
void EnviaTextoLCD_RAM(PARAM_SCLASS char *);

/* putrsXLCD
 * Writes a string of characters in ROM to the LCD
 */
void EnviaTextoLCD_ROM(const char *);

/* User defines these routines according to the oscillator frequency */
void DelayFor18TCY(void);
void DelayPORXLCD(void);
void DelayXLCD(void);

#endif