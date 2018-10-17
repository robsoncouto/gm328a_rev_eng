/* ************************************************************************
 *
 *   resistor measurements
 *
 *   (c) 2012-2017 by Markus Reschke
 *   based on code from Markus Frejek and Karl-Heinz Kübbeler
 *
 * ************************************************************************ */


/*
 *  local constants
 */

/* source management */
#define RESISTOR_C


/*
 *  include header files
 */

/* local includes */
#include "config.h"           /* global configuration */
#include "common.h"           /* common header file */
#include "variables.h"        /* global variables */
#include "functions.h"        /* external functions */



/* ************************************************************************
 *   resistance measurements
 * ************************************************************************ */


/*
 *  measure a resistor with low resistance (< 100 Ohms)
 *  - doesn't work with some inductors
 *
 *  requires:
 *  - auto-zero flag
 *    0: don't auto-zero
 *    1: auto-zero
 *
 *  returns:
 *  - resistance in 0.01 Ohm
 */

uint16_t SmallResistor(uint8_t ZeroFlag)
{
  uint16_t          R = 0;         /* return value */
  uint8_t           Probe;         /* probe ID */
  uint8_t           Mode;          /* measurement mode */
  uint8_t           Counter;       /* sample counter */
  uint32_t          Value;         /* ADC sample value */
  uint32_t          Value1 = 0;    /* U_Rl temp. value */
  uint32_t          Value2 = 0;    /* U_R_i_L temp. value */

  DischargeProbes();                    /* try to discharge probes */
  if (Check.Found == COMP_ERROR) return R;   /* skip on error */


  /*
   *  measurement method:
   *  - use Rl as current shunt
   *  - measure voltage at high side of DUT for 100 times 
   *  - repeat that for the low side of the DUT
   */

  /* set probes: GND -- probe 2 / probe 1 -- Rl -- 5V */
  ADC_PORT = 0;                         /* set ADC port to low */
  ADC_DDR = Probes.Pin_2;               /* pull-down probe 2 directly */
  R_PORT = Probes.Rl_1;                 /* pull-up probe 1 via Rl */
  R_DDR = Probes.Rl_1;                  /* enable Rl for probe 1 */
  wait10ms();                           /* settle time */
  /* todo: check if we have to increase the delay for large inductances */

#define MODE_HIGH        0b00000001
#define MODE_LOW         0b00000010


  /*
   *   measurement loop
   */

  Mode = MODE_HIGH;

  while (Mode > 0)
  {
    /* set up measurement */
    if (Mode & MODE_HIGH) Probe = Probes.ADC_1;
    else Probe = Probes.ADC_2;

    wdt_reset();              /* reset watchdog */
    Counter = 0;              /* reset loop counter */
    Value = 0;                /* reset sample value */

    /* set ADC to use bandgap reference and run a dummy conversion */
    Probe |= ADC_REF_BANDGAP;
    ADMUX = Probe;                   /* set input channel and U reference */
    wait100us();                     /* time for voltage stabilization */
    ADCSRA |= (1 << ADSC);           /* start conversion */
    while (ADCSRA & (1 << ADSC));    /* wait until conversion is done */


    /*
     *  measurement loop (about 0.5ms per cycle)
     */

    while (Counter < 100)
    {
      /* get ADC reading (about 100µs) */
      ADCSRA |= (1 << ADSC);            /* start conversion */
      while (ADCSRA & (1 << ADSC));     /* wait until conversion is done */
      Value += ADCW;                    /* add ADC reading */

      wait400us();                      /* wait */

      Counter++;                        /* next round */
    }

    /* convert ADC reading into voltage (sum of samples) */
    Value *= Cfg.Bandgap;          /* * U_bandgap */
    Value /= 1024;                 /* / 1024 for 10bit ADC */

    /* loop control */
    if (Mode & MODE_HIGH)          /* probe #1 / Rl */
    {
      Mode = MODE_LOW;             /* switch to low side */
      Value1 = Value;              /* save measured value */
    }
    else                           /* probe #2 / R_i_L */
    {
      Mode = 0;                    /* end loop */
      Value2 = Value;              /* save measured value */
    }
  }

  /* stop current */
  R_PORT = 0;
  ADC_DDR = Probes.Pin_2 | Probes.Pin_1;


  /*
   *  process measurement
   */

  if (Value1 > Value2)             /* sanity check */
  {
    /* I = U/R = (5V - U_Rl)/(Rl + R_i_H) */
    Value = 100UL * Cfg.Vcc;                 /* Vcc * number of samples */
    Value -= Value1;                         /* - sum of U_Rl */
    Value *= 100;                            /* de-sample to 0.1 µV */
    Value /= ((R_LOW * 10) + NV.RiH);        /* I in µA */

    /* U = U_Rl - U_R_i_L = U_Rl - (R_i_L * I) */
    /* U = U_probe1 - U_probe2 */
    Value1 -= Value2;                        /* sums in mV */
    Value1 *= 1000;                          /* de-sample to 0.01 µV */  

    /* R = U/I (including R of probe leads) */
    Value1 /= Value;                         /* in 0.01 Ohms */
    R = (uint16_t)Value1;                    /* copy result */

    if (ZeroFlag == 1)        /* auto-zero */
    {
      if (R > NV.RZero)       /* should be larger than offset */
      {
        R -= NV.RZero;        /* subtract offset */
      }
      else                    /* can't be less then zero */
      {
        R = 0;
      }
    }
  }

#undef MODE_LOW
#undef MODE_HIGH

  /* update Uref flag for next ADC run */
  Cfg.RefFlag = ADC_REF_BANDGAP;        /* update flag */

  return R;
}



/*
 *  check for resistor
 */

void CheckResistor(void)
{
  Resistor_Type     *Resistor;     /* pointer to resistor */
  uint32_t          Value1;        /* resistance of measurement #1 */
  uint32_t          Value2;        /* resistance of measurement #2 */
  uint32_t          Value;         /* resistance value */
  uint32_t          Temp;          /* temp. value */
  int8_t            Scale;         /* resistance scale */
  uint8_t           n;             /* counter */
  int8_t            Compare = 0;   /* control flag */

  /* voltages */
  uint16_t          U_Rl_H;        /* voltage at Rl pulled up */
  uint16_t          U_Rl_L;        /* voltage at Rl pulled down */
  uint16_t          U_Ri_H;        /* voltage at Ri oulled up */
  uint16_t          U_Ri_L;        /* voltage at Ri pulled down */
  uint16_t          U_Rh_H;        /* voltage at Rh pulled up */
  uint16_t          U_Rh_L;        /* voltage ar Rh pulled down */

  wdt_reset();                     /* reset watchdog */

  /*
   *  resistor measurement
   *  - We assume: resistor between probe-1 and probe-2
   *  - Set up a voltage divider with well known probe resistors and
   *    measure the voltage at the DUT.
   *  - For low resistance consider the internal resistors of the MCU
   *    for pulling up/down.
   *  - Calculate resistance via the total current and the voltage
   *    at the DUT.
   *  - We could also use the voltage divider rule:
   *    (Ra / Rb) = (Ua / Ub) -> Ra = Rb * (Ua / Ub)
   *  - A resistor has the same resistance in both directions.
   *  - We measure both directions with both probe resistors.
   */


  /*
   *  check for a capacitor
   *  - Apply current to charge a possible capacitor.
   *  - Then discharge possible capacitor by Rh and measure voltage.
   *  - A capacitor would need some time to discharge but for
   *    a resistor the voltage would drop immediately.
   */

  /* 
   *  Charge possible capacitor and get voltages for Rl pulled up.
   */

  /*  set probes: Gnd -- probe-2 / probe-1 -- Rl -- Vcc */
  ADC_PORT = 0;                         /* set ADC port to low */
  ADC_DDR = Probes.Pin_2;               /* pull down probe-2 directly */
  R_DDR = Probes.Rl_1;                  /* enable Rl for probe-1 */
  R_PORT = Probes.Rl_1;                 /* pull up probe-1 via Rl */
  U_Ri_L = ReadU_5ms(Probes.ADC_2);     /* get voltage at internal R of MCU */
  U_Rl_H = ReadU(Probes.ADC_1);         /* get voltage at Rl pulled up */


  /*
   *  discharge possible capacitor
   */

  /* set probes: Gnd -- probe-2 / Gnd -- Rh -- probe-1 */
  R_PORT = 0;                           /* set resistor port low */
  R_DDR = Probes.Rh_1;                  /* pull down probe-1 via Rh */
  U_Rh_L = ReadU_5ms(Probes.ADC_1);     /* get voltage at probe-1 */

  /* we got a resistor if the voltage is near Gnd */
  if (U_Rh_L <= 20)
  {
    /*
     *  get voltage at Rh pulled up
     */

    /* set probes: Gnd -- probe-2 / probe-1 -- Rh -- Vcc */
    R_PORT = Probes.Rh_1;                    /* pull up probe-1 via Rh */
    U_Rh_H = ReadU_5ms(Probes.ADC_1);        /* get voltage at Rh pulled up */


    /*
     *  get voltage at Rl pulled down and Rh pulled down
     */

    /* set probes: Gnd -- Rl -- probe-2 / probe-1 -- Vcc */
    ADC_DDR = Probes.Pin_1;                  /* set probe-1 to output */
    ADC_PORT = Probes.Pin_1;                 /* pull up probe-1 directly */
    R_PORT = 0;                              /* set resistor port to low */ 
    R_DDR = Probes.Rl_2;                     /* pull down probe-2 via Rl */
    U_Ri_H = ReadU_5ms(Probes.ADC_1);        /* get voltage at internal R of MCU */
    U_Rl_L = ReadU(Probes.ADC_2);            /* get voltage at Rl pulled down */

    /* set probes: Gnd -- Rh -- probe-2 / probe-1 -- Vcc */
    R_DDR = Probes.Rh_2;                /* pull down probe-2 via Rh */
    U_Rh_L = ReadU_5ms(Probes.ADC_2);   /* get voltage at Rh pulled down */

    /* if voltage breakdown is sufficient */
    if ((U_Rl_H >= 4400) || (U_Rh_H <= 97))   /* R >= 5.1k or R < 9.3k */
    {
      if (U_Rh_H < 4972)            /* R < 83.4M & prevent division by zero */
      {
        /* voltage breaks down with low test current and it is not nearly 
           shorted => resistor */

        Value = 0;                      /* reset value of resistor */

        if (U_Rl_L < 169)               /* R > 19.5k */
        {
          /*
           *  use measurements done with Rh
           */

          /* resistor is less than 60MOhm */
          if (U_Rh_L >= 38)        /* R < 61.4M & prevent division by zero */
          {
            /*
             *  Rh pulled up (above DUT):
             *  I = U_Rh / Rh = (Vcc - U_Rh_H) / Rh
             *  R = U_R / I = U_Rh_H / ((Vcc - U_Rh_H) / Rh)
             *    = Rh * U_Rh_H / (Vcc - U_Rh_H)
             *
             *  Or via voltage divider:
             *  R = Rh * (U_dut / U_Rh)
             *    = Rh * (U_Rh_H / (Vcc - U_Rh_H))
             */

            Value1 = R_HIGH * U_Rh_H;
            Value1 /= (Cfg.Vcc - U_Rh_H);

            /*
             *  Rh pulled down (below DUT):
             *  I = U_Rh_L / Rh
             *  R = U_R / I = (Vcc - U_Rh_L) / (U_Rh_L / Rh)
             *    = Rh * (Vcc - U_Rh_L) / U_Rh_L
             *
             *  Or via voltage divider:
             *  R = Rh * (U_R / U_Rh)
             *    = Rh * ((Vcc - U_Rh_L) / U_Rh_L)
             */

            Value2 = R_HIGH * (Cfg.Vcc - U_Rh_L);
            Value2 /= U_Rh_L;

            /*
             *  calculate weighted average of both measurements
             *  - Voltages below the bandgap reference got a higher resolution
             *    (1.1mV instead of 4.9mV).
             */

            if (U_Rh_H < 990)           /* below bandgap reference */
            {
              /* weighted average for U_Rh_H */
              Value = (Value1 * 4);
              Value += Value2;
              Value /= 5;
            }
            else if (U_Rh_L < 990)      /* below bandgap reference */
            {
              /* weighted average for U_Rh_L */
              Value = (Value2 * 4);
              Value += Value1;
              Value /= 5;
            }
            else                        /* higher than bandgap reference */
            {
              /* classic average */
              Value = (Value1 + Value2) / 2;
            }

            Value += RH_OFFSET;         /* add offset value for Rh */
            Value *= 10;                /* upscale to 0.1 Ohms */
            Compare = 1;                /* signal valid measurement */
          }
        }
        else                       /* U_Rl_L: R <= 19.5k */
        {
          /*
           *  use measurements done with Rl
           */

          /* voltages below and above DUT match voltage divider */
          /* voltage below DUT can't be higher than above DUT */
          if ((U_Rl_H >= U_Ri_L) && (U_Ri_H >= U_Rl_L))
          {

            /*
             *  Rl pulled up (above DUT):
             *  I = U_Rl_RiH / (Rl + RiH) = (Vcc - U_Rl_H) / (Rl + RiH)
             *  R = U_Dut / I
             *    = (U_Rl_H - U_Ri_L) / ((Vcc - U_Rl_H) / (Rl + RiH))
             *    = (Rl + RiH) * (U_Rl_H - U_Ri_L) / (Vcc - U_Rl_H)
             *
             *  Or via voltage divider:
             *  R = (Rl + RiH) * (U_R_RiL / U_Rl_RiH) - RiL
             *    = (Rl + RiH) * (U_R_RiL / (Vcc - U_dut_RiL)) - RiL
             */

            if (U_Rl_H == Cfg.Vcc)      /* prevent division by zero */
            {
              U_Rl_H = Cfg.Vcc - 1;
            }

            Value1 = (R_LOW * 10) + NV.RiH;       /* Rl + RiH in 0.1 Ohm */
            Value1 *= (U_Rl_H - U_Ri_L);
            Value1 /= (Cfg.Vcc - U_Rl_H);

            /*
             *  Rl pulled down (below DUT):
             *  I = U_Rl_RiL / (Rl + RiL)
             *  R = U_R / I
             *    = (U_Ri_H - U_Rl_L) / (U_Rl_RiL / (Rl + RiL))
             *    = (Rl + RiL) * (U_Ri_H - U_Rl_L) / U_Rl_RiL
             *
             *  Or via voltage divider:
             *  R = (Rl + RiL) * (U_R_RiH / U_Rl_RiL) - RiH
             *    = (Rl + RiL) * ((Vcc - U_Rl_RiL) / U_Rl_RiL) - RiH
             */

            Value2 = (R_LOW * 10) + NV.RiL;       /* Rl + RiL in 0.1 Ohms */
            Value2 *= (U_Ri_H - U_Rl_L);
            Value2 /= U_Rl_L;

            /*
             *  calculate weighted average of both measurements
             *  - Voltages below the bandgap reference got a higher resolution
             *    (1.1mV instead of 4.9mV).
             */

            if (U_Rl_H < 990)           /* below bandgap reference */
            {
              /* weighted average for U_Rh_H */
              Value = (Value1 * 4);
              Value += Value2;
              Value /= 5;
            }
            else if (U_Rl_L < 990)      /* below bandgap reference */
            {
              /* weighted average for U_Rh_L */
              Value = (Value2 * 4);
              Value += Value1;
              Value /= 5;
            }
            else                        /* higher than bandgap reference */
            {
              /* classic average */
              Value = (Value1 + Value2) / 2;
            }
          }
          else      /* may happen for very low resistances */
          {
            if (U_Rl_L > 4750)     /* U_Rl_L: R < 15 Ohms */
            {
              /* this will trigger the low resistance measurement below */
              Value = 10;           /* 1 Ohm */
            }
          }

          Compare = 1;             /* signal valid measurement */
        }


        /*
         *  process results of the resistance measurement
         */

        if (Compare == 1)          /* valid measurement */
        {
          Scale = -1;              /* 0.1 Ohms by default */

          /*
           *  meassure small resistor <10 Ohms with special method
           */

          if (Value < 100UL)            /* < 10 Ohms */
          {
            /* run low resistance measurement (in 0.01 Ohms) */
            Value2 = (uint32_t)SmallResistor(1);

            /* check for valid result */
            Value1 = Value * 2;         /* allow 100% tolerance */       
            Value1 *= 10;               /* re-scale to 0.01 Ohms */     

            if (Value1 == 0)            /* if limit is 0 */ 
            {
              Value1 = 100;             /* use 1 Ohm */
            }

            if (Value1 > Value2)        /* got expected value */
            {
              Value = Value2;           /* take new value */
              Scale = -2;               /* 0.01 Ohms */
            }
          }


          /*
           *  check for measurement in reversed direction
           */

          n = 0;
          while (n < Check.Resistors)           /* loop through resistors */
          {
            Resistor = &Resistors[n];           /* pointer to element */

            if ((Resistor->A == Probes.ID_1) && (Resistor->B == Probes.ID_2))
            {
              /*
               *  check if the reverse measurement is within a specific tolerance
               */

              /* set lower and upper tolerance limits */
              if (CmpValue(Value, Scale, 2, 0) == -1)  /* < 2 Ohms */
              {
                /* if reverse measurement is also < 2 Ohms */
                if (CmpValue(Resistor->Value, Resistor->Scale, 2, 0) == -1)
                {
                  /* cope with possible probe contact issues */
                  if (Resistor->Scale == Scale)   /* same method */
                  {
                    /* use larger value */
                    Compare = 1;
                  }
                  else                            /* different method */
                  {
                    /* use lower value */
                    Compare = -1;
                  }

                  if (CmpValue(Resistor->Value, Resistor->Scale, Value, Scale) == Compare)
                  {
                    /* use reverse measurement */
                    Value = Resistor->Value;
                    Scale = Resistor->Scale;
                  }
                  else
                  {
                    /* use this measurement */
                    Resistor->Value = Value;
                    Resistor->Scale = Scale;
                  }
                }

                Temp = Value / 2;            /* 50% */
              }
              else                                     /* >= 2 Ohms */
              {
                Temp = Value / 20;           /* 5% */
              }

              Value1 = Value - Temp;         /* 95% or 50% */
              Value2 = Value + Temp;         /* 105% or 150% */

              /* check if value matches the given tolerance */
              if ((CmpValue(Resistor->Value, Resistor->Scale, Value1, Scale) >= 0) &&
                  (CmpValue(Resistor->Value, Resistor->Scale, Value2, Scale) <= 0))
              {
                n = 100;                     /* end loop and signal match */
                Check.Found = COMP_RESISTOR;
              }
              else                 /* no match */
              {
                n = 200;                     /* end loop and signal mis-match */
              }
            }
            else                             /* no match */
            {
              n++;                           /* next one */
            }
          }


          /*
           *  we got a new resistor
           */

          if (n != 100)            /* not a known resistor */
          {
            if (Check.Resistors < 3)              /* prevent array overflow */
            {
              /* save data */
              Resistor = &Resistors[Check.Resistors];  /* unused dataset */
              Resistor->A = Probes.ID_2;               /* pin facing Gnd */
              Resistor->B = Probes.ID_1;               /* pin facing Vcc */
              Resistor->Value = Value;
              Resistor->Scale = Scale;
              Check.Resistors++;                       /* another one found */
            }
          }
        }
      }
    }
  }
}



/*
 *  check for a specific single resistor
 *
 *  requires:
 *  - HighPin = pin facing Vcc
 *  - LowPin = pin facing Gnd
 *  - maximum resistance in kOhms
 *    0: don't check limit
 */

uint8_t CheckSingleResistor(uint8_t HighPin, uint8_t LowPin, uint8_t Max)
{
  uint8_t                Flag = 0;      /* return value */

  /* check for a specific resistor */
  Check.Resistors = 0;                  /* reset counter */
  UpdateProbes(HighPin, LowPin, 0);     /* set probes */
  CheckResistor();                      /* check for resistor */

  if (Check.Resistors == 1)             /* found resistor */
  {
    Flag = 1;            /* signal detected resistor */

    if (Max > 0)         /* check upper limit */
    {
      /* measured value isn't below maximum */
      if (CmpValue((uint32_t)Max, 3, Resistors[0].Value, Resistors[0].Scale) != 1) 
      {
        Flag = 0;        /* reset flag */
      }
    }
  }

  return Flag;
}



/* ************************************************************************
 *   clean-up of local constants
 * ************************************************************************ */


/* source management */
#undef RESISTOR_C



/* ************************************************************************
 *   EOF
 * ************************************************************************ */
