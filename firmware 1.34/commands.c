/* ************************************************************************
 *
 *   automation / remote commands via serial interface 
 *
 *   (c) 2018 by Markus Reschke
 *
 * ************************************************************************ */


/*
 *  hints:
 *  - display output has to be switched from LCD to TTL serial
 *    before running any of the functions below
 *  - don't forget to switch the output back to the LCD afterwards
 */


/* local includes */
#include "config.h"           /* global configuration */

#ifdef UI_SERIAL_COMMANDS


/*
 *  local constants
 */

/* source management */
#define COMMANDS_C

/* signaling */
#define SIGNAL_NONE      0         /* none */
#define SIGNAL_ERR       1         /* error / unsupported */
#define SIGNAL_NA        2         /* n/a (not available) */
#define SIGNAL_OK        3         /* ok / success */


/*
 *  include header files
 */

/* local includes */
#include "common.h"           /* common header file */
#include "variables.h"        /* global variables */
#include "functions.h"        /* external functions */


/*
 *  local variables
 */

/* control logic */
uint8_t             FirstFlag;     /* multiple strings in a line */



/* ************************************************************************
 *   support functions
 * ************************************************************************ */


/*
 *  control logic for sending multiple strings in a line 
 *  - add space before 2nd string and all other following strings
 */

void SpaceLogic(void)
{
  if (FirstFlag)         /* first string in line */
  {
    FirstFlag = 0;            /* reset flag */
  }
  else                   /* another string */
  {
    Display_Space();          /* send space */
  }
}



/*
 *  show FET/IGBT channel type
 */

void FET_Channel(void)
{
  SpaceLogic();                         /* space logic */

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

void FET_Mode(void)
{
  SpaceLogic();                         /* space logic */

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
 *  pinout for 3 pin semiconductors
 *
 *  required:
 *  - character for pin A
 *  - character for pin B
 *  - character for pin C
 */

void SemiPinout(uint8_t A, uint8_t B, uint8_t C)
{
  uint8_t           n;        /* counter */
  uint8_t           Char;     /* character */

  /* display pin IDs */
  for (n = 0; n <= 2; n++)         /* loop through probe pins */
  {
    if (n == Semi.A) Char = A;          /* probe A - ID A */
    else if (n == Semi.B) Char = B;     /* probe B - ID B */
    else Char = C;                      /*         - ID C */

    Display_Char(Char);            /* send ID */
  }
}



/*
 *  pinout for 2 pin component
 *
 *  required:
 *  - probe number for pin A
 *  - probe number for pin B
 *  - character for pin A
 *  - character for pin B
 */

void SimplePinout(uint8_t Probe_A, uint8_t Probe_B, uint8_t ID_A, uint8_t ID_B)
{
  uint8_t           n;        /* counter */
  uint8_t           Char;     /* character */

  /* display pin IDs */
  for (n = 0; n <= 2; n++)         /* loop through probe pins */
  {
    if (n == Probe_A) Char = ID_A;           /* probe A - ID A */
    else if (n == Probe_B) Char = ID_B;      /* probe B - ID B */
    else Char = '-';                         /* not connected */

    Display_Char(Char);            /* send ID */
  }
}



/*
 *  return pointer to selected component
 *
 *  returns:
 *  - pointer to selected component
 *  - NULL on error
 */

void *SelectedComp(void)
{
  void         *Comp = NULL;       /* return value */

  if (Info.Selected == 1)          /* #1 */
  {
    Comp = Info.Comp1;             /* pointer to first one */
  }
  else if (Info.Selected == 2)     /* #2 */
  {
    Comp = Info.Comp2;             /* pointer to second one */
  }

  return Comp;
}



/* ************************************************************************
 *   probing feedback
 * ************************************************************************ */


#if 0

/*
 *  command: X
 *  - return X
 *
 *  returns:
 *  - SIGNAL_ERR on error
 *  - SIGNAL_NA on n/a
 *  - SIGNAL_OK on success
 */

uint8_t Cmd_X(void)
{
  uint8_t           Flag = SIGNAL_OK;   /* return value */

  switch (Check.Found)        /* based on component */
  {
    case COMP_ERROR:          /* error */
      break;

    case COMP_DIODE:          /* diode(s) */
      break;

    case COMP_BJT:            /* BJT */
      break;

    case COMP_FET:            /* FET (JFET/MOSFET) */
      break;

    case COMP_IGBT:           /* IGBT */
      break;

    case COMP_THYRISTOR:      /* SCR */
      break;

    case COMP_TRIAC:          /* TRIAC */
      break;

    case COMP_PUT:            /* PUT */
      break;

    #ifdef SW_UJT
    case COMP_UJT:            /* UJT */
      break;
    #endif

    case COMP_RESISTOR:       /* resistor(s) */
      break;

    case COMP_CAPACITOR:      /* capacitor */
      break;

    default:                  /* unsupported */
      Flag = SIGNAL_ERR;      /* signal error */
      break;
  }

  return Flag;
}

#endif




/*
 *  command: MSG
 *  - return error message
 *
 *  returns:
 *  - SIGNAL_ERR on error
 *  - SIGNAL_NA on n/a
 *  - SIGNAL_OK on success
 */

uint8_t Cmd_MSG(void)
{
  uint8_t           Flag = SIGNAL_ERR;  /* return value */

  if (Check.Found == COMP_ERROR)        /* we got an error */
  {
    Flag = SIGNAL_NA;                   /* n/a by default */

    if (Check.Type == TYPE_DISCHARGE)   /* discharge failed */
    {
      Display_EEString_Space(DischargeFailed_str);   /* send: Battery? */
      Display_ProbeNumber(Check.Probe);              /* send probe ID */
      Display_Char(':');
      Display_Space();
      Display_Value(Check.U, -3, 'V');               /* send voltage */

      Flag = SIGNAL_OK;                 /* signal success */
    }
  }

  return Flag;
}



/*
 *  command: TYPE
 *  - return more specific type of component
 *
 *  returns:
 *  - SIGNAL_ERR on error
 *  - SIGNAL_NA on n/a
 *  - SIGNAL_OK on success
 */

uint8_t Cmd_TYPE(void)
{
  uint8_t           Flag = SIGNAL_OK;   /* return value */

  FirstFlag = 1;              /* reset multi string logic */

  switch (Check.Found)        /* based on component */
  {
    case COMP_BJT:            /* BJT */
      if (Check.Type & TYPE_NPN)        /* NPN */
      {
        Display_EEString(NPN_str);      /* send: NPN */
      }
      else                              /* PNP */
      {
        Display_EEString(PNP_str);      /* send: PNP */
      }
      break;

    case COMP_FET:            /* FET (JFET/MOSFET) */
      /* FET type */
      SpaceLogic();                     /* space logic */
      if (Check.Type & TYPE_MOSFET)     /* MOSFET */
      {
        Display_EEString(MOS_str);      /* display: MOS */
      }
      else                              /* JFET */
      {
        Display_Char('J');              /* display: J */
      }
      Display_EEString(FET_str);        /* display: FET */

      FET_Channel();                    /* channel type */
      if (Check.Type & TYPE_MOSFET)     /* MOSFET only */
      {
        FET_Mode();                     /* FET mode */
      }
      break;

    case COMP_IGBT:           /* IGBT */
      FET_Channel();                    /* channel type */
      FET_Mode();                       /* IGBT mode */
      break;

    default:                  /* unsupported */
      Flag = SIGNAL_ERR;      /* signal error */
      break;
  }

  return Flag;
}



/*
 *  command: HINT
 *  - return hints about special features
 *
 *  returns:
 *  - SIGNAL_ERR on error
 *  - SIGNAL_NA on n/a
 *  - SIGNAL_OK on success
 */

uint8_t Cmd_HINT(void)
{
  uint8_t           Flag = SIGNAL_OK;   /* return value */

  FirstFlag = 1;              /* reset multi string logic */

  switch (Check.Found)        /* based on component */
  {
    case COMP_DIODE:          /* diode(s) */
      if (Info.Flags & INFO_D_BJT_NPN)       /* possible NPN BJT */
      {
        SpaceLogic();                        /* space logic */
        Display_EEString(NPN_str);           /* send: NPN */
      }
      if (Info.Flags & INFO_D_BJT_PNP)       /* possible PNP BJT */
      {
        SpaceLogic();                        /* space logic */
        Display_EEString(PNP_str);           /* send: PNP */
      }
      if (Info.Flags & INFO_D_R_BE)          /* R_BE */
      {
        SpaceLogic();                        /* space logic */
        Display_EEString(Cmd_R_be_str);      /* send: R_BE */
      }
      break;

    case COMP_BJT:            /* BJT */
      if (Info.Flags & INFO_BJT_D_FB)        /* flyback diode */
      {
        SpaceLogic();                        /* space logic */
        Display_EEString(Cmd_D_fb_str);      /* send: D_FB */
      }
      if (Info.Flags & INFO_BJT_R_BE)        /* R_BE */
      {
        SpaceLogic();                        /* space logic */
        Display_EEString(Cmd_R_be_str);      /* send: R_BE */
      }
      if (Check.Type & TYPE_PARASITIC)       /* parasitic BJT */
      {
        SpaceLogic();                        /* space logic */
        Display_EEString(Cmd_BJT_str);       /* send: BJT */
        Display_Char('+');                   /* send: + */
      }
      break;

    case COMP_FET:            /* FET (JFET/MOSFET) */
      if (Info.Flags & INFO_FET_D_FB)        /* flyback diode */
      {
        SpaceLogic();                        /* space logic */
        Display_EEString(Cmd_D_fb_str);      /* send: D_FB */
      }
      if (Check.Type & TYPE_SYMMETRICAL)     /* symmetrical Drain and Source */
      {
        SpaceLogic();                        /* space logic */
        Display_EEString(Cmd_SYM_str);       /* send: SYM */
      }
      break;

    case COMP_IGBT:           /* IGBT */
      if (Info.Flags & INFO_FET_D_FB)        /* flyback diode */
      {
        SpaceLogic();                        /* space logic */
        Display_EEString(Cmd_D_fb_str);      /* send: D_FB */
      }
      break;

    default:                  /* unsupported */
      Flag = SIGNAL_ERR;      /* signal error */
      break;
  }

  if ((Flag == SIGNAL_OK) && FirstFlag)      /* no features available */
  {
    Flag = SIGNAL_NA;      /* signal N/A */
  }

  return Flag;
}



/*
 *  command: PIN
 *  - return pinout
 *
 *  returns:
 *  - SIGNAL_ERR on error
 *  - SIGNAL_NA on n/a
 *  - SIGNAL_OK on success
 */

uint8_t Cmd_PIN(void)
{
  uint8_t           Flag = SIGNAL_OK;   /* return value */
  Diode_Type        *D;                 /* pointer to diode */
  Resistor_Type     *R;                 /* pointer to resistor */
  Capacitor_Type    *C;                 /* pointer to capacitor */

  switch (Check.Found)        /* based on component */
  {
    case COMP_DIODE:          /* diode(s) */
      D = (Diode_Type *)SelectedComp();      /* get pointer */
      if (D)                                 /* valid pointer */
      {
        SimplePinout(D->A, D->C, 'A', 'C');  /* send pinout */
      }
      break;

    case COMP_BJT:            /* BJT */
      SemiPinout('B', 'C', 'E');             /* send pinout */
      break;

    case COMP_FET:            /* FET (JFET/MOSFET) */
      if (Check.Type & TYPE_SYMMETRICAL)     /* symmetrical Drain and Source */
      {
        /* we can't distinguish D and S */
        SemiPinout('G', 'x', 'x');           /* send pinout */
      }
      else                                   /* unsymmetrical Drain and Source */
      {
        SemiPinout('G', 'D', 'S');           /* send pinout */
      }
      break;

    case COMP_IGBT:           /* IGBT */
      SemiPinout('G', 'C', 'E');             /* send pinout */
      break;

    case COMP_THYRISTOR:      /* SCR */
      SemiPinout('G', 'A', 'C');             /* send pinout */
      break;

    case COMP_TRIAC:          /* TRIAC */
      SemiPinout('G', '2', '1');             /* send pinout */
      break;

    case COMP_PUT:            /* PUT */
      SemiPinout('G', 'A', 'C');             /* send pinout */
      break;

    #ifdef SW_UJT
    case COMP_UJT:            /* UJT */
      SemiPinout('E', '2', '1');             /* send pinout */
      break;
    #endif

    case COMP_RESISTOR:       /* resistor(s) */
      R = (Resistor_Type *)SelectedComp();   /* get pointer */
      if (R)                                 /* valid pointer */
      {
        SimplePinout(R->A, R->B, 'x', 'x');  /* send pinout */
      }
      break;

    case COMP_CAPACITOR:      /* capacitor */
      C = (Capacitor_Type *)Info.Comp1;      /* get pointer */
      if (C)                                 /* valid pointer */
      {
        SimplePinout(C->A, C->B, 'x', 'x');  /* send pinout */
      }
      break;

    default:                  /* unsupported */
      Flag = SIGNAL_ERR;      /* signal error */
      break;
  }

  return Flag;
}



/*
 *  command: R
 *  - return resistance value
 *
 *  returns:
 *  - SIGNAL_ERR on error
 *  - SIGNAL_NA on n/a
 *  - SIGNAL_OK on success
 */

uint8_t Cmd_R(void)
{
  uint8_t           Flag = SIGNAL_OK;   /* return value */
  Resistor_Type     *R;                 /* pointer to resistor */

  if (Check.Found == COMP_RESISTOR)     /* resistor(s) */
  {
    R = (Resistor_Type *)SelectedComp();     /* get pointer */

    if (R)                                   /* valid pointer */
    {
      /* send value */
      Display_Value(R->Value, R->Scale, LCD_CHAR_OMEGA);
    }
  }
  else                                  /* other component */
  {
    Flag = SIGNAL_ERR;                      /* signal error */
  }

  return Flag;
}



/*
 *  command: C
 *  - return capacitance value
 *
 *  returns:
 *  - SIGNAL_ERR on error
 *  - SIGNAL_NA on n/a
 *  - SIGNAL_OK on success
 */

uint8_t Cmd_C(void)
{
  uint8_t           Flag = SIGNAL_OK;   /* return value */
  Capacitor_Type    *C;                 /* pointer to resistor */

  if (Check.Found == COMP_CAPACITOR)    /* capacitor */
  {  
    C = (Capacitor_Type *)Info.Comp1;        /* get pointer */

    if (C)                                   /* valid pointer */
    {
      /* send value */
      Display_Value(C->Value, C->Scale, 'F');
    }
  }
  else                                  /* other component */
  {
    Flag = SIGNAL_ERR;                      /* signal error */
  }

  return Flag;
}



#ifdef SW_INDUCTOR

/*
 *  command: L
 *  - return inductance value
 *
 *  returns:
 *  - SIGNAL_ERR on error
 *  - SIGNAL_NA on n/a
 *  - SIGNAL_OK on success
 */

uint8_t Cmd_L(void)
{
  uint8_t           Flag = SIGNAL_OK;   /* return value */

  if (Check.Found == COMP_RESISTOR)     /* resistor(s) */
  {
    /* just for a single resistor */
    if (Info.Flags & INFO_R_L)          /* inductance measured */
    {
      /* send value */
      Display_Value(Inductor.Value, Inductor.Scale, 'H');
    }
    else
    {
      Flag = SIGNAL_NA;                 /* signal n/a */
    }
  }
  else                                  /* other component */
  {
    Flag = SIGNAL_ERR;                  /* signal error */
  }

  return Flag;
}

#endif



#if defined (SW_ESR) || defined (SW_OLD_ESR)

/*
 *  command: ESR
 *  - return ESR value
 *
 *  returns:
 *  - SIGNAL_ERR on error
 *  - SIGNAL_NA on n/a
 *  - SIGNAL_OK on success
 */

uint8_t Cmd_ESR(void)
{
  uint8_t           Flag = SIGNAL_OK;   /* return value */

  if (Check.Found == COMP_CAPACITOR)    /* capacitor */
  {
    if (Info.Val1 < UINT16_MAX)         /* valid ESR */
    {
      /* send value */
      Display_Value(Info.Val1, -2, LCD_CHAR_OMEGA);    /* in 0.01 Ohms */
    }
    else
    {
      Flag = SIGNAL_NA;                 /* signal n/a */
    }
  }
  else                                  /* other component */
  {
    Flag = SIGNAL_ERR;                  /* signal error */
  }

  return Flag;
}

#endif



/*
 *  command: I_L
 *  - return I_leak value
 *
 *  returns:
 *  - SIGNAL_ERR on error
 *  - SIGNAL_NA on n/a
 *  - SIGNAL_OK on success
 */

uint8_t Cmd_I_L(void)
{
  uint8_t           Flag = SIGNAL_OK;   /* return value */
  Capacitor_Type    *C;                 /* pointer to cap */

  if (Check.Found == COMP_CAPACITOR)    /* capacitor */
  {
    C = (Capacitor_Type *)Info.Comp1;   /* get pointer */

    if (C)                              /* valid pointer */
    {
      if (C->I_leak > 0)                /* got I_leak */
      {
        /* send value */
        Display_Value(C->I_leak, -8, 'A');   /* in 10nA */
      }
      else
      {
        Flag = SIGNAL_NA;                 /* signal n/a */
      }
    }
  }
  else                                  /* other component */
  {
    Flag = SIGNAL_ERR;                  /* signal error */
  }

  return Flag;
}



/*
 *  command: V_F
 *  - return V_f value
 *
 *  returns:
 *  - SIGNAL_ERR on error
 *  - SIGNAL_NA on n/a
 *  - SIGNAL_OK on success
 */

uint8_t Cmd_V_F(void)
{
  uint8_t           Flag = SIGNAL_OK;   /* return value */
  Diode_Type        *D = NULL;          /* pointer to diode */

  switch (Check.Found)        /* based on component */
  {
    case COMP_DIODE:          /* diode(s) */
      D = (Diode_Type *)SelectedComp();      /* get pointer */
      break;

    case COMP_BJT:            /* BJT */
      if (Info.Flags & INFO_BJT_D_FB)   /* found diode */
      {
        D = (Diode_Type *)Info.Comp1;        /* get pointer */
      }
      break;

    case COMP_FET:            /* FET (JFET/MOSFET) */
    case COMP_IGBT:           /* IGBT */
      if (Info.Flags & INFO_FET_D_FB)   /* found diode */
      {
        D = (Diode_Type *)Info.Comp1;        /* get pointer */
      }
      break;

    case COMP_PUT:            /* PUT */
      Display_Value(AltSemi.U_1, -3, 'V');   /* send Vf */
      break;

    default:                  /* unsupported */
      Flag = SIGNAL_ERR;      /* signal error */
      break;
  }

  if (D)                                /* valid pointer */
  {
    /* send Vf */
    Display_Value(D->V_f, -3, 'V');     /* in mV */
  }

  return Flag;
}



/*
 *  command: V_F2
 *  - return V_f value of low current measurement
 *
 *  returns:
 *  - SIGNAL_ERR on error
 *  - SIGNAL_NA on n/a
 *  - SIGNAL_OK on success
 */

uint8_t Cmd_V_F2(void)
{
  uint8_t           Flag = SIGNAL_OK;   /* return value */
  Diode_Type        *D;                 /* pointer to diode */

  if (Check.Found == COMP_DIODE)        /* diode(s) */
  {
    D = (Diode_Type *)SelectedComp();   /* get pointer */

    if (D)                              /* valid pointer */
    {
      /* send value */
      Display_Value(D->V_f2, -3, 'V');  /* in mV */
    }
  }
  else                                  /* other component */
  {
    Flag = SIGNAL_ERR;                  /* signal error */
  }

  return Flag;
}



/*
 *  command: C_D
 *  - return C_D value (capacitance of diode)
 *
 *  returns:
 *  - SIGNAL_ERR on error
 *  - SIGNAL_NA on n/a
 *  - SIGNAL_OK on success
 */

uint8_t Cmd_C_D(void)
{
  uint8_t           Flag = SIGNAL_OK;   /* return value */
  uint8_t           n;

  if (Check.Found == COMP_DIODE)        /* diode(s) */
  {
    /* capacitance values are stored in Caps[0] and Caps[1] */
    if (Info.Selected <= 2)
    {
      n = Info.Selected - 1;            /* align with array */

      /* send value */
      Display_Value(Caps[n].Value, Caps[n].Scale, 'F');
    }
  }
  else                                  /* other component */
  {
    Flag = SIGNAL_ERR;                  /* signal error */
  }
  return Flag;
}



/*
 *  command: I_R / I_CEO / I_DDS
 *  - return
 *    I_R value (diode: reverse leakage current)
 *    I_CEO value (BJT: collector-emitter current, open base)
 *    I_DSS value (dep. mode FET: drain-source current, shorted gate)
 *
 *  requires:
 *  - Cmd: command ID
 *
 *  returns:
 *  - SIGNAL_ERR on error
 *  - SIGNAL_NA on n/a
 *  - SIGNAL_OK on success
 */

uint8_t Cmd_I_R(uint8_t Cmd)
{
  uint8_t           Flag = SIGNAL_NA;   /* return value */

  switch (Check.Found)        /* based on component */
  {
    case COMP_DIODE:          /* diode(s) */
      if (Cmd == CMD_I_R)               /* I_R */
      {
        /* just for a single diode */
        if (Info.Flags & INFO_D_I_R)    /* I_R measured */
        {
          Flag = SIGNAL_OK;             /* signal ok */
        }
      }
      else                              /* wrong command */
      {
        Flag = SIGNAL_ERR;              /* signal err */
      }
      break;

    case COMP_BJT:            /* BJT */
      if (Cmd == CMD_I_CEO)             /* I_CEO */
      {
        Flag = SIGNAL_OK;               /* signal ok */
      }
      else                              /* wrong command */
      {
        Flag = SIGNAL_ERR;              /* signal err */
      }
      break;

    case COMP_FET:            /* FET */
      if (Cmd == CMD_I_DSS)             /* I_DSS */
      {
        if (Check.Type & TYPE_DEPLETION)     /* dep. mode */
        {
          Flag = SIGNAL_OK;                  /* signal ok */
        }
      }
      else                              /* wrong command */
      {
        Flag = SIGNAL_ERR;              /* signal err */
      }
      break;

    default:                  /* unsupported */
      Flag = SIGNAL_ERR;      /* signal error */
      break;
  }

  if (Flag == SIGNAL_OK)      /* got I */
  {
    /* send value */
    Display_Value(Semi.I_value, Semi.I_scale, 'A');
  }

  return Flag;
}



/*
 *  command: R_BE / R_BB
 *  - return
 *    R_BE value (BJT: base-emitter resistor)
 *    R_BB value (UJT: interbase resistance)
 *
 *  requires:
 *  - Cmd: command ID
 *
 *  returns:
 *  - SIGNAL_ERR on error
 *  - SIGNAL_NA on n/a
 *  - SIGNAL_OK on success
 */

uint8_t Cmd_R_BE(uint8_t Cmd)
{
  uint8_t           Flag = SIGNAL_NA;   /* return value */

  switch (Check.Found)        /* based on component */
  {
    case COMP_DIODE:          /* diode(s) */
      if (Cmd == CMD_R_BE)              /* R_BE */
      {
        if (Info.Flags & INFO_D_R_BE)   /* detected R_BE */
        {
          Flag = SIGNAL_OK;             /* signal ok */
        }
      }
      else                              /* wrong command */
      {
        Flag = SIGNAL_ERR;              /* signal err */
      }
      break;

    case COMP_BJT:            /* BJT */
      if (Cmd == CMD_R_BE)              /* R_BE */
      {
        if (Info.Flags & INFO_BJT_R_BE)      /* detected R_BE */
        {
          Flag = SIGNAL_OK;                  /* signal ok */
        }
      }
      else                              /* wrong command */
      {
        Flag = SIGNAL_ERR;              /* signal err */
      }
      break;

    #ifdef SW_UJT    
    case COMP_UJT:            /* UJT */
      if (Cmd == CMD_R_BB)              /* R_BB */
      {
        Flag = SIGNAL_OK;               /* signal ok */
      }
      else                              /* wrong command */
      {
        Flag = SIGNAL_ERR;              /* signal err */
      }
      break;
    #endif

    default:                  /* unsupported */
      Flag = SIGNAL_ERR;      /* signal error */
      break;
  }

  if (Flag == SIGNAL_OK)      /* got R_BE/R_BB */
  {
    /* R_BE is stored in Resistors[0] */
    /* R_BB is also stored in Resistors[0] */
    /* send value */
    Display_Value(Resistors[0].Value, Resistors[0].Scale, LCD_CHAR_OMEGA);
  }

  return Flag;
}



/*
 *  command: H_FE
 *  - return h_FE value
 *
 *  returns:
 *  - SIGNAL_ERR on error
 *  - SIGNAL_NA on n/a
 *  - SIGNAL_OK on success
 */

uint8_t Cmd_H_FE(void)
{
  uint8_t           Flag = SIGNAL_OK;   /* return value */

  if (Check.Found == COMP_BJT)          /* BJT */
  {
    if (! (Info.Flags & INFO_BJT_R_BE))      /* no R_BE detected */
    {
      /* send value */
      Display_Value(Semi.F_1, 0, 0);
    }
    else                                     /* R_BE detected */
    {
      /* can't measure h_FE with R_BE */
      Flag = SIGNAL_NA;                 /* signal n/a */
    }
  }
  else                                  /* other component */
  {
    Flag = SIGNAL_ERR;                  /* signal error */
  }

  return Flag;
}



/*
 *  command: V_BE
 *  - return V_BE value
 *
 *  returns:
 *  - SIGNAL_ERR on error
 *  - SIGNAL_NA on n/a
 *  - SIGNAL_OK on success
 */

uint8_t Cmd_V_BE(void)
{
  uint8_t           Flag = SIGNAL_OK;   /* return value */

  if (Check.Found == COMP_BJT)          /* BJT */
  {
    if (! (Info.Flags & INFO_BJT_R_BE))      /* no R_BE detected */
    {
      /* send value */
      Display_Value(Info.Val1, -3, 'V');     /* in mV */
    }
    else                                     /* R_BE detected */
    {
      /* can't measure V_BE with R_BE */
      Flag = SIGNAL_NA;                 /* signal n/a */
    }
  }
  else                                  /* other component */
  {
    Flag = SIGNAL_ERR;                  /* signal error */
  }

  return Flag;
}



/*
 *  command: V_TH
 *  - return V_th value
 *
 *  returns:
 *  - SIGNAL_ERR on error
 *  - SIGNAL_NA on n/a
 *  - SIGNAL_OK on success
 */

uint8_t Cmd_V_TH(void)
{
  uint8_t           Flag = SIGNAL_NA;   /* return value */

  /* enh. mode MOSFET or IGBT */
  if ((Check.Found == COMP_FET) || (Check.Found == COMP_IGBT))
  {
    if (Info.Flags & INFO_FET_V_TH)     /* measured Vth */
    {
      /* send value */
      Display_SignedValue(Semi.U_2, -3, 'V');     /* in mV */

      Flag = SIGNAL_OK;                 /* signal ok */
    }
  }
  else                                  /* other component */
  {
    Flag = SIGNAL_ERR;                  /* signal error */
  }


  return Flag;
}



/*
 *  command: C_GS / C_GE
 *  - return
 *    C_GS value (enh. mode FET: gate-source capacitance)
 *    C_GE value (enh. mode IGBT: gate-emitter capacitance)
 *
 *  requires:
 *  - Cmd: command ID
 *
 *  returns:
 *  - SIGNAL_ERR on error
 *  - SIGNAL_NA on n/a
 *  - SIGNAL_OK on success
 */

uint8_t Cmd_C_GS(uint8_t Cmd)
{
  uint8_t           Flag = SIGNAL_NA;   /* return value */

  switch (Check.Found)        /* based on component */
  {
    case COMP_FET:            /* FET */
      if (Cmd == CMD_C_GS)              /* C_GS */
      {
        if (Info.Flags & INFO_FET_C_GS)      /* measured C_GS */
        {
          Flag = SIGNAL_OK;                  /* signal ok */
        }
      }
      else                              /* wrong command */
      {
        Flag = SIGNAL_ERR;              /* signal err */
      }
      break;

    case COMP_IGBT:           /* IGBT */
      if (Cmd == CMD_C_GE)              /* C_GE */
      {
        if (Info.Flags & INFO_FET_C_GS)      /* measured C_GE */
        {
          Flag = SIGNAL_OK;                  /* signal ok */
        }
      }
      else                              /* wrong command */
      {
        Flag = SIGNAL_ERR;              /* signal err */
      }
      break;

    default:                  /* unsupported */
      Flag = SIGNAL_ERR;      /* signal error */
      break;
  }

  if (Flag == SIGNAL_OK)      /* got C_GS/C_GE */
  {
    /* send value */
    Display_Value(Semi.C_value, Semi.C_scale, 'F');
  }

  return Flag;
}



/*
 *  command: R_DS
 *  - return R_DS_on value
 *
 *  returns:
 *  - SIGNAL_ERR on error
 *  - SIGNAL_NA on n/a
 *  - SIGNAL_OK on success
 */

uint8_t Cmd_R_DS(void)
{
  uint8_t           Flag = SIGNAL_NA;   /* return value */

  /* enh. mode MOSFET */
  if (Check.Found == COMP_FET)          /* FET */
  {
    if (Info.Flags & INFO_FET_R_DS)     /* measured R_DS_on */
    {
      /* send value */
      Display_Value(Semi.U_1, -2, LCD_CHAR_OMEGA);     /* in 0.01 Ohms */

      Flag = SIGNAL_OK;                 /* signal ok */
    }
  }
  else                                  /* other component */
  {
    Flag = SIGNAL_ERR;                  /* signal error */
  }

  return Flag;
}



/*
 *  command: V_GT
 *  - return V_GT value
 *
 *  returns:
 *  - SIGNAL_ERR on error
 *  - SIGNAL_NA on n/a
 *  - SIGNAL_OK on success
 */

uint8_t Cmd_V_GT(void)
{
  uint8_t           Flag = SIGNAL_NA;   /* return value */

  /* SCR or TRIAC */
  if ((Check.Found == COMP_THYRISTOR) || (Check.Found == COMP_TRIAC))
  {
    if (Semi.U_1 > 0)                   /* if not zero */
    {
      /* send value */
      Display_Value(Semi.U_1, -3, 'V');      /* in mV */

      Flag = SIGNAL_OK;                 /* signal ok */
    }
  }
  else                                  /* other component */
  {
    Flag = SIGNAL_ERR;                  /* signal error */
  }

  return Flag;
}



/*
 *  command: V_T
 *  - return V_T value
 *
 *  returns:
 *  - SIGNAL_ERR on error
 *  - SIGNAL_NA on n/a
 *  - SIGNAL_OK on success
 */

uint8_t Cmd_V_T(void)
{
  uint8_t           Flag = SIGNAL_OK;   /* return value */

  if (Check.Found == COMP_PUT)          /* PUT */
  {
    /* send value */
    Display_Value(AltSemi.U_2, -3, 'V');     /* in mV */
  }
  else                                  /* other component */
  {
    Flag = SIGNAL_ERR;                  /* signal error */
  }

  return Flag;
}



/* ************************************************************************
 *   command parsing and processing
 * ************************************************************************ */


/*
 *  check RX buffer for known command
 *
 *  returns:
 *  - command ID
 */

uint8_t FindCommand(void)
{
  uint8_t           ID = CMD_NONE;      /* command ID */
  uint8_t           Flag = 1;           /* loop control flag */
  uint8_t           n;                  /* counter */
  uint8_t           CmdID;              /* command ID */
  char              CmdChar;            /* single character */
  unsigned char     *CmdAddr;           /* address of command string */
  Cmd_Type          *Data;              /* address of table entry */
  uint8_t           *Addr;              /* address pointer */

  /*
   *  Compare command in RX buffer with command strings referenced by
   *  a stored table.
   */

  Data = (Cmd_Type *)&Cmd_Table;   /* start address of table */

  while (Flag)                /* loop through table entries */
  {
    /* read entry from reference table */
    Addr = (uint8_t *)Data;             /* start of current entry */
    CmdID = eeprom_read_byte(Addr);     /* read command ID */
    Addr++;                             /* for next data field */
    /* read string address */
    CmdAddr = (unsigned char *)eeprom_read_word((uint16_t *)Addr);

    if (CmdID)                /* valid command ID */
    {
      /* read and compare characterwise */
      n = 0;
      while (n < RX_BUFFER_SIZE)        /* loop through string */
      {
        CmdChar = eeprom_read_byte(CmdAddr);      /* read char */

        if (CmdChar == RX_Buffer[n])    /* match */
        {
          if (CmdChar == 0)             /* end of string */
          {
            n = RX_BUFFER_SIZE;         /* end string loop */
            Flag = 0;                   /* end table loop */
            ID = CmdID;                 /* found command */
          }

          n++;                          /* next char */
          CmdAddr++;                    /* next char */
        }
        else                            /* mismatch */
        {
          n = RX_BUFFER_SIZE;           /* end string loop */
        }
      }
    }
    else                      /* reached end of table */
    {
      Flag = 0;                         /* end table loop */
    }

    Data++;                             /* next entry */
  }

  return ID;
}



/*
 *  get command received via serial interface
 *
 *  returns:
 *  - command ID
 */

uint8_t GetCommand(void)
{
  uint8_t           ID = CMD_NONE;      /* command ID */

  /* check for locked RX buffer */
  if (Cfg.OP_Control & OP_RX_LOCKED)    /* buffer locked */
  {
    /* check if we don't have a overflow */
    if (! (Cfg.OP_Control & OP_RX_OVERFLOW))   /* no buffer overflow */
    {
      /* check for command */
      ID = FindCommand();               /* get command */
    }
    /* else: overflow triggers output of error */

    if (ID == CMD_NONE)                 /* no command found */
    {
      Display_EEString_NL(Cmd_ERR_str);      /* send: ERR & newline */
    }

    /* unlock buffer and clear overflow flag */
    RX_Pos = 0;                         /* reset position */
    Cfg.OP_Control &= (~OP_RX_LOCKED & ~OP_RX_OVERFLOW);
  }

  return ID;
}



/*
 *  run command received via serial interface
 *
 *  requires:
 *  - ID: command ID
 *
 *  returns:
 *  - virtual key
 */

uint8_t RunCommand(uint8_t ID)
{
  uint8_t           Key = KEY_NONE;     /* virtual key */
  uint8_t           Flag = SIGNAL_OK;   /* result of command function */

  /*
   *  run command
   */

  switch (ID)
  {
    case CMD_VER:             /* return firmware version */
      Display_EEString(Version_str);         /* send firmware version & newline */
      break;

    case CMD_PROBE:           /* probe component */
      Key = KEY_PROBE;                       /* set virtual key */
      /* OK is returned after probing by main() */
      Flag = SIGNAL_NONE;                    /* no newline */ 
      break;

    case CMD_OFF:             /* power off */
      Key = KEY_POWER_OFF;                   /* set virtual key */
      Display_EEString(Cmd_OK_str);          /* send: OK */
      break;

    case CMD_COMP:            /* return component type ID */
      Display_Value(Check.Found, 0, 0);      /* send component type ID */
      break;

    case CMD_MSG:             /* return error message */
      Flag = Cmd_MSG();                      /* run command */
      break;

    case CMD_QTY:             /* return component quantity */
      Display_Value(Info.Quantity, 0, 0);    /* send quantity */
      break;

    case CMD_NEXT:            /* select next component */
      /* allow only 2nd component */
      if ((Info.Selected == 1) && (Info.Quantity == 2))
      {
        Info.Selected = 2;                   /* 2nd one */
        Display_EEString(Cmd_OK_str);        /* send: OK */
      }
      else
      {
        Flag = SIGNAL_NA;                    /* signal n/a */
      }
      break;

    case CMD_TYPE:            /* return more specific type */
      Flag = Cmd_TYPE();                     /* run command */
      break;

    case CMD_HINT:            /* return hints about special features */
      Flag = Cmd_HINT();                     /* run command */
      break;

    case CMD_PIN:             /* return pinout */
      Flag = Cmd_PIN();                      /* run command */
      break;

    case CMD_R:               /* return resistance */
      Flag = Cmd_R();                        /* run command */
      break;

    case CMD_C:               /* return capcitance */
      Flag = Cmd_C();                        /* run command */
      break;

    #ifdef SW_INDUCTOR
    case CMD_L:               /* return inductance */
      Flag = Cmd_L();                        /* run command */
      break;
    #endif

    #if defined (SW_ESR) || defined (SW_OLD_ESR)
    case CMD_ESR:             /* return ESR */
      Flag = Cmd_ESR();                      /* run command */
      break;
    #endif

    case CMD_I_L:             /* return I_leak */
      Flag = Cmd_I_L();                      /* run command */
      break;

    case CMD_V_F:             /* return V_f */
      Flag = Cmd_V_F();                      /* run command */
      break;

    case CMD_V_F2:            /* return V_f for low current measurement */
      Flag = Cmd_V_F2();                     /* run command */
      break;

    case CMD_C_D:             /* return C_D */
      Flag = Cmd_C_D();                      /* run command */
      break;

    case CMD_I_R:             /* return I_R */
    case CMD_I_CEO:           /* return I_CEO */
    case CMD_I_DSS:           /* return I_DSS */
      Flag = Cmd_I_R(ID);                    /* run command */
      break;

    case CMD_R_BE:            /* return R_BE */
    #ifdef SW_UJT
    case CMD_R_BB:            /* return R_BB */
    #endif
      Flag = Cmd_R_BE(ID);                   /* run command */
      break;

    case CMD_H_FE:            /* return hFE */
      Flag = Cmd_H_FE();                     /* run command */
      break;

    case CMD_V_BE:            /* return V_BE */
      Flag = Cmd_V_BE();                     /* run command */
      break;

    case CMD_V_TH:            /* return V_th */
      Flag = Cmd_V_TH();                     /* run command */
      break;

    case CMD_C_GS:            /* return C_GS */
    case CMD_C_GE:            /* return C_GE */
      Flag = Cmd_C_GS(ID);                   /* run command */
      break;

    case CMD_R_DS:            /* return R_DS */
      Flag = Cmd_R_DS();                     /* run command */
      break;

    case CMD_V_GT:            /* return V_GT */
      Flag = Cmd_V_GT();                     /* run command */
      break;

    case CMD_V_T:             /* return V_T */
      Flag = Cmd_V_T();                      /* run command */
      break;

    default:                  /* unknown/unsupported */
      Flag = SIGNAL_ERR;                     /* signal error */
      break;
  }


  /*
   *  error handling
   */

  if (Flag == SIGNAL_ERR)          /* error */
  {
    Display_EEString(Cmd_ERR_str);      /* send: ERR */
  }
  else if (Flag == SIGNAL_NA)      /* n/a */
  {
    Display_EEString(Cmd_NA_str);       /* send: N/A */
  }

  /* end line by sending newline */
  if (Flag != SIGNAL_NONE)         /* valid response */
  {
    Serial_NewLine();                   /* send newline */
  }

  return Key;
}



/* ************************************************************************
 *   clean-up of local constants
 * ************************************************************************ */


/* signaling */
#undef SIGNAL_ERR
#undef SIGNAL_NA
#undef SIGNAL_OK

/* source management */
#undef COMMANDS_C

#endif

/* ************************************************************************
 *   EOF
 * ************************************************************************ */
