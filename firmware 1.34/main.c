/* ************************************************************************
 *
 *   main part
 *
 *   (c) 2012-2018 by Markus Reschke
 *   based on code from Markus Frejek and Karl-Heinz Kübbeler
 *
 * ************************************************************************ */


/*
 *  local constants
 */

/* source management */
#define MAIN_C


/*
 *  include header files
 */

/* local includes */
#include "config.h"           /* global configuration */
#include "common.h"           /* common header file */
#include "variables.h"        /* global variables */
#include "functions.h"        /* external functions */
#include "colors.h"           /* color definitions */


/*
 *  local variables
 */

/* program control */
uint8_t        MissedParts;          /* counter for failed/missed components */



/* ************************************************************************
 *   output components and errors
 * ************************************************************************ */


/*
 *  show pinout for semiconductors
 *
 *  required:
 *  - character for pin A
 *  - character for pin B
 *  - character for pin C
 */

void Show_SemiPinout(uint8_t A, uint8_t B, uint8_t C)
{
  uint8_t           n;             /* counter */
  uint8_t           Char;          /* character */
  #ifdef SW_PROBE_COLORS
  uint16_t          Color;         /* color value */

  Color = UI.PenColor;             /* save current color */
  #endif

  /* display: 123 */
  for (n = 0; n <= 2; n++)
  {
    Display_ProbeNumber(n);
  }

  /* display: = */
  Display_Char('=');

  /* display pin IDs */
  for (n = 0; n <= 2; n++)         /* loop through probe pins */
  {
    #ifdef SW_PROBE_COLORS
    UI.PenColor = ProbeColors[n];  /* set probe color */
    #endif

    if (n == Semi.A) Char = A;          /* probe A - ID A */
    else if (n == Semi.B) Char = B;     /* probe B - ID B */
    else Char = C;                      /*         - ID C */

    Display_Char(Char);            /* display ID */
  }

  #ifdef SW_PROBE_COLORS
  UI.PenColor = Color;             /* restore old color */
  #endif
}



/*
 *  show simple pinout
 *
 *  required:
 *  - ID: characters for probes 1, 2 and 3
 *    0 -> not displayed
 */

void Show_SimplePinout(uint8_t ID_1, uint8_t ID_2, uint8_t ID_3)
{
  uint8_t           n;        /* counter */
  unsigned char     ID[3];    /* component pin IDs */
  #ifdef SW_PROBE_COLORS
  uint16_t          Color;    /* color value */

  Color = UI.PenColor;             /* save current color */
  #endif

  /* copy probe pin characters/IDs */
  ID[0] = ID_1;
  ID[1] = ID_2;
  ID[2] = ID_3;

  for (n = 0; n <= 2; n++)         /* loop through probe pins */
  {
    if (ID[n] != 0)                /* display this one */
    {
      Display_ProbeNumber(n);
      Display_Char(':');

      #ifdef SW_PROBE_COLORS
      UI.PenColor = ProbeColors[n];     /* set probe color */
      #endif

      Display_Char(ID[n]);

      #ifdef SW_PROBE_COLORS
      UI.PenColor = Color;              /* restore old color */
      #endif

      Display_Space();
    }
  }
}



/*
 *  show failed test
 */

void Show_Fail(void)
{
  /* display info */
  Display_EEString(Failed1_str);        /* display: No component */
  Display_NL_EEString(Failed2_str);     /* display: found!*/  

  MissedParts++;              /* increase counter */
}



/*
 *  show error
 */

void Show_Error()
{
  if (Check.Type == TYPE_DISCHARGE)     /* discharge failed */
  {
    Display_EEString(DischargeFailed_str);   /* display: Battery? */

    /* display probe number and remaining voltage */
    Display_NextLine();
    Display_ProbeNumber(Check.Probe);
    Display_Char(':');
    Display_Space();
    Display_Value(Check.U, -3, 'V');
  }
}



/*
 *  show single (first) resistor
 *
 *  requires:
 *  - ID1: pin ID #1 character
 *  - ID2: pin ID #2 character
 */

void Show_SingleResistor(uint8_t ID1, uint8_t ID2)
{
  Resistor_Type     *Resistor;     /* pointer to resistor */

  Resistor = &Resistors[0];        /* pointer to first resistor */

  /* show pinout */
  Display_Char(ID1);
  Display_EEString(Resistor_str);
  Display_Char(ID2); 

  /* show resistance value */
  Display_Space();
  Display_Value(Resistor->Value, Resistor->Scale, LCD_CHAR_OMEGA);
}



/*
 *  show resistor(s)
 */

void Show_Resistor(void)
{
  Resistor_Type     *R1;           /* pointer to resistor #1 */
  Resistor_Type     *R2;           /* pointer to resistor #2 */
  uint8_t           Pin;           /* ID of common pin */

  R1 = &Resistors[0];              /* pointer to first resistor */

  if (Check.Resistors == 1)        /* single resistor */
  {
    R2 = NULL;                     /* disable second resistor */
    Pin = R1->A;                   /* make B the first pin */
  }
  else                             /* multiple resistors */
  {
    R2 = R1;
    R2++;                          /* pointer to second resistor */
    #ifdef UI_SERIAL_COMMANDS
    /* set data for remote commands */
    Info.Quantity = 2;             /* got two */
    #endif

    if (Check.Resistors == 3)      /* three resistors */
    {
      Resistor_Type     *Rmax;     /* pointer to largest resistor */    

      /*
       *  3 resistors mean 2 single resistors and both resistors in series.
       *  So we have to single out that series resistor by finding the
       *  largest resistor.
       */

      Rmax = R1;                   /* starting point */
      for (Pin = 1; Pin <= 2; Pin++)
      {
        if (CmpValue(R2->Value, R2->Scale, Rmax->Value, Rmax->Scale) == 1)
        {
          Rmax = R2;          /* update largest one */
        }

        R2++;                 /* next one */
      }

      /* get the two smaller resistors */
      if (R1 == Rmax) R1++;
      R2 = R1;
      R2++;
      if (R2 == Rmax) R2++;
    }

    /* find common pin of both resistors */
    if ((R1->A == R2->A) || (R1->A == R2->B)) Pin = R1->A;
    else Pin = R1->B;
  }

  #ifdef UI_SERIAL_COMMANDS
  /* set data for remote commands */
  Info.Comp1 = (void *)R1;       /* first resistor */
  Info.Comp2 = (void *)R2;       /* second resistor */
  #endif


  /*
   *  display the pins
   */

  /* first resistor */
  if (R1->A != Pin) Display_ProbeNumber(R1->A);
  else Display_ProbeNumber(R1->B);
  Display_EEString(Resistor_str);
  Display_ProbeNumber(Pin);

  if (R2)           /* second resistor */
  {
    Display_EEString(Resistor_str);
    if (R2->A != Pin) Display_ProbeNumber(R2->A);
    else Display_ProbeNumber(R2->B);
  }


  /*
   *  display the values
   */

  /* first resistor */
  Display_NextLine();
  Display_Value(R1->Value, R1->Scale, LCD_CHAR_OMEGA);

  if (R2)                /* second resistor */
  {
    Display_Space();
    Display_Value(R2->Value, R2->Scale, LCD_CHAR_OMEGA);
  }
  #ifdef SW_INDUCTOR
  else                   /* single resistor */
  {
    /* get inductance and display if relevant */
    if (MeasureInductor(R1) == 1)
    {
      Display_Space();
      Display_Value(Inductor.Value, Inductor.Scale, 'H');

      #ifdef UI_SERIAL_COMMANDS
      /* set data for remote commands */
      Info.Flags |= INFO_R_L;      /* inductance measured */
      #endif
    }
  }
  #endif
}


/*
 *  show capacitor
 */

void Show_Capacitor(void)
{
  Capacitor_Type    *MaxCap;       /* pointer to largest cap */
  Capacitor_Type    *Cap;          /* pointer to cap */
  #if defined (SW_ESR) || defined (SW_OLD_ESR)
  uint16_t          ESR;           /* ESR (in 0.01 Ohms) */
  #endif
  uint8_t           Counter;       /* loop counter */

  /* find largest cap */
  MaxCap = &Caps[0];               /* pointer to first cap */
  Cap = MaxCap;

  for (Counter = 1; Counter <= 2; Counter++) 
  {
    Cap++;                         /* next cap */

    if (CmpValue(Cap->Value, Cap->Scale, MaxCap->Value, MaxCap->Scale) == 1)
    {
      MaxCap = Cap;
    }
  }

  #ifdef UI_SERIAL_COMMANDS
  /* set data for remote commands */
  Info.Comp1 = (void *)MaxCap;     /* largest cap */
  #endif

  /* display pinout */
  Display_ProbeNumber(MaxCap->A);  /* display pin #1 */
  Display_EEString(Cap_str);       /* display capacitor symbol */
  Display_ProbeNumber(MaxCap->B);  /* display pin #2 */

  /* show capacitance */
  Display_NextLine();              /* move to next line */
  Display_Value(MaxCap->Value, MaxCap->Scale, 'F');

  #if defined (SW_ESR) || defined (SW_OLD_ESR)
  /* show ESR */
  ESR = MeasureESR(MaxCap);        /* measure ESR */
  if (ESR < UINT16_MAX)            /* if successfull */
  {
    Display_Space();
    Display_Value(ESR, -2, LCD_CHAR_OMEGA);  /* display ESR */
  }
    #ifdef UI_SERIAL_COMMANDS
    /* set data for remote commands */
    Info.Val1 = ESR;               /* copy ESR */
    #endif
  #endif

  /* show discharge leakage current */
  if (MaxCap->I_leak > 0)
  {
    Display_NL_EEString_Space(I_leak_str);
    Display_Value(MaxCap->I_leak, -8, 'A');  /* in 10nA */
  }
}



/*
 *  show current (leakage or whatever) of semiconductor
 */

void Show_SemiCurrent(const unsigned char *String)
{
  if (CmpValue(Semi.I_value, Semi.I_scale, 50, -9) >= 0)  /* show if >=50nA */
  {
    Display_NL_EEString_Space(String);               /* display: <string> */
    Display_Value(Semi.I_value, Semi.I_scale, 'A');  /* display current */
  }
}



#ifndef UI_SERIAL_COMMANDS

/*
 *  display capacitance of a diode
 *
 *  requires:
 *  - pointer to diode structure
 */

void Show_Diode_Cap(Diode_Type *Diode)
{
  /* get capacitance (reversed direction) */
  MeasureCap(Diode->C, Diode->A, 0);

  /* and show capacitance */
  Display_Value(Caps[0].Value, Caps[0].Scale, 'F');
}

#endif



/*
 *  show diode
 */

void Show_Diode(void)
{
  Diode_Type        *D1;           /* pointer to diode #1 */
  Diode_Type        *D2 = NULL;    /* pointer to diode #2 */
  uint8_t           CapFlag = 1;   /* flag for capacitance output */
  uint8_t           A = 5;         /* ID of common anode */
  uint8_t           C = 5;         /* ID of common cothode */
  uint8_t           R_Pin1 = 5;    /* B_E resistor's pin #1 */
  uint8_t           R_Pin2 = 5;    /* B_E resistor's pin #2 */
  uint8_t           n;             /* counter */
  uint8_t           m;             /* counter */

  D1 = &Diodes[0];                 /* pointer to first diode */

  /*
   *  figure out which diodes to display
   */

  if (Check.Diodes == 1)           /* single diode */
  {
    C = D1->C;                     /* make cathode first pin */
  }
  else if (Check.Diodes == 2)      /* two diodes */
  {
    D2 = D1;
    D2++;                          /* pointer to second diode */

    if (D1->A == D2->A)            /* common anode */
    {
      A = D1->A;                   /* save common anode */

      /* possible PNP BJT with low value B-E resistor and flyback diode */
      R_Pin1 = D1->C;
      R_Pin2 = D2->C;
    }
    else if (D1->C == D2->C)       /* common cathode */
    {
      C = D1->C;                   /* save common cathode */

      /* possible NPN BJT with low value B-E resistor and flyback diode */
      R_Pin1 = D1->A;
      R_Pin2 = D2->A;
    }
    else if ((D1->A == D2->C) && (D1->C == D2->A))   /* anti-parallel */
    {
      A = D1->A;                   /* anode and cathode */
      C = A;                       /* are the same */
      CapFlag = 0;                 /* skip capacitance */
    }
  }
  else if (Check.Diodes == 3)      /* three diodes */
  {
    /*
     *  Two diodes in series are detected as a virtual third diode:
     *  - Check for any possible way the 2 diodes could be connected in series.
     *  - Only once the cathode of diode #1 matches the anode of diode #2.
     */

    for (n = 0; n <= 2; n++)       /* loop for first diode */
    {
      D1 = &Diodes[n];             /* get pointer of first diode */

      for (m = 0; m <= 2; m++)     /* loop for second diode */
      {
        D2 = &Diodes[m];           /* get pointer of second diode */

        if (n != m)                /* don't check same diode :-) */
        {
          if (D1->C == D2->A)      /* got match */
          {
            n = 5;                 /* end loops */
            m = 5;
          }
        }
      }
    }

    if (n < 5) D2 = NULL;          /* no match found */
    C = D1->C;                     /* cathode of first diode */
    A = 3;                         /* in series mode */
  }
  else                             /* too much diodes */
  {
    Display_EEString(Diode_AC_str);     /* display: -|>|- */
    Display_Space();                    /* display space */
    Display_Char(Check.Diodes + '0');   /* display number of diodes found */
    #ifdef UI_SERIAL_COMMANDS
    /* set data for remote commands */
    Info.Quantity = Check.Diodes;       /* set quantity */
    #endif

    return;
  }

  #ifdef UI_SERIAL_COMMANDS
  /* set data for remote commands */
  Info.Comp1 = (void *)D1;       /* first diode */
  Info.Comp2 = (void *)D2;       /* second diode */
  #endif


  /*
   *  display pins 
   */

  /* first diode */
  if (A < 3)        /* common anode: show C first */
  {
    Display_ProbeNumber(D1->C);         /* show C */
    Display_EEString(Diode_CA_str);     /* show -|<- */
    Display_ProbeNumber(A);             /* show A */
  }
  else              /* common cathode: show A first */
  {
    Display_ProbeNumber(D1->A);         /* show A */
    Display_EEString(Diode_AC_str);     /* show ->|- */
    Display_ProbeNumber(C);             /* show C */
  }

  if (D2)           /* second diode */
  {
    if (A <= 3)          /* common anode or in-series */
    {
      Display_EEString(Diode_AC_str);   /* show ->|- */
    }
    else                 /* common cathode */
    {
      Display_EEString(Diode_CA_str);   /* show -|<- */
    }

    if (A == C)          /* anti parallel */
    {
      n = D2->A;              /* get anode */
    }
    else if (A <= 3)     /* common anode or in-series */
    {
      n = D2->C;              /* get cathode */
    }
    else                 /* common cathode */
    {
      n = D2->A;              /* get anode */
    }

    Display_ProbeNumber(n);             /* display pin */

    #ifdef UI_SERIAL_COMMANDS
    /* set data for remote commands */
    Info.Quantity = 2;       /* got two */
    #endif
  }

  /* check for B-E resistor of possible BJT */
  if (R_Pin1 < 5)                  /* possible BJT */
  {
    /* B-E resistor below 25kOhms */
    if (CheckSingleResistor(R_Pin1, R_Pin2, 25) == 1)
    {
      /* show: PNP/NPN? */
      Display_Space();
      if (A < 3)                        /* PNP */
      {
        Display_EEString(PNP_str);
        #ifdef UI_SERIAL_COMMANDS
        /* set data for remote commands */
        Info.Flags |= INFO_D_R_BE | INFO_D_BJT_PNP;    /* R_BE & PNP */
        #endif
      }
      else                              /* NPN */
      {
        Display_EEString(NPN_str);
        #ifdef UI_SERIAL_COMMANDS
        /* set data for remote commands */
        Info.Flags |= INFO_D_R_BE | INFO_D_BJT_NPN;    /* R_BE & NPN */
        #endif
      }
      Display_Char('?');

      Display_NextLine();               /* move to line #2 */
      R_Pin1 += '1';                    /* convert pin ID to character */
      R_Pin2 += '1';
      Show_SingleResistor(R_Pin1, R_Pin2);   /* show resistor */
      CapFlag = 0;                      /* skip capacitance */
    }
  }


  /*
   *  display:
   *  - Uf (forward voltage)
   *  - reverse leakage current (for single diodes)
   *  - capacitance (not for anti-parallel diodes)
   */

  /* display Uf */
  Display_NL_EEString_Space(Vf_str);    /* display: Vf */

  /* first diode */
  Display_Value(D1->V_f, -3, 'V');      /* in mV */

  Display_Space();

  /* display low current Uf and reverse leakage current for a single diode */
  if (D2 == NULL)                       /* single diode */
  {
    /* display low current Uf if it's quite low (Ge/Schottky diode) */
    if (D1->V_f2 < 250)            /* < 250mV */
    {
      Display_Char('(');
      Display_Value(D1->V_f2, 0, 0);    /* no unit */
      Display_Char(')');
    }

    /* reverse leakage current */
    UpdateProbes(D1->C, D1->A, 0);      /* reverse diode */
    GetLeakageCurrent(1);               /* get current */
    Show_SemiCurrent(I_R_str);          /* display I_R */

    #ifdef UI_SERIAL_COMMANDS
    /* set data for remote commands */
    Info.Flags |= INFO_D_I_R;           /* measured I_R */
    #endif
  }
  else                                  /* two diodes */
  {
    /* show Uf of second diode */
    Display_Value(D2->V_f, -3, 'V');
  }

  /* display capacitance */
  if (CapFlag == 1)                     /* if feasable */ 
  {
    Display_NL_EEString_Space(DiodeCap_str);   /* display: C */

    #ifndef UI_SERIAL_COMMANDS
    /* first diode */
    Show_Diode_Cap(D1);                 /* measure & show capacitance */

    if (D2)                             /* second diode */
    {
      Display_Space();
      Show_Diode_Cap(D2);               /* measure & show capacitance */
    }
    #endif

    #ifdef UI_SERIAL_COMMANDS
    /* first diode */
    MeasureCap(D1->C, D1->A, 0);        /* get capacitance (reversed direction) */
    Display_Value(Caps[0].Value, Caps[0].Scale, 'F');

    if (D2)                   /* second diode */
    {
      Display_Space();
      MeasureCap(D2->C, D2->A, 1);      /* get capacitance (reversed direction) */
      Display_Value(Caps[1].Value, Caps[1].Scale, 'F');
    }
    #endif
  }
}



/*
 *  show BJT
 */

void Show_BJT(void)
{
  Diode_Type        *Diode;        /* pointer to diode */
  unsigned char     *String;       /* string pointer (EEPROM) */
  uint8_t           Char;          /* character */
  uint8_t           BE_A;          /* V_BE: pin acting as anode */
  uint8_t           BE_C;          /* V_BE: pin acting as cathode */
  uint8_t           CE_A;          /* flyback diode: pin acting as anode */
  uint8_t           CE_C;          /* flyback diode: pin acting as cathode */
  uint16_t          V_BE;          /* V_BE */
  int16_t           Slope;         /* slope of forward voltage */

  /*
   *  Mapping for Semi structure:
   *  A   - Base pin
   *  B   - Collector pin
   *  C   - Emitter pin
   *  U_1 - U_BE (mV) (not yet)
   *  F_1 - hFE
   *  I_value/I_scale - I_CEO
   */

  /* preset stuff based on BJT type */
  if (Check.Type & TYPE_NPN)       /* NPN */
  {
    String = (unsigned char *)NPN_str;       /* "NPN" */

    /* direction of B-E diode: B -> E */
    BE_A = Semi.A;       /* anode at base */
    BE_C = Semi.C;       /* cathode at emitter */

    /* direction of optional flyback diode */
    CE_A = Semi.C;       /* anode at emitter */
    CE_C = Semi.B;       /* cathode at collector */
    Char = LCD_CHAR_DIODE_CA;      /* |<| */
  }
  else                             /* PNP */
  {
    String = (unsigned char *)PNP_str;       /* "PNP" */

    /* direction of B-E diode: E -> B */
    BE_A = Semi.C;       /* anode at emitter */
    BE_C = Semi.A;       /* cathode at base */

    /* direction of optional flyback diode */
    CE_A = Semi.B;       /* anode at collector */
    CE_C = Semi.C;       /* cathode at emitter */
    Char = LCD_CHAR_DIODE_AC;      /* |>| */
  }

  /* display type */
  Display_EEString_Space(BJT_str);      /* display: BJT */
  Display_EEString(String);             /* display: NPN / PNP */

  /* parasitic BJT (freewheeling diode on same substrate) */
  if (Check.Type & TYPE_PARASITIC)
  {
    Display_Char('+');
  }

  Display_NextLine();                   /* next line (#2) */

  /* display pinout */
  Show_SemiPinout('B', 'C', 'E');

  /* optional freewheeling diode */
  Diode = SearchDiode(CE_A, CE_C);     /* search for matching diode */
  if (Diode != NULL)                   /* got it */
  {
    Display_Space();          /* display space */
    Display_Char('C');        /* collector */
    Display_Char(Char);       /* display diode symbol */
    Display_Char('E');        /* emitter */

    #ifdef UI_SERIAL_COMMANDS
    /* set data for remote commands */
    Info.Flags |= INFO_BJT_D_FB;   /* found flyback diode */
    Info.Comp1 = Diode;            /* copy diode */
    #endif
  }


  /*
   *  display either optional B-E resistor or h_FE & V_BE
   */

  /* check for B-E resistor below 25kOhms */
  if (CheckSingleResistor(BE_C, BE_A, 25) == 1)   /* found B-E resistor */
  {
    Display_NextLine();            /* next line (#3) */
    Show_SingleResistor('B', 'E');
    /* B-E resistor renders hFE and V_BE measurements useless */

    #ifdef SW_SYMBOLS
    UI.SymbolLine = 4;             /* display fancy pinout in line #4 */
    #endif

    #ifdef UI_SERIAL_COMMANDS
    /* set data for remote commands */
    Info.Flags |= INFO_BJT_R_BE;   /* R_BE */
    #endif
  }
  else                                            /* no B-E resistor found */
  {
    /* h_FE and V_BE */

    /* display h_FE */
    Display_NL_EEString_Space(h_FE_str);     /* display: hFE */
    Display_Value(Semi.F_1, 0, 0);           /* display h_FE */

    /* display V_BE (taken from diode's forward voltage) */
    Diode = SearchDiode(BE_A, BE_C);    /* search for matching B-E diode */
    if (Diode != NULL)                  /* got it */
    {
      Display_NL_EEString_Space(V_BE_str);   /* display: Vbe */

      /*
       *  V_f is quite linear for a logarithmicly scaled I_b.
       *  So we may interpolate the V_f values of low and high test current
       *  measurements for a virtual test current. Low test current is 10µA
       *  and high test current is 7mA. That's a logarithmic scale of
       *  3 decades.
       */

      /* calculate slope for one decade */
      Slope = Diode->V_f - Diode->V_f2;
      Slope /= 3;

      /* select V_BE based on hFE */
      if (Semi.F_1 < 100)               /* low h_FE */
      {
        /*
         *  BJTs with low hFE are power transistors and need a large I_b
         *  to drive the load. So we simply take Vf of the high test current
         *  measurement (7mA). 
         */

        V_BE = Diode->V_f;
      }
      else if (Semi.F_1 < 250)          /* mid-range h_FE */
      {
        /*
         *  BJTs with a mid-range hFE are signal transistors and need
         *  a small I_b to drive the load. So we interpolate Vf for
         *  a virtual test current of about 1mA.
         */

        V_BE = Diode->V_f - Slope;
      }
      else                              /* high h_FE */
      {
        /*
         *  BJTs with a high hFE are small signal transistors and need
         *  only a very small I_b to drive the load. So we interpolate Vf
         *  for a virtual test current of about 0.1mA.
         */

        V_BE = Diode->V_f2 + Slope;
      }

      Display_Value(V_BE, -3, 'V');     /* in mV */

      #ifdef UI_SERIAL_COMMANDS
      /* set data for remote commands */
      Info.Val1 = V_BE;            /* copy V_BE */
      #endif
    }
  }

  /* I_CEO: collector emitter open current (leakage) */
  Show_SemiCurrent(I_CEO_str);          /* display I_CEO */
}



/*
 *  show MOSFET/IGBT extras
 *  - diode
 *  - V_th
 *  - Cgs
 */

void Show_FET_Extras(void)
{
  Diode_Type        *Diode;        /* pointer to diode */  
  uint8_t           Anode;         /* anode of diode */
  uint8_t           Cathode;       /* cathode of diode */
  uint8_t           Char_1;        /* pin name */
  uint8_t           Char_2;        /* pin name */
  uint8_t           Symbol;        /* diode symbol */

  /*
   *  Mapping for Semi structure:
   *  A   - Gate pin
   *  B   - Drain pin
   *  C   - Source pin
   *  U_1 - R_DS_on (0.01 Ohms)
   *  U_2 - V_th (mV)
   */

  /*
   *  show instrinsic/freewheeling diode
   */

  if (Check.Type & TYPE_N_CHANNEL)      /* n-channel/NPN */
  {
    Anode = Semi.C;                /* source/emitter */
    Cathode = Semi.B;              /* drain/collector */
    Symbol = LCD_CHAR_DIODE_CA;    /* |<| */
  }
  else                                  /* p-channel/PNP */
  {
    Anode = Semi.B;                /* drain/collector */
    Cathode = Semi.C;              /* source/emitter */
    Symbol = LCD_CHAR_DIODE_AC;    /* |>| */
  }

  if (Check.Found == COMP_FET)     /* FET */
  {
    Char_1 = 'D';
    Char_2 = 'S';
  }
  else                             /* IGBT */
  {
    Char_1 = 'C';
    Char_2 = 'E';
  }

  /* search for matching diode */
  Diode = SearchDiode(Anode, Cathode);
  if (Diode != NULL)          /* got it */
  {
    /* show diode */
    Display_Space();          /* space */
    Display_Char(Char_1);     /* left pin name */
    Display_Char(Symbol);     /* diode symbol */
    Display_Char(Char_2);     /* right pin name */

    #ifdef UI_SERIAL_COMMANDS
    /* set data for remote commands */
    Info.Flags |= INFO_FET_D_FB;   /* found flyback diode */
    Info.Comp1 = Diode;            /* copy diode */
    #endif
  }

  /* skip remaining stuff for depletion-mode FETs/IGBTs */
  if (Check.Type & TYPE_DEPLETION) return;

  /* gate threshold voltage V_th */
  if (Semi.U_2 != 0)
  {
    Display_NL_EEString_Space(Vth_str);      /* display: Vth */
    Display_SignedValue(Semi.U_2, -3, 'V');  /* display V_th in mV */

    #ifdef UI_SERIAL_COMMANDS
    /* set data for remote commands */
    Info.Flags |= INFO_FET_V_TH;             /* measured Vth */
    #endif
  }

  /* display gate-source capacitance C_GS */
  /* todo: display "Cge" for IGBT? */
  Display_NL_EEString_Space(Cgs_str);             /* display: Cgs */
  Display_Value(Semi.C_value, Semi.C_scale, 'F'); /* display value and unit */

  #ifdef UI_SERIAL_COMMANDS
  /* set data for remote commands */
  Info.Flags |= INFO_FET_C_GS;               /* measured C_GS */
  #endif

  /* display R_DS_on, if available */
  if (Semi.U_1 > 0)
  {
    Display_NL_EEString_Space(R_DS_str);          /* display: Rds */
    Display_Value(Semi.U_1, -2, LCD_CHAR_OMEGA);  /* display value */

    #ifdef UI_SERIAL_COMMANDS
    /* set data for remote commands */
    Info.Flags |= INFO_FET_R_DS;             /* measured R_DS */
    #endif
  }

  /* display V_f of diode, if available */
  if (Diode != NULL)
  {
    Display_NL_EEString_Space(Vf_str);       /* display: Vf */
    Display_Value(Diode->V_f, -3, 'V');      /* display value */
  }
}



/*
 *  show FET/IGBT channel type
 */

void Show_FET_Channel(void)
{
  Display_Space();                      /* display space */

  /* channel type */
  if (Check.Type & TYPE_N_CHANNEL)      /* n-channel */
  {
    Display_Char('N');                  /* display: N */
  }
  else                                  /* p-channel */
  {
    Display_Char('P');                  /* display: P */
  }

  Display_EEString(Channel_str);        /* display: -ch */
}



/*
 *  show FET/IGBT mode
 */

void Show_FET_Mode(void)
{
  Display_Space();                      /* display space */

  if (Check.Type & TYPE_ENHANCEMENT)    /* enhancement mode */
  {
    Display_EEString(Enhancement_str);  /* display: enh. */
  }
  else                                  /* depletion mode */
  {
    Display_EEString(Depletion_str);    /* display: dep. */
  }
}



/*
 *  show FET (MOSFET & JFET)
 */

void Show_FET(void)
{
  /*
   *  Mapping for Semi structure:
   *  A   - Gate pin
   *  B   - Drain pin
   *  C   - Source pin
   *  U_1 - R_DS_on (0.01 Ohms)
   *  U_2 - V_th (mV)
   */

  /* display type */
  if (Check.Type & TYPE_MOSFET)    /* MOSFET */
  {
    Display_EEString(MOS_str);          /* display: MOS */
  }
  else                             /* JFET */
  {
    Display_Char('J');                  /* display: J */
  }
  Display_EEString(FET_str);       /* display: FET */

  /* display channel type */
  Show_FET_Channel();
      
  /* display mode for MOSFETs*/
  if (Check.Type & TYPE_MOSFET) Show_FET_Mode();

  /* pinout */
  Display_NextLine();                   /* next line (#2) */

  if (Check.Type & TYPE_SYMMETRICAL)    /* symmetrical Drain and Source */
  {
    /* we can't distinguish D and S */
    Show_SemiPinout('G', 'x', 'x');     /* show pinout */
  }
  else                                  /* unsymmetrical Drain and Source */
  {
    Show_SemiPinout('G', 'D', 'S');     /* show pinout */
  }

  /* show body diode, V_th and Cgs for MOSFETs */
  if (Check.Type & TYPE_MOSFET) Show_FET_Extras();

  /* show I_DSS for depletion mode FET */
  if (Check.Type & TYPE_DEPLETION)
  {
    Show_SemiCurrent(I_DSS_str);        /* display Idss */
  }
}



/*
 *  show IGBT  
 */

void Show_IGBT(void)
{
  /*
   *  Mapping for Semi structure:
   *  A   - Gate pin
   *  B   - Collector pin
   *  C   - Emitter pin
   *  U_2 - V_th (mV)
   */

  Display_EEString(IGBT_str);      /* display: IGBT */
  Show_FET_Channel();              /* display channel type */
  Show_FET_Mode();                 /* display mode */

  Display_NextLine();              /* next line (#2) */
  Show_SemiPinout('G', 'C', 'E');  /* show pinout */

  Show_FET_Extras();               /* show diode, V_th and C_GE */
}



/*
 *  show Thyristor and Triac
 */

void Show_ThyristorTriac(void)
{
  /*
   *  Mapping for Semi structure:
   *        SCR        Triac
   *  A   - Gate       Gate
   *  B   - Anode      MT2
   *  C   - Cathode    MT1
   *  U_1 - V_GT (mV)
   */

  /* display component type any pinout */
  if (Check.Found == COMP_THYRISTOR)    /* SCR */
  {
    Display_EEString(Thyristor_str);    /* display: thyristor */
    Display_NextLine();                 /* next line (#2) */
    Show_SemiPinout('G', 'A', 'C');     /* display pinout */
  }
  else                                  /* Triac */
  {
    Display_EEString(Triac_str);        /* display: triac */
    Display_NextLine();                 /* next line (#2) */
    Show_SemiPinout('G', '2', '1');     /* display pinout */
  }

  /* show V_GT (gate trigger voltage) */
  if (Semi.U_1 > 0)                /* show if not zero */
  {
    Display_NL_EEString_Space(V_GT_str);     /* display: V_GT */
    Display_Value(Semi.U_1, -3, 'V');        /* display V_GT in mV */
  }
}



/*
 *  show PUT
 */

void Show_PUT(void)
{
  /*
   *  Mapping for AltSemi structure:
   *  A   - Gate
   *  B   - Anode
   *  C   - Cathode
   *  U_1 - V_f
   *  U_2 - V_T
   *
   *  Mapping for Semi structure:
   *  A   - Gate
   *  B   - Anode
   *  C   - Cathode
   */

  Display_EEString(PUT_str);            /* display: PUT */
  Display_NextLine();                   /* move to line #2 */
  Show_SemiPinout('G', 'A', 'C');       /* display pinout */

  /* display V_T */
  Display_NL_EEString_Space(V_T_str);   /* display: VT */
  Display_Value(AltSemi.U_2, -3, 'V');  /* display V_T */

  /* display V_f */
  Display_NL_EEString_Space(Vf_str);    /* display: Vf */
  Display_Value(AltSemi.U_1, -3, 'V');  /* display V_f */
}



#ifdef SW_UJT

/*
 *  show UJT
 */

void Show_UJT(void)
{
  /*
   *  Mapping for AltSemi structure:
   *  A   - Emitter
   *  B   - B2
   *  C   - B1
   +
   *  Mapping for Semi structure:
   *  A   - Gate
   *  B   - Anode
   *  C   - Cathode
   */

  Display_EEString(UJT_str);            /* display: UJT */
  Display_NextLine();                   /* next line (#2) */
  Show_SemiPinout('E', '2', '1');       /* display pinout */

  /* display r_BB */
  Display_NL_EEString_Space(R_BB_str);  /* display: R_BB */  
  Display_Value(Resistors[0].Value, Resistors[0].Scale, LCD_CHAR_OMEGA);
}

#endif



/* ************************************************************************
 *   the one and only main()
 * ************************************************************************ */


/*
 *  main function
 */

int main(void)
{
  uint8_t           Test;          /* test value */
  uint8_t           Key;           /* user feedback */
  #if defined (HW_REF25) || ! defined (BAT_NONE)
  uint16_t          U_Bat;         /* voltage of power supply */
  uint32_t          Temp;          /* some value */
  #endif


  /*
   *  init hardware
   */

  /* switch on power to keep me alive */
  CONTROL_DDR = (1 << POWER_CTRL);      /* set pin as output */
  CONTROL_PORT = (1 << POWER_CTRL);     /* set pin to drive power management transistor */

  /* set up MCU */
  MCUCR = (1 << PUD);                        /* disable pull-up resistors globally */
  ADCSRA = (1 << ADEN) | ADC_CLOCK_DIV;      /* enable ADC and set clock divider */

  #ifdef HW_DISCHARGE_RELAY
  /* init discharge relay (safe mode) */
                                        /* ADC_PORT should be 0 */
  ADC_DDR = (1 << TP_REF);              /* short circuit probes */
  #endif

  /* catch watchdog */  
  Test = (MCUSR & (1 << WDRF));         /* save watchdog flag */
  MCUSR &= ~(1 << WDRF);                /* reset watchdog flag */
  wdt_disable();                        /* disable watchdog */


  /*
   *  set important default values
   */

  #if defined (UI_AUTOHOLD) || defined (UI_SERIAL_COMMANDS)
    /* reset mode/state flags and set auto-hold mode */
    Cfg.OP_Mode = OP_AUTOHOLD;          /* set auto-hold */
  #else
    /* reset mode/state flags and set continous mode */
    Cfg.OP_Mode = OP_NONE;              /* none = continous */
  #endif
  Cfg.OP_Control = OP_OUT_LCD;          /* reset control/signal flags */
                                        /* enable output to display */
  #ifdef SAVE_POWER
  Cfg.SleepMode = SLEEP_MODE_PWR_SAVE;  /* sleep mode: power save */
  #endif                                /* we have to keep Timer2 running */


  /*
   *  set up busses and interfaces
   */

  #ifdef HW_SERIAL
  Serial_Setup();                       /* set up TTL serial interface */
  #endif

  #ifdef HW_I2C
  I2C_Setup();                          /* set up I2C bus */
  #endif

  /* LCD module */
  LCD_BusSetup();                       /* set up LCD bus */
  #ifdef HW_TOUCH
  Touch_BusSetup();                     /* set up touch screen */
  #endif

  #ifdef ONEWIRE_IO_PIN
  OneWire_Setup();                      /* set up OneWire bus */
  #endif


  /*
   *  watchdog was triggered (timeout 2s)
   *  - This is after the MCU done a reset driven by the watchdog.
   *  - Does only work if the capacitor at the base of the power management
   *    transistor is large enough to survive a MCU reset. Otherwise the
   *    tester simply loses power.
   */

  if (Test)
  {
    LCD_Clear();                        /* display was initialized before */
    Display_EEString(Timeout_str);      /* display: timeout */
    Display_NL_EEString(Error_str);     /* display: error */
    MilliSleep(2000);                   /* give user some time to read */
    CONTROL_PORT = 0;                   /* power off myself */
    return 0;                           /* exit program */
  }


  /*
   *  operation mode selection
   *  - short key press -> continous mode
   *  - long key press -> auto-hold mode
   *  - very long key press -> reset to defaults
   */

  Key = 0;                              /* reset key press type */

  /* catch key press */
  if (!(CONTROL_PIN & (1 << TEST_BUTTON)))   /* test button pressed */
  {
    Test = 0;                      /* ticks counter */

    while (Key == 0)               /* loop until we got a type */
    {
      MilliSleep(20);                   /* wait 20ms */

      if (!(CONTROL_PIN & (1 << TEST_BUTTON)))    /* button still pressed */
      {
        Test++;                         /* increase counter */
        if (Test > 100) Key = 3;        /* >2000ms */
      }
      else                                        /* button released */
      {
        Key = 1;                        /* <300ms */
        if (Test > 15) Key = 2;         /* >300ms */
      }
    }
  }


  #ifndef UI_SERIAL_COMMANDS
  /* key press >300ms selects alternative operation mode */
  if (Key > 1)
  {
    #ifdef UI_AUTOHOLD
      /* change mode to continous */
      Cfg.OP_Mode &= ~OP_AUTOHOLD;      /* clear auto-hold */
    #else
      /* change mode to auto-hold */
      Cfg.OP_Mode |= OP_AUTOHOLD;       /* set auto-hold */
    #endif
  }
  #endif


  /*
   *  init display module
   */

  LCD_Init();                           /* initialize LCD */
  UI.LineMode = LINE_STD;               /* reset next-line mode */
  #ifdef LCD_COLOR
  UI.PenColor = COLOR_TITLE;            /* set pen color */
  #endif
  #ifdef HW_TOUCH
  Touch_Init();                         /* init touch screen */
  #endif


  /*
   *  load saved adjustment offsets and values
   */

  if (Key == 3)               /* key press >2s resets to defaults */
  {
    SetAdjustmentDefaults();       /* set default values */
  }
  else                        /* normal mode */
  {
    /* load adjustment values: profile #1 */
    ManageAdjustmentStorage(STORAGE_LOAD, 1);
  }

  /* set extra stuff */
  #ifdef SW_CONTRAST
  LCD_Contrast(NV.Contrast);            /* set LCD contrast */
  #endif


  /*
   *  welcome user
   */

  #ifdef UI_SERIAL_COPY
  SerialCopy_On();                      /* enable serial output & NL */
  #endif
  Display_EEString(Tester_str);         /* display: Component Tester */
  Display_NL_EEString(Version_str);     /* display firmware version */
  #ifdef UI_SERIAL_COPY
  SerialCopy_Off();                     /* disable serial output & NL */
  #endif
  #ifdef LCD_COLOR
  UI.PenColor = COLOR_PEN;              /* set pen color */
  #endif
  MilliSleep(1500);                     /* let the user read the display */


  /*
   *  init variables
   */

  /* cycling */
  MissedParts = 0;                      /* reset counter */
  Key = KEY_POWER_ON;                   /* just powered on */

  /* default offsets and values */
  Cfg.Samples = ADC_SAMPLES;            /* number of ADC samples */
  Cfg.AutoScale = 1;                    /* enable ADC auto scaling */
  Cfg.RefFlag = 1;                      /* no ADC reference set yet */
  Cfg.Vcc = UREF_VCC;                   /* voltage of Vcc */
  wdt_enable(WDTO_2S);		        /* enable watchdog (timeout 2s) */

  #ifdef HW_TOUCH
  /* adjust touch screen if not done yet */
  if ((Touch.X_Left == 0) && (Touch.X_Right == 0))
  {
    Test = Touch_Adjust();         /* adjust touch screen */

    if (Test == 0)                 /* error */
    {
      LCD_ClearLine2();
      Display_EEString(Error_str);      /* display: Error */
      MilliSleep(1000);                 /* smooth UI */
      TestKey(2500, CURSOR_BLINK | UI_OP_MODE);
    }
  }
  #endif

  sei();                           /* enable interrupts */


  /*
   *  main processing cycle
   */

cycle_start:

  /* reset variables */
  Check.Found = COMP_NONE;         /* no component */
  Check.Type = 0;                  /* reset type flags */
  Check.Done = DONE_NONE;
  Check.AltFound = COMP_NONE;      /* no alternative component */
  Check.Diodes = 0;                /* zero diodes */
  Check.Resistors = 0;             /* zero resistors */
  Semi.U_1 = 0;                    /* reset value */
  Semi.U_2 = 0;
  Semi.F_1 = 0;
  Semi.I_value = 0;
  AltSemi.U_1 = 0;
  AltSemi.U_2 = 0;
  #ifdef UI_SERIAL_COMMANDS
  Info.Quantity = 0;               /* zero components */
  Info.Selected = 1;               /* select first component */
  Info.Flags = INFO_NONE;          /* reset flags */
  Info.Comp1 = NULL;               /* reset pointer to first component */
  Info.Comp2 = NULL;               /* reset pointer to second component */
  #endif
  #ifdef HW_KEYS
  UI.KeyOld = KEY_NONE;            /* no key */
  UI.KeyStepOld = 1;               /* step size 1 */
  #endif
  #ifdef SW_SYMBOLS
  UI.SymbolLine = 3;               /* default: line #3 */
  #endif

  /* reset hardware */
  ADC_DDR = 0;                     /* set all pins of ADC port as input */
  #ifdef HW_DISCHARGE_RELAY
    /* this also switches the discharge relay to remove the short circuit */
  #endif

  UI.LineMode = LINE_KEEP;              /* next-line mode: keep first line */
  LCD_Clear();                          /* clear LCD */


  /*
   *  voltage reference
   */

  #ifdef HW_REF25
  /* external 2.5V reference */
  Cfg.Samples = 200;               /* do a lot of samples for high accuracy */
  U_Bat = ReadU(TP_REF);           /* read voltage of reference (mV) */
  Cfg.Samples = ADC_SAMPLES;       /* set samples back to default */

  /* check for valid voltage range */
  if ((U_Bat > 2250) && (U_Bat < 2750))      /* voltage is fine */
  {
    /* adjust Vcc (assuming 2.495V typically) */
    Temp = ((uint32_t)Cfg.Vcc * UREF_25) / U_Bat;
    Cfg.Vcc = (uint16_t)Temp;

    Cfg.OP_Mode |= OP_EXT_REF;          /* set flag */
  }
  else                                       /* voltage out of range */
  {
    Cfg.OP_Mode &= ~OP_EXT_REF;         /* clear flag */
  }
  #endif

  /* internal bandgap reference */
  Cfg.Bandgap = ReadU(ADC_BANDGAP);     /* dummy read for bandgap stabilization */
  Cfg.Samples = 200;                    /* do a lot of samples for high accuracy */
  Cfg.Bandgap = ReadU(ADC_BANDGAP);     /* get voltage of bandgap reference (mV) */
  Cfg.Samples = ADC_SAMPLES;            /* set samples back to default */
  Cfg.Bandgap += NV.RefOffset;          /* add voltage offset */ 


  /*
   *  battery check
   */

  #ifdef BAT_NONE
    /* no battery monitoring */
    Display_EEString(Tester_str);       /* display: Component Tester */
  #else
    /* get current battery voltage */
    U_Bat = ReadU(TP_BAT);              /* read voltage U2 (mV) */

    #ifdef BAT_DIVIDER
    /*
     *  ADC pin is connected to a voltage divider (top: R1 / bottom: R2).
     *  - U2 = (Uin / (R1 + R2)) * R2 
     *  - Uin = (U2 * (R1 + R2)) / R2
     */

    Temp = (((uint32_t)(BAT_R1 + BAT_R2) * 1000) / BAT_R2);   /* factor (0.001) */
    Temp *= U_Bat;                      /* Uin (0.001 mV) */
    Temp /= 1000;                       /* Uin (mV) */
    U_Bat = (uint16_t)Temp;
    #endif

    U_Bat += BAT_OFFSET;                /* add offset for voltage drop */

    /* display battery voltage */
    Display_EEString_Space(Battery_str);     /* display: Bat. */

    #ifdef BAT_EXT_UNMONITORED
    if (U_Bat < 900)               /* < 0.9V */
    {
      /* low voltage caused by diode's leakage current */
      Display_EEString(External_str);   /* display: ext */
    }
    else                           /* battery operation */
    {
    #endif

      /* display battery voltage */
      Display_Value(U_Bat / 10, -2, 'V');
      Display_Space();

      /* check limits */
      if (U_Bat < BAT_LOW)         /* low level reached */
      {
        Display_EEString(Low_str);      /* display: low */
        MilliSleep(2000);               /* let user read info */
        Key = KEY_POWER_OFF;            /* signal power off */
        goto cycle_action;              /* power off */
      }
      else if (U_Bat < BAT_WEAK)   /* warning level reached */
      {
        Display_EEString(Weak_str);     /* display: weak */
      }
      else                         /* ok */
      {
        Display_EEString(OK_str);       /* display: ok */
      }

    #ifdef BAT_EXT_UNMONITORED
    }
    #endif
  #endif


  /*
   *  probing
   */

  #ifdef UI_SERIAL_COMMANDS
  /* skip first probing after power-on */
  if (Key == KEY_POWER_ON)         /* first cycle */
  {
    goto cycle_control;            /* skip probing */
    /* will also change Key */
  }
  #endif

  /* display start of probing */
  Display_NL_EEString(Probing_str);     /* display: probing... */

  /* try to discharge any connected component */
  DischargeProbes();
  if (Check.Found == COMP_ERROR)   /* discharge failed */
  {
    goto show_component;           /* skip all other checks */
  }

  #ifdef UI_SHORT_CIRCUIT_MENU
  /* enter main menu if requested by short-circuiting all probes */
  if (ShortedProbes() == 3)        /* all probes short-circuited */
  {
    MainMenu();                    /* enter mainmenu */
    goto cycle_control;            /* skip probing */
  }
  #endif

  /* check all 6 combinations of the 3 probes */ 
  CheckProbes(PROBE_1, PROBE_2, PROBE_3);
  CheckProbes(PROBE_2, PROBE_1, PROBE_3);
  CheckProbes(PROBE_1, PROBE_3, PROBE_2);
  CheckProbes(PROBE_3, PROBE_1, PROBE_2);
  CheckProbes(PROBE_2, PROBE_3, PROBE_1);
  CheckProbes(PROBE_3, PROBE_2, PROBE_1);
  CheckAlternatives();             /* process alternatives */

  /* if component might be a capacitor */
  if ((Check.Found == COMP_NONE) ||
      (Check.Found == COMP_RESISTOR))
  {
    /* tell user to be patient with large caps :-) */
    Display_Space();
    Display_Char('C');    

    /* check all possible combinations */
    MeasureCap(PROBE_3, PROBE_1, 0);
    MeasureCap(PROBE_3, PROBE_2, 1);
    MeasureCap(PROBE_2, PROBE_1, 2);
  }


  /*
   *  output test results
   */

show_component:

  LCD_Clear();                     /* clear LCD */

  /* next-line mode */
  Test = LINE_KEEP | LINE_KEY;     /* keep first line and wait for key/timeout */
  #ifdef UI_SERIAL_COMMANDS
  if (Key == KEY_PROBE)            /* probing by command */
  {
    Test = LINE_KEEP;              /* don't wait for key/timeout */
  }
  #endif
  UI.LineMode = Test;              /* change mode */

  #ifdef UI_SERIAL_COPY
  SerialCopy_On();                 /* enable serial output & NL */
  #endif

  #ifdef UI_SERIAL_COMMANDS
  if (Check.Found >= COMP_RESISTOR)
  {
    Info.Quantity = 1;             /* got one at least */
  }
  #endif

  /* call output function based on component type */
  switch (Check.Found)
  {
    case COMP_ERROR:
      Show_Error();
      break;

    case COMP_DIODE:
      Show_Diode();
      break;

    case COMP_BJT:
      Show_BJT();
      break;

    case COMP_FET:
      Show_FET();
      break;

    case COMP_IGBT:
      Show_IGBT();
      break;

    case COMP_THYRISTOR:
      Show_ThyristorTriac();
      break;

    case COMP_TRIAC:
      Show_ThyristorTriac();
      break;

    case COMP_PUT:
      Show_PUT();
      break;

    #ifdef SW_UJT
    case COMP_UJT:
      Show_UJT();
      break;
    #endif

    case COMP_RESISTOR:
      Show_Resistor();
      break;

    case COMP_CAPACITOR:
      Show_Capacitor();
      break;

    default:                  /* no component found */
      Show_Fail();
      break;
  }

  #ifdef UI_SERIAL_COPY
  SerialCopy_Off();                  /* disable serial output & NL */
  #endif

  #ifdef SW_SYMBOLS
  /* display fancy pinout for 3-pin semiconductors */
  if (Check.Found >= COMP_BJT)     /* 3-pin semi */
  {
    if (UI.SymbolLine)             /* not zero */
    {
      LCD_FancySemiPinout(UI.SymbolLine);    /* display pinout */
    }
  }
  #endif

  #ifdef UI_SERIAL_COMMANDS
  if (Key == KEY_PROBE)       /* probing by command */
  {
    Display_LCD2Serial();               /* switch output to serial */
    Display_EEString_NL(Cmd_OK_str);    /* send: OK & newline */
    Display_Serial2LCD();               /* switch output back to LCD */

    /* We don't have to restore the next-line mode since it will be 
       changed a few line below anyway. */
  }
  #endif

  /* component was found */
  if (Check.Found >= COMP_RESISTOR)
  {
    MissedParts = 0;          /* reset counter */
  }


  /*
   *  manage cycling and power-off
   */

cycle_control:

  #ifdef HW_DISCHARGE_RELAY
  ADC_DDR = (1 << TP_REF);         /* short circuit probes */
  #endif

  #ifdef SERIAL_RW
  Serial_Ctrl(SER_RX_RESUME);      /* enable TTL serial RX */
  #endif

  UI.LineMode = LINE_STD;          /* reset next-line mode */

  /* get key press or timeout */
  Key = TestKey((uint16_t)CYCLE_DELAY, CURSOR_BLINK | UI_OP_MODE);

  if (Key == KEY_TIMEOUT)          /* timeout (no key press) */
  {
    /* implies continious mode */
    /* check if we reached the maximum number of missed parts in a row */
    if (MissedParts >= CYCLE_MAX)
    {
      Key = KEY_POWER_OFF;         /* signal power off */
    }
  }
  else if (Key == KEY_SHORT)       /* short key press */
  {
    /* a second key press triggers main menu */
    MilliSleep(50);
    Key = TestKey(300, CURSOR_NONE);

    if (Key > KEY_TIMEOUT)         /* any key press */
    {
      Key = KEY_MAINMENU;          /* signal mainmenu */
    }
  }
  else if (Key == KEY_LONG)        /* long key press */
  {
    Key = KEY_POWER_OFF;           /* signal power off */
  }
  #ifdef HW_KEYS
  else if (Key == KEY_LEFT)        /* rotary encoder: left turn */
  {
    Key = KEY_MAINMENU;            /* signal mainmenu */
  }
  #endif
  #ifdef SERIAL_RW
  else if (Key == KEY_COMMAND)     /* remote command */
  {
    #ifdef UI_SERIAL_COMMANDS
    Key = KEY_NONE;                /* reset key */
    Display_LCD2Serial();          /* switch output to serial */
    Test = GetCommand();           /* get command */
    if (Test != CMD_NONE)          /* valid command */
    {
      Key = RunCommand(Test);      /* run command */
    }
    Display_Serial2LCD();          /* switch output back to LCD */

    /* if we get a virtual key perform requested action */
    if (Key != KEY_NONE) goto cycle_action;
    #endif

    goto cycle_control;            /* re-run cycle control */
  }
  #endif


cycle_action:

  #ifdef SERIAL_RW
  Serial_Ctrl(SER_RX_PAUSE);       /* disable TTL serial RX */
  /* todo: when we got a locked buffer meanwhile? */
  #endif

  if (Key == KEY_MAINMENU)         /* run main menu */
  {
    #ifdef SAVE_POWER
    /* change sleep mode the Idle to keep timers & other stuff running */
    Test = Cfg.SleepMode;               /* get current mode */
    Cfg.SleepMode = SLEEP_MODE_IDLE;    /* change sleep mode to Idle */
    #endif

    #ifdef HW_DISCHARGE_RELAY
    ADC_DDR = 0;                   /* remove short circuit */
    /* todo: move this to MainMenu()? (after selecting item) */
    #endif

    MainMenu();                    /* enter main menu */

    #ifdef SAVE_POWER
    /* change sleep mode back */
    Cfg.SleepMode = Test;          /* change sleep mode back */
    #endif

    goto cycle_control;            /* re-run cycle control */
  }
  else if (Key == KEY_POWER_OFF)   /* power off */
  {
    /* display feedback (otherwise the user will wait :) */
    LCD_Clear();
    #ifdef LCD_COLOR
    UI.PenColor = COLOR_TITLE;               /* set pen color */
    #endif
    Display_EEString(Bye_str);

    cli();                                   /* disable interrupts */
    wdt_disable();                           /* disable watchdog */
    CONTROL_PORT &= ~(1 << POWER_CTRL);      /* power off myself */
  }
  else                             /* default action */
  {
    goto cycle_start;              /* next round */
  }

  return 0;
}



/* ************************************************************************
 *   clean-up of local constants
 * ************************************************************************ */


/* source management */
#undef MAIN_C



/* ************************************************************************
 *   EOF
 * ************************************************************************ */
