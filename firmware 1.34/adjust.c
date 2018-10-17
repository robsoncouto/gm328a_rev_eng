/* ************************************************************************
 *
 *   self-adjustment functions
 *
 *   (c) 2012-2018 by Markus Reschke
 *   based on code from Markus Frejek and Karl-Heinz Kübbeler
 *
 * ************************************************************************ */


/*
 *  local constants
 */

/* source management */
#define ADJUST_C


/*
 *  include header files
 */

/* local includes */
#include "config.h"           /* global configuration */
#include "common.h"           /* common header file */
#include "variables.h"        /* global variables */
#include "functions.h"        /* external functions */
#include "colors.h"           /* color definitions */



/* ************************************************************************
 *   storage of adjustment values in EEPROM
 * ************************************************************************ */


/*
 *  set default adjustment values
 */

void SetAdjustmentDefaults(void)
{
  /* set defaults for basic offsets/values */
  NV.RiL = R_MCU_LOW;
  NV.RiH = R_MCU_HIGH;
  NV.RZero = R_ZERO;
  NV.CapZero = C_ZERO;
  NV.RefOffset = UREF_OFFSET;
  NV.CompOffset = COMPARATOR_OFFSET;
  NV.Contrast = LCD_CONTRAST;

  #ifdef HW_TOUCH
  /* set defaults for touch screen */
  Touch.X_Left = 0;
  Touch.X_Right = 0;
  Touch.Y_Top = 0;
  Touch.Y_Bottom = 0;
  /* this triggers the touch adjustment at startup */
  #endif
}



#ifdef HW_TOUCH

/*
 *  functions optimized for generic usage
 */


/*
 *  calculate checksum for adjustment values
 *
 *  requires:
 *  - pointer to data structure in RAM
 *  - size of data structure
 */

uint8_t CheckSum(uint8_t *Data, uint8_t Size)
{
  uint8_t      Checksum = 0;            /* checksum / return value */
  uint8_t      n = 0;                   /* counter */

  /* we simply add all bytes, besides the checksum */
  Size--;                     /* last byte is checksum */
  while (n < Size)
  {  
    Checksum += *Data;        /* add byte */
    Data++;                   /* next byte */
    n++;                      /* next byte */
  }

  /* fix for zero (not updated yet) */
  if (Checksum == 0) Checksum++;

  return Checksum;
}



/*
 *  load/save adjustment values from/to EEPROM
 *
 *  requires:
 *  - pointer to data structure in RAM
 *  - pointer to data structure in EEPROM
 *  - size of data structure
 *  - mode: load/save
 *
 *  returns:
 *  - 0 on checksum error for read operation
 *  - number of bytes read/written
 */

uint8_t DataStorage(uint8_t *Data_RAM, uint8_t *Data_EE, uint8_t Size, uint8_t Mode)
{
  uint8_t      n;                       /* counter & return value */
  uint8_t      *Help;                   /* pointer */

  /* update checksum */
  Help = Data_RAM;                      /* start pointer */
  Help += Size - 1;                     /* last byte (checksum) */
  *Help = CheckSum(Data_RAM, Size);     /* update checksum */

  Help = Data_RAM;                      /* save pointer */


  /*
   *  read/write EEPROM byte-wise to/from data structure in RAM
   */

  n = 0;
  while (n < Size)
  {
    if (Mode == STORAGE_SAVE)           /* write */
    {
      eeprom_write_byte(Data_EE, *Data_RAM);    /* write a byte */
    }
    else                                /* read */
    {
      *Data_RAM = eeprom_read_byte(Data_EE);    /* read a byte */
    }

    Data_RAM++;               /* next byte */
    Data_EE++;                /* next byte */
    n++;                      /* next byte */
  }


  /*
   *  check checksum on read
   */

  if (Mode != STORAGE_SAVE)        /* read mode */
  {
    n = CheckSum(Help, Size);      /* checksum of data in RAM */

    /* data structure's last byte is checksum */
    Data_RAM--;                    /* one byte back */
    if (*Data_RAM != 0)            /* EEPROM updated */
    {
      if (*Data_RAM != n)          /* mismatch */
      {
        /* tell user */
        LCD_Clear();
        Display_EEString(Checksum_str);      /* display: Checksum */
        Display_NL_EEString(Error_str);      /* display: error! */
        MilliSleep(2000);                    /* give user some time to read */

        n = 0;           /* signal checksum problem */
      }
    }
  }

  return n;
}



/*
 *  manage storage of adjustment offsets and values
 *
 *  requires:
 *  - mode: load/save
 *    STORAGE_LOAD  load data from EEPROM to RAM
 *    STORAGE_SAVE  save data from RAM to EEPROM
 *  - ID: profile ID (1 or 2) for basic adjustment values
 */

void ManageAdjustmentStorage(uint8_t Mode, uint8_t ID)
{
  uint8_t      n;                            /* temp. value */
  uint8_t      Flag = 1;                     /* control flag */
  uint8_t      *Data_EE;                     /* pointer to EEPROM */

  /*
   *  basic adjustment offsets/values
   */

  /* determine EEPROM address */
  if (ID == 2)                     /* profile #2 */
  {
    Data_EE = (uint8_t *)&NV_Adjust_2;
  }
  else                             /* profile #1 */
  {
    Data_EE = (uint8_t *)&NV_Adjust_1;
  }

  /* write/read EEPROM */
  n = DataStorage((uint8_t *)&NV, Data_EE, sizeof(Adjust_Type), Mode);
  if (n == 0) Flag = 0;       /* checksum error */


  /*
   *  additional offsets
   */

  /* touch screen offsets */
  n = DataStorage((uint8_t *)&Touch, (uint8_t *)&NV_Touch, sizeof(Touch_Type), Mode);
  if (n == 0) Flag = 0;       /* checksum error */


  if ((Mode == STORAGE_LOAD) && (Flag == 0))      /* checksum error */
  {
    SetAdjustmentDefaults();       /* set defaults */
  }
}


#else


/*
 *  functions optimized for basic adjustment offsets and values
 */


/*
 *  calculate checksum for adjustment values
 */

uint8_t CheckSum(void)
{
  uint8_t      Checksum = 0;            /* checksum / return value */
  uint8_t      n;                       /* counter */
  uint8_t      *Data = (uint8_t *)&NV;  /* pointer to RAM */

  /* we simply add all bytes, besides the checksum */
  for (n = 0; n < (sizeof(Adjust_Type) - 1); n++)
  {  
    Checksum += *Data;        /* add byte */
    Data++;                   /* next byte */
  }

  /* fix for zero (not updated yet) */
  if (Checksum == 0) Checksum++;

  return Checksum;
}



/*
 *  manage storage of adjustment offsets and values
 *  - save/load values to/from EEPROM
 *
 *  requires:
 *  - mode: load/save
 *    STORAGE_LOAD  load data from EEPROM to RAM
 *    STORAGE_SAVE  save data from RAM to EEPROM
 *  - ID: profile ID
 */

void ManageAdjustmentStorage(uint8_t Mode, uint8_t ID)
{
  uint8_t      n;                            /* counter */
  uint8_t      *Addr_RAM = (uint8_t *)&NV;   /* pointer to RAM */
  uint8_t      *Addr_EE;                     /* pointer to EEPROM */

  /* determine EEPROM address */
  if (ID == 2)                     /* profile #2 */
  {
    Addr_EE = (uint8_t *)&NV_Adjust_2;
  }
  else                             /* profile #1 */
  {
    Addr_EE = (uint8_t *)&NV_Adjust_1;
  }

  NV.CheckSum = CheckSum();        /* update checksum */


  /*
   *  read/write EEPROM byte-wise to/from data structure 
   */

  for (n = 0; n < sizeof(Adjust_Type); n++)
  {
    if (Mode == STORAGE_SAVE)           /* write */
    {
      eeprom_write_byte(Addr_EE, *Addr_RAM);    /* write a byte */
    }
    else                                /* read */
    {
      *Addr_RAM = eeprom_read_byte(Addr_EE);    /* read a byte */
    }

    Addr_RAM++;               /* next byte */
    Addr_EE++;                /* next byte */
  }


  /*
   *  check checksum on read
   */

  if (Mode != STORAGE_SAVE)        /* read mode */
  {
    n = CheckSum();                /* get checksum */

    if (NV.CheckSum != 0)          /* EEPROM updated */
    {
      if (NV.CheckSum != n)        /* mismatch */
      {
        /* tell user */
        LCD_Clear();
        Display_EEString(Checksum_str);      /* display: Checksum */
        Display_NL_EEString(Error_str);      /* display: error! */
        MilliSleep(2000);                    /* give user some time to read */

        SetAdjustmentDefaults();             /* set defaults */
      }
    }
  }
}

#endif



/* ************************************************************************
 *   self adjustment
 * ************************************************************************ */


/*
 *  show basic adjustment values and offsets
 */

void ShowAdjustmentValues(void)
{
  UI.LineMode = LINE_KEY;     /* next-line mode: wait for key/timeout */

  /* display RiL and RiH */
  LCD_Clear();
  Display_EEString_Space(RiLow_str);              /* display: Ri- */
  Display_Value(NV.RiL, -1, LCD_CHAR_OMEGA);
  Display_NL_EEString_Space(RiHigh_str);          /* display: Ri+ */
  Display_Value(NV.RiH, -1, LCD_CHAR_OMEGA);

  /* display C-Zero */
  Display_NL_EEString_Space(CapOffset_str);       /* display: C0 */
  Display_Value(NV.CapZero, -12, 'F');            /* display C0 offset */

  /* display R-Zero */
  Display_NL_EEString_Space(ROffset_str);         /* display: R0 */
  Display_Value(NV.RZero, -2, LCD_CHAR_OMEGA);    /* display R0 */

  /* display internal bandgap reference */
  Display_NL_EEString_Space(URef_str);       /* display: Vref */
  Display_Value(Cfg.Bandgap, -3, 'V');       /* display bandgap ref */

  /* display Vcc */
  Display_NL_EEString_Space(Vcc_str);        /* display: Vcc */
  Display_Value(Cfg.Vcc, -3, 'V');           /* display Vcc */

  if (Cfg.OP_Mode & OP_EXT_REF)         /* external 2.5V reference used */
  {
    Display_Space();                    /* display space */
    Display_Char('*');                  /* display: * */
  }

  /* display offset of analog comparator */
  Display_NL_EEString_Space(CompOffset_str);      /* display: AComp */
  Display_SignedValue(NV.CompOffset, -3, 'V');    /* display offset */

  WaitKey();                  /* let the user read */
}



/*
 *  self adjustment
 *
 *  returns:
 *  - 0 on error
 *  - 1 on success
 */

uint8_t SelfAdjustment(void)
{
  uint8_t           Flag = 0;           /* return value & loop couner */
  uint8_t           Step;               /* step counter */
  uint8_t           DisplayFlag;        /* display flag */
  uint16_t          Val1 = 0, Val2 = 0, Val3 = 0;   /* voltages */
  uint8_t           CapCounter = 0;     /* number of C_Zero measurements */
  uint16_t          CapSum = 0;         /* sum of C_Zero values */
  uint8_t           RCounter = 0;       /* number of R_Zero measurements */
  uint16_t          RSum = 0;           /* sum of R_Zero values */
  uint8_t           RiL_Counter = 0;    /* number of U_RiL measurements */
  uint16_t          U_RiL = 0;          /* sum of U_RiL values */
  uint8_t           RiH_Counter = 0;    /* number of U_RiH measurements */
  uint16_t          U_RiH = 0;          /* sum of U_RiL values */
  uint32_t          Val0;               /* temp. value */
  #ifdef HW_ADJUST_CAP
  uint8_t           RefCounter = 0;     /* number of ref/offset runs */
  #endif

  #ifdef HW_ADJUST_CAP
    Step = 1;            /* start with step #1 */
  #else
    Step = 2;            /* start with step #2 */
  #endif


  /*
   *  measurements
   */

  /* make sure all probes are shorted */
  Flag = ShortCircuit(1);
  if (Flag == 0) Step = 10;        /* skip adjustment on error */

  while (Step <= 6)      /* loop through steps */
  {
    Flag = 1;            /* reset loop counter */

    /* repeat each measurement 5 times */
    while (Flag <= 5)
    {
      /* display step number */
      LCD_Clear();
      Display_Char('A');                /* display: A (for Adjust) */
      Display_Char('0' + Step);         /* display number */
      Display_Space();

      DisplayFlag = 1;        /* display values by default */

      /*
       *  adjustment steps
       */

      switch (Step)
      {
        #ifdef HW_ADJUST_CAP
        case 1:     /* voltage offsets */

          Display_EEString_Space(URef_str);  /* display: Vref */
          Display_EEString(CompOffset_str);  /* display: AComp */

          RefCounter += RefCap();            /* use fixed cap */

          Display_NextLine();
          Display_SignedValue(NV.RefOffset, -3, 'V');  /* display offset in mV */
          Display_Space();
          Display_SignedValue(NV.CompOffset, -3, 'V'); /* display offset in mV */

          DisplayFlag = 0;                   /* reset flag */
          break;
        #endif        

        case 2:     /* resistance of probe leads (probes shorted) */
          Display_EEString_Space(ROffset_str);    /* display: R0 */
          Display_EEString(ProbeComb_str);        /* display: 12 13 23 */          

          /*
           *  The resistance is for two probes in series and we expect it to be
           *  lower than 1.50 Ohms, i.e. 0.75 Ohms for a single probe.
           */

          UpdateProbes(PROBE_2, PROBE_1, 0);
          Val1 = SmallResistor(0);
          if (Val1 < 150)                    /* within limit */
          {
            RSum += Val1;
            RCounter++;
          }

          UpdateProbes(PROBE_3, PROBE_1, 0);
          Val2 = SmallResistor(0);
          if (Val2 < 150)                    /* whithin limit */
          {
            RSum += Val2;
            RCounter++;
          }

          UpdateProbes(PROBE_3, PROBE_2, 0);
          Val3 = SmallResistor(0);
          if (Val3 < 150)                    /* within limit */
          {
            RSum += Val3;
            RCounter++;
          }

          break;

        case 3:     /* un-short probes */
          ShortCircuit(0);              /* make sure probes are not shorted */
          Flag = 100;                   /* skip test */
          DisplayFlag = 0;              /* don't display any result */
          break;

        case 4:     /* internal resistance of MCU pin in pull-down mode */
          Display_EEString(RiLow_str);  /* display: Ri- */

          /* TP1:  Gnd -- RiL -- probe-1 -- Rl -- RiH -- Vcc */
          ADC_PORT = 0;
          ADC_DDR = (1 << TP1);
          R_PORT = (1 << R_RL_1);
          R_DDR = (1 << R_RL_1);
          Val1 = ReadU_5ms(TP1);        /* U across RiL */
          U_RiL += Val1;

          /* TP2: Gnd -- RiL -- probe-2 -- Rl -- RiH -- Vcc */
          ADC_DDR = (1 << TP2);
          R_PORT =  (1 << R_RL_2);
          R_DDR = (1 << R_RL_2);
          Val2 = ReadU_5ms(TP2);        /* U across RiL */
          U_RiL += Val2;

          /* TP3: Gnd -- RiL -- probe-3 -- Rl -- RiH -- Vcc */
          ADC_DDR = (1 << TP3);
          R_PORT =  (1 << R_RL_3);
          R_DDR = (1 << R_RL_3);
          Val3 = ReadU_5ms(TP3);        /* U across RiL */
          U_RiL += Val3;

          RiL_Counter += 3;
          break;

        case 5:     /* internal resistance of MCU pin in pull-up mode */
          Display_EEString(RiHigh_str);      /* display: Ri+ */

          /* TP1: Gnd -- RiL -- Rl -- probe-1 -- RiH -- Vcc */
          R_PORT = 0;
          ADC_PORT = (1 << TP1);
          ADC_DDR = (1 << TP1);
          R_DDR = (1 << R_RL_1);
          Val1 = Cfg.Vcc - ReadU_5ms(TP1);   /* U across RiH */
          U_RiH += Val1;

          /* TP2: Gnd -- RiL -- Rl -- probe-2 -- RiH -- Vcc */
          ADC_PORT = (1 << TP2);
          ADC_DDR = (1 << TP2);
          R_DDR = (1 << R_RL_2);
          Val2 = Cfg.Vcc - ReadU_5ms(TP2);   /* U across RiH */
          U_RiH += Val2;

          /* TP3: Gnd -- RiL -- Rl -- probe-3 -- RiH -- Vcc */
          ADC_PORT = (1 << TP3);
          ADC_DDR = (1 << TP3);
          R_DDR = (1 << R_RL_3);
          Val3 = Cfg.Vcc - ReadU_5ms(TP3);   /* U across RiH */
          U_RiH += Val3;

          RiH_Counter += 3;
          break;

        case 6:     /* capacitance offset (PCB and probe leads) */
          Display_EEString_Space(CapOffset_str);  /* display: C0 */
          Display_EEString(ProbeComb_str);        /* display: 12 13 23 */

          /*
           *  The capacitance is for two probes and we expect it to be
           *  less than 100pF.
           */

          MeasureCap(PROBE_2, PROBE_1, 0);
          Val1 = (uint16_t)Caps[0].Raw;
          /* limit offset to 100pF */
          if ((Caps[0].Scale == -12) && (Caps[0].Raw <= 100))
          {
            CapSum += Val1;
            CapCounter++;            
          }

          MeasureCap(PROBE_3, PROBE_1, 1);
          Val2 = (uint16_t)Caps[1].Raw;
          /* limit offset to 100pF */
          if ((Caps[1].Scale == -12) && (Caps[1].Raw <= 100))
          {
            CapSum += Val2;
            CapCounter++;            
          }

          MeasureCap(PROBE_3, PROBE_2, 2);
          Val3 = (uint16_t)Caps[2].Raw;
          /* limit offset to 100pF */
          if ((Caps[2].Scale == -12) && (Caps[2].Raw <= 100))
          {
            CapSum += Val3;
            CapCounter++;            
          }

          break;
      }

      /* reset ports to defaults */
      ADC_DDR = 0;                      /* input mode */
      ADC_PORT = 0;                     /* all pins low */
      R_DDR = 0;                        /* input mode */
      R_PORT = 0;                       /* all pins low */

      /* display values */
      if (DisplayFlag)
      {
        Display_NextLine();             /* move to line #2 */
        Display_Value(Val1, 0 , 0);     /* display value probe-1 */
        Display_Space();
        Display_Value(Val2, 0 , 0);     /* display value probe-2 */
        Display_Space();
        Display_Value(Val3, 0 , 0);     /* display value probe-3 */
      }

      /* wait and check test push button */
      if (Flag < 100)                   /* when we don't skip this test */
      {
        DisplayFlag = TestKey(1000, CURSOR_NONE); /* catch key press or timeout */

        /* short press -> next test / long press -> end selftest */
        if (DisplayFlag > KEY_TIMEOUT)
        {
          Flag = 100;                   /* skip current test anyway */
          if (DisplayFlag == KEY_LONG) Step = 100;  /* also skip selftest */
        } 
      }
 
      Flag++;                           /* next run */
    }

    Step++;                             /* next step */
  }


  /*
   *  calculate values and offsets
   */

  Flag = 0;                   /* reset adjustment counter */

  /* capacitance auto-zero: calculate average value for all probe pairs */
  if (CapCounter == 15)
  {
    /* calculate average offset (pF) */
    NV.CapZero = CapSum / CapCounter;
    Flag++;                   /* adjustment done */
  }

  /* resistance auto-zero: calculate average value for all probes pairs */
  if (RCounter == 15)
  { 
    /* calculate average offset (0.01 Ohms) */
    NV.RZero = RSum / RCounter;
    Flag++;                   /* adjustment done */
  }

  /* RiL & RiH */
  if ((RiL_Counter == 15) && (RiH_Counter == 15))
  {
    /*
     *  Calculate RiL and RiH using the voltage divider rule:
     *  Ri = Rl * (U_Ri / U_Rl)
     *  - scale up by 100, round up/down and scale down by 10
     */

    /* use values multiplied by 3 to increase accuracy */    
    U_RiL /= 5;                         /* average sum of 3 U_RiL */
    U_RiH /= 5;                         /* average sum of 3 U_RiH */
    Val1 = (Cfg.Vcc * 3) - U_RiL - U_RiH;    /* U_Rl * 3 */

    /* RiL */
    Val0 = ((uint32_t)R_LOW * 100 * U_RiL) / Val1;     /* Rl * U_Ri / U_Rl in 0.01 Ohm */
    Val0 += 5;                                         /* for automagic rounding */
    Val0 /= 10;                                        /* scale down to 0.1 Ohm */
    if (Val0 < 250UL)         /* < 25 Ohms */
    {
      NV.RiL = (uint16_t)Val0;
      Flag++;                 /* adjustment done */
    }

    /* RiH */
    Val0 = ((uint32_t)R_LOW * 100 * U_RiH) / Val1;     /* Rl * U_Ri / U_Rl in 0.01 Ohm */
    Val0 += 5;                                         /* for automagic rounding */
    Val0 /= 10;                                        /* scale down to 0.1 Ohm */
    if (Val0 < 280UL)         /* < 29 Ohms */
    {
      NV.RiH = (uint16_t)Val0;
      Flag++;                 /* adjustment done */
    }
  }

  #ifdef HW_ADJUST_CAP
  if (RefCounter != 5)        /* we expect 5 successful runs */
  {
    Flag = 0;                 /* signal error */
  }
  #endif

  /* show basic values and offsets */
  ShowAdjustmentValues();

  if (Flag == 4)         /* all adjustments done */
  {
    Flag = 1;            /* signal success */
  }
  else                   /* missing adjustments */
  {
    Flag = 0;            /* signal error */
  }

  return Flag;
}



/* ************************************************************************
 *   selftest
 * ************************************************************************ */


/*
 *  selftest
 *  - perform measurements on internal voltages and probe resistors
 *  - display results
 *
 *  returns:
 *  - 0 on error
 *  - 1 on success
 */

uint8_t SelfTest(void)
{
  uint8_t           Flag = 0;           /* return value & loop counter */
  uint8_t           Test = 1;           /* test counter */
  uint8_t           DisplayFlag;        /* display flag */
  uint16_t          Val0;               /* voltage/value */
  int16_t           Val1 = 0, Val2 = 0, Val3 = 0;   /* voltages/values */
  int16_t           Temp;               /* value */

  /* make sure all probes are shorted */
  Flag = ShortCircuit(1);
  if (Flag == 0) Test = 10;        /* skip selftest */

  /* loop through all tests */
  while (Test <= 6)
  {
    Flag = 1;                 /* reset loop counter */

    /* repeat each test 5 times */
    while (Flag <= 5)
    {
      /* display test number */
      LCD_Clear();
      Display_Char('T');                /* display: T */
      Display_Char('0' + Test);         /* display test number */
      Display_Space();

      DisplayFlag = 1;                  /* display values by default */

      /*
       *  tests
       */

      switch (Test)
      {
        case 1:     /* reference voltage */
          Val0 = ReadU(ADC_BANDGAP);    /* dummy read for bandgap stabilization */
          Val0 = ReadU(ADC_BANDGAP);    /* read bandgap reference voltage */ 
          Display_EEString(URef_str);   /* display: Vref */

          Display_NextLine();
          Display_Value(Val0, -3, 'V');      /* display voltage in mV */

          DisplayFlag = 0;                   /* reset flag */
          break;

        case 2:     /* compare Rl resistors (probes still shorted) */
          Display_EEString_Space(Rl_str);    /* display: +Rl- */
          Display_EEString(ProbeComb_str);   /* display: 12 13 23 */

          /* set up a voltage divider using two Rl */
          /* get offset by substracting theoretical voltage of voltage divider */

          /* voltage of voltage divider */
          Temp = ((int32_t)Cfg.Vcc * (R_MCU_LOW + R_LOW)) / (R_MCU_LOW + R_LOW + R_LOW + R_MCU_HIGH);

          /* TP3: Gnd -- Rl -- probe-2 -- probe-1 -- Rl -- Vcc */
          R_PORT = (1 << R_RL_1);
          R_DDR = (1 << R_RL_1) | (1 << R_RL_2);
          Val3 = ReadU_20ms(TP3);
          Val3 -= Temp;

          /* TP2: Gnd -- Rl -- probe-3 -- probe-1 -- Rl -- Vcc */
          R_DDR = (1 << R_RL_1) | (1 << R_RL_3);
          Val2 = ReadU_20ms(TP2);
          Val2 -= Temp;

          /* TP1: Gnd -- Rl -- probe-3 -- probe-2 -- Rl -- Vcc */
          R_PORT = (1 << R_RL_2);
          R_DDR = (1 << R_RL_2) | (1 << R_RL_3);
          Val1 = ReadU_20ms(TP1);
          Val1 -= Temp;

          break;

        case 3:     /* compare Rh resistors (probes still shorted) */
          Display_EEString_Space(Rh_str);    /* display: +Rh- */
          Display_EEString(ProbeComb_str);   /* display: 12 13 23 */

          /* set up a voltage divider using two Rh */
          /* get offset by substracting theoretical voltage of voltage divider */

          /* voltage of voltage divider (ignore RiL and RiH) */
          Temp = Cfg.Vcc / 2;

          /* TP3: Gnd -- Rh -- probe-2 -- probe-1 -- Rh -- Vcc */
          R_PORT = (1 << R_RH_1);
          R_DDR = (1 << R_RH_1) | (1 << R_RH_2);
          Val3 = ReadU_20ms(TP3);
          Val3 -= Temp;

          /* TP2: Gnd -- Rh -- probe-3 -- probe-1 -- Rh -- Vcc */
          R_DDR = (1 << R_RH_1) | (1 << R_RH_3);
          Val2 = ReadU_20ms(TP2);
          Val2 -= Temp;

          /* TP1: Gnd -- Rh -- probe-3 -- probe-2 -- Rh -- Vcc */
          R_PORT = (1 << R_RH_2);
          R_DDR = (1 << R_RH_2) | (1 << R_RH_3);
          Val1 = ReadU_20ms(TP1);
          Val1 -= Temp;

          break;

        case 4:     /* un-short probes */
          ShortCircuit(0);         /* make sure probes are not shorted */
          Flag = 100;              /* skip test */
          DisplayFlag = 0;         /* don't display any result */
          break;

        case 5:     /* leakage check: probes pulled down */
          Display_EEString(RhLow_str);       /* display: Rh- */

          /* pull down probe via Rh and get voltage */
          /* voltage expected to be near zero */

          /* TP1: Gnd -- Rh -- probe-1 */
          R_PORT = 0;
          R_DDR = (1 << R_RH_1);
          Val1 = ReadU_20ms(TP1);

          /* TP2: Gnd -- Rh -- probe-2 */
          R_DDR = (1 << R_RH_2);
          Val2 = ReadU_20ms(TP2);

          /* TP3: Gnd -- Rh -- probe-3 */
          R_DDR = (1 << R_RH_3);
          Val3 = ReadU_20ms(TP3);

          break;

        case 6:     /* leakage check: probes pulled up */
          Display_EEString(RhHigh_str);      /* display: Rh+ */

          /* pull up probe via Rh and get voltage */
          /* voltage expected to be near Vcc */

          /* TP1: probe-1 -- Rh -- Vcc */
          R_DDR = (1 << R_RH_1);
          R_PORT = (1 << R_RH_1);
          Val1 = ReadU_20ms(TP1);

          /* TP2: probe-2 -- Rh -- Vcc */
          R_DDR = (1 << R_RH_2);
          R_PORT = (1 << R_RH_2);
          Val2 = ReadU_20ms(TP2);

          /* TP3: probe-3 -- Rh -- Vcc */
          R_DDR = (1 << R_RH_3);
          R_PORT = (1 << R_RH_3);
          Val3 = ReadU_20ms(TP3);

          break;
      }

      /* reset ports to defaults */
      R_DDR = 0;                             /* input mode */
      R_PORT = 0;                            /* all pins low */

      /* display voltages/values of all probes */
      if (DisplayFlag)
      {
        Display_NextLine();                  /* move to line #2 */
        Display_SignedValue(Val1, 0 , 0);    /* display value probe-1 */
        Display_Space();
        Display_SignedValue(Val2, 0 , 0);    /* display value probe-2 */
        Display_Space();
        Display_SignedValue(Val3, 0 , 0);    /* display value probe-3 */
      }

      /* wait and check test push button */
      if (Flag < 100)                   /* when we don't skip this test */
      {
        DisplayFlag = TestKey(1000, CURSOR_NONE); /* catch key press or timeout */

        /* short press -> next test / long press -> end selftest */
        if (DisplayFlag > KEY_TIMEOUT)
        {
          Flag = 100;                   /* skip current test anyway */
          if (DisplayFlag == KEY_LONG) Test = 100;  /* also skip selftest */
        } 
      }
 
      Flag++;                      /* next run */
    }

    Test++;                             /* next one */
  }

  Flag = 1;         /* signal success */
  return Flag;
} 



/* ************************************************************************
 *   clean-up of local constants
 * ************************************************************************ */


/* source management */
#undef ADJUST_C



/* ************************************************************************
 *   EOF
 * ************************************************************************ */
