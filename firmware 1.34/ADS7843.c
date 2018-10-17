/* ************************************************************************
 *
 *   driver functions for ADS7843 compatible touchscreen controller
 *   (compatibles: XPT2046)
 *
 *   (c) 2015-2017 by Markus Reschke
 *
 * ************************************************************************ */

/*
 *  hints:
 *  - pin assignment
 *    DCLK          SPI: SCK
 *    DIN           SPI: MOSI
 *    DOUT          SPI: MISO
 *    /CS           TOUCH_CS
 *    /PENIRQ       TOUCH_PEN
 *  - max. SPI clock: 2 MHz
 */


/* local includes */
#include "config.h"           /* global configuration */

#ifdef TOUCH_ADS7843


/*
 *  local constants
 */

/* source management */
#define TOUCH_DRIVER_C


/*
 *  include header files
 */

/* local includes */
#include "common.h"           /* common header file */
#include "colors.h"           /* color definitions */
#include "variables.h"        /* global variables */
#include "functions.h"        /* external functions */
#include "ADS7843.h"          /* ADS7843 specifics */


/*
 *  local variables
 */

#ifdef SPI_HARDWARE
/* SPI */
uint8_t             ClockRate;     /* SPI clock rate bits */
uint8_t             OldClockRate;  /* SPI clock rate bits */
#endif



/* ************************************************************************
 *   low level functions for SPI interface
 * ************************************************************************ */


/*
 *  set up interface bus
 *  - should be called at firmware startup
 */

void Touch_BusSetup(void)
{
  uint8_t           Bits;          /* bitmask */

  /*
   *  set control signals
   */

  Bits = TOUCH_DDR;                     /* get current directions */

  /* required pins */
  Bits |= (1 << TOUCH_CS);              /* /CS */
  Bits &= ~(1 << TOUCH_PEN);            /* /PEN_IRQ */

  TOUCH_DDR = Bits;                     /* set new directions */

  /* set default levels */
  TOUCH_PORT |= (1 << TOUCH_CS);        /* set /CS high */


  /*
   *  init SPI bus
   */

  #ifdef SPI_HARDWARE
  /*
   *  set SPI clock rate (max. 2MHz)
   */

  /* 1MHz -> f_osc/2 (SPR1 = 0, SPR0 = 0, SPI2X = 1) */
  #if CPU_FREQ / 1000000 == 1
    ClockRate = SPI_CLOCK_2X;
  #endif

  /* 8MHz -> f_osc/4 (SPR1 = 0, SPR0 = 0, SPI2X = 0) */
  #if CPU_FREQ / 1000000 == 8
    ClockRate = 0
  #endif

  /* 16MHz -> f_osc/8 (SPR1 = 0, SPR0 = 1, SPI2X = 1) */
  #if CPU_FREQ / 1000000 == 16
    ClockRate = SPI_CLOCK_R0 | SPI_CLOCK_2X;
  #endif

  /* 20MHz -> f_osc/16 (SPR1 = 0, SPR0 = 1, SPI2X = 0) */
  #if CPU_FREQ / 1000000 == 20
    ClockRate = SPI_CLOCK_R0;
  #endif

  #endif

  SPI_Setup();                /* set up SPI bus */
}



/*
 *  send control byte and return result of conversion
 *
 *  required:
 *  - byte value to send
 */

uint16_t Touch_Xfer(uint8_t Command)
{
  uint16_t          Value;    /* return value */
  uint8_t           Byte;     /* */

  /* send control byte */
  SPI_Write_Byte(Command);              /* write control byte */

  /*
   *  send two dummy bytes and read conversion value (12 bits)
   *  - first byte: 0 followed by first 7 bits
   *  - second byte: last 5 bits followed by 0s
   */ 

  Value = SPI_WriteRead_Byte(0);        /* write dummy byte #1 */
  Value <<= 5;                          /* shift to MSB position */

  Byte = SPI_WriteRead_Byte(0);         /* write dummy byte #2 */
  Byte >>= 3;                           /* remove padding */

  Value |= Byte;                        /* combine bytes */

  return Value;
}



/*
 *  select touch controller
 *  - also update clock for hardware SPI
 */

void Touch_SelectChip(void)
{
  /* select chip */
  TOUCH_PORT &= ~(1 << TOUCH_CS);       /* set /CS low */

  #ifdef SPI_HARDWARE
  /* change SPI clock for touch controller */
  OldClockRate = SPI.ClockRate;         /* save old clock settings */
  SPI.ClockRate = ClockRate;            /* set new clock rate */
  SPI_Clock();                          /* update SPI clock */
  #endif
}



/*
 *  deselect touch controller
 *  - also update clock for hardware SPI
 */

void Touch_DeselectChip(void)
{
  /* disable chip */
  TOUCH_PORT |= (1 << TOUCH_CS);        /* set /CS high */

  #ifdef SPI_HARDWARE
  /* change SPI clock for display controller */
  SPI.ClockRate = OldClockRate;         /* set old clock rate */
  SPI_Clock();                          /* update SPI clock */
  #endif
}



/*
 *  get X and Y values from controller
 */

void Touch_Read_XY(void)
{
  uint8_t           n = 1;    /* counter */
  uint16_t          X = 0;    /* raw X position */
  uint16_t          Y = 0;    /* raw Y position */

  /*
   *  We use the differential reference and 12 bit resolution
   *  - hint: power mode "always on" disables /PEN_IRQ
   */

  Touch_SelectChip();                   /* select chip */

  /*
   *  todo: optimize SPI communication
   *  - send next control byte as third byte of current xfer
   *  - Touch_Xfer(Cmd1, Cmd2)
   *    - send startbyte if Cmd1 != 0
   *      Cmd1 = 0 creates 2 byte xfer for last run
   *    - send Cmd2 in third byte
   */

  /*
   *  todo:
   *  - don't just average 4 samples
   *  - check if positions are within some margin window
   *  - add return value (flag)?
   */

  while (n <= 4)              /* take 4 samples */
  {
    /* get raw X value */
    X += Touch_Xfer(FLAG_STARTBIT | FLAG_CHAN_X | FLAG_DFR | FLAG_PWR_ALWAYS);

    /* get raw Y value */
    Y += Touch_Xfer(FLAG_STARTBIT | FLAG_CHAN_Y | FLAG_DFR | FLAG_PWR_ALWAYS);

    n++;            /* next round */
  }

  X /= 4;           /* calculate average */
  Y /= 4;           /* calculate average */

  /* dummy conversion to enable /PEN_IRQ again */
  Touch_Xfer(FLAG_STARTBIT | FLAG_CHAN_X | FLAG_SER | FLAG_PWR_DOWN_1);

  Touch_DeselectChip();                 /* disable chip */

  /* update global variables */
  UI.TouchRaw_X = X;
  UI.TouchRaw_Y = Y;
}



/* ************************************************************************
 *   high level functions
 * ************************************************************************ */


/*
 *  convert raw position into char position
 */

void Touch_CharPos(void)
{
  uint8_t           CPos;          /* char posistion */
  uint16_t          Pos;           /* position */
  uint16_t          Factor;        /* correlation factor */

  /*
   *  convert raw touch position to char position
   *  - consider possible rotation and flips
   *  - consider offset between touch screen and display
   *  - 0/0 is assumend to be at top left 
   *
   *  Conversion:
   *  - f_corr = (Offset_right - Offset_left) / CharMax_X
   *  - CharPos_x = ((RawPos_x - Offset_left) / f_corr) + 1
   */

  #ifdef TOUCH_ROTATE

    /*
     *  swap X and Y (rotate by 90°)
     */

    /*
     *  X direction, but take Y as input
     */

    Factor = Touch.Y_Bottom - Touch.Y_Top;  /* delta of offsets */
    Factor /= UI.CharMax_Y;        /* correlation factor */
    Pos = UI.TouchRaw_Y;           /* get raw position */
    if (Pos < Touch.Y_Top)         /* prevent underun */
    {
      Pos = Touch.Y_Top;
    }
    if (Pos > Touch.Y_Bottom)      /* prevent overrun */
    {
      Pos = Touch.Y_Bottom;
    }

    #ifdef TOUCH_FLIP_X
      /* flip X direction */
      Pos -= Touch.Y_Bottom;       /* consider offset for right/bottom */
    #else
      /* native X direction */
      Pos -= Touch.Y_Top;          /* consider offset for left/top */
    #endif

    Pos /= Factor;                 /* convert to char position */
    CPos = (uint8_t)Pos;
    CPos++;                        /* starts at 1, not 0 */
    if (CPos > UI.CharMax_X)       /* prevent overrun */
    {
      CPos = UI.CharMax_X;
    }
    UI.TouchPos_X = CPos;          /* update X position */


    /*
     *  Y direction, but take X as input
     */

    Factor = Touch.X_Right - Touch.X_Left;  /* delta of offsets */
    Factor /= UI.CharMax_Y;        /* correlation factor */
    Pos = UI.TouchRaw_X;           /* get raw position */
    if (Pos < Touch.X_Left)        /* prevent underun */
    {
      Pos = Touch.X_Left;
    }
    if (Pos > Touch.X_Right)       /* prevent overrun */
    {
      Pos = Touch.X_Right;  
    }

    #ifdef TOUCH_FLIP_Y
      /* flip Y direction */
      Pos -= Touch.X_Right;        /* consider offset for bottom/right */
    #else
      /* native Y direction */
      Pos -= Touch.X_Left;         /* consider offset for top/left */
    #endif

    Pos /= Factor;                 /* convert to char position */
    CPos = (uint8_t)Pos;
    CPos++;                        /* starts at 1, not 0 */
    if (CPos > UI.CharMax_Y)       /* prevent overrun */
    {
      CPos = UI.CharMax_Y; 
    }
    UI.TouchPos_Y = CPos;          /* update Y position */

  #else

    /*
     *  keep native X and Y
     */

    /*
     *  X direction
     */

    Factor = Touch.X_Right - Touch.X_Left;  /* delta of offsets */
    Factor /= UI.CharMax_X;        /* correlation factor */
    Pos = UI.TouchRaw_X;           /* get raw position */
    if (Pos < Touch.X_Left)        /* prevent underun */
    {
      Pos = Touch.X_Left;
    }
    if (Pos > Touch.X_Right)       /* prevent overrun */
    {
      Pos = Touch.X_Right;
    }

    #ifdef TOUCH_FLIP_X
      /* flip X direction */
      Pos -= Touch.X_Right;        /* consider offset for right side */
    #else
      /* native X direction */
      Pos -= Touch.X_Left;         /* consider offset for left side */
    #endif

    Pos /= Factor;                 /* convert to char position */
    CPos = (uint8_t)Pos;
    CPos++;                        /* starts at 1, not 0 */
    if (CPos > UI.CharMax_X)       /* prevent overrun */
    {
      CPos = UI.CharMax_X;
    }
    UI.TouchPos_X = CPos;          /* update X position */


    /*
     *  Y direction
     */

    Factor = Touch.Y_Bottom - Touch.Y_Top;  /* delta of offsets */
    Factor /= UI.CharMax_Y;        /* correlation factor */
    Pos = UI.TouchRaw_Y;           /* get raw position */
    if (Pos < Touch.Y_Top)         /* prevent underun */
    {
      Pos = Touch.Y_Top;
    }
    if (Pos > Touch.Y_Bottom)      /* prevent overrun */
    {
      Pos = Touch.Y_Bottom;  
    }

    #ifdef TOUCH_FLIP_Y
      /* flip Y direction */
      Pos -= Touch.Y_Bottom;       /* consider offset for bottom */
    #else
      /* native Y direction */
      Pos -= Touch.Y_Top;          /* consider offset for top */
    #endif

    Pos /= Factor;                 /* convert to char position */
    CPos = (uint8_t)Pos;
    CPos++;                        /* starts at 1, not 0 */
    if (CPos > UI.CharMax_Y)       /* prevent overrun */
    {
      CPos = UI.CharMax_Y; 
    }
    UI.TouchPos_Y = CPos;          /* update Y position */

  #endif
}



/*
 *  initialize touch screen controller
 */
 
void Touch_Init(void)
{
  /*
   *  ADS7843 doesn't need any specific setup, since everything is
   *  managed by a control byte sent for each action. But to enable
   *  /PEN_IRQ we have to run a dummy conversion.
   */

  Touch_SelectChip();                   /* select chip */

  /* dummy conversion to enable /PEN_IRQ */
  Touch_Xfer(FLAG_STARTBIT | FLAG_CHAN_X | FLAG_SER| FLAG_PWR_DOWN_1);

  Touch_DeselectChip();                 /* disable chip */
}



/*
 *  check touch screen and get position when touched
 *
 *  returns:
 *  - 0 not touched
 *  - 1 touched
 */

uint8_t Touch_Check(void)
{
  uint8_t           Flag = 0;      /* return value */
  uint8_t           Test;

  /* check for /PEN_IRQ */
  Test = TOUCH_PIN;                /* read port */
  Test &= (1 << TOUCH_PEN);        /* filter /PEN_IRQ */
  if (Test == 0)                   /* /PEN_IRQ low */
  {
    /* touch event */
    Touch_Read_XY();          /* read raw position */
    Touch_CharPos();          /* and convert to char position */

    Flag = 1;                 /* signal touch event */
  }

  return Flag;
}



/*
 *  touch screen adjustment for a specific character position
 *
 *  requires:
 *  - X character posistion (1-)
 *  - Y character posistion (1-)
 *
 *  returns:
 *  - 0 on error/abort
 *  - 1 on touch event
 *  - 2 on matching char position
 */

uint8_t Touch_AdjustPos(uint8_t Char_X, uint8_t Char_Y)
{
  uint8_t           Flag = 0;      /* return value */

  /* display marker */
  LCD_CharPos(Char_X, Char_Y);

  #ifdef LCD_COLOR
  UI.PenColor = COLOR_SYMBOL;         /* set pen color */
  #endif
  LCD_Char('*');                      /* display: * */
  #ifdef LCD_COLOR
  UI.PenColor = COLOR_PEN;            /* reset pen color */
  #endif

  /* wait for user */
  while (Flag == 0)
  {
    MilliSleep(30);           /* delay */

    /* check test key (to abort adjustment) */
    if (!(CONTROL_PIN & (1 << TEST_BUTTON)))      /* test button pressed */
    {
      break;                  /* abort */
    }

    Flag = Touch_Check();     /* check for touch event */
  }

  /* clear marker */
  LCD_CharPos(Char_X, Char_Y);
  LCD_Char(' ');

  /* check for matching char position */
  if (Flag == 1)                   /* touch event */
  {
    if ((UI.TouchPos_X == Char_X) && (UI.TouchPos_Y == Char_Y))
    {
      Flag++;            /* 2: signal matching char pos */
    }

    /* feedback for user */
    LCD_ClearLine2();
    LCD_Char('x');
    Display_FullValue(UI.TouchRaw_X, 0, 0);
    LCD_Char(' ');
    LCD_Char('y');
    Display_FullValue(UI.TouchRaw_Y, 0, 0);

    MilliSleep(500);               /* delay */
  }

  return Flag;
}



/*
 *  adjustment for touch screen
 *  - get offsets for left, right, top & bottom
 *  - we ignore rotation (small screen, low resolution)
 *
 *  returns:
 *  - 0 on error/abort
 *  - 1 on success
 */

uint8_t Touch_Adjust(void)
{
  uint8_t           Flag1;         /* control flag */
  uint8_t           Flag2;         /* control flag */
  uint8_t           n;             /* counter */
  uint16_t          X_Left, X_Right;    /* raw position */
  uint16_t          Y_Top, Y_Bottom;    /* raw position #2 */

  /* tell user */
  LCD_Clear();                     /* clear display */
  LCD_CharPos(1, 1);
  // todo: make sure we output string only to display
  Display_EEString(TouchSetup_str);


  /*
   *  adjustment
   */

  n = 0;
  while (n < 3)               /* up to 3 tries */
  {
    n++;                      /* increase counter */
    Flag2 = 0;                /* reset flag */

    /*
     *  top right
     */

    Flag1 = Touch_AdjustPos(UI.CharMax_X, 1);

    /* get offset for X direction */
    #ifdef TOUCH_FLIP_X
      X_Left = UI.TouchRaw_X;
    #else
      X_Right = UI.TouchRaw_X;
    #endif

    /* get offset for  direction */
    #ifdef TOUCH_FLIP_Y
      Y_Bottom = UI.TouchRaw_Y;
    #else
      Y_Top = UI.TouchRaw_Y;
    #endif


    /*
     *  bottom left
     */

    if (Flag1)
    {
      Flag2 = Touch_AdjustPos(1, UI.CharMax_Y);

      /* get offset for X direction */
      #ifdef TOUCH_FLIP_X
        X_Right = UI.TouchRaw_X;
      #else
        X_Left = UI.TouchRaw_X;
      #endif

      /* get offset for Y direction */
      #ifdef TOUCH_FLIP_Y
        Y_Top = UI.TouchRaw_Y;
      #else
       Y_Bottom = UI.TouchRaw_Y;
      #endif
    }


    /*
     *  control logic
     */

    if (Flag1 && Flag2)            /* got both touch events */
    {
      /* sanity check */
      if ((X_Right > X_Left) && (Y_Bottom > Y_Top))    /* valid offsets */
      {
        /* check for matching char positions */
        Flag1 += Flag2;
        if (Flag1 == 4)                 /* matching char pos */
        {
          break;                        /* job done */
        }
        else                            /* no matching char pos */
        {
          /* update offsets for next run */
          Touch.X_Left = X_Left;
          Touch.X_Right = X_Right;
          Touch.Y_Top = Y_Top;
          Touch.Y_Bottom = Y_Bottom;
        }
      }
      else                                             /* invalid offsets */
      {
        n = 5;                     /* end loop */
      }
    }
    else                           /* abort */
    {
      n = 4;                       /* end loop */
    }
  }

  if (n <= 3) n = 1;          /* signal success */
  else n = 0;                 /* signal error */

  return n;
}



/* ************************************************************************
 *   clean-up of local constants
 * ************************************************************************ */

/* source management */
#undef TOUCH_DRIVER_C

#endif

/* ************************************************************************
 *   EOF
 * ************************************************************************ */
