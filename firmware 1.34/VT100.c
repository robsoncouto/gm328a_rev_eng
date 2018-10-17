/* ************************************************************************
 *
 *   virtual LCD display for VT100 serial terminal
 *
 *   (c) 2018 by Markus Reschke
 *
 * ************************************************************************ */


/* local includes */
#include "config.h"           /* global configuration */

#ifdef LCD_VT100


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


/*
 *  local constants
 */

/* escape */
#define VT100_ESCAPE     0x1b      /* escape character */


/*
 *  local variables
 */

#ifdef LCD_COLOR
/* color management */
uint8_t             Color = 0;     /* foreground color ID */
#endif



/* ************************************************************************
 *   low level functions
 * ************************************************************************ */


/*
 *  set up interface bus
 *  - should be called at firmware startup
 */

void LCD_BusSetup(void)
{
  /*
   *  TTL serial is set up in main(). So we don't have to do
   *  anything here.
   */
}



/* ************************************************************************
 *   support functions
 * ************************************************************************ */


/*
 *  send number as ASCII digits
 *
 *  requires:
 *  - number (0-255)
 */

void LCD_SendNumber(uint8_t Number)
{
  uint8_t           Run = 1;       /* loop control */
  uint8_t           Digit;         /* decimal digit */

  while (Run)
  {
    if (Number >= 100)             /* 3 digits */
    {
      Digit = Number / 100;        /* get first digit */
      Number -= Digit * 100;       /* update value */
    }
    else if (Number >= 10)         /* 2 digits */
    {
      Digit = Number / 10;         /* get first digit */
      Number -= Digit * 10;        /* update value */
    }
    else                           /* one digit */
    {
      Digit = Number;              /* take this digit */
      Run = 0;                     /* end loop */
    }

    Serial_Char('0' + Digit);      /* send digit */
  }
}



/*
 *  set color
 *  - foreground, background and special features
 *
 *  requires:
 *  - color ID
 */

void LCD_SetColor(uint8_t ID)
{
  /* set color: Esc[<color>m */
  Serial_Char(VT100_ESCAPE);       /* send: escape */
  Serial_Char('[');                /* send: [ */
  LCD_SendNumber(ID);              /* send color ID */
  Serial_Char('m');                /* send: m */
}



/* ************************************************************************
 *   high level functions
 * ************************************************************************ */


/*
 *  set LCD character position
 *  - top left: 1/1
 *
 *  requires:
 *  - x:  horizontal position (1-)
 *  - y:  vertical position (1-)
 */

void LCD_CharPos(uint8_t x, uint8_t y)
{
  /* update UI */
  UI.CharPos_X = x;
  UI.CharPos_Y = y;

  /* move cursor to screen location x,y: Esc[<y>;<x>H */
  Serial_Char(VT100_ESCAPE);       /* send: escape */
  Serial_Char('[');                /* send: [ */
  LCD_SendNumber(y);               /* send y pos */
  Serial_Char(';');                /* send: ; */
  LCD_SendNumber(x);               /* send x pos */
  Serial_Char('H');                /* send: H */
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
  uint8_t           X = 1;              /* x position */

  if (Line == 0)              /* special case: rest of current line */
  {
    Line = UI.CharPos_Y;      /* get current line */
    X = UI.CharPos_X;         /* get current character position */
  }

  LCD_CharPos(X, Line);       /* set char position */

  /*
   *  clear entire line: Esc[2K
   *  clear line from cursor right: Esc[K
   */

  Serial_Char(VT100_ESCAPE);       /* send: escape */
  Serial_Char('[');                /* send: [ */

  if (X == 1)                      /* complete line */
  {
    Serial_Char('2');              /* send: 2 */
  }

  Serial_Char('K');                /* send: K */
}



/*
 *  clear the display 
 */

void LCD_Clear(void)
{
  /* clear entire screen: Esc[2J */
  Serial_Char(VT100_ESCAPE);       /* send: escape */
  Serial_Char('[');                /* send: [ */
  Serial_Char('2');                /* send: 2 */
  Serial_Char('J');                /* send: J */

  LCD_CharPos(1, 1);               /* reset character position */
}



/*
 *  initialize LCD
 */
 
void LCD_Init(void)
{
  /* update maximums */
  UI.CharMax_X = LCD_CHAR_X;       /* characters per line */
  UI.CharMax_Y = LCD_CHAR_Y;       /* lines */

  /* make cusor invisible: Esc[?25l */
  Serial_Char(VT100_ESCAPE);       /* send: escape */
  Serial_Char('[');                /* send: [ */
  Serial_Char('?');                /* send: ? */
  Serial_Char('2');                /* send: 2 */
  Serial_Char('5');                /* send: 5 */
  Serial_Char('l');                /* send: l */

  #ifdef LCD_COLOR
  /* set backgound color ID (= foreground color ID + 10) */
  LCD_SetColor(COLOR_BACKGROUND + 10);
  #endif

  LCD_Clear();                     /* clear display */
}


/*
 *  display a single character
 *
 *  requires:
 *  - Char: character to display
 */

void LCD_Char(unsigned char Char)
{
  /* prevent x overflow */
  if (UI.CharPos_X > LCD_CHAR_X) return;

  #ifdef LCD_COLOR
  /* only set color after a change */
  if (UI.PenColor != Color)        /* color has changed */
  {
    Color = UI.PenColor;           /* update color ID */
    LCD_SetColor(Color);           /* and set new color */
  }
  #endif

  Serial_Char(Char);               /* send character */

  /* update character position */
  UI.CharPos_X++;                  /* next character in current line */
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
  LCD_CharPos(LCD_CHAR_X, LCD_CHAR_Y);       /* move to bottom right */

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
 *   clean-up of local constants
 * ************************************************************************ */


/* source management */
#undef LCD_DRIVER_C

#endif

/* ************************************************************************
 *   EOF
 * ************************************************************************ */
