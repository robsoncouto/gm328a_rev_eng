/* ************************************************************************
 *
 *   probing testpins
 *
 *   (c) 2012-2018 by Markus Reschke
 *   based on code from Markus Frejek and Karl-Heinz Kübbeler
 *
 * ************************************************************************ */


/*
 *  local constants
 */

/* source management */
#define PROBES_C


/*
 *  include header files
 */

/* local includes */
#include "config.h"           /* global configuration */
#include "common.h"           /* common header file */
#include "variables.h"        /* global variables */
#include "functions.h"        /* external functions */



/* ************************************************************************
 *   support functions
 * ************************************************************************ */


/*
 *  set up probes, bitmasks for probes and test resistors
 *
 *  requires:
 *  - Probe1: pin ID [0-2], mostly high level pin
 *  - Probe2: pin ID [0-2], mostly low level pin
 *  - Probe3: pin ID [0-2], mostly switch/gate pin
 */

void UpdateProbes(uint8_t Probe1, uint8_t Probe2, uint8_t Probe3)
{
  /* set probe IDs */
  Probes.ID_1 = Probe1;
  Probes.ID_2 = Probe2;
  Probes.ID_3 = Probe3;

  /* set masks using bitmask tables */
  Probes.Rl_1 = eeprom_read_byte(&Rl_table[Probe1]);
  Probes.Rl_2 = eeprom_read_byte(&Rl_table[Probe2]);
  Probes.Rl_3 = eeprom_read_byte(&Rl_table[Probe3]);
  Probes.Rh_1 = eeprom_read_byte(&Rh_table[Probe1]);
  Probes.Rh_2 = eeprom_read_byte(&Rh_table[Probe2]);
  Probes.Rh_3 = eeprom_read_byte(&Rh_table[Probe3]);
  Probes.Pin_1 = eeprom_read_byte(&Pin_table[Probe1]);
  Probes.Pin_2 = eeprom_read_byte(&Pin_table[Probe2]);
  Probes.Pin_3 = eeprom_read_byte(&Pin_table[Probe3]);

  /* set ADC MUX input addresses */
  Probes.ADC_1 = eeprom_read_byte(&ADC_table[Probe1]);
  Probes.ADC_2 = eeprom_read_byte(&ADC_table[Probe2]);
  Probes.ADC_3 = eeprom_read_byte(&ADC_table[Probe3]);
}



/*
 *  restore original probe IDs
 */

void RestoreProbes(void)
{
  /* call probe update for saved IDs */
  UpdateProbes(Probes.ID2_1, Probes.ID2_2, Probes.ID2_3);
}



/*
 *  backup current probe IDs
 */

void BackupProbes(void)
{
  /* copy probe IDs */
  Probes.ID2_1 = Probes.ID_1;
  Probes.ID2_2 = Probes.ID_2;
  Probes.ID2_3 = Probes.ID_3;
}



/*
 *  get ID of third probe
 *
 *  requires:
 *  - Probe1: ID of first probe (0-2)
 *  - Probe2: ID of second probe (0-2)
 *
 *  returns:
 *  - ID of third probe (0-2)
 */

uint8_t GetThirdProbe(uint8_t Probe1, uint8_t Probe2)
{
  uint16_t          Probe3;             /* ID of third probe */

  /*
   *  we know IDs #1 and #2, so get ID of third probe
   *  - 0+1=1 -> 2
   *    0+2=2 -> 1
   *    1+2=3 -> 0
   *  - third probe = 3 - (probe ID #1 + probe ID #2)
   */

  Probe3 = 3;
  Probe3 -= Probe1;
  Probe3 -= Probe2;

  return Probe3;
}



/*
 *  check for a short circuit between two probes
 *  - changes probe settings
 *
 *  requires:
 *  - ID of first probe (0-2)
 *  - ID of second probe (0-2)
 *
 *  returns:
 *  - 0 if not shorted
 *  - 1 if shorted
 */

uint8_t ShortedPair(uint8_t Probe1, uint8_t Probe2)
{
  uint8_t           Flag = 0;      /* return value */
  uint16_t          U1;            /* voltage at probe #1 in mV */
  uint16_t          U2;            /* voltage at probe #2 in mV */
  uint16_t          Min;           /* lower threshold */
  uint16_t          Max;           /* upper threshold */

  UpdateProbes(Probe1, Probe2, 0);      /* update probes */

  /*
   *  Set up a voltage divider between the two probes:
   *  - Gnd -- Rl -- probe-2 / probe-1 -- Rl -- Vcc
   *  - third probe: HiZ
   */

  ADC_DDR = 0;                          /* set ADC port to HiZ */
  R_PORT = Probes.Rl_1;                 /* pull up probe-1 via Rl */
  R_DDR = Probes.Rl_1 | Probes.Rl_2;    /* and pull down probe-2 via Rl */

  /* read voltages */
  U1 = ReadU_5ms(Probes.ADC_1);
  U2 = ReadU(Probes.ADC_2);

  /*
   *  We expect both probe voltages to be about the same and
   *  to be half of Vcc (allowed difference +/- 30mV).
   */

  Min = (Cfg.Vcc / 2) - 30;        /* lower voltage */
  Max = (Cfg.Vcc / 2) + 30;        /* upper voltage */

  if ((U1 > Min) && (U1 < Max))    /* U1 within window */
  { 
    if ((U2 > Min) && (U2 < Max))  /* U2 within window */
    {
      Flag = 1;                    /* about the same */
    }    
  }

  R_DDR = 0;             /* reset port */

  return Flag;
}



/*
 *  check for a short circuit between all probes
 *
 *  returns:
 *  - 0 if no probes are short-circuited
 *  - number of probe pairs short-circuited (3 = all)
 */

uint8_t ShortedProbes(void)
{
  uint8_t           Flag = 0;      /* return value */

  /* check all possible combinations */
  Flag = ShortedPair(PROBE_1, PROBE_2);
  Flag += ShortedPair(PROBE_1, PROBE_3);
  Flag += ShortedPair(PROBE_2, PROBE_3);

  return Flag;  
}



/*
 *  try to discharge any connected components, e.g. capacitors
 *  - detect batteries
 *  - sometimes large caps are detected as a battery
 */

void DischargeProbes(void)
{
  uint8_t           Counter;            /* loop control */
  uint8_t           Limit = 40;         /* sliding timeout (2s) */
  uint8_t           ID;                 /* test pin */
  uint8_t           DischargeMask;      /* bitmask */
  uint16_t          U_c;                /* current voltage */
  uint16_t          U_old[3];           /* old voltages */


  /*
   *  set probes to a save discharge mode (pull-down via Rh) 
   */

  /* set ADC port to HiZ input */
  ADC_DDR = 0;
  ADC_PORT = 0;

  /* all probe pins: Rh and Rl pull-down */
  R_PORT = 0;
  R_DDR = (1 << R_RH_1) | (1 << R_RH_2) | (1 << R_RH_3) |
          (1 << R_RL_1) | (1 << R_RL_2) | (1 << R_RL_3);

  /* get current voltages */
  U_old[0] = ReadU(TP1);
  U_old[1] = ReadU(TP2);
  U_old[2] = ReadU(TP3);

  /*
   *  try to discharge probes
   *  - We check if the voltage decreases over time.
   *  - A slow discharge rate will increase the timeout to support
   *    large caps.
   *  - A very large cap will discharge too slowly and an external voltage
   *    maybe never :-)
   */

  Counter = 1;
  ID = 2;
  DischargeMask = 0;

  while (Counter > 0)
  {
    ID++;                               /* next probe */
    if (ID > 2) ID = 0;                 /* start with probe #1 again */

    if (DischargeMask & (1 << ID))      /* skip discharged probe */
      continue;

    U_c = ReadU(ID);                    /* get voltage of probe */

    if (U_c < U_old[ID])                /* voltage decreased */
    {
      U_old[ID] = U_c;                  /* update old value */

      /* adapt timeout based on discharge rate */
      if ((Limit - Counter) < 20)
      {
        /* increase timeout while preventing overflow */
        if (Limit < (255 - 20)) Limit += 20;
      }

      Counter = 1;                      /* reset no-changes counter */
    }
    else                                /* voltage not decreased */
    {
      /* increase limit if we start at a low voltage */
      if ((U_c < 10) && (Limit <= 40)) Limit = 80;

      Counter++;              /* increase no-changes counter */
    }

    if (U_c <= CAP_DISCHARGED)          /* seems to be discharged */
    {
      DischargeMask |= (1 << ID);       /* set flag */
    }
    else if (U_c < 800)                 /* extra pull-down */
    {
      /* it's save now to pull down probe pin directly */
      ADC_DDR |= eeprom_read_byte(&Pin_table[ID]);
    }

    if (DischargeMask == 0b00000111)    /* all probes discharged */
    {
      Counter = 0;                        /* end loop */
    }
    else if (Counter > Limit)             /* no decrease for some time */
    {
      /* might be a battery or a super cap */
      Check.Found = COMP_ERROR;           /* report error */
      Check.Type = TYPE_DISCHARGE;        /* discharge problem */
      Check.Probe = ID;                   /* save probe */
      Check.U = U_c;                      /* save voltage */
      Counter = 0;                        /* end loop */
    }
    else                                /* go for another round */
    {
      wdt_reset();                        /* reset watchdog */
      MilliSleep(50);                     /* wait for 50ms */
    }
  }

  /* reset probes */
  R_DDR = 0;                       /* set resistor port to input mode */
  ADC_DDR = 0;                     /* set ADC port to input mode */
}



/*
 *  pull probe up/down via probe resistor for 1 or 10 ms
 *
 *  requires:
 *  - mask for probe resistors
 *  - pull mode (bit flags):
 *    0b00000000 = down
 *    0b00000001 = up 
 *    0b00000100 = 1ms
 *    0b00001000 = 10ms
 */

void PullProbe(uint8_t Mask, uint8_t Mode)
{
  /* set pull mode */
  if (Mode & PULL_UP) R_PORT |= Mask;   /* pull-up */
  else R_PORT &= ~Mask;                 /* pull-down */
  R_DDR |= Mask;                        /* enable pulling */

  if (Mode & PULL_1MS) wait1ms();       /* wait 1ms */
  else wait10ms();                      /* wait 10ms */

  /* reset pulling */
  R_DDR &= ~Mask;                       /* set to HiZ mode */
  R_PORT &= ~Mask;                      /* set 0 */
}



/*
 *  lookup a voltage/ratio based factor in a table and interpolate it's value
 *  - value decreases with index position
 *
 *  requires:
 *  - voltage (in mV) or ratio
 *  - table ID
 *
 *  returns:
 *  - multiplicator/factor
 */

uint16_t GetFactor(uint16_t U_in, uint8_t ID)
{
  uint16_t          Factor;             /* return value */
  uint16_t          U_Diff;             /* voltage difference to table start */
  uint16_t          Fact1, Fact2;       /* table entries */
  uint16_t          TabStart;           /* table start voltage */
  uint16_t          TabStep;            /* table step voltage */
  uint16_t          TabIndex;           /* table entries (-2) */
  uint16_t          *Table;             /* pointer to table */
  uint8_t           Index;              /* table index */
  uint8_t           Diff;               /* difference to next entry */

  /*
   *  set up table specific stuff
   */

  if (ID == TABLE_SMALL_CAP)
  {
    TabStart = 1000;                         /* table starts at 1000mV */
    TabStep = 50;                            /* 50mV steps between entries */
    TabIndex = (NUM_SMALL_CAP - 2);          /* entries in table - 2 */
    Table = (uint16_t *)&SmallCap_table[0];  /* pointer to table start */
  }
  else if (ID == TABLE_LARGE_CAP)
  {
    TabStart = 300;                          /* table starts at 1000mV */
    TabStep = 25;                            /* 25mV steps between entries */
    TabIndex = (NUM_LARGE_CAP - 2);          /* entries in table - 2 */
    Table = (uint16_t *)&LargeCap_table[0];  /* pointer to table start */
  }
  #ifdef SW_INDUCTOR
  else if (ID == TABLE_INDUCTOR)
  {
    TabStart = 200;                          /* table starts at 200 */
    TabStep = 25;                            /* steps between entries */
    TabIndex = (NUM_INDUCTOR - 2);           /* entries in table - 2 */
    Table = (uint16_t *)&Inductor_table[0];  /* pointer to table start */
  }
  #endif
  else
  {
    return 0;
  }

  /*
   *  We interpolate the table values corresponding to the given voltage/ratio.
   */

  /* difference to start of table */
  if (U_in >= TabStart) U_Diff = U_in - TabStart;  
  else U_Diff = 0;

  /* calculate table index */
  Index = U_Diff / TabStep;             /* index (position in table) */
  Diff = U_Diff % TabStep;              /* difference to index */
  Diff = TabStep - Diff;                /* difference to next entry */

  /* prevent index overflow */
  if (Index > TabIndex) Index = TabIndex;

  /* get values for index and next entry */
  Table += Index;                       /* advance to index */
  Fact1 = eeprom_read_word(Table);
  Table++;                              /* next entry */
  Fact2 = eeprom_read_word(Table);

  /* interpolate values based on the difference */
  Factor = Fact1 - Fact2;
  Factor *= Diff;
  Factor += TabStep / 2;
  Factor /= TabStep;
  Factor += Fact2;

  return Factor;
}



/* ************************************************************************
 *   identify component
 * ************************************************************************ */


/*
 *  probe connected component and try to identify it
 *
 *  requires:
 *  - Probe1: ID of probe to be pulled up [0-2]
 *  - Probe2: ID of probe to be pulled down [0-2]
 *  - Probe3: ID of probe to be in HiZ mode [0-2]
 */

void CheckProbes(uint8_t Probe1, uint8_t Probe2, uint8_t Probe3)
{
  uint8_t           Flag;          /* temporary value */
  uint16_t          U_Rl;          /* voltage across Rl (load) */
  uint16_t          U_1;           /* voltage #1 */

  /* init */
  if (Check.Found == COMP_ERROR) return;   /* skip check on any error */
  wdt_reset();                             /* reset watchdog */
  UpdateProbes(Probe1, Probe2, Probe3);    /* update bitmasks */

  /*
   *  We measure the current from probe 2 to ground with probe 1 pulled up
   *  to 5V and probe 3 in HiZ mode to determine if we got a self-conducting
   *  part, i.e. diode, resistor or depletion-mode FET. Rl is used as current
   *  shunt.
   *
   *  In case of a FET we have to take care about the gate charge based on
   *  the channel type.
   */

  /* set probes: Gnd -- Rl -- probe-2 / probe-1 -- Vcc */
  R_PORT = 0;                      /* set resistor port to Gnd */
  R_DDR = Probes.Rl_2;             /* pull down probe-2 via Rl */
  ADC_DDR = Probes.Pin_1;          /* set probe-1 to output */
  ADC_PORT = Probes.Pin_1;         /* pull-up probe-1 directly */

  /*
   *  For a possible n channel FET we pull down the gate for a few ms,
   *  assuming: probe-1 = D / probe-2 = S / probe-3 = G
   *
   *  Hint: The pull-down of the gate will trigger a possible PUT.
   */

  PullProbe(Probes.Rl_3, PULL_10MS | PULL_DOWN);  /* discharge gate via Rl */
  U_Rl = ReadU_5ms(Probes.ADC_2);                 /* get voltage at Rl */

  /*
   *  If we got conduction we could have a p channel FET. For any
   *  other part U_Rl will stay the same.
   */
 
  if (U_Rl >= 977)            /* > 1.4mA */
  {
    /*
     *  For a possible p channel FET we pull up the gate for a few ms,
     *  assuming: probe-1 = S / probe-2 = D / probe-3 = G
     */

    PullProbe(Probes.Rl_3, PULL_10MS | PULL_UP);  /* discharge gate via Rl */
    U_Rl = ReadU_5ms(Probes.ADC_2);               /* get voltage at Rl */
  }


  /*
   *  If there's some current we could have a depletion-mode FET
   *  (self-conducting).
   *
   *  Other possibilities:
   *  - diode or resistor
   */

  if (U_Rl > 15)         /* > 21µA */
  {
    if (Check.Done == DONE_NONE)        /* not sure yet */
    {
      CheckDepletionModeFET(U_Rl);
    }
  }


  /*
   *  If there's nearly no conduction (just a small leakage current) between
   *  probe-1 and probe-2 we might have a semiconductor:
   *  - BJT
   *  - enhancement mode FET or IGBT
   *  - Thyristor or Triac
   *  - or a large resistor
   */

  if (U_Rl < 977)         /* load current < 1.4mA (resistance > 3k) */
  {
    /*
     *  check for:
     *  - PNP BJT (common emitter circuit)
     *  - p-channel MOSFET (low side switching circuit) or IGBT
     */

    if (Check.Done == DONE_NONE)        /* not sure yet */
    {
      /*
       *  we assume:
       *  - BJT: probe-1 = E / probe-2 = C / probe-3 = B
       *  - FET: probe-1 = S / probe-2 = D / probe-3 = G
       */

      /* set probes: Gnd -- Rl - probe-2 / probe-1 -- Vcc / probe-3 -- Rl -- Gnd */
      R_DDR = Probes.Rl_2;                /* enable Rl for probe-2 */
      R_PORT = 0;                         /* pull down collector via Rl */
      ADC_DDR = Probes.Pin_1;             /* set probe-1 to output */
      ADC_PORT = Probes.Pin_1;            /* pull up emitter directly */
      wait5ms();
      R_DDR = Probes.Rl_2 | Probes.Rl_3;  /* pull down base via Rl */
      U_1 = ReadU_5ms(Probe2);            /* get voltage at collector */ 
 
      /*
       *  If DUT is conducting we might have a PNP BJT or p-channel FET.
       */

      if (U_1 > 3422)                   /* detected current > 4.8mA */
      {
        /* distinguish PNP BJT from p-channel MOSFET or IGBT */
        CheckTransistor(TYPE_PNP, U_Rl);
      }
    }


    /*
     *  check for
     *  - NPN BJT (common emitter circuit)
     *  - Thyristor and Triac
     *  - n-channel MOSFET (high side switching circuit) or IGBT
     */

    if (Check.Done == DONE_NONE)        /* not sure yet */
    {
      /*
       *  we assume:
       *  - BJT: probe-1 = C / probe-2 = E / probe-3 = B
       *  - FET: probe-1 = D / probe-2 = S / probe-3 = G
       *  - SCR: probe-1 = A / probe-2 = C / probe-3 = G
       *  - TRIAC: probe-1 = MT2 / probe-2 = MT1 / probe-3 = G
       */

      /* set probes: Gnd -- probe-2 / probe-1 -- Rl -- Vcc / probe-3 -- Rl -- Vcc */
      ADC_DDR = Probes.Pin_2;                /* set probe-2 to output mode */
      ADC_PORT = 0;                          /* pull down probe-2 directly */
      R_DDR = Probes.Rl_1 | Probes.Rl_3;     /* select Rl for probe-1 & Rl for probe-3 */
      R_PORT = Probes.Rl_1 | Probes.Rl_3;    /* pull up collector & base via Rl */
      U_1 = ReadU_5ms(Probe1);               /* get voltage at collector */

      /*
       *  If DUT is conducting we might have a NPN BJT, something similar or
       *  a n-channel MOSFET.
       */

      if (U_1 < 1600)                   /* detected current > 4.8mA */
      {
        /* first check for thyristor and triac */
        Flag = CheckThyristorTriac();

        if (Flag == 0)                 /* no thyristor or triac */
        {
          /* We might got a NPN BJT, a n-channel MOSFET or IGBT. */
          CheckTransistor(TYPE_NPN, U_Rl);
        }
      }
    }

    #ifdef SW_UJT

    /*
     *  check for UJT
     */

    if (Check.Done == DONE_NONE)        /* not sure yet */
    {
      CheckUJT();
    }

    #endif
  }


  /*
   *  If there's conduction between probe-1 and probe-2 we might have a
   *  - diode (conducting)
   *  - small resistor (checked later on)
   */

  else              /* load current > 1.4mA  (resistance < 3k) */
  {
    /*
     *  check for a PUT
     */

    if (Check.Done == DONE_NONE)        /* not sure yet */
    {
      CheckPUT();
    }

    /*
     *  We check for a diode even if we already found a component to get Vf, 
     *  since there could be a body/protection diode of a transistor.
     */

    CheckDiode();
  }


  /*
   *  Check for a resistor (or another one)
   *  - if no other component is found yet
   *  - if we've got a resistor already 
   */

  if ((Check.Found == COMP_NONE) ||
      (Check.Found == COMP_RESISTOR))
  {
    CheckResistor();
  }


  /*
   *  ... otherwise run some final checks.
   */

  else
  {
    /* verify a MOSFET */
    if ((Check.Found == COMP_FET) && (Check.Type & TYPE_MOSFET))
    {
      VerifyMOSFET();
    }
  }


  /* clean up */
  ADC_DDR = 0;           /* set ADC port to HiZ mode */
  ADC_PORT = 0;          /* set ADC port low */
  R_DDR = 0;             /* set resistor port to HiZ mode */
  R_PORT = 0;            /* set resistor port low */
}



/*
 *  logic for alternative components which might be found
 */

void CheckAlternatives(void)
{
  uint8_t           Flag = 0;

  /*
   *  problematic components:
   *  - PNP with B-E resistor and flyback diode passes UJT test once
   *  - UJT might pass NPN test once
   *  - TRIAC passes PUT test twice, but PUT only once
   */

  if (Check.AltFound != COMP_NONE)           /* alternative found */
  {
    if (! (Check.Done & DONE_SEMI))          /* not sure about common transistor */
    {
      /* but sure about alternative or no common transistor found */
      if ((Check.Done & DONE_ALTSEMI) || (Check.Found < COMP_BJT))
      {
        Flag = 1;                       /* choose alternative component */
      }
    }
  }

  if (Flag)         /* take alternative component */
  {
    /* copy some data */
    Check.Found = Check.AltFound;

    Semi.A = AltSemi.A;
    Semi.B = AltSemi.B;
    Semi.C = AltSemi.C;

    #ifdef SW_SYMBOLS
    Check.Symbol = Check.AltSymbol;
    #endif
  }


  /*
   *  add other special checks here
   */
}



/* ************************************************************************
 *   clean-up of local constants
 * ************************************************************************ */


/* source management */
#undef PROBES_C



/* ************************************************************************
 *   EOF
 * ************************************************************************ */
