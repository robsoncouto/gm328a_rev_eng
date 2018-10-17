/* ************************************************************************
 *
 *   driver functions for HD44780 compatible character displays
 *   - 4 bit parallel interface
 *
 *   (c) 2015-2018 by Markus Reschke
 *
 * ************************************************************************ */

/*
 *  hints:
 *  - pin assignment for 4 bit parallel
 *    DB4    LCD_DB4 (default: LCD_PORT Bit #0)
 *    DB5    LCD_DB5 (default: LCD_PORT Bit #1)
 *    DB6    LCD_DB6 (default: LCD_PORT Bit #2)
 *    DB7    LCD_DB7 (default: LCD_PORT Bit #3)
 *    RS     LCD_RS
 *    R/W    Gnd
 *    E      LCD_EN1
 *  - write only since R/W is hardwired to Gnd
 *  - max. clock for interface: 2 MHz
 *  - pin assignment for PCF8574 backpack
 *    DB4    LCD_DB4 (default: P4)
 *    DB5    LCD_DB5 (default: P5)
 *    DB6    LCD_DB6 (default: P6)
 *    DB7    LCD_DB7 (default: P7)
 *    RS     LCD_RS  (default: P0)
 *    R/W    LCD_RW  (default: P1)
 *    E      LCD_EN1 (default: P2)
 *    LED    LCD_LED (default: P3)
 */


/* local includes */
#include "config.h"           /* global configuration */

#ifdef LCD_HD44780


/*
 *  local constants
 */

/* source management */
#define LCD_DRIVER_C


/*
 *  include header files
 */

/* local includes */
#include "common.h"           /* common header file */
#include "variables.h"        /* global variables */
#include "functions.h"        /* external functions */
#include "HD44780.h"          /* HD44780 specifics */

/* fonts */
#include "font_HD44780_int.h"      /* international font version */
#include "font_HD44780_cyr.h"      /* Cyrillic font version */



/* ************************************************************************
 *   low level functions for 4 bit parallel interface
 * ************************************************************************ */


#ifdef LCD_PAR_4

/*
 *  create Enable pulse
 *  - LCD needs an Enable pulse to take in data for processing
 */

void LCD_EnablePulse(void)
{
  LCD_PORT |= (1 << LCD_EN1);      /* set EN1 high */

  /* the LCD needs some time */
  /* if required adjust time according to LCD's datasheet */
  wait10us();

  LCD_PORT &= ~(1 << LCD_EN1);     /* set EN1 low */
}



/*
 *  set up interface bus
 *  - should be called at firmware startup
 */

void LCD_BusSetup(void)
{
  /*
   *  set port pins to output mode
   *  - control lines: LCD_RS & LCD_EN1
   *  - data lines: LCD_DB4, LCD_DB5, LCD_DB6 & LCD_DB7
   */

  LCD_DDR |= (1 << LCD_RS) | (1 << LCD_EN1) | (1 << LCD_DB4) | (1 << LCD_DB5) | (1 << LCD_DB6) | (1 << LCD_DB7);

  /* LCD_EN1 should be low by default */
}



/*
 *  send a nibble to the LCD (4 bit mode)
 *
 *  requires:
 *  - nibble value to send
 */

void LCD_SendNibble(uint8_t Nibble)
{
  uint8_t           Data;

  Data = LCD_PORT;            /* get current bits */
  /* clear all 4 data lines */
  Data &= ~((1 << LCD_DB4) | (1 << LCD_DB5) | (1 << LCD_DB6) | (1 << LCD_DB7));

  #ifdef LCD_DB_STD
    /* standard pins: simply take nibble */
    Data |= Nibble;
  #else
    /* non-standard pins: set bits individually */
    if (Nibble & 0b00000001) Data |= (1 << LCD_DB4);
    if (Nibble & 0b00000010) Data |= (1 << LCD_DB5);
    if (Nibble & 0b00000100) Data |= (1 << LCD_DB6);
    if (Nibble & 0b00001000) Data |= (1 << LCD_DB7);
  #endif

  LCD_PORT = Data;            /* set nibble */

  /* give LCD some time */
  #if CPU_FREQ < 2000000
    _delay_us(5);
  #else
    wait5us();
  #endif

  LCD_EnablePulse();          /* trigger LCD */
}



/*
 *  send a byte (data or command) to the LCD
 *  - send byte as two nibbles (MSB first, LSB last)
 *
 *  requires:
 *  - byte value to send
 */

void LCD_Send(uint8_t Byte)
{
  uint8_t           Nibble;

  /* send upper nibble (bits 4-7) */
  Nibble = (Byte >> 4) & 0x0F;          /* get upper nibble */
  LCD_SendNibble(Nibble);

  /* send lower nibble (bits 0-3) */
  Nibble = Byte & 0x0F;                 /* get lower nibble */
  LCD_SendNibble(Nibble);

  wait50us();            /* LCD needs some time for processing */

  /* clear data lines on port */  
  Nibble = ~((1 << LCD_DB4) | (1 << LCD_DB5) | (1 << LCD_DB6) | (1 << LCD_DB7));
  LCD_PORT &= Nibble;         /* clear port */
}



/*
 *  send a command to the LCD
 *
 *  requires:
 *  - byte value to send
 */
 
void LCD_Cmd(uint8_t Cmd)
{
  /* indicate command mode */
  LCD_PORT &= ~(1 << LCD_RS);      /* set RS low */

  LCD_Send(Cmd);                   /* send command */
}



/*
 *  display a single character
 *
 *  requires:
 *  - Char: character to display
 */

void LCD_Char(unsigned char Char)
{
  uint8_t           *Table;        /* pointer to table */
  uint8_t           ID;            /* char ID */  

  /* prevent x overflow */
  if (UI.CharPos_X > LCD_CHAR_X) return;

  /* get character ID from lookup table */
  Table = (uint8_t *)&FontTable;        /* start address */
  Table += Char;                        /* add offset for character */
  ID = pgm_read_byte(Table);            /* get ID number */
  if (ID == 0xff) return;               /* no character available */

  /* indicate data mode */
  LCD_PORT |= (1 << LCD_RS);       /* set RS high */
 
  LCD_Send(ID);                    /* send character ID */

  /* update character position */
  UI.CharPos_X++;                  /* next character in current line */
  /* LCD's RAM address already points to next character */
}



/*
 *  interface specific display setup
 */

void LCD_DisplaySetup(void)
{
  uint8_t           Bits;            /* port bits */  

  /*
   *  first we have to send three times:
   *  - RS and R/W unset
   *  - DB4 and DB5 set
   */

  /* round #1 */
  MilliSleep(30);                       /* wait 30ms */
  Bits = LCD_PORT;                      /* get current bits */
  /* clear LCD_RS, LCD_DB6 & LCD_DB7 */
  Bits &= ~(1 << LCD_RS) & ~(1 << LCD_DB6) & ~(1 << LCD_DB7);
  /* set LCD_DB4 & LCD_DB5 */
  Bits |= (1 << LCD_DB5) | (1 << LCD_DB4);
  LCD_PORT = Bits;                      /* set new bits */
  LCD_EnablePulse();

  /* round #2 */
  MilliSleep(5);                        /* wait 5ms */
  LCD_EnablePulse();

  /* round #3 */
  MilliSleep(1);                        /* wait 1ms */
  LCD_EnablePulse();


  /*
   *  init 4 bit mode
   */

  MilliSleep(1);
  /* function set: DB5 set (to switch from 8 to 4 bit mode) */
  Bits = LCD_PORT;                      /* get current bits */
  /* same as above besides DB4, so only clear LCD_DB4 */
  Bits &= ~(1 << LCD_DB4);
  LCD_PORT = Bits;                      /* set new bits */
  MilliSleep(1);
  LCD_EnablePulse();
  MilliSleep(1);
}



/*
 *  load a custom character into LCD module
 *
 *  requires:
 *  - pointer of fixed character data
 *  - ID for custom character (0-7)
 */

void LCD_CustomChar(uint8_t ID)
{
  uint8_t      i;                  /* counter */
  uint8_t      Byte;               /* data byte */
  uint8_t      *Table;             /* pointer to char data */

  /* set data start address */
  Table = (uint8_t *)&FontData;         /* start address */
  Table += (ID * 8);                    /* add offset for character */


  /*
   *  set CG RAM start address (for a 5x8 character)
   *  - lower 3 bits determine the row in a character
   *  - higher 3 bits determine the start (ID) of the character
   *  - so we have to shift the ID to the higher part
   *  - LCD module supports up to 8 custom characters (5x8 font)
   */

  LCD_Cmd(CMD_SET_CG_RAM_ADDR | (ID << 3));

  /* indicate data mode */
  LCD_PORT |= (1 << LCD_RS);            /* set RS high */

  /* write custom character */
  for (i = 0; i < 8; i++)               /* 8 bytes */
  {
    Byte = pgm_read_byte(Table);        /* read byte */
    LCD_Send(Byte);                     /* send byte */

    Table++;                            /* next byte */
  }
}

#endif



/* ************************************************************************
 *   low level functions for PCF8574 based backpack (I2C)
 *   - based on idea of Duncan McIntyre <duncan@calligram.co.uk>
 * ************************************************************************ */


#ifdef LCD_PCF8574

/*
 *  local variables
 */

/* LCD interface (PCF8574's port) */
uint8_t             Control;       /* LCD's control lines */



/*
 *  set PCF8574's port pins via I2C
 */

void PCF8574_Write(uint8_t Byte)
{
  if (I2C_Start(I2C_START) == I2C_OK)             /* start */
  {
    I2C.Byte = LCD_I2C_ADDR << 1;       /* address (7 bit & write) */

    if (I2C_WriteByte(I2C_ADDRESS) == I2C_ACK)    /* address slave */
    {
      I2C.Byte = Byte;                  /* port pins */
      I2C_WriteByte(I2C_DATA);          /* send data */
    }
    /* todo: error handling? */
  }

  I2C_Stop();                                     /* stop */
}



/*
 *  set up interface bus
 *  - should be called at firmware startup
 */

void LCD_BusSetup(void)
{
  /* I2C is set up in main() already */
  I2C.Timeout = 1;            /* ACK timeout 10µs */


  /*
   *  set port pins
   *  - backlight: set LCD_LED
   *  - control lines: clear LCD_RW, LCD_RS & LCD_EN1
   *    we'll keep the LCD in write mode
   *  - data lines: all 0
   */

  Control = (1 << LCD_LED);
  PCF8574_Write(Control);
}



/*
 *  write data into LCD
 *  - LCD needs an Enable pulse to take in data for processing
 *
 *  requires:
 *  - byte value to send (PCF8574's port)
 */

void LCD_Write(uint8_t Byte)
{
  Byte |= (1 << LCD_EN1);     /* set Enable bit */
  PCF8574_Write(Byte);        /* update port pins */

  /* the LCD needs some time */
  /* if required adjust time according to LCD's datasheet */
  wait10us();

  Byte &= ~(1 << LCD_EN1);    /* clear Enable bit */
  PCF8574_Write(Byte);        /* update port pins */
}



/*
 *  send a nibble to the LCD (4 bit mode)
 *
 *  requires:
 *  - nibble value to send
 */

void LCD_SendNibble(uint8_t Nibble)
{
  uint8_t           Data;

  Data = Control;        /* get current control bitmask */

  #ifdef LCD_DB_STD
    /* standard pins: shift nibble */
    Nibble <<= 4;             /* shift to bits 4-7 */
    Data |= Nibble;
  #else
    /* non-standard pins: set bits individually */
    if (Nibble & 0b00000001) Data |= (1 << LCD_DB4);
    if (Nibble & 0b00000010) Data |= (1 << LCD_DB5);
    if (Nibble & 0b00000100) Data |= (1 << LCD_DB6);
    if (Nibble & 0b00001000) Data |= (1 << LCD_DB7);
  #endif

  LCD_Write(Data);       /* write nibble */
}



/*
 *  send a byte (data or command) to the LCD
 *  - send byte as two nibbles (MSB first, LSB last)
 *
 *  requires:
 *  - byte value to send
 */

void LCD_Send(uint8_t Byte)
{
  uint8_t           Nibble;

  wdt_reset();                /* reset watchdog */

  /*
   *  send upper nibble (bits 4-7)
   */

  Nibble = (Byte >> 4) & 0x0F;          /* get upper nibble */
  LCD_SendNibble(Nibble);


  /*
   *  send lower nibble (bits 0-3)
   */

  Nibble = Byte & 0x0F;                 /* get lower nibble */
  LCD_SendNibble(Nibble);

  wait50us();            /* LCD needs some time for processing */


  /*
   *  clear data lines on port
   */  

  PCF8574_Write(Control);     /* update port pins */
}



/*
 *  send a command to the LCD
 *
 *  requires:
 *  - byte value to send
 */
 
void LCD_Cmd(uint8_t Cmd)
{
  /* indicate command mode */
  Control &= ~(1 << LCD_RS);       /* set RS to 0 */

  LCD_Send(Cmd);                   /* send command */
}



/*
 *  display a single character
 *
 *  requires:
 *  - Char: character to display
 */

void LCD_Char(unsigned char Char)
{
  uint8_t           *Table;        /* pointer to table */
  uint8_t           ID;            /* char ID */  

  /* prevent x overflow */
  if (UI.CharPos_X > LCD_CHAR_X) return;

  /* get character ID from lookup table */
  Table = (uint8_t *)&FontTable;        /* start address */
  Table += Char;                        /* add offset for character */
  ID = pgm_read_byte(Table);            /* get ID number */
  if (ID == 0xff) return;               /* no character available */

  /* indicate data mode */
  Control |= (1 << LCD_RS);        /* set RS to 1 */
 
  LCD_Send(ID);                    /* send character ID */

  /* update character position */
  UI.CharPos_X++;                  /* next character in current line */
  /* LCD's RAM address already points to next character */
}



/*
 *  interface specific display setup
 */

void LCD_DisplaySetup(void)
{
  uint8_t           Bits;            /* port bits */  

  /*
   *  first we have to send three times:
   *  - RW, RS and R/W unset
   *  - DB4 and DB5 set
   */

  /* round #1 */
  MilliSleep(30);                       /* wait 30ms */
  Bits = (1 << LCD_LED) | (1 << LCD_DB5) | (1 << LCD_DB4);
  LCD_Write(Bits);

  /* round #2 */
  MilliSleep(5);                        /* wait 5ms */
  LCD_Write(Bits);

  /* round #3 */
  MilliSleep(1);                        /* wait 1ms */
  LCD_Write(Bits);


  /*
   *  init 4 bit mode
   */

  MilliSleep(1);
  /* function set: DB5 set (to switch from 8 to 4 bit mode) */
  Bits = (1 << LCD_LED) | (1 << LCD_DB5);
  LCD_Write(Bits);
  MilliSleep(1);
}



/*
 *  load a custom character into LCD module
 *
 *  requires:
 *  - pointer of fixed character data
 *  - ID for custom character (0-7)
 */

void LCD_CustomChar(uint8_t ID)
{
  uint8_t      i;                  /* counter */
  uint8_t      Byte;               /* data byte */
  uint8_t      *Table;             /* pointer to char data */

  /* set data start address */
  Table = (uint8_t *)&FontData;         /* start address */
  Table += (ID * 8);                    /* add offset for character */


  /*
   *  set CG RAM start address (for a 5x8 character)
   *  - lower 3 bits determine the row in a character
   *  - higher 3 bits determine the start (ID) of the character
   *  - so we have to shift the ID to the higher part
   *  - LCD module supports up to 8 custom characters (5x8 font)
   */

  LCD_Cmd(CMD_SET_CG_RAM_ADDR | (ID << 3));

  /* indicate data mode */
  Control |= (1 << LCD_RS);        /* set RS to 1 */

  /* write custom character */
  for (i = 0; i < 8; i++)               /* 8 bytes */
  {
    Byte = pgm_read_byte(Table);        /* read byte */
    LCD_Send(Byte);                     /* send byte */

    Table++;                            /* next byte */
  }
}


#endif



/* ************************************************************************
 *   high level functions
 * ************************************************************************ */


/*
 *  clear the display 
 */ 

void LCD_Clear(void)
{
  LCD_Cmd(CMD_CLEAR_DISPLAY);      /* send clear command */
  MilliSleep(2);                   /* LCD needs some time for processing */

  /* reset character position */
  UI.CharPos_X = 1;
  UI.CharPos_Y = 1;
}



/*
 *  initialize LCD
 *  - for 4bit mode
 */
 
void LCD_Init(void)
{
  LCD_DisplaySetup();         /* interface specific setup */

 
  /*
   *  set modes
   */

  /* function set: 4 bit interface / 2 rows / font 5x7 */
  LCD_Cmd(CMD_FUNCTION_SET | FLAG_LINES_2);

  /* display: display on / cursor off / no blinking */
  LCD_Cmd(CMD_DISPLAY_CONTROL | FLAG_DISPLAY_ON);

  /* entry mode: increment cursor position / no scrolling */    
  LCD_Cmd(CMD_ENTRY_MODE_SET | FLAG_CURSOR_INCREASE);


  /*
   *  load custom characters
   */

  /* todo: why not a simple loop? */

  /* custom symbols for components */
  LCD_CustomChar(LCD_CHAR_DIODE_AC);    /* diode symbol '|>|' */
  LCD_CustomChar(LCD_CHAR_DIODE_CA);    /* diode symbol '|<|' */
  LCD_CustomChar(LCD_CHAR_CAP);         /* capacitor symbol '||' */
  LCD_CustomChar(LCD_CHAR_RESISTOR_L);  /* resistor symbol '[' */
  LCD_CustomChar(LCD_CHAR_RESISTOR_R);  /* resistor symbol ']' */

  /* kyrillish LCD character set lacks omega and µ */
  LCD_CustomChar(LCD_CHAR_OMEGA);       /* Omega */
  LCD_CustomChar(LCD_CHAR_MICRO);       /* µ / micro */


  /* and clear display */
  LCD_Clear();

  /* update character maximums */
  UI.CharMax_X = LCD_CHAR_X;
  UI.CharMax_Y = LCD_CHAR_Y;
}



/*
 *  set LCD character position
 *  - since we can't read the LCD and don't use a RAM buffer
 *    we have to move page-wise in y direction
 *  - top left: 0/0
 *
 *  requires:
 *  - x:  horizontal position 
 *  - y:  vertical position
 */

void LCD_CharPos(uint8_t x, uint8_t y)
{
  uint8_t           Address = 0;        /* RAM address */

  /* update character position */
  UI.CharPos_X = x;
  UI.CharPos_Y = y;

  /*
   *  vertical position
   *
   *  Line 1: 0x00
   *  Line 2: 0x40
   *  Line 3: 0x14
   *  Line 4: 0x54
   */

  y--;                             /* for binary magic */

  /* line 3 or 4: base starts at 20 bytes */
  if (y & 0b00000010) Address = 0x14;

  /* line 2 or 4: base + 20 bytes */
  if (y & 0b00000001) Address += 0x40;


  /*
   *  horizontal position
   *
   *  line address + horizontal position
   */

  x--;                             /* address starts at 0 */
  Address += x;                    /* add x position */

  /* set RAM address */
  LCD_Cmd(CMD_SET_DD_RAM_ADDR | Address);    /* send command */
}



/*
 *  clear one single character line
 *
 *  requires:
 *  - Line: line number (1-4)
 *    special case line 0: clear remaining space in current line
 */ 

void LCD_ClearLine(uint8_t Line)
{
  uint8_t           n = 0;         /* counter */

  if (Line == 0)         /* special case: rest of current line */
  {
    n = UI.CharPos_X;         /* current character position */
    n--;                      /* starts at 0 */
  }
  else                   /* standard case: some line */
  {
    LCD_CharPos(1, Line);     /* go to beginning of line */
  }

  while (n < 20)              /* up to 20 bytes */
  {
    LCD_Char(' ');            /* send space */
    n++;                      /* next one */
  }
}



/*
 *  set cursor
 *
 *  required:
 *  - Mode: cursor mode
 *    0: cursor on
 *    1: cursor off
 */

void LCD_Cursor(uint8_t Mode)
{
  uint8_t           Command;

  LCD_CharPos(LCD_CHAR_X, LCD_CHAR_Y);       /* move to bottom right */

  /* default: cursor off */
  Command = CMD_DISPLAY_CONTROL | FLAG_DISPLAY_ON | FLAG_CURSOR_OFF;

  /* enable cursor if requested */
  if (Mode) Command |= FLAG_CURSOR_ON;

  LCD_Cmd(Command);           /* send command */
}



/* ************************************************************************
 *   clean-up of local constants
 * ************************************************************************ */

/* source management */
#undef LCD_DRIVER_C

#endif

/* ************************************************************************
 *   EOF
 * ************************************************************************ */
