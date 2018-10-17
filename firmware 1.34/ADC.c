/* ************************************************************************
 *
 *   ADC functions
 *
 *   (c) 2012-2017 by Markus Reschke
 *   based on code from Markus Frejek and Karl-Heinz Kübbeler
 *
 * ************************************************************************ */


/*
 *  local constants
 */

/* source management */
#define ADC_C


/*
 *  include header files
 */

/* local includes */
#include "config.h"           /* global configuration */
#include "common.h"           /* common header file */
#include "variables.h"        /* global variables */
#include "functions.h"        /* external functions */


/* ************************************************************************
 *   ADC
 * ************************************************************************ */


/*
 *  read ADC and return voltage in mV
 *  - use Vcc as reference by default
 *  - switch to bandgap reference for low voltages (< 1.0V) to improve
 *    ADC resolution
 *  - with a 125kHz ADC clock a single conversion needs about 0.1ms
 *    with 25 samples we end up with about 2.6ms
 *
 *  requires:
 *  - Probe: input channel of ADC MUX (lower 4 or 5 bits)
 *           must not include setting of voltage reference
 *
 */

uint16_t ReadU(uint8_t Probe)
{
  uint16_t          U;             /* return value (mV) */
  uint8_t           Counter;       /* loop counter */
  uint8_t           Bits;          /* reference bits */
  uint32_t          Value;         /* ADC value */

  Probe |= ADC_REF_VCC;            /* use AVcc as default reference */
                                   /* and external buffer cap anyway */

sample:

  ADMUX = Probe;                   /* set input channel and U reference */

  /* 
   *  dummy conversion
   *  - if voltage reference has changed run a dummy conversion
   *  - recommended by datasheet
   */

  Bits = Probe & ADC_REF_MASK;     /* get reference bits */
  if (Bits != Cfg.RefFlag)         /* reference has changed */
  {
    wait100us();                   /* time for voltage stabilization */

    ADCSRA |= (1 << ADSC);         /* start conversion */
    while (ADCSRA & (1 << ADSC));  /* wait until conversion is done */

    Cfg.RefFlag = Bits;            /* update bits */
  }


  /*
   *  sample ADC readings
   */

  Value = 0UL;                     /* reset sampling variable */
  Counter = 0;                     /* reset counter */

  while (Counter < Cfg.Samples)    /* take samples */
  {
    ADCSRA |= (1 << ADSC);         /* start conversion */
    while (ADCSRA & (1 << ADSC));  /* wait until conversion is done */

    Value += ADCW;                 /* add ADC reading */

    /* auto-switch voltage reference for low readings */
    if (Counter == 4)                   /* 5 samples */
    {
      if ((uint16_t)Value < 1024)       /* < 1V (5V / 5 samples) */
      {
        if (Bits != ADC_REF_BANDGAP)    /* bandgap ref not selected */
        {
          if (Cfg.AutoScale == 1)       /* autoscaling enabled */
          {
            Probe &= ~ADC_REF_MASK;     /* clear reference bits */
            Probe |= ADC_REF_BANDGAP;   /* select bandgap reference */

            goto sample;                /* re-run sampling */
          }
        }
      }
    }

    Counter++;                     /* one less to do */
  }


  /*
   *  convert ADC reading to voltage
   *  - single sample: U = ADC reading * U_ref / 1024
   */

  /* get voltage of reference used */
  if (Bits == ADC_REF_BANDGAP)     /* bandgap reference */
  {
    U = Cfg.Bandgap;          /* voltage of bandgap reference */
  }
  else                             /* - */
  {
    U = Cfg.Vcc;              /* voltage of Vcc */   
  }

  /* convert to voltage; */
  Value *= U;                      /* ADC readings * U_ref */
//  Value += 511 * Cfg.Samples;      /* automagic rounding */
  Value /= 1024;                   /* / 1024 for 10bit ADC */

  /* de-sample to get average voltage */
  Value /= Cfg.Samples;
  U = (uint16_t)Value;

  return U; 
}



/* ************************************************************************
 *   convenience functions
 * ************************************************************************ */


/*
 *  wait 5ms and then read ADC
 *  - same as ReadU()
 */

uint16_t ReadU_5ms(uint8_t Probe)
{
   wait5ms();       /* wait 5ms */

   return (ReadU(Probe));
}



/*
 *  wait 20ms and then read ADC
 *  - same as ReadU()
 */

uint16_t ReadU_20ms(uint8_t Probe)
{
  wait20ms();       /* wait 20ms */

  return (ReadU(Probe));
}



/* ************************************************************************
 *   clean-up of local constants
 * ************************************************************************ */


/* source management */
#undef ADC_C



/* ************************************************************************
 *   EOF
 * ************************************************************************ */
