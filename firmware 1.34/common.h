/* ************************************************************************
 *
 *   common header file
 *
 *   (c) 2012-2018 by Markus Reschke
 *   based on code from Markus Frejek and Karl-Heinz Kübbeler
 *
 * ************************************************************************ */


/*
 *  include header files
 */

/* basic includes */
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <stdint.h>
#include <math.h>

/* AVR */
#include <avr/io.h>
#include <util/delay.h>
#include <avr/sleep.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>


/* source management */
#define COMMON_H



/* ************************************************************************
 *   constants for operation and UI
 * ************************************************************************ */


/* UI feedback mode (bit mask) */
#define CURSOR_NONE           0b00000000     /* no cursor */
#define CURSOR_STEADY         0b00000001     /* steady cursor */
#define CURSOR_BLINK          0b00000010     /* blinking cursor */
#define UI_OP_MODE            0b00000100     /* consider operation mode */


/* keys (test push button etc.) */
#define KEY_NONE              0    /* no key or error */
#define KEY_TIMEOUT           0    /* timeout */
#define KEY_SHORT             1    /* test push button: short key press */
#define KEY_LONG              2    /* test push button: long key press */
#define KEY_RIGHT             3    /* rotary encoder: right turn */
                                   /* push buttons: increase */
#define KEY_LEFT              4    /* rotary encoder: left turn */
                                   /* push buttons: decrease */
#define KEY_INCDEC            5    /* push buttons: increase and decrease */

/* virtual keys */
#define KEY_COMMAND           100  /* remote command (from serial interface) */
#define KEY_MAINMENU          101  /* main menu */
#define KEY_POWER_ON          102  /* just powered up */
#define KEY_POWER_OFF         103  /* power off */
#define KEY_PROBE             104  /* probe component */


/* operation mode/state flags (bitmask) */
#define OP_NONE               0b00000000     /* no flags */
#define OP_AUTOHOLD           0b00000001     /* auto hold mode (instead of continous) */
#define OP_EXT_REF            0b00000100     /* external voltage reference used */
#define OP_SPI                0b00001000     /* SPI is set up */
#define OP_I2C                0b00010000     /* I2C is set up */

/* operation control/signaling flags (bitmask) */
#define OP_BREAK_KEY          0b00000001     /* exit key processing */
#define OP_OUT_LCD            0b00000010     /* output to LCD display */
#define OP_OUT_SER            0b00000100     /* output to TTL serial */
#define OP_RX_LOCKED          0b00001000     /* RX buffer locked */
#define OP_RX_OVERFLOW        0b00010000     /* RX buffer overflow */


/* UI line modes (bitmask) */
#define LINE_STD              0b00000000     /* standard mode */
#define LINE_KEY              0b00000001     /* wait for key press */
#define LINE_KEEP             0b00000010     /* keep first line */


/* storage modes */
#define STORAGE_LOAD          1    /* load adjustment values */
#define STORAGE_SAVE          2    /* save adjustment values */


/* SPI */
/* clock rate bitmask */
#define SPI_CLOCK_R0          0b00000001     /* divider bit 0 (SPR0) */
#define SPI_CLOCK_R1          0b00000010     /* divider bit 1 (SPR1) */
#define SPI_CLOCK_2X          0b00000100     /* double clock rate (SPI2X) */


/* I2C */
#define I2C_ERROR             0              /* bus error */
#define I2C_OK                1              /* operation done */
#define I2C_START             1              /* start condition */
#define I2C_REPEATED_START    2              /* repeated start condition */
#define I2C_DATA              1              /* data byte */
#define I2C_ADDRESS           2              /* address byte */
#define I2C_ACK               1              /* acknowledge */
#define I2C_NACK              2              /* not-acknowledge */


/* TTL serial */
/* control */
#define SER_RX_PAUSE          1              /* pause RX */
#define SER_RX_RESUME         2              /* resume RX */

/* special characters */
#define CHAR_XON              17             /* software flow control: XON */
#define CHAR_XOFF             19             /* software flow control: XOFF */



/* ************************************************************************
 *   constants for arrays in variables.h
 * ************************************************************************ */


/* buffer sizes */
#define OUT_BUFFER_SIZE      12    /* 11 chars + terminating 0 */
#define RX_BUFFER_SIZE       11    /* 10 chars + terminating 0 */

/* number of entries in data tables */
#define NUM_PREFIXES          7    /* unit prefixes */
#define NUM_LARGE_CAP         46   /* large cap factors */
#define NUM_SMALL_CAP         9    /* small cap factors */
#define NUM_PWM_FREQ          8    /* PWM frequencies */
#define NUM_INDUCTOR          32   /* inductance factors */
#define NUM_TIMER1            5    /* Timer1 prescalers and bits */



/* ************************************************************************
 *   constants for remote commands
 * ************************************************************************ */


/*
 *  command IDs
 */

/* basic commands */
#define CMD_NONE              0    /* no command */
#define CMD_VER               1    /* print firmware version */
#define CMD_OFF               2    /* power off */

/* probing commands */
#define CMD_PROBE             10    /* probe component */
#define CMD_COMP              11    /* return component type ID */
#define CMD_MSG               12    /* return error message */
#define CMD_QTY               13    /* return component quantity */
#define CMD_NEXT              14    /* select next component */
#define CMD_TYPE              15    /* return more sepcific type */
#define CMD_HINT              16    /* return hint on special features */
#define CMD_PIN               17    /* return pinout */
#define CMD_R                 18    /* return resistance */
#define CMD_C                 19    /* return capacitance */
#define CMD_L                 20    /* return inductance */
#define CMD_ESR               21    /* return ESR */
#define CMD_I_L               22    /* return I_leak */
#define CMD_V_F               23    /* return V_f */
#define CMD_V_F2              24    /* return V_f of low current measurement */
#define CMD_C_D               25    /* return C_D */
#define CMD_I_R               26    /* return I_R */
#define CMD_R_BE              27    /* return R_BE */
#define CMD_H_FE              28    /* return hFE */
#define CMD_V_BE              29    /* return V_BE */
#define CMD_I_CEO             30    /* return I_CEO */
#define CMD_V_TH              31    /* return V_th */
#define CMD_C_GS              32    /* return C_GS */
#define CMD_R_DS              33    /* return R_DS */
#define CMD_I_DSS             34    /* return I_DSS */
#define CMD_C_GE              35    /* return C_GE */
#define CMD_V_GT              36    /* return V_GT */
#define CMD_V_T               37    /* return V_T */
#define CMD_R_BB              38    /* return R_BB */



/*
 *  flags for Info_Type
 */

#define INFO_NONE             0b00000000     /* no flags set */

/* resistor */
#define INFO_R_L              0b00000001     /* measured inductance */

/* diode */
#define INFO_D_R_BE           0b00000001     /* detected B-E resistor */
#define INFO_D_BJT_NPN        0b00000010     /* possible NPN BJT */
#define INFO_D_BJT_PNP        0b00000100     /* possible PNP BJT */
#define INFO_D_I_R            0b00001000     /* measured reverse leakage current */
#define INFO_D_CAP1           0b00010000     /* diode #1: measured capacitance */
#define INFO_D_CAP2           0b00100000     /* diode #2: measured capacitance */

/* BJT */
#define INFO_BJT_D_FB         0b00000001     /* detected flyback diode */
#define INFO_BJT_R_BE         0b00000010     /* detected B-E resistor */

/* FET/IGBT */
#define INFO_FET_D_FB         0b00000001     /* detected body/flyback diode */
#define INFO_FET_SYM          0b00000010     /* symmetrical drain and source */
#define INFO_FET_V_TH         0b00000100     /* measured Vth */
#define INFO_FET_C_GS         0b00001000     /* measured C_GS */
#define INFO_FET_R_DS         0b00010000     /* measured R_DS_on */



/* ************************************************************************
 *   constants for probing
 * ************************************************************************ */


/* probe IDs */
#define PROBE_1               0    /* probe #1 */
#define PROBE_2               1    /* probe #2 */
#define PROBE_3               2    /* probe #3 */


/* component IDs */
/* non-components */
#define COMP_NONE             0
#define COMP_ERROR            1
/* misc components */
#define COMP_RESISTOR        10
#define COMP_CAPACITOR       11
#define COMP_INDUCTOR        12
#define COMP_DIODE           20
/* 3 pin semiconductors */
#define COMP_BJT             30
#define COMP_FET             31
#define COMP_IGBT            32
#define COMP_TRIAC           33
#define COMP_THYRISTOR       34
#define COMP_PUT             35
#define COMP_UJT             36


/* error type IDs */
#define TYPE_DISCHARGE        1    /* discharge error */


/* FET types, also used for IGBTs (bit mask) */
#define TYPE_N_CHANNEL        0b00000001     /* n channel */
#define TYPE_P_CHANNEL        0b00000010     /* p channel */
#define TYPE_ENHANCEMENT      0b00000100     /* enhancement mode */
#define TYPE_DEPLETION        0b00001000     /* depletion mode */
#define TYPE_MOSFET           0b00010000     /* MOSFET */
#define TYPE_JFET             0b00100000     /* JFET */
#define TYPE_SYMMETRICAL      0b01000000     /* symmetrical drain/source */


/* BJT types (bit mask) */ 
#define TYPE_NPN              0b00000001     /* NPN */
#define TYPE_PNP              0b00000010     /* PNP */
#define TYPE_PARASITIC        0b00000100     /* parasitic BJT */


/* diode types (bit mask) */
#define TYPE_STANDARD         0b00000001     /* standard diode */


/* flags for semicondutor detection logic */
#define DONE_NONE             0
#define DONE_SEMI             1
#define DONE_ALTSEMI          2


/* multiplicator table IDs */
#define TABLE_SMALL_CAP       1
#define TABLE_LARGE_CAP       2
#define TABLE_INDUCTOR        3


/* bit flags for PullProbe() (bit mask) */
#define PULL_DOWN             0b00000000     /* pull down */
#define PULL_UP               0b00000001     /* pull up */
#define PULL_1MS              0b00001000     /* pull for 1ms */
#define PULL_10MS             0b00010000     /* pull for 10ms */



/* ************************************************************************
 *   constants for display output
 * ************************************************************************ */


/* custom chars/symbols */
#define LCD_CHAR_0            0    /* just a place holder */
#define LCD_CHAR_DIODE_AC     1    /* diode icon '>|' */
#define LCD_CHAR_DIODE_CA     2	   /* diode icon '|<' */
#define LCD_CHAR_CAP          3    /* capacitor icon '||' */
#define LCD_CHAR_OMEGA        4    /* omega */
#define LCD_CHAR_MICRO        5    /* µ / micro */
#define LCD_CHAR_RESISTOR_L   6    /* resistor left icon '[' */
#define LCD_CHAR_RESISTOR_R   7    /* resistor right icon ']' */


/* component symbols */
#define SYMBOL_BJT_NPN        0    /* BJT npn */
#define SYMBOL_BJT_PNP        1    /* BJT pnp */
#define SYMBOL_MOSFET_ENH_N   2    /* MOSFET enhancement mode, n-channel */
#define SYMBOL_MOSFET_ENH_P   3    /* MOSFET enhancement mode, p-channel */
#define SYMBOL_MOSFET_DEP_N   4    /* MOSFET depletion mode, n-channel */
#define SYMBOL_MOSFET_DEP_P   5    /* MOSFET depletion mode, p-channel */
#define SYMBOL_JFET_N         6    /* JFET n-channel */
#define SYMBOL_JFET_P         7    /* JFET p-channel */
#define SYMBOL_IGBT_ENH_N     8    /* IGBT enhancement mode, n-channel */
#define SYMBOL_IGBT_ENH_P     9    /* IGBT enhancement mode, p-channel */
#define SYMBOL_SCR           10    /* SCR / thyristor */
#define SYMBOL_TRIAC         11    /* TRIAC */
#define SYMBOL_PUT           12    /* PUT */
#define SYMBOL_UJT           13    /* UJT */


/* pinout positions (bit mask) */
#define PIN_NONE              0b00000000     /* no output */
#define PIN_LEFT              0b00000001     /* left of symbol */
#define PIN_RIGHT             0b00000010     /* right of symbol */
#define PIN_BOTTOM            0b00000100     /* bottom */
#define PIN_TOP               0b00001000     /* top */



/* ************************************************************************
 *   constants for additional hardware
 * ************************************************************************ */


/* port pins of PCF8574 I2C IO chip */
#define PCF8574_P0            0b00000000     /* pin #0 */
#define PCF8574_P1            0b00000001     /* pin #1 */
#define PCF8574_P2            0b00000010     /* pin #2 */
#define PCF8574_P3            0b00000011     /* pin #3 */
#define PCF8574_P4            0b00000100     /* pin #4 */
#define PCF8574_P5            0b00000101     /* pin #5 */
#define PCF8574_P6            0b00000110     /* pin #6 */
#define PCF8574_P7            0b00000111     /* pin #7 */



/* ************************************************************************
 *   structures
 * ************************************************************************ */


/* tester modes, states, offsets and values */
typedef struct
{
  uint8_t           OP_Mode;       /* operation mode & state flags */
  volatile uint8_t  OP_Control;    /* operation control & signal flags */
  #ifdef SAVE_POWER
  uint8_t           SleepMode;     /* MCU sleep mode */
  #endif
  uint8_t           Samples;       /* number of ADC samples */
  uint8_t           AutoScale;     /* flag to disable/enable ADC auto scaling */
  uint8_t           RefFlag;       /* internal control flag for ADC */
  uint16_t          Bandgap;       /* voltage of internal bandgap reference (mV) */
  uint16_t          Vcc;           /* voltage of Vcc (mV) */
} Config_Type;


/* basic adjustment offsets and values (stored in EEPROM) */
typedef struct
{
  uint16_t          RiL;           /* internal pin resistance of MCU in low mode (0.1 Ohms) */
  uint16_t          RiH;           /* internal pin resistance of MCU in high mode (0.1 Ohms) */
  uint16_t          RZero;         /* resistance of probe leads (2 in series) (0.01 Ohms) */
  uint8_t           CapZero;       /* capacity zero offset (input + leads) (pF) */
  int8_t            RefOffset;     /* voltage offset of bandgap reference (mV) */
  int8_t            CompOffset;    /* voltage offset of analog comparator (mV) */
  uint8_t           Contrast;      /* current contrast value */
  uint8_t           CheckSum;      /* checksum for stored values */
} Adjust_Type;


/* touch screen adjustment offsets (stored in EEPROM) */
typedef struct
{
  uint16_t          X_Left;        /* offset for left side */
  uint16_t          X_Right;       /* offset for right side */
  uint16_t          Y_Top;         /* offset for top */
  uint16_t          Y_Bottom;      /* offset for bottom */
  uint8_t           CheckSum;      /* checksum for stored values */
} Touch_Type;


/* user interface */
typedef struct
{
  /* display */
  uint8_t           LineMode;      /* line mode for LCD_NextLine() */
  uint8_t           CharPos_X;     /* current character x position */
  uint8_t           CharPos_Y;     /* current character y position */
                                   /* top left is 1/1 */
  uint8_t           CharMax_X;     /* max. characters per line */
  uint8_t           CharMax_Y;     /* max. number of lines */
  uint8_t           MaxContrast;   /* maximum contrast */
  /* color support */
  #ifdef LCD_COLOR
  uint16_t          PenColor;      /* pen color */ 
  #endif
  /* fancy pinout with symbols */
  #ifdef SW_SYMBOLS
  uint8_t           SymbolLine;    /* line for output */
  uint8_t           SymbolSize_X;  /* x size in characters */
  uint8_t           SymbolSize_Y;  /* y size in characters */
  uint8_t           SymbolPos_X;   /* x char position (left) */
  uint8_t           SymbolPos_Y;   /* y char position (top) */
  #endif

  /* keys (push buttons etc.) */
  #ifdef HW_KEYS
  uint8_t           KeyOld;        /* former key */
  uint8_t           KeyStep;       /* step size (1-7) */
  uint8_t           KeyStepOld;    /* former step size */
  #endif
  /* rotary encoder */
  #ifdef HW_ENCODER
  uint8_t           EncState;      /* last AB status */
  uint8_t           EncDir;        /* turning direction */
  uint8_t           EncPulses;     /* number of Gray code pulses */
  uint8_t           EncTicks;      /* time counter */
  #endif
  /* increase/decrease push buttons */
  #ifdef HW_INCDEC_KEYS
  #endif
  /* touch screen */
  #ifdef HW_TOUCH
  uint16_t          TouchRaw_X;    /* raw touch screen x position */
  uint16_t          TouchRaw_Y;    /* raw touch screen y position */
  uint8_t           TouchPos_X;    /* charater x position */
  uint8_t           TouchPos_Y;    /* charater y position */
  #endif

} UI_Type;


/* probes */
typedef struct
{
  /* probe IDs */
  uint8_t           ID_1;          /* probe-1 */
  uint8_t           ID_2;          /* probe-2 */
  uint8_t           ID_3;          /* probe-3 */

  /* backup probe IDs */
  uint8_t           ID2_1;         /* probe-1 */
  uint8_t           ID2_2;         /* probe-2 */
  uint8_t           ID2_3;         /* probe-3 */

  /* bit masks for switching probes and test resistors */
  uint8_t           Rl_1;          /* Rl mask for probe-1 */
  uint8_t           Rl_2;          /* Rl mask for probe-2 */
  uint8_t           Rl_3;          /* Rl mask for probe-3 */
  uint8_t           Rh_1;          /* Rh mask for probe-1 */
  uint8_t           Rh_2;          /* Rh mask for probe-2 */
  uint8_t           Rh_3;          /* Rh mask for probe-3 */
  uint8_t           Pin_1;         /* pin mask for probe-1 */
  uint8_t           Pin_2;         /* pin mask for probe-2 */
  uint8_t           Pin_3;         /* pin mask for probe-3 */
  uint8_t           ADC_1;         /* ADC MUX input address for probe-1 */
  uint8_t           ADC_2;         /* ADC MUX input address for probe-2 */
  uint8_t           ADC_3;         /* ADC MUX input address for probe-3 */
} Probe_Type;


/* checking/probing */
typedef struct
{
  uint8_t           Found;         /* component type */ 
  uint8_t           Type;          /* component specific subtype */
  uint8_t           Done;          /* flag for transistor detection done */
  uint8_t           AltFound;      /* alternative component type */
  uint8_t           Resistors;     /* number of resistors found */
  uint8_t           Diodes;        /* number of diodes found */
  uint8_t           Probe;         /* error: probe pin */ 
  uint16_t          U;             /* error: voltage in mV */
  #ifdef SW_SYMBOLS
  uint8_t           Symbol;        /* symbol ID */
  uint8_t           AltSymbol;     /* symbol ID for alternative component */
  #endif
} Check_Type;


/* resistor */
typedef struct
{
  uint8_t           A;             /* probe pin #1 */
  uint8_t           B;             /* probe pin #2 */
  int8_t            Scale;         /* exponent of factor (value * 10^x) */
  unsigned long     Value;         /* resistance */
} Resistor_Type;


/* capacitor */
typedef struct
{
  uint8_t           A;             /* probe pin #1 */
  uint8_t           B;             /* probe pin #2 */
  int8_t            Scale;         /* exponent of factor (value * 10^x) */
  unsigned long     Value;         /* capacitance incl. zero offset */
  unsigned long     Raw;           /* capacitance excl. zero offset */
  unsigned long     I_leak;        /* leakage current (in 10nA) */
} Capacitor_Type;


/* inductor */
typedef struct
{
  int8_t            Scale;         /* exponent of factor (value * 10^x) */
  unsigned long     Value;         /* inductance */  
} Inductor_Type;


/* diode */
typedef struct
{
  uint8_t           A;             /* probe pin connected to anode */
  uint8_t           C;             /* probe pin connected to cathode */
  uint16_t          V_f;           /* forward voltage in mV (high current) */
  uint16_t          V_f2;          /* forward voltage in mV (low current) */
} Diode_Type;


/* common semiconductors */
typedef struct
{
  uint8_t           A;             /* probe pin connected to pin A */
  uint8_t           B;             /* probe pin connected to pin B */
  uint8_t           C;             /* probe pin connected to pin C */
  uint16_t          U_1;           /* voltage #1 */
  int16_t           U_2;           /* voltage #2 (+/-) */
  uint32_t          F_1;           /* factor #1 */
  uint32_t          I_value;       /* current */
  int8_t            I_scale;       /* exponent of factor (value * 10^x) */
  uint32_t          C_value;       /* capacitance */
  int8_t            C_scale;       /* exponent of factor (value * 10^x) */
} Semi_Type;

/* 
  Mapping

           BJT          FET          SCR          Triac        IGBT
  ----------------------------------------------------------------------
  A        Base         Gate         Gate         Gate         Gate
  B        Collector    Drain        Anode        MT2          Collector
  C        Emitter      Source       Cathode      MT1          Emitter
  U_1      V_BE (mV)    R_DS (0.01)  V_GT (mV)    V_GT (mV)
  U_2                   V_th (mV)                              V_th (mV)
  F_1      h_E                                    MT2 (mV)
  I_value  I_CEO        I_DSS
  I_scale  I_CEO        I_DSS
  C_value  C_BE
  C_scale  C_BE
*/


/* special semiconductors */
typedef struct
{
  uint8_t           A;             /* probe pin connected to pin A */
  uint8_t           B;             /* probe pin connected to pin B */
  uint8_t           C;             /* probe pin connected to pin C */
  uint16_t          U_1;           /* voltage #1 */
  uint16_t          U_2;           /* voltage #2 */
} AltSemi_Type;

/* 
  Mapping

          PUT         UJT
  ------------------------------------------------------------------
  A       Gate        Emitter
  B       Anode       B2
  C       Cathode     B1
  U_1     V_f
  U_2     V_T
*/


/* additional component data for remote commands */
typedef struct
{
  /* common stuff */
  uint8_t           Quantity;      /* component quantity */
  uint8_t           Selected;      /* selected component */
  uint8_t           Flags;         /* misc flags */
  void              *Comp1;        /* pointer to component #1 */
  void              *Comp2;        /* pointer to component #2 */
  uint16_t          Val1;          /* value #1 */
} Info_Type;

/* 
  Mapping

          R   C           D    BJT        FET    IGBT
  -----------------------------------------------------------------
  Comp1   R1  C           D1   D_FB       D_FB   D_FB
  Comp2   R2              D2
  Val1        ESR (0.01)       V_BE (mV)
*/


/* SPI */
typedef struct
{
  uint8_t           ClockRate;     /* clock rate bits */
} SPI_Type;


/* I2C */
typedef struct
{
  uint8_t           Byte;          /* address/data byte */
  uint8_t           Timeout;       /* ACK timeout in 10µs */
} I2C_Type;


/* remote command */
typedef struct
{
  uint8_t                ID;       /* command ID */
  const unsigned char    *Cmd;     /* storage address of command string */
} Cmd_Type;



/* ************************************************************************
 *   EOF
 * ************************************************************************ */
