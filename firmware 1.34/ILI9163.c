/* ************************************************************************
 *
 *   driver functions for ILI9163 compatible color graphic displays
 *   - 128 x 160 (132 x 162) pixels
 *   - SPI interface (4 line)
 *
 *   (c) 2017-2018 by Markus Reschke
 *
 * ************************************************************************ */

/*
 *  hints:
 *  - pin assignment for SPI (4 wire)
 *    /RESX        LCD_RES (optional)
 *    /CSX         LCD_CS (optional)
 *    D/CX (WRX)   LCD_DC
 *    SCL (D/CX)   LCD_SCL / SPI_SCK
 *    SDIO (D[0])  LCD_SDA / SPI_MOSI
 *    For hardware SPI LCD_SCL and LCD_SDA have to be the MCU's SCK and
 *    MOSI pins.
 *  - max. SPI clock: 15.1MHz write, 6.6MHz read 
 */



/* local includes */
#include "config.h"           /* global configuration */

#ifdef LCD_ILI9163


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
#include "colors.h"           /* color definitions */
#include "ILI9163.h"          /* ILI9163 specifics */


/* fonts and symbols */
/* horizontally aligned, horizontal bit order flipped */
#include "font_8x8_hf.h"
#include "font_8x16_cyrillic_hf.h"
#include "font_10x16_hf.h"
#include "symbols_24x24_hf.h"
#include "symbols_30x32_hf.h"



/*
 *  derived constants
 */

/* maximum number of pixels for X and Y direction */
#ifdef LCD_ROTATE
  #define LCD_PIXELS_X        LCD_DOTS_Y
  #define LCD_PIXELS_Y        LCD_DOTS_X
#else
  #define LCD_PIXELS_X        LCD_DOTS_X
  #define LCD_PIXELS_Y        LCD_DOTS_Y
#endif

/* number of lines and characters per line */
#define LCD_CHAR_X            (LCD_PIXELS_X / FONT_SIZE_X)
#define LCD_CHAR_Y            (LCD_PIXELS_Y / FONT_SIZE_Y)

/* component symbols */
#ifdef SW_SYMBOLS
  /* resize symbols by a factor of 1 */
  #define SYMBOL_RESIZE         1

  /* size in relation to a character */
  #define LCD_SYMBOL_CHAR_X   (((SYMBOL_SIZE_X * SYMBOL_RESIZE) + FONT_SIZE_X - 1) / FONT_SIZE_X)
  #define LCD_SYMBOL_CHAR_Y   (((SYMBOL_SIZE_Y * SYMBOL_RESIZE) + FONT_SIZE_Y - 1) / FONT_SIZE_Y)

  /* check y size: we need at least 2 lines */
  #if LCD_SYMBOL_CHAR_Y < 2
    #error <<< Symbols too small! >>>
  #endif
#endif



/*
 *  local variables
 */

/* address window */
uint16_t            X_Start;       /* start position X (column) */
uint16_t            X_End;         /* end position X (column) */
uint16_t            Y_Start;       /* start position Y (row) */
uint16_t            Y_End;         /* end position Y (row) */

/* text line management */
uint16_t            LineMask;      /* bit mask for up to 16 lines */



/* ************************************************************************
 *   low level functions for SPI interface (4 wire)
 * ************************************************************************ */


#ifdef LCD_SPI

/*
 *  set up interface bus
 *  - should be called at firmware startup
 */

void LCD_BusSetup(void)
{
  uint8_t           Bits;          /* bitmask */


  /*
   *  set control signals
   */

  Bits = LCD_DDR;                  /* get current directions */

  /* basic output pins */
  Bits |= (1 << LCD_DC);           /* D/C */

  /* optional output pins */
  #ifdef LCD_RES
    Bits |= (1 << LCD_RES);        /* /RESX */
  #endif 
  #ifdef LCD_CS
    Bits |= (1 << LCD_CS);         /* /CSX */
  #endif

  LCD_DDR = Bits;                  /* set new directions */


  /* set default levels */
  #ifdef LCD_CS
    /* disable chip */
    LCD_PORT |= (1 << LCD_CS);          /* set /CSX high */
  #endif
  #ifdef LCD_RES
    /* disable reset */
    LCD_PORT |= (1 << LCD_RES);         /* set /RESX high */
  #endif


  /*
   *  init SPI bus
   */

  #ifdef SPI_HARDWARE
  /*
   *  set SPI clock rate (max. 15 MHz)
   *  - max. MCU clock 20MHz / 2 = 10MHz
   *  - f_osc/2 (SPR1 = 0, SPR0 = 0, SPI2X = 1)
   */

  SPI.ClockRate = SPI_CLOCK_2X;    /* set clock rate flags */
  #endif

  SPI_Setup();                     /* set up SPI bus */
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
  LCD_PORT &= ~(1 << LCD_DC);      /* set D/CX low */

  /* select chip, if pin available */
  #ifdef LCD_CS
    LCD_PORT &= ~(1 << LCD_CS);    /* set /CSX low */
  #endif

  SPI_Write_Byte(Cmd);             /* write command byte */

  /* deselect chip, if pin available */
  #ifdef LCD_CS
    LCD_PORT |= (1 << LCD_CS);     /* set /CSX high */
  #endif
}



/*
 *  send data to the LCD
 *
 *  requires:
 *  - byte value to send
 */

void LCD_Data(uint8_t Data)
{
  /* indicate data mode */
  LCD_PORT |= (1 << LCD_DC);       /* set D/CX high */

  /* select chip, if pin available */
  #ifdef LCD_CS
    LCD_PORT &= ~(1 << LCD_CS);    /* set /CSX low */
  #endif

  SPI_Write_Byte(Data);            /* write data byte */

  /* deselect chip, if pin available */
  #ifdef LCD_CS
    LCD_PORT |= (1 << LCD_CS);     /* set /CSX high */
  #endif
}



/*
 *  send data to the LCD
 *
 *  requires:
 *  - 2-byte value to send
 */

void LCD_Data2(uint16_t Data)
{
  uint8_t           Byte;     /* data byte */

  /* indicate data mode */
  LCD_PORT |= (1 << LCD_DC);       /* set D/CX high */

  /* select chip, if pin available */
  #ifdef LCD_CS
    LCD_PORT &= ~(1 << LCD_CS);    /* set /CSX low */
  #endif

  Byte = (uint8_t)Data;            /* save LSB */
  Data >>= 8;                      /* get MSB */

  SPI_Write_Byte((uint8_t)Data);   /* write MSB of data */
  SPI_Write_Byte(Byte);            /* write LSB of data */

  /* deselect chip, if pin available */
  #ifdef LCD_CS
    LCD_PORT |= (1 << LCD_CS);     /* set /CSX high */
  #endif
}

#endif



/* ************************************************************************
 *   high level functions
 * ************************************************************************ */


/*
 *  set address window
 *  - 0 up to (max - 1)
 */

void LCD_AddressWindow(void)
{
  /* X -> column */
  LCD_Cmd(CMD_COL_ADDR_SET);
  LCD_Data2(X_Start);               /* start column */
  LCD_Data2(X_End);                 /* end column */

  /* Y -> row */
  LCD_Cmd(CMD_ROW_ADDR_SET);
  LCD_Data2(Y_Start);               /* start row */
  LCD_Data2(Y_End);                 /* end row */
}



/*
 *  set LCD character position
 *
 *  requires:
 *  - x:  horizontal position (1-)
 *  - y:  vertical position (1-)
 */

void LCD_CharPos(uint8_t x, uint8_t y)
{
  uint16_t          Mask = 1;  

  /* update UI */
  UI.CharPos_X = x;
  UI.CharPos_Y = y;

  y--;                        /* rows start at zero */

  /* mark text line as used */
  if (y < 16)                 /* prevent overflow */
  {
    Mask <<= y;               /* shift to bit position for line */
    LineMask |= Mask;         /* set bit for line */
  }

  /* horizontal position (column) */
  x--;                        /* columns starts at 0 */
  Mask = x;                   /* expand to 16 bit */
  Mask *= FONT_SIZE_X;        /* offset for character */
  #ifdef LCD_OFFSET_X
  Mask += LCD_OFFSET_X;       /* additional X offset */
  #endif
  X_Start = Mask;             /* update start position */

  /* vertical position (row) */
  Mask = y;                   /* expand to 16 bit */
  Mask *= FONT_SIZE_Y;        /* offset for character */
  Y_Start = Mask;             /* update start position */
}



/*
 *  clear one single character line
 *
 *  requires:
 *  - Line: line number (1-)
 *    special case line 0: clear remaining space in current line
 */ 

void LCD_ClearLine(uint8_t Line)
{
  uint16_t          x = 0;         /* x position */
  uint8_t           y;             /* y position */
  uint8_t           Pos = 1;       /* character position */

  wdt_reset();                /* reset watchdog */

  if (Line == 0)         /* special case: rest of current line */
  {
    Line = UI.CharPos_Y;           /* get current line */
    Pos = UI.CharPos_X;            /* get current character position */
  }

  /* have we to clear this line? */
  if (Line <= 16)                  /* prevent overflow */
  {
    y = Line - 1;                  /* bitmask starts at zero */
    x = 1;                         /* set start bit */
    x <<= y;                       /* bit for this line */

    if (! (LineMask & x))          /* bit not set */
    {
      return;                      /* nothing do to */
    }
  }

  /* manage address window */
  LCD_CharPos(Pos, Line);         /* update character position */
                                  /* also updates X_Start and Y_Start */
  if (Pos == 1)                   /* complete line */
  {
    if (x > 0)                    /* got line bit */
    {
      LineMask &= ~x;             /* clear bit */
    }
  }

  #ifdef LCD_OFFSET_X
    /* last column considering x offset */
    X_End = LCD_PIXELS_X - 1 + LCD_OFFSET_X;
  #else
    X_End = LCD_PIXELS_X - 1;           /* last column */
  #endif

  Y_End = Y_Start + FONT_SIZE_Y - 1;    /* last row */
  y = FONT_SIZE_Y;                      /* set default */

  /* partial text line at bottom of display */
  if (Y_End > (LCD_PIXELS_Y - 1))       /* row overflow */
  {
    x = Y_End - (LCD_PIXELS_Y - 1);     /* difference */
    y -= (uint8_t)x;                    /* adjust number of rows */
    Y_End = LCD_PIXELS_Y - 1;           /* set last row */    
  }

  LCD_AddressWindow();                  /* set window */

  /* send background color */
  LCD_Cmd(CMD_MEM_WRITE);          /* start writing */

  while (y > 0)                    /* character height (pages) */
  {
    x = X_Start;                   /* reset start position */
    #ifdef LCD_OFFSET_X
    x -= LCD_OFFSET_X;             /* additional X offset */
    #endif

    while (x < LCD_PIXELS_X)       /* all columns */
    {
      /* send background color */
      LCD_Data2(COLOR_BACKGROUND);

      x++;                         /* next column */
    }

    y--;                           /* next page */
  }
}



/*
 *  clear the display 
 */ 

void LCD_Clear(void)
{
  uint8_t           n = 1;         /* counter */

  /* we have to clear all dots manually :-( */
  while (n <= (LCD_CHAR_Y + 1))    /* for all text lines */
  {
    /* +1 is for a possible partial line at the bottom */

    LCD_ClearLine(n);              /* clear line */
    n++;                           /* next line */
  }

  LCD_CharPos(1, 1);          /* reset character position */
}



/*
 *  initialize LCD
 */
 
void LCD_Init(void)
{
  uint8_t           Bits;

  /* hardware reset */
  #ifdef LCD_RES
  LCD_PORT = LCD_PORT & ~(1 << LCD_RES);     /* set /RESX low */
  wait10us();                                /* wait 10µs */
  LCD_PORT = LCD_PORT | (1 << LCD_RES);      /* set /RESX high */
  /* blanking sequence needs up to 120ms */
  /* but we may send command after 5ms */
  MilliSleep(5);                             /* wait 5ms */
  #endif

  /* memory access control */
  LCD_Cmd(CMD_MEM_CTRL);
  Bits = FLAG_RGB_BGR;             /* color bit order: RGB */
  /* should be FLAG_RGB_RGB / incorrect datasheet? */
  #ifdef LCD_ROTATE
    Bits |= FLAG_MV_REV;           /* swap x and y */
  #endif
  #ifdef LCD_FLIP_X 
    Bits |= FLAG_MX_REV;           /* flip x */
  #endif
  #ifdef LCD_FLIP_Y
    Bits |= FLAG_MY_REV;           /* flip y */
  #endif
  LCD_Data(Bits);

  /* set pixel format for RGB image data */
  LCD_Cmd(CMD_PIX_FORMAT);
  LCD_Data(FLAG_IFPF_16);          /* 16 Bits per pixel */

  /* address window */
  #ifdef LCD_OFFSET_X
    X_Start = LCD_OFFSET_X;          /* additional X offset */
    X_End = LCD_PIXELS_X - 1 + LCD_OFFSET_X;
  #else
    X_Start = 0;
    X_End = LCD_PIXELS_X - 1;
  #endif
  Y_Start = 0;
  Y_End = LCD_PIXELS_Y - 1;
  LCD_AddressWindow();

  /* power on */
  MilliSleep(115);                 /* pause for 120ms (blanking sequence) */
  LCD_Cmd(CMD_SLEEP_OUT);          /* leave sleep mode */
  MilliSleep(120);                 /* pause for 120ms (booster & clocks) */
  #ifndef LCD_LATE_ON
    /* turn on display early as visual feedback */
    LCD_Cmd(CMD_DISPLAY_ON);         /* enable display output */
  #endif

  /* update maximums */
  UI.CharMax_X = LCD_CHAR_X;       /* characters per line */
  UI.CharMax_Y = LCD_CHAR_Y;       /* lines */
  #ifdef SW_SYMBOLS
  UI.SymbolSize_X = LCD_SYMBOL_CHAR_X;  /* x size in chars */
  UI.SymbolSize_Y = LCD_SYMBOL_CHAR_Y;  /* y size in chars */
  #endif

  /* set default color in case the color feature is disabled */
  #ifndef LCD_COLOR
    UI.PenColor = COLOR_PEN;       /* set pen color */
  #endif

  LineMask = 0xffff;               /* clear all lines by default */
  LCD_Clear();                     /* clear display */
  #ifdef LCD_LATE_ON
    /* turn on display after clearing it */
    LCD_Cmd(CMD_DISPLAY_ON);         /* enable display output */  
  #endif
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
  uint8_t           Index;         /* font index */
  uint16_t          Offset;        /* address offset */
  uint8_t           Pixels;        /* pixels in y direction */
  uint8_t           x;             /* bitmap x byte counter */
  uint8_t           y = 1;         /* bitmap y byte counter */
  uint8_t           Bits;          /* number of bits to be sent */
  uint8_t           n;             /* bitmap bit counter */

  /* prevent x overflow */
  if (UI.CharPos_X > LCD_CHAR_X) return;

  /* get font index number from lookup table */
  Table = (uint8_t *)&FontTable;        /* start address */
  Table += Char;                        /* add offset for character */
  Index = pgm_read_byte(Table);         /* get index number */
  if (Index == 0xff) return;            /* no character bitmap available */

  /* calculate start address of character bitmap */
  Table = (uint8_t *)&FontData;        /* start address of font data */
  Offset = FONT_BYTES_N * Index;       /* offset for character */
  Table += Offset;                     /* address of character data */

  /* LCD's address window */
  LCD_CharPos(UI.CharPos_X, UI.CharPos_Y);   /* update character position */
                                             /* also updates X_Start and Y_Start */
  X_End = X_Start + FONT_SIZE_X - 1;   /* offset for end */
  Y_End = Y_Start + FONT_SIZE_Y - 1;   /* offset for end */
  LCD_AddressWindow();                 /* set address window */

  Offset = UI.PenColor;                /* get pen color */
  LCD_Cmd(CMD_MEM_WRITE);              /* start writing */

  /* read character bitmap and send it to display */
  while (y <= FONT_BYTES_Y)
  {
    Pixels = FONT_SIZE_X;               /* track x bits to be sent */
    x = 1;                              /* reset counter */

    /* read and send all bytes for this row */
    while (x <= FONT_BYTES_X)
    {
      /* track x bits */
      if (Pixels >= 8) Bits = 8;
      else Bits = Pixels;
      Pixels -= Bits;

      Index = pgm_read_byte(Table);     /* read byte */

      /* send color for each bit */
      n = Bits;
      while (n > 0)
      {
        if (Index & 0b00000001)         /* bit set */
        {
          LCD_Data2(Offset);            /* foreground color */
        }
        else                            /* bit unset */
        {
          LCD_Data2(COLOR_BACKGROUND);  /* background color */
        }

        Index >>= 1;                      /* shift byte for next bit */
        n--;                              /* next bit */
      }

      Table++;                          /* address for next byte */
      x++;                              /* next byte */
    }

    y++;                                /* next row */
  }

  UI.CharPos_X++;             /* update character position */
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
  LCD_CharPos(LCD_CHAR_X, LCD_CHAR_Y);     /* move to bottom right */

  if (Mode)              /* cursor on */
  {
    LCD_Char('>');
  }
  else                   /* cursor off */
  {
    LCD_Char(' ');
  }
}



/* ************************************************************************
 *   fancy stuff
 * ************************************************************************ */


#ifdef SW_SYMBOLS

/*
 *  display a component symbol
 *
 *  requires:
 *  - ID: symbol to display
 */

void LCD_Symbol(uint8_t ID)
{
  uint8_t           *Table;        /* pointer to symbol table */
  uint8_t           *Table2;       /* pointer */
  uint8_t           Data;          /* symbol data */
  uint16_t          Offset;        /* address offset */
  uint8_t           Pixels;        /* pixels in y direction */
  uint8_t           x;             /* bitmap x byte counter */
  uint8_t           y = 1;         /* bitmap y byte counter */
  uint8_t           Bits;          /* number of bits to be sent */
  uint8_t           n;             /* bitmap bit counter */
  uint8_t           factor = SYMBOL_RESIZE;  /* resize factor */

  /* calculate start address of character bitmap */
  Table = (uint8_t *)&SymbolData;       /* start address of symbol data */
  Offset = SYMBOL_BYTES_N * ID;         /* offset for symbol */
  Table += Offset;                      /* address of symbol data */

  /* LCD's address window */
  LCD_CharPos(UI.CharPos_X, UI.CharPos_Y);   /* update character position */
                                             /* also updates X_Start and Y_Start */
  X_End = X_Start + (SYMBOL_SIZE_X * SYMBOL_RESIZE) - 1;  /* offset for end */
  Y_End = Y_Start + (SYMBOL_SIZE_Y * SYMBOL_RESIZE) - 1;  /* offset for end */
  LCD_AddressWindow();                  /* set address window */

  Offset = UI.PenColor;                 /* get pen color */
  LCD_Cmd(CMD_MEM_WRITE);               /* start writing */

  /* read character bitmap and send it to display */
  while (y <= SYMBOL_BYTES_Y)
  {
    Table2 = Table;           /* save current pointer */

    while (factor > 0)        /* resize symbol */
    {
      Table = Table2;                   /* reset start pointer */

      Pixels = SYMBOL_SIZE_X;           /* track x bits to be sent */
      x = 1;                            /* reset counter */

      /* read and send all bytes for this row */
      while (x <= SYMBOL_BYTES_X)
      {
        /* track x bits */
        if (Pixels >= 8) Bits = 8;
        else Bits = Pixels;
        Pixels -= Bits;

        Data = pgm_read_byte(Table);    /* read byte */

        /* send color for each bit */
        n = Bits;                       /* reset counter */
        n *= SYMBOL_RESIZE;             /* and consider size factor */

        while (n > 0)                   /* x pixels */
        {
          if (Data & 0b00000001)             /* bit set */
          {
            LCD_Data2(Offset);               /* foreground color */
          }
          else                               /* bit unset */
          {
            LCD_Data2(COLOR_BACKGROUND);     /* background color */
          }

          n--;                          /* next pixel */

          if (n % SYMBOL_RESIZE == 0)   /* for every resize step */
          {
            Data >>= 1;                 /* shift byte for next bit */
          }
        }

        Table++;                        /* address for next byte */
        x++;                            /* next byte */
      }

      factor--;                    /* one part done */
    }

    if (factor == 0)               /* all parts done */
    {
      factor = SYMBOL_RESIZE;      /* reset resize factor */
      y++;                         /* next row */
    }              
  }

  /* mark text lines as used */
  n = LCD_SYMBOL_CHAR_Y;           /* set line counter */
  x = UI.SymbolPos_Y;              /* start line */
  while (n > 1)                    /* first line already set */
  {
    x++;                           /* next line */
    LCD_CharPos(1, x);             /* mark line */
    n--;                           /* next line */
  }
}

#endif



/* ************************************************************************
 *   clean-up of local constants
 * ************************************************************************ */

/* source management */
#undef LCD_DRIVER_C

#endif

/* ************************************************************************
 *   EOF
 * ************************************************************************ */
