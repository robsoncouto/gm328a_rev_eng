/* ************************************************************************
 *
 *   inductor measurements
 *
 *   (c) 2012-2016 by Markus Reschke
 *   based on code from Karl-Heinz Kübbeler
 *
 * ************************************************************************ */

/* local includes */
#include "config.h"           /* global configuration */

#ifdef SW_INDUCTOR


/*
 *  local constants
 */

/* source management */
#define INDUCTOR_C


/*
 *  include header files
 */

/* local includes */
#include "common.h"           /* common header file */
#include "variables.h"        /* global variables */
#include "functions.h"        /* external functions */


/*
 *  local defines
 */

/* mode bitmask */
#define MODE_LOW_CURRENT      0b00000001     /* low test current */
#define MODE_HIGH_CURRENT     0b00000010     /* high test current */
#define MODE_DELAYED_START    0b00000100     /* delayed start */



/* ************************************************************************
 *   inductance measurements
 * ************************************************************************ */


/*

Current of an inductor when switching on power:

  i_L(t) = I_0 * (1 - e^(-t R_total / L))

  I_0: end current 

  i_L(t) / I_0 = 1 - e^(-t R_total / L)

  e^(-t R_total / L) = 1 - (i_L(t) / I_0))

With ln(e^x) = x we get:

  -t R_total / L = ln(1 - (i_L(t) / I_0))

  L = -t R_total / ln(1 - (i_L(t) / I_0))

So we can measure the current at a specific time after switching on to
get L.

  R_total = Ri_H + R_L + Rl + Ri_L

  I_0 = 5V / R_total = 5V / (Ri_H + R_L + Rl + Ri_L) 

We get i_L(t) by measuring the voltage accross a shunt resistor. For a proper
time measurement we'll use the integrated analog comparator and a timer, i.e.
we'll wait until the voltage across the shunt resistor reaches the voltage
of the bandgap reference.

  R_shunt = Rl + Ri_L

  i_L(t_stop) = U_ref / R_shunt

  L = -t_stop * R_total / ln(1 - ((U_ref / R_shunt) / (5V / R_total)))

    = -t_stop * R_total / ln(1 - (U_ref * R_total) / (5V * R_shunt))

Instead of calculating L directly we'll use a table with pre-calculated values
to speed up things and keep the firmware small. The table is based on the
the ratio:
  a = (U_ref * R_total) / (5V * R_shunt)

Estimates for minimal and maximal values (R_L max. 2k Ohm):
- min: 1.0V * 720 Ohm / 5V * 700 Ohm = 0.206
- max: 1.2V * 2770 Ohm / 5V * 700 Ohm = 0.950
  999 is maximum due to ln(1-a/1000)
  Hence the maximum R_L supported is 2k Ohms.

Table:
- ratio a = ((U_ref * R_total) / (5V * R_shunt)) * 10^3
  estimated range: 206 - 977
- values are: (-1 / ln(1 - (a * 10^-3))) * 10^3
  - internal scale factor 10^3 
- bc:
  - options: -i -l
  - define x (a) { return (-1 / l(1 - a/1000)) * 1000; }

For a small inductance we have to use a higher test current, i.e. using Ri_L as
current shunt (Rl = 0).

Estimates for minimal and maximal values (R_L max. 40 Ohm):
- min: 1.0V * 40 Ohm / 5V * 20 Ohm = 0,4
- max: 1.2V * 80 Ohm / 5V * 20 Ohm = 0.960
  999 is maximum due to ln(1-a/1000)
  Hence the maximum R_L supported is 40 Ohms.

Since the range overlaps with the low test current we may use a single table.

*/



/*
 *  measure inductance via time between two probe pins
 *  - probes have to be set by UpdateProbes()
 *
 *  requires:
 *  - pointer to time variable (ns)
 *  - measurement mode (low/high current, delayed start)
 *
 *  returns:
 *  - 3 on success
 *  - 2 if inductance is too low
 *  - 1 if inductance is too high (not implemented)
 *  - 0 on any problem
 */

uint8_t MeasureInductance(uint32_t *Time, uint8_t Mode)
{
  uint8_t           Flag = 3;      /* return value */
  uint8_t           Test;          /* test flag */
  uint8_t           Offset;        /* counter offet */
  uint16_t          Ticks_L;       /* timer counter */
  uint16_t          Ticks_H;       /* timer overflow counter */
  uint32_t          Counter;       /* counter */

  /* sanity check */
  if (Time == NULL) return 0;

  DischargeProbes();                    /* try to discharge probes */
  if (Check.Found == COMP_ERROR) return 0;


  /*
   *  measurement modes:
   *  - low current: Gnd -- Rl -- probe-2 / probe-1 -- Vcc
   *  - high current: Gnd -- probe-2 / probe-1 -- Vcc
   */


  /*
   *  init hardware
   */

  /* set probes: Gnd -- (Rl) -- probe-2 / Gnd -- probe-1 */
  R_PORT = 0;                           /* set resistor port to low */
  ADC_PORT = 0;                         /* set ADC port to low */

  if (Mode & MODE_LOW_CURRENT)     /* low current */
  {
    /* set probes: Gnd -- Rl -- probe-2 / Gnd -- probe-1 */
    R_DDR = Probes.Rl_2;                /* pull down probe-2 via Rl */
    ADC_DDR = Probes.Pin_1;             /* pull down probe-1 directly */
  }
  else                             /* high current */
  {
    /* set probes: Gnd -- probe-2 / Gnd -- probe-1 */
    R_DDR = 0;                          /* disable probe resistors */
    /* pull down probe-1 and probe-2 directly */
    ADC_DDR = Probes.Pin_1 | Probes.Pin_2;
  }

  /* set up analog comparator */
  ADCSRA = ADC_CLOCK_DIV;               /* disable ADC, but keep clock dividers */
  ADCSRB = (1 << ACME);                 /* use ADC multiplexer as negative input */
  ADMUX = ADC_REF_BANDGAP | Probes.ADC_2;   /* switch ADC multiplexer to probe-2 */
                                        /* and set AREF to Vcc */
  ACSR = (1 << ACBG) | (1 << ACIC);     /* use bandgap as positive input, trigger timer1 */
  wait1ms();                            /* allow bandgap reference to settle */
  wdt_reset();                          /* reset watchdog */


  /*
   *  set up timer
   */

  Ticks_H = 0;                          /* reset timer overflow counter */
  TCCR1A = 0;                           /* set default mode */
  TCCR1B = 0;                           /* set more timer modes */
  /* timer stopped, falling edge detection, noise canceler disabled */
  TCNT1 = 0;                            /* set Counter1 to 0 */
  /* clear all flags (input capture, compare A & B, overflow */
  TIFR1 = (1 << ICF1) | (1 << OCF1B) | (1 << OCF1A) | (1 << TOV1);

  if (Mode & MODE_DELAYED_START)        /* delayed start */
  {
    Test = MCU_CYCLES_PER_US;           /* MCU cycles per µs */

    /* change probes: Gnd -- (Rl) -- probe-2 / probe-1 -- Vcc */
    ADC_PORT = Probes.Pin_1;            /* pull up probe-1 directly */

    /*
     *  delay timer by about 3-4µs to skip capacitive effects of large inductors
     *  - a loop run needs 4 cycles, the last loop run just 3
     *  - cycles burnt: <MCU cycles per µs> * 4 - 1
     *    time delay: 4µs - 1 MCU cycle
     */

    while (Test > 0)
    {
      Test--;
      asm volatile("nop");              /* 1 MCU cycle */
    }

    TCCR1B |= (1 << CS10);              /* start timer (1/1 clock divider) */
  }
  else                                  /* immediate start */
  {
    TCCR1B |= (1 << CS10);              /* start timer (1/1 clock divider) */
    /* change probes: Gnd -- Rl -- probe-2 / probe-1 -- Vcc */
    ADC_PORT = Probes.Pin_1;            /* pull up probe-1 directly */
  }


  /*
   *  timer loop
   *  - run until voltage threshold is reached
   *  - detect timer overflows
   */

  while (1)
  {
    Test = TIFR1;                       /* get timer1 flags */

    /* end loop if input capture flag is set (= same voltage) */
    if (Test & (1 << ICF1)) break;

    /* detect timer overflow by checking the overflow flag */
    if (Test & (1 << TOV1))
    {
      /* happens at 65.536ms for 1MHz or 8.192ms for 8MHz */
      TIFR1 = (1 << TOV1);              /* reset flag */
      Ticks_H++;                        /* increase overflow counter */

      /* if it takes too long (0.26s) */
      if (Ticks_H == (CPU_FREQ / 250000))
      {
        Flag = 0;             /* signal timeout */
        break;                /* end loop */
      }
    }
  }

  /* stop counter */
  TCCR1B = 0;                           /* stop timer */
  TIFR1 = (1 << ICF1);                  /* reset Input Capture flag */

  Ticks_L = ICR1;                       /* get counter value */

  /* prepare cut off: Gnd -- Rl -- probe-2 / probe-1 -- Rl -- Gnd */
  R_DDR = Probes.Rl_2 | Probes.Rl_1;  

  /* stop current flow */
  ADC_DDR = 0;

  /* catch missed timer overflow */
  if ((TCNT1 > Ticks_L) && (Test & (1 << TOV1)))
  {
    TIFR1 = (1 << TOV1);                /* reset overflow flag */
    Ticks_H++;                          /* increase overflow counter */
  }

  /* enable ADC again */
  ADCSRA = (1 << ADEN) | (1 << ADIF) | ADC_CLOCK_DIV;


  /*
   *  process counters
   */

  if (Flag)                   /* got valid measurement */
  {
    /* combine both counter values */
    Counter = (uint32_t)Ticks_L;             /* lower 16 bits */
    Counter |= (uint32_t)Ticks_H << 16;      /* upper 16 bits */

    /*
     *  offset handling
     */

    Offset = 3;               /* processing overhead */

    /* start offset */
    if (Mode & MODE_DELAYED_START)      /* delayed start */
    {
      /* add MCU cycles for delayed start */
      Counter += (MCU_CYCLES_PER_US * 4) - 1;
    }
    else                                /* immediate start */
    {
      Offset++;               /* timer started one cycle too early */
    }

    /* method and MCU clock specific offset */
    if (Mode & MODE_LOW_CURRENT)        /* low current check */
    {
      /* based on MCU clock */
      #if CPU_FREQ >= 16000000
        Offset += 3;
      #else
        Offset += 1;
      #endif
    }
    #if CPU_FREQ >= 16000000
    else                                /* high current check */
    {
      Offset += 1;
    }
    #endif

    if (Counter > Offset) Counter -= Offset;    /* subtract offset */
    else Counter = 0;                           /* prevent underflow */


    /*
     *  time handling
     */

    /* convert counter (MCU cycles) to time (in ns) */
    Counter *= (10000 / MCU_CYCLES_PER_US);     /* convert to 0.1 ns */
    Counter += 5;                               /* for rounding */
    Counter /= 10;                              /* scale to ns */
    *Time = Counter;               /* save time */

    /* check for lower limit */
    if (Mode & MODE_LOW_CURRENT)   /* low current mode */
    {
      if (Counter < 1250)          /* time < 1.25µs */
      {
        Flag = 2;                  /* signal "inductance too low" */
      }
    }
    else                           /* high current mode */
    {
      if (Counter <= 100)          /* time <= 100ns */
      {
        Flag = 2;                  /* signal "inductance too low" */
      }
    }
  }

  return Flag;
}



/*
 *  measure inductance between two probe pins of a resistor
 *
 *  requires:
 *  - pointer the resistor data structure
 *
 *  returns:
 *  - 1 on success
 *  - 0 on any error
 */

uint8_t MeasureInductor(Resistor_Type *Resistor)
{
  uint8_t           Test = 0;      /* return value / measurement result */
  uint8_t           Mode;          /* measurement mode */
  uint8_t           Scale;         /* scale of value */
  uint16_t          R_total;       /* total resistance */
  uint16_t          Factor;        /* factor */
  uint16_t          Temp;
  int16_t           Offset = 0;    /* offset for U_ref */
  uint32_t          Value;         /* value */
  uint32_t          Time1;         /* time #1 */

  /* reset data */
  Inductor.Scale = 0;
  Inductor.Value = 0;

  /* sanity check */
  if (Resistor == NULL) return Test;

  /* limit resistance to 2k (feasibilty & prevent variable overflow) */ 
  if (CmpValue(Resistor->Value, Resistor->Scale, 2000, 0) >= 0) return Test;


  /*
   *  Manage measurements:
   *  - run in delayed mode to deal with capacitive effects of large inductors
   *    (low current only)
   *  - run in low current mode to check for high inductance
   *  - run in high current mode to check for low inductance, when high
   *    current mode failed and DUT's resistance is low
   */

  UpdateProbes(Resistor->A, Resistor->B, 0);      /* update probes */

  /*
   *  low current & delayed start
   *  - to catch large inductance with capacitive effect
   */

  Mode = MODE_LOW_CURRENT | MODE_DELAYED_START;
  Test = MeasureInductance(&Time1, Mode);
  if (Test == 3)              /* valid measurement */
  {
    /* a valid time should be larger than the delay (4µs) */
    if (Time1 <= 5000)         /* <= 5µs */
    {
      Test = 0;                /* invalid time */
    }
  }


  /*
   *  low current & immediate start
   *  - to check for high inductance
   */

  if (Test == 0)               /* no valid measurement yet */
  {
    Mode = MODE_LOW_CURRENT;
    Test = MeasureInductance(&Time1, Mode);

    if (Test == 2)             /* inductance too low */
    {
      /*
       *  high current & immediate start
       *  - to check for low inductance
       */

      /* if resistance < 40 Ohms we may run the high current test */
      if (CmpValue(Resistor->Value, Resistor->Scale, 40, 0) < 0)
      {
        Mode = MODE_HIGH_CURRENT;
        Test = MeasureInductance(&Time1, Mode);
      }
    }
  }

  if (Test != 3) Test = 0;         /* all measurements failed */


  /*
   *  calculate inductance
   */

  if (Test == 3)                   /* valid measurement */
  {
    /*
     *  resistance
     */

    /* total resistance (in 0.1 Ohms) */
    R_total = RescaleValue(Resistor->Value, Resistor->Scale, -1);  /* R_L */
    Factor = NV.RZero;             /* probe leads (0.01 Ohms) */
    Factor += 5;                   /* for rounding */
    Factor /= 10;                  /* scale to 0.1 Ohms */
    R_total += NV.RiH + NV.RiL + Factor;

    Factor = NV.RiL;          /* shunt resistance (in 0.1 Ohms) */

    if (Mode & MODE_LOW_CURRENT)   /* low current measurement mode */
    {
      /* add R_l */
      R_total += (R_LOW * 10);
      Factor += (R_LOW * 10);

      /*
       *  compensation offset for U_ref
       *  - positive/larger offset -> lower L
       *  - negative/lower offset -> higher L
       */

      #if CPU_FREQ == 8000000
      /* 8 MHz */
      if (Time1 < 6000)       /* < 6µs / < 18mH */
      {
        Offset = 38;
      }
      else if (Time1 < 9500) /* 6-9.5µs / 18-27mH */
      {
        Offset = -10;
      }
      else                    /* > 9.5µs / > 27mH */
      {
        Offset = 20;
      }
      #endif

      #if (CPU_FREQ == 16000000) || (CPU_FREQ == 20000000)
      /* 16 & 20 MHz */
      if (Time1 < 6000)       /* < 6µs / < 18mH */
      {
        Offset = 80;
      }
      else if (Time1 < 12500) /* 6-12.5µs / 18-33mH */
      {
        Offset = 45;
      }
      else                    /* > 12.5µs / > 33mH */
      {
        Offset = 55;
      }
      #endif
    }
    else                           /* high current measurement mode */
    {
      /*
       *  compensation offset for U_ref
       *  - positive/larger offset -> lower L
       *  - negative/lower offset -> higher L
       */

      Temp = (uint16_t)Time1;      /* time < 50µs expected */

      #if CPU_FREQ == 8000000
      /* 8 MHz */
      if (Temp < 1500)        /* < 1.5µs / < 100µH */
      {
        Offset = -10;
      }
      else if (Temp < 5000)   /* 1.5-5µs / 100-330µH */
      {
        Offset = -5; 
      }
      else                    /* > 5µs / > 330µH */
      {
        Offset = -50;
      }
      #endif

      #if CPU_FREQ == 16000000
      /* 16 MHz */
      if (Temp < 1500)        /* < 1.5µs / < 100µH */
      {
        Offset = 10;
      }
      else if (Temp < 5000)   /* 1.5-5µs / 100-330µH */
      {
        Offset = -5; 
      }
      else                    /* > 5µs / > 330µH */
      {
        Offset = -50;
      }
      #endif

      #if CPU_FREQ == 20000000
      /* 20 MHz */
      if (Temp < 1500)        /* < 1.5µs / < 100µH */
      {
        Offset = 10;
      }
      else if (Temp < 5000)   /* 1.5-5µs / 100-330µH */
      {
        Offset = -20; 
      }
      else                    /* > 5µs / > 330µH */
      {
        Offset = -70;
      }
      #endif
    }


    /*
     *  ratio and factor
     *  - ratio = ((U_ref * R_total) / (5V * R_shunt)) * 10^3
     */

    /* calculate ratio */
    Value = Cfg.Bandgap + NV.CompOffset;          /* = U_ref (in mV) */
    Value += Offset;                              /* +/- offset */
    Value *= R_total;                             /* * R_total (in 0.1 Ohms) */
    Value /= Factor;                              /* / R_shunt (in 0.1 Ohms) */
    Value /= 5;                                   /* / 5000mV, * 10^3 */

    /* get ratio based factor */
    Factor = GetFactor((uint16_t)Value, TABLE_INDUCTOR);


   /*
    *  calculate inductance
    *  L = t_stop * R_total * factor
    */

    Scale = -9;               /* nH by default */
    Value = Time1;            /* t_stop (in ns) */

    while (Value > 100000)    /* re-scale to prevent overflow */
    {
      Value += 5;             /* for automagic rounding */
      Value /= 10;            /* scale down by 10^1 */
      Scale++;                /* increase exponent by 1 */
    }

    Value *= Factor;          /* * factor (in 10^-3) */

    while (Value > 100000)    /* re-scale to prevent overflow */
    {
      Value += 5;             /* for automagic rounding */
      Value /= 10;            /* scale down by 10^1 */
      Scale++;                /* increase exponent by 1 */
    }

    Value *= R_total;         /* * R_total (in 0.1 Ohms) */
    Value /= 10000;           /* /10 for 1 Ohms, /1000 for factor */

    /* update data */
    Inductor.Scale = Scale;
    Inductor.Value = Value;
    Test = 1;                 /* signal success */
  }

  return Test;
}



/* ************************************************************************
 *   clean-up of local constants
 * ************************************************************************ */


/* local defines */
#undef MODE_LOW_CURRENT
#undef MODE_HIGH_CURRENT
#undef MODE_DELAYED_START


/* source management */
#undef INDUCTOR_C

#endif

/* ************************************************************************
 *   EOF
 * ************************************************************************ */
