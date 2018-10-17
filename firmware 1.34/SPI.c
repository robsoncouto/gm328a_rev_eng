/* ************************************************************************
 *
 *   SPI (bit-bang & hardware)
 *
 *   (c) 2017-2018 by Markus Reschke
 *
 * ************************************************************************ */

/*
 *  hints:
 *  - port and pins for bit-bang SPI
 *    SPI_PORT      port data register
 *    SPI_DDR       port data direction register
 *    SPI_PIN       port input pins register
 *    SPI_SCK       pin for SCK
 *    SPI_MOSI      pin for MOSI
 *    SPI_MISO      pin for MISO
 *  - For hardware SPI the MCU specific pins are used:
 *    ATmega 328: SCK / MOSI / MISO 
 *    ATmega 644: SCK PB7 / MOSI PB5 / MISO PB6
 *  - /CS and other control signals have to be managed by the specific
 *    chip driver
 */


/* local includes */
#include "config.h"           /* global configuration */

#ifdef HW_SPI


/*
 *  local constants
 */

/* source management */
#define SPI_C


/*
 *  include header files
 */

/* local includes */
#include "common.h"           /* common header file */
#include "variables.h"        /* global variables */
#include "functions.h"        /* external functions */



/* ************************************************************************
 *   functions for bit-bang SPI
 * ************************************************************************ */


#ifdef SPI_BITBANG

/*
 *  set up SPI bus
 *  - SCK, MOSI and MISO lines
 *  - don't care about clock
 */

void SPI_Setup(void)
{
  /* set up bus only once */
  if (Cfg.OP_Mode & OP_SPI) return;

  /*
   *  set up bitbang SPI:
   *  - master mode
   */

  #ifdef SPI_MISO
  /* set MISO line to input mode */
  SPI_DDR &= ~(1 << SPI_MISO);
  #endif

  /* set SCK and MOSI to output mode */
  SPI_DDR |= (1 << SPI_SCK) | (1 << SPI_MOSI);

  /* preset lines to 0 */
  SPI_PORT &= ~((1 << SPI_SCK) | (1 << SPI_MOSI));

  Cfg.OP_Mode |= OP_SPI;      /* bus is set up */
}



/*
 *  write a single byte
 */

void SPI_Write_Byte(uint8_t Byte)
{
  uint8_t           n = 8;         /* counter */

  /* expected state: SCK low / MOSI undefined */

  /*
   *  bitbang 8 bits:
   *  - SPI mode 0 (CPOL = 0, CPHA = 0)
   *    - set MOSI before rising SCK
   *  - MSB first
   */

  while (n > 0)               /* for 8 bits */
  {
    /* get current MSB and set MOSI */
    if (Byte & 0b10000000)    /* 1 */
    {
      /* set MOSI high */
      SPI_PORT |= (1 << SPI_MOSI);
    }
    else                      /* 0 */
    {
      /* set MOSI low */
      SPI_PORT &= ~(1 << SPI_MOSI);
    }

    /* end clock cycle (rising edge takes bit) */
    SPI_PORT |= (1 << SPI_SCK);

    /* start next clock cycle (falling edge) */
    SPI_PORT &= ~(1 << SPI_SCK);

    Byte <<= 1;               /* shift bits one step left */
    n--;                      /* next bit */
  }

  /* current state: SCK low / MOSI undefined */
}



#if SPI_RW

/*
 *  write and read a single byte
 */

uint8_t SPI_WriteRead_Byte(uint8_t Byte)
{
  uint8_t           Byte2 = 0;     /* return value */
  uint8_t           n = 8;         /* counter */
  uint8_t           Temp;          /* temporary value */

  /* expected state: SCK low / MOSI undefined */

  /*
   *  bitbang 8 bits:
   *  - SPI mode 0 (CPOL = 0, CPHA = 0)
   *    - set MOSI before rising SCK
   *    - read MISO after rising SCK
   *  - MSB first
   */

  while (n > 0)               /* for 8 bits */
  {
    /* get current MSB and set MOSI */
    if (Byte & 0b10000000)    /* 1 */
    {
      /* set MOSI high */
      SPI_PORT |= (1 << SPI_MOSI);
    }
    else                      /* 0 */
    {
      /* set MOSI low */
      SPI_PORT &= ~(1 << SPI_MOSI);
    }

    /* end clock cycle (rising edge takes bit) */
    SPI_PORT |= (1 << SPI_SCK);

    /* read MISO */
    Temp = SPI_PIN;              /* read port */
    Temp &= (1 << SPI_MISO);     /* filter MISO */
    Byte2 <<= 1;                 /* shift bits one step left */
    if (Temp)                    /* MISO set */
    {
      Byte2 |= 0b00000001;       /* set bit */
    }

    /* start next clock cycle (falling edge) */
    SPI_PORT &= ~(1 << SPI_SCK);

    Byte <<= 1;               /* shift bits one step left */
    n--;                      /* next bit */
  }

  /* current state: SCK low / MOSI undefined */

  return Byte2;
}

#endif

#endif



/* ************************************************************************
 *   functions for hardware SPI
 * ************************************************************************ */


#ifdef SPI_HARDWARE

/*
 *  set SPI clock rate
 *  - uses SPI.ClockRate for input
 */

void SPI_Clock(void)
{
  uint8_t           Clock;    /* clock rate bits */
  uint8_t           Bits;     /* bits/bitmask */

  Clock = SPI.ClockRate;           /* get clock rate flags */

  Bits = SPCR;                            /* get control register */
  Bits &= ~((1 << SPR1) | (1 << SPR0));   /* clear clock rate bits */

  /* set divider bits */
  if (Clock & SPI_CLOCK_R0) Bits |= (1 << SPR0);
  if (Clock & SPI_CLOCK_R1) Bits |= (1 << SPR1);

  SPCR = Bits;                     /* set new clock rate */


  Bits = 0;                        /* clear "double" bit */

  /* set bit to double SPI speed */
  if (Clock & SPI_CLOCK_2X) Bits = (1 << SPI2X);

  SPSR = Bits;                     /* update register */
}



/*
 *  set up SPI bus
 *  - clock and mode
 *  - lines are set up automatically
 */

void SPI_Setup(void)
{
  uint8_t           Bits;     /* bits/bitmask */

  /* set up bus only once */
  if (Cfg.OP_Mode & OP_SPI) return;

  /* set SCK and MOSI to output mode */
  /* using variable Bits to keep compiler happy */
  Bits = SPI_DDR;
  Bits |= (1 << SPI_SCK) | (1 << SPI_MOSI);
  SPI_DDR = Bits;

  /* MISO is automatically set to input mode by enabling SPI */

  /*
   *  set up hardware SPI
   *  - master mode (MSTR = 1)
   *  - SPI mode 0 (CPOL = 0, CPHA = 0)
   *  - MSB first (DORD = 0)
   *  - polling mode (SPIE = 0)
   */

  /* set mode and enable SPI */
  SPCR = (1 << SPE) | (1 << MSTR);

  /* set clock rate */
  SPI_Clock();

  /* clear SPI interrupt flag, just in case */
  Bits = SPSR;           /* read flag */
  Bits = SPDR;           /* clear flag by reading data */

  Cfg.OP_Mode |= OP_SPI;      /* bus is set up */
}



/*
 *  write a single byte
 */

void SPI_Write_Byte(uint8_t Byte)
{
  /* send byte */
  SPDR = Byte;                     /* start transmission */
  while (!(SPSR & (1 << SPIF)));   /* wait for flag */
  Byte = SPDR;                     /* clear flag by reading data */
}



#if SPI_RW

/*
 *  write and read a single byte
 */

uint8_t SPI_WriteRead_Byte(uint8_t Byte)
{
  uint8_t           Byte2;         /* return value */

  /* send byte */
  SPDR = Byte;                     /* start transmission */
  while (!(SPSR & (1 << SPIF)));   /* wait for flag */

  /* get received byte */
  Byte2 = SPDR;                    /* clear flag by reading data */

  return Byte2;
}

#endif

#endif



/* ************************************************************************
 *   clean-up of local constants
 * ************************************************************************ */

/* source management */
#undef SPI_C

#endif

/* ************************************************************************
 *   EOF
 * ************************************************************************ */
