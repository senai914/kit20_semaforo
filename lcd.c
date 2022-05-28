#include <xc.h>
//#include "delays.h"
#include "lcd.h"


#define _XTAL_FREQ 4000000

/*
Atraso de 18 ciclos no mínimo
 */
void DelayFor18TCY(void){
_delay(20);
}

/*
 Atraso de 15ms no mínimo
 */
void DelayPORXLCD(void){
__delay_ms(15);
}

/*
Atraso de 5ms no mínimo
*/
void DelayXLCD(void){
__delay_ms(5);
}

/********************************************************************
*       Function Name:  BusyXLCD                                    *
*       Return Value:   char: busy status of LCD controller         *
*       Parameters:     void                                        *
*       Description:    This routine reads the busy status of the   *
*                       Hitachi HD44780 LCD controller.             *
********************************************************************/
unsigned char AguardarProcLCD(void)
{
__delay_ms(1);
return 0;               // Return FALSE
}


/********************************************************************
*       Function Name:  ResetLCD                                    *
*       Return Value:                                               *
*       Parameters:     void                                        *
*       Description:    Reseta o LCD por software.                  *
*                       Necessário na inicialização do LCD          *
********************************************************************/
void ResetLCD(void){
//-------------------reset procedure through software----------------------
#ifdef LCD_BIT8                             // 8-bit mode, use whole port
       EnviaComandoLCD(0x30);
                while(AguardarProcLCD());
                //Delay10KTCYx(0x05);

         EnviaComandoLCD(0x30);
                while(AguardarProcLCD());
                //Delay10KTCYx(0x01);

         EnviaComandoLCD(0x30);
                while(AguardarProcLCD());
                //Delay10KTCYx(0x01);


         EnviaComandoLCD(0x32);
        while( AguardarProcLCD() );
#else                                   // 4-bit mode
#ifdef LCD_UPPER                            // LCD_UPPER 4-bits of the port
         LCD_TRIS_DATA_PORT &= 0x0f;
         LCD_DATA_PORT &= 0x0f;
         LCD_DATA_PORT |= 0b00110000;        // Function set cmd(4-bit interface)
         LCD_E_PIN = 1;                      // Clock the cmd in
         DelayFor18TCY();
         LCD_E_PIN = 0;
         while(AguardarProcLCD());


         LCD_TRIS_DATA_PORT &= 0x0f;
         LCD_DATA_PORT &= 0x0f;
         LCD_DATA_PORT |= 0b00110000;        // Function set cmd(4-bit interface)
         LCD_E_PIN = 1;                      // Clock the cmd in
         DelayFor18TCY();
         LCD_E_PIN = 0;
         while(AguardarProcLCD());


         LCD_DATA_PORT &= 0x0f;              // Function set cmd(4-bit interface)
         LCD_DATA_PORT |= 0b00110000;
         LCD_E_PIN = 1;                      // Clock the cmd in
         DelayFor18TCY();
         LCD_E_PIN = 0;
         while(AguardarProcLCD());

         LCD_DATA_PORT &= 0x0f;              // Function set cmd(4-bit interface)
         LCD_DATA_PORT |= 0b00100000;
         LCD_E_PIN = 1;                      // Clock cmd in
         DelayFor18TCY();
         LCD_E_PIN = 0;
         LCD_TRIS_DATA_PORT |= 0xf0;         // Make data nibble input
         while(AguardarProcLCD());
#else
         LCD_TRIS_DATA_PORT &= 0xf0;
         LCD_DATA_PORT &= 0xf0;
         LCD_DATA_PORT |= 0b00110000;        // Function set cmd(4-bit interface)
         LCD_E_PIN = 1;                      // Clock the cmd in
         DelayFor18TCY();
         LCD_E_PIN = 0;
         while(AguardarProcLCD());


         LCD_TRIS_DATA_PORT &= 0xf0;
         LCD_DATA_PORT &= 0xf0;
         LCD_DATA_PORT |= 0b00110000;        // Function set cmd(4-bit interface)
         LCD_E_PIN = 1;                      // Clock the cmd in
         DelayFor18TCY();
         LCD_E_PIN = 0;
         while(AguardarProcLCD());


         LCD_DATA_PORT &= 0xf0;              // Function set cmd(4-bit interface)
         LCD_DATA_PORT |= 0b00110000;
         LCD_E_PIN = 1;                      // Clock the cmd in
         DelayFor18TCY();
         LCD_E_PIN = 0;
         while(AguardarProcLCD());

         LCD_DATA_PORT &= 0xf0;              // Function set cmd(4-bit interface)
         LCD_DATA_PORT |= 0b00100000;
         LCD_E_PIN = 1;                      // Clock cmd in
         DelayFor18TCY();
         LCD_E_PIN = 0;
         LCD_TRIS_DATA_PORT |= 0x0f;         // Make data nibble input
         while(AguardarProcLCD());
#endif
#endif
}


/********************************************************************
*       Function Name:  OpenXLCD                                    *
*       Return Value:   void                                        *
*       Parameters:     lcdtype: sets the type of LCD (lines)       *
*       Description:    This routine configures the LCD. Based on   *
*                       the Hitachi HD44780 LCD controller. The     *
*                       routine will configure the I/O pins of the  *
*                       microcontroller, setup the LCD for 4- or    *
*                       8-bit mode and clear the display. The user  *
*                       must provide three delay routines:          *
*                       DelayFor18TCY() provides a 18 Tcy delay     *
*                       DelayPORXLCD() provides at least 15ms delay *
*                       DelayXLCD() provides at least 5ms delay     *
********************************************************************/
void InicializaLCD(unsigned char lcdtype)
{

        __delay_ms(120);
        // The data bits must be either a 8-bit port or the LCD_LCD_LCD_UPPER or
        // lower 4-bits of a port. These pins are made into inputs
#ifdef LCD_BIT8                             // 8-bit mode, use whole port
        LCD_DATA_PORT = 0;
        LCD_TRIS_DATA_PORT = 0x00;
#else                                   // 4-bit mode
#ifdef LCD_UPPER                            // LCD_UPPER 4-bits of the port
        LCD_DATA_PORT &= 0x0f;
        LCD_TRIS_DATA_PORT &= 0x0F;
#else                                   // Lower 4-bits of the port
        LCD_DATA_PORT &= 0xf0;
        LCD_TRIS_DATA_PORT &= 0xF0;
#endif
#endif
        LCD_TRIS_RS = 0;
        LCD_TRIS_E = 0;
        LCD_RS_PIN = 0;                     // Register select pin made low
        LCD_E_PIN = 0;                      // Clock pin made low

        // Delay for 15ms to allow for LCD Power on reset
        DelayPORXLCD();


        ResetLCD();


        // Set data interface width, # lines, font
        while(AguardarProcLCD());              // Wait if LCD busy
        EnviaComandoLCD(lcdtype);          // Function set cmd

        // Turn the display on then off
        while(AguardarProcLCD());              // Wait if LCD busy
        EnviaComandoLCD(LCD_DON&LCD_CURSOR_OFF&LCD_BLINK_OFF);        // Display OFF/Blink OFF
        //while(BusyXLCD());              // Wait if LCD busy
        //WriteCmdXLCD(LCD_DON&LCD_CURSOR_ON&LCD_BLINK_ON);           // Display ON/Blink ON

        // Clear display
        while(AguardarProcLCD());              // Wait if LCD busy
        EnviaComandoLCD(0x01);             // Clear display

        // Set entry mode inc, no shift
        while(AguardarProcLCD());              // Wait if LCD busy
        //WriteCmdXLCD(LCD_SHIFT_CUR_LEFT);   // Entry Mode
        EnviaComandoLCD(LCD_SHIFT_DISP_LEFT);   // Entry Mode

        // Set DD Ram address to 0
        while(AguardarProcLCD());              // Wait if LCD busy
        PosicaoLCD(0x80);                // Set Display data ram address to 0

        return;
}

/********************************************************************
*       Function Name:  SetDDRamAddr                                *
*       Return Value:   void                                        *
*       Parameters:     CGaddr: display data address                *
*       Description:    This routine sets the display data address  *
*                       of the Hitachi HD44780 LCD controller. The  *
*                       user must check to see if the LCD controller*
*                       is busy before calling this routine.        *
********************************************************************/
void PosicaoLCD(unsigned char DDaddr)
{
#ifdef LCD_BIT8                                     // 8-bit interface
        LCD_TRIS_DATA_PORT = 0;                     // Make port output
        LCD_DATA_PORT = DDaddr | 0b10000000;        // Write cmd and address to port
        LCD_RS_PIN = 0;
        DelayFor18TCY();
        LCD_E_PIN = 1;                              // Clock the cmd and address in
        DelayFor18TCY();
        LCD_E_PIN = 0;
        DelayFor18TCY();
        LCD_TRIS_DATA_PORT = 0xff;                  // Make port input
#else                                           // 4-bit interface
#ifdef LCD_UPPER                                    // Upper nibble  interface
        LCD_TRIS_DATA_PORT &= 0x0f;                 // Make port output
        LCD_DATA_PORT &= 0x0f;                      // and write upper nibble
        LCD_DATA_PORT |= ((DDaddr | 0b10000000) & 0xf0);
#else                                           // Lower nibble interface
        LCD_TRIS_DATA_PORT &= 0xf0;                 // Make port output
        LCD_DATA_PORT &= 0xf0;                      // and write LCD_UPPER nibble
        LCD_DATA_PORT |= (((DDaddr | 0b10000000)>>4) & 0x0f);
#endif
        LCD_RS_PIN = 0;
        DelayFor18TCY();
        LCD_E_PIN = 1;                              // Clock the cmd and address in
        DelayFor18TCY();
        LCD_E_PIN = 0;
#ifdef LCD_UPPER                                    // Upper nibble interface
        LCD_DATA_PORT &= 0x0f;                      // Write lower nibble
        LCD_DATA_PORT |= ((DDaddr<<4)&0xf0);
#else                                           // Lower nibble interface
        LCD_DATA_PORT &= 0xf0;                      // Write lower nibble
        LCD_DATA_PORT |= (DDaddr&0x0f);
#endif
        DelayFor18TCY();
        LCD_E_PIN = 1;                              // Clock the cmd and address in
        DelayFor18TCY();
        LCD_E_PIN = 0;
#ifdef LCD_UPPER                                    // Upper nibble interface
        LCD_TRIS_DATA_PORT |= 0xf0;                 // Make port input
#else                                           // Lower nibble interface
        LCD_TRIS_DATA_PORT |= 0x0f;                 // Make port input
#endif
#endif
        return;
}


/********************************************************************
*       Function Name:  putsXLCD
*       Return Value:   void
*       Parameters:     buffer: pointer to string
*       Description:    This routine writes a string of bytes to the
*                       Hitachi HD44780 LCD controller. The user
*                       must check to see if the LCD controller is
*                       busy before calling this routine. The data
*                       is written to the character generator RAM or
*                       the display data RAM depending on what the
*                       previous SetxxRamAddr routine was called.
********************************************************************/
void EnviaTextoLCD_RAM(char *buffer)
{
        while(*buffer)                  // Write data to LCD up to null
        {
                while(AguardarProcLCD());      // Wait while LCD is busy
                EnviaDadoLCD(*buffer); // Write character to LCD
                buffer++;               // Increment buffer
        }
        return;
}

/********************************************************************
*       Function Name:  putrsXLCD
*       Return Value:   void
*       Parameters:     buffer: pointer to string
*       Description:    This routine writes a string of bytes to the
*                       Hitachi HD44780 LCD controller. The user
*                       must check to see if the LCD controller is
*                       busy before calling this routine. The data
*                       is written to the character generator RAM or
*                       the display data RAM depending on what the
*                       previous SetxxRamAddr routine was called.
********************************************************************/
void EnviaTextoLCD_ROM(const char *buffer)
{
        while(*buffer)                  // Write data to LCD up to null
        {
                while(AguardarProcLCD());      // Wait while LCD is busy
                EnviaDadoLCD(*buffer); // Write character to LCD
                buffer++;               // Increment buffer
        }
        return;
}

/********************************************************************
*       Function Name:  WriteDataXLCD                               *
*       Return Value:   void                                        *
*       Parameters:     data: data byte to be written to LCD        *
*       Description:    This routine writes a data byte to the      *
*                       Hitachi HD44780 LCD controller. The user    *
*                       must check to see if the LCD controller is  *
*                       busy before calling this routine. The data  *
*                       is written to the character generator RAM or*
*                       the display data RAM depending on what the  *
*                       previous SetxxRamAddr routine was called.   *
********************************************************************/
void EnviaDadoLCD(char data)
{
#ifdef LCD_BIT8                             // 8-bit interface
        LCD_TRIS_DATA_PORT = 0;             // Make port output
        LCD_DATA_PORT = data;               // Write data to port
        LCD_RS_PIN = 1;                     // Set control bits
        DelayFor18TCY();
        LCD_E_PIN = 1;                      // Clock data into LCD
        DelayFor18TCY();
        LCD_E_PIN = 0;
        LCD_RS_PIN = 0;                     // Reset control bits
        LCD_TRIS_DATA_PORT = 0xff;          // Make port input
#else                                   // 4-bit interface
#ifdef LCD_UPPER                            // Upper nibble interface
        LCD_TRIS_DATA_PORT &= 0x0f;
        LCD_DATA_PORT &= 0x0f;
        LCD_DATA_PORT |= data&0xf0;
#else                                   // Lower nibble interface
        LCD_TRIS_DATA_PORT &= 0xf0;
        LCD_DATA_PORT &= 0xf0;
        LCD_DATA_PORT |= ((data>>4)&0x0f);
#endif
        LCD_RS_PIN = 1;                     // Set control bits
        DelayFor18TCY();
        LCD_E_PIN = 1;                      // Clock nibble into LCD
        DelayFor18TCY();
        LCD_E_PIN = 0;
#ifdef LCD_UPPER                            // Upper nibble interface
        LCD_DATA_PORT &= 0x0f;
        LCD_DATA_PORT |= ((data<<4)&0xf0);
#else                                   // Lower nibble interface
        LCD_DATA_PORT &= 0xf0;
        LCD_DATA_PORT |= (data&0x0f);
#endif
        DelayFor18TCY();
        LCD_E_PIN = 1;                      // Clock nibble into LCD
        DelayFor18TCY();
        LCD_E_PIN = 0;
#ifdef LCD_UPPER                            // Upper nibble interface
        LCD_TRIS_DATA_PORT |= 0xf0;
#else                                   // Lower nibble interface
        LCD_TRIS_DATA_PORT |= 0x0f;
#endif
#endif
        return;
}

/********************************************************************
*       Function Name:  WriteCmdXLCD                                *
*       Return Value:   void                                        *
*       Parameters:     cmd: command to send to LCD                 *
*       Description:    This routine writes a command to the Hitachi*
*                       HD44780 LCD controller. The user must check *
*                       to see if the LCD controller is busy before *
*                       calling this routine.                       *
********************************************************************/
void EnviaComandoLCD(unsigned char cmd)
{
#ifdef LCD_BIT8                             // 8-bit interface
        LCD_TRIS_DATA_PORT = 0;             // Data port output
        LCD_DATA_PORT = cmd;                // Write command to data port
        LCD_RS_PIN = 0;                     // for sending a command
        DelayFor18TCY();
        LCD_E_PIN = 1;                      // Clock the command in
        DelayFor18TCY();
        LCD_E_PIN = 0;
        DelayFor18TCY();
        LCD_TRIS_DATA_PORT = 0xff;          // Data port input
#else                                   // 4-bit interface
#ifdef LCD_UPPER                            // Upper nibble interface
        LCD_TRIS_DATA_PORT &= 0x0f;
        LCD_DATA_PORT &= 0x0f;
        LCD_DATA_PORT |= cmd&0xf0;
#else                                   // Lower nibble interface
        LCD_TRIS_DATA_PORT &= 0xf0;
        LCD_DATA_PORT &= 0xf0;
        LCD_DATA_PORT |= (cmd>>4)&0x0f;
#endif
        LCD_RS_PIN = 0;
        DelayFor18TCY();
        LCD_E_PIN = 1;                      // Clock command in
        DelayFor18TCY();
        LCD_E_PIN = 0;
#ifdef LCD_UPPER                            // Upper nibble interface
        LCD_DATA_PORT &= 0x0f;
        LCD_DATA_PORT |= (cmd<<4)&0xf0;
#else                                   // Lower nibble interface
        LCD_DATA_PORT &= 0xf0;
        LCD_DATA_PORT |= cmd&0x0f;
#endif
        DelayFor18TCY();
        LCD_E_PIN = 1;                      // Clock command in
        DelayFor18TCY();
        LCD_E_PIN = 0;
#ifdef LCD_UPPER                            // Make data nibble input
        LCD_TRIS_DATA_PORT |= 0xf0;
#else
        LCD_TRIS_DATA_PORT |= 0x0f;
#endif
#endif
        return;
}

/********************************************************************
*       Function Name:  ReadDataXLCD                                *
*       Return Value:   char: data byte from LCD controller         *
*       Parameters:     void                                        *
*       Description:    This routine reads a data byte from the     *
*                       Hitachi HD44780 LCD controller. The user    *
*                       must check to see if the LCD controller is  *
*                       busy before calling this routine. The data  *
*                       is read from the character generator RAM or *
*                       the display data RAM depending on what the  *
*                       previous SetxxRamAddr routine was called.   *
********************************************************************/
char LeDadoLCD(void)
{
        char data;

#ifdef LCD_BIT8                             // 8-bit interface
        LCD_RS_PIN = 1;                     // Set the control bits
        DelayFor18TCY();
        LCD_E_PIN = 1;                      // Clock the data out of the LCD
        DelayFor18TCY();
        data = DATA_PORT;               // Read the data
        LCD_E_PIN = 0;
        LCD_RS_PIN = 0;                     // Reset the control bits
#else                                   // 4-bit interface
        LCD_RS_PIN = 1;
        DelayFor18TCY();
        LCD_E_PIN = 1;                      // Clock the data out of the LCD
        DelayFor18TCY();
#ifdef LCD_UPPER                            // Upper nibble interface
        data = LCD_DATA_PORT&0xf0;          // Read the upper nibble of data
#else                                   // Lower nibble interface
        data = (LCD_DATA_PORT<<4)&0xf0;     // read the upper nibble of data
#endif
        LCD_E_PIN = 0;                      // Reset the clock line
        DelayFor18TCY();
        LCD_E_PIN = 1;                      // Clock the next nibble out of the LCD
        DelayFor18TCY();
#ifdef LCD_UPPER                            // Upper nibble interface
        data |= (LCD_DATA_PORT>>4)&0x0f;    // Read the lower nibble of data
#else                                   // Lower nibble interface
        data |= LCD_DATA_PORT&0x0f;         // Read the lower nibble of data
#endif
        LCD_E_PIN = 0;
        LCD_RS_PIN = 0;                     // Reset the control bits
#endif
        return(data);                   // Return the data byte
}

/*********************************************************************
*       Function Name:  ReadAddrXLCD                                 *
*       Return Value:   char: address from LCD controller            *
*       Parameters:     void                                         *
*       Description:    This routine reads an address byte from the  *
*                       Hitachi HD44780 LCD controller. The user     *
*                       must check to see if the LCD controller is   *
*                       busy before calling this routine. The address*
*                       is read from the character generator RAM or  *
*                       the display data RAM depending on what the   *
*                       previous SetxxRamAddr routine was called.    *
*********************************************************************/
unsigned char LeEnderecoLCD(void)
{
        char data;                      // Holds the data retrieved from the LCD

#ifdef LCD_BIT8                             // 8-bit interface
        LCD_RS_PIN = 0;
        DelayFor18TCY();
        LCD_E_PIN = 1;                      // Clock data out of the LCD controller
        DelayFor18TCY();
        data = DATA_PORT;               // Save the data in the register
        LCD_E_PIN = 0;
#else                                   // 4-bit interface
        LCD_RS_PIN = 0;
        DelayFor18TCY();
        LCD_E_PIN = 1;                      // Clock data out of the LCD controller
        DelayFor18TCY();
#ifdef LCD_UPPER                            // Upper nibble interface
        data = LCD_DATA_PORT&0xf0;          // Read the nibble into the upper nibble of data
#else                                   // Lower nibble interface
        data = (LCD_DATA_PORT<<4)&0xf0;     // Read the nibble into the upper nibble of data
#endif
        LCD_E_PIN = 0;                      // Reset the clock
        DelayFor18TCY();
        LCD_E_PIN = 1;                      // Clock out the lower nibble
        DelayFor18TCY();
#ifdef LCD_UPPER                            // Upper nibble interface
        data |= (LCD_DATA_PORT>>4)&0x0f;    // Read the nibble into the lower nibble of data
#else                                   // Lower nibble interface
        data |= LCD_DATA_PORT&0x0f;         // Read the nibble into the lower nibble of data
#endif
        LCD_E_PIN = 0;
#endif
        return (data&0x7f);             // Return the address, Mask off the busy bit
}



void InicializaLCD4(unsigned char lcdtype)
 {
   // For PICDEM 2 Plus, setup the A/D bits so they
         //  don't interfere with the XLCD control bits.
 //ADCON1 = 0b00001110;
 //DelayPORXLCD();  // delay 15ms
 /* intialize VCC of LCD */
 /*********Violin*********/
 /* intialize VCC of LCD */
 //DDRDbits.RD7 = 0; // LCD VCC pin output
 //PORTDbits.RD7 = 1;
 /*********Violin*********/
 /*****/
         // The data bits must be either a 8-bit port or the upper or
         // lower 4-bits of a port. These pins are made into inputs
 #ifdef LCD_BIT8                             // 8-bit mode, use whole port
         DATA_PORT = 0;
         TRIS_DATA_PORT = 0xff;
 #else                                   // 4-bit mode
 #ifdef LCD_UPPER                            // Upper 4-bits of the port
         LCD_DATA_PORT &= 0x0f;
         LCD_TRIS_DATA_PORT |= 0xf0;
 #else                                   // Lower 4-bits of the port
         LCD_DATA_PORT &= 0xf0;
         LCD_TRIS_DATA_PORT |= 0x0f;
 #endif
 #endif
         //TRIS_RW = 0;                    // All control signals made outputs
         LCD_TRIS_RS = 0;
         LCD_TRIS_E = 0;
         //RW_PIN = 0;                     // R/W pin made low
         LCD_RS_PIN = 0;                     // Register select pin made low
         LCD_E_PIN = 0;                      // Clock pin made low
         // Delay for 15ms to allow for LCD Power on reset
         while(AguardarProcLCD());

         /*

         LCD_TRIS_DATA_PORT &= 0x0f;
         LCD_DATA_PORT &= 0x0f;
         LCD_DATA_PORT |= 0b00110000;        // Function set cmd(4-bit interface)
         LCD_E_PIN = 1;                      // Clock the cmd in
         DelayFor18TCY();
         LCD_E_PIN = 0;
         while(AguardarProcLCD());

         
         LCD_TRIS_DATA_PORT &= 0x0f;
         LCD_DATA_PORT &= 0x0f;
         LCD_DATA_PORT |= 0b00110000;        // Function set cmd(4-bit interface)
         LCD_E_PIN = 1;                      // Clock the cmd in
         DelayFor18TCY();
         LCD_E_PIN = 0;
         while(AguardarProcLCD());


         LCD_DATA_PORT &= 0x0f;              // Function set cmd(4-bit interface)
         LCD_DATA_PORT |= 0b00110000;
         LCD_E_PIN = 1;                      // Clock the cmd in
         DelayFor18TCY();
         LCD_E_PIN = 0;
         while(AguardarProcLCD());

         LCD_DATA_PORT &= 0x0f;              // Function set cmd(4-bit interface)
         LCD_DATA_PORT |= 0b00100000;
         LCD_E_PIN = 1;                      // Clock cmd in
         DelayFor18TCY();
         LCD_E_PIN = 0;
         LCD_TRIS_DATA_PORT |= 0xf0;         // Make data nibble input
         while(AguardarProcLCD());
*/



                 EnviaComandoLCD(0x30);
                        while(AguardarProcLCD());
                        //Delay10KTCYx(0x05);

		 EnviaComandoLCD(0x30);
                        while(AguardarProcLCD());
                        //Delay10KTCYx(0x01);

                 EnviaComandoLCD(0x30);
                        while(AguardarProcLCD());
                        //Delay10KTCYx(0x01);


		 EnviaComandoLCD(0x32);
		while( AguardarProcLCD() );






        //        WriteCmdXLCD(lcdtype);          // Function set cmd
         EnviaComandoLCD(0x28);          // Function set cmd
 /*****/
 /*****/
         // Turn the display on then off
 //        while(BusyXLCD());              // Wait if LCD busy
        __delay_ms(40);
 //        WriteCmdXLCD(DOFF&CURSOR_OFF&BLINK_OFF);        // Display OFF/Blink OFF
         EnviaComandoLCD(0x0c);
 /*****/
 /*****/
 //        while(BusyXLCD());              // Wait if LCD busy
        __delay_ms(40);
 //        WriteCmdXLCD(DON&CURSOR_ON&BLINK_ON);           // Display ON/Blink ON
         // Clear display
 //        while(BusyXLCD());              // Wait if LCD busy
        __delay_ms(40);
         EnviaComandoLCD(0x01);             // Clear display
 /*****/
         // Set entry mode inc, no shift
 //        while(BusyXLCD());              // Wait if LCD busy
        __delay_ms(40);
 //        WriteCmdXLCD(SHIFT_CUR_LEFT);   // Entry Mode
         EnviaComandoLCD(0x06);
 /*****/
 /*****/
         // Set DD Ram address to 0
 //        while(BusyXLCD());              // Wait if LCD busy
        __delay_ms(40);
 //        SetDDRamAddr(0);                // Set Display data ram address to 0
         EnviaComandoLCD(0x80);             // Set Display data ram address to 0
 /*****/
        __delay_ms(40);
         return;
 }