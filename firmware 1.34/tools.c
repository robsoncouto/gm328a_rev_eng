/* ************************************************************************
 *
 *   tools / hardware options / software options
 *
 *   (c) 2012-2018 by Markus Reschke
 *
 * ************************************************************************ */


/*
 *  local constants
 */

/* source management */
#define EXTRAS_C

/* probes */
#define PROBES_PWM       0
#define PROBES_ESR       1



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

#ifdef HW_FREQ_COUNTER_BASIC
volatile uint16_t      FreqPulses;      /* number of pulses */
#endif

#ifdef HW_FREQ_COUNTER_EXT
volatile uint32_t      FreqPulses;      /* number of pulses */
#endif

#ifdef SW_SERVO
uint8_t                SweepStep;       /* sweep step */
volatile uint8_t       SweepDir;        /* sweep direction */
#endif



/* ************************************************************************
 *   support functions
 * ************************************************************************ */


#if defined (SW_PWM_SIMPLE) || defined (SW_PWM_PLUS) || defined (SW_SQUAREWAVE) || defined (SW_SERVO) || defined (SW_ESR) || defined (SW_OLD_ESR)

/*
 *  display probe pinout
 *
 *  required:
 *  - Mode
 *    PROBES_PWM  PWM signal
 *    PROBES_ESR  ESR measurement
 */

void ProbePinout(uint8_t Mode)
{
  uint8_t           ID_1, ID_2, ID_3;   /* characters */

  LCD_ClearLine2();                /* info goes to line #2 */

  if (Mode == PROBES_PWM)     /* PWM */
  {
    /* 1: Gnd / 2: signal / 3: Gnd */
    ID_1 = '-';
    ID_2 = 's';
    ID_3 = '-';
  }
  #if defined (SW_ESR) || defined (SW_OLD_ESR)
  else                        /* ESR */
  {
    /* 1: + / 3: - */
    ID_1 = '+';
    ID_2 = 0;
    ID_3 = '-';
  }
  #endif

  Show_SimplePinout(ID_1, ID_2, ID_3);  /* display pinout */

  TestKey(5000, CURSOR_NONE);      /* wait for any key press or 5s */  
  LCD_ClearLine2();                /* clear line #2 */
}

#endif



/* ************************************************************************
 *   PWM
 * ************************************************************************ */


#ifdef SW_PWM_SIMPLE

/*
 *  PWM generator with simple UI
 *  - uses probe #2 (OC1B) as PWM output
 *    and probe #1 & probe #3 as ground
 *  - alternative: dedicated signal output via OC1B
 *  - max. reasonable PWM frequency for 8MHz MCU clock is 40kHz
 *  - requires idle sleep mode to keep timer running when MCU is sleeping
 *
 *  requires:
 *  - Freqency in Hz
 */

void PWM_Tool(uint16_t Frequency)
{
  uint8_t           Test = 1;           /* loop control and user feedback */
  uint8_t           Ratio;              /* PWM ratio (in %) */
  uint8_t           Prescaler;          /* timer prescaler */
  uint16_t          Top;                /* top value */
  uint16_t          Toggle;             /* counter value to toggle output */
  uint32_t          Value;              /* temporary value */

  /*
   *  Timer1:
   *  - phase correct PWM:    f_PWM = f_MCU / (2 * prescaler * top)
   *  - available prescalers: 1, 8, 64, 256, 1024
   *  - range of top:         (2^2 - 1) up to (2^16 - 1)
   *
   *  - ranges for a 8MHz MCU clock:
   *    prescaler  /2pre     top 2^16   top 2^2    top 100
   *    1          4MHz      61Hz       1MHz       40kHz
   *    8          500kHz    7.6Hz      125kHz     5kHz
   *    64         62.5kHz   0.95Hz     15.625kHz  625Hz
   *    256        15625Hz   0.24Hz     3906.25Hz  156.25Hz
   *  - to support a PWM ratio of 1% top should be 100 at least
   */

  ShortCircuit(0);                    /* make sure probes are not shorted */
  LCD_Clear();
  Display_EEString_Space(PWM_str);    /* display: PWM */
  Display_Value(Frequency, 0, 'H');   /* display frequency */
  Display_Char('z');                  /* make it Hz :-) */
  #ifndef HW_FIXED_SIGNAL_OUTPUT
  ProbePinout(PROBES_PWM);            /* show probes used */
  #endif

  #ifndef HW_FIXED_SIGNAL_OUTPUT
  /* probes 1 and 3 are signal ground, probe 2 is signal output */
  ADC_PORT = 0;                         /* pull down directly: */
  ADC_DDR = (1 << TP1) | (1 << TP3);    /* probe 1 & 3 */
  R_DDR = (1 << R_RL_2);                /* enable Rl for probe 2 */
  R_PORT = 0;                           /* pull down probe 2 initially */
  #endif

  #ifdef HW_FIXED_SIGNAL_OUTPUT
  /* dedicated output via OC1B */
  SIGNAL_PORT &= ~(1 << SIGNAL_OUT);    /* low by default */
  SIGNAL_DDR |= (1 << SIGNAL_OUT);      /* enable output */
  #endif

  /*
   *  calculate required prescaler and top value based on MCU clock
   *  - top = f_MCU / (2 * prescaler * f_PWM)
   */

  Value = CPU_FREQ / 2;
  Value /= Frequency;

  if (Value > 2000000)        /* low frequency (<20Hz @8MHz) */
  {
    Value /= 256;
    Prescaler = (1 << CS12);                 /* 256 */
  }
  else if (Value > 16000)     /* mid-range frequency (<250Hz @8MHz) */
  {
    Value /= 64;
    Prescaler = (1 << CS11) | (1 << CS10);   /* 64 */
  }
  else                        /* high frequency */
  {
    Prescaler = (1 << CS10);                 /* 1 */
  }

  Top = (uint16_t)Value;      /* keep lower 16 bits */

  /* set start values */
  Ratio = 50;                 /* default ratio is 50% */
  Toggle = Top / 2;           /* compare value for 50% */


  /*
   *  set up Timer1 for PWM
   *  - phase correct PWM
   *  - top value by OCR1A
   *  - OC1B non-inverted output
   */

  TCCR1B = 0;                                /* stop timer */
  /* enable OC1B pin and set timer mode */
  TCCR1A = (1 << WGM11) | (1 << WGM10) | (1 << COM1B1);
  TCCR1B = (1 << WGM13);
  TCNT1 = 0;                                 /* set counter to 0 */
  OCR1A = Top;                               /* set top value (-1) */
  OCR1B = Toggle;                            /* set value to compare with */

  /* start counter by setting clock prescaler */
  TCCR1B = (1 << WGM13) | Prescaler;


  /*
   *  ratio control
   */

  while (Test > 0)
  {
    /* show current ratio */
    LCD_ClearLine2();
    Display_Value(Ratio, 0, '%');       /* show ratio in % */
    #ifdef HW_KEYS
    if (Test <= KEY_LONG)               /* just for test button usage */
    #endif
    MilliSleep(500);                    /* smooth UI */

    /*
     *  user interface
     *  - short key press -> increase ratio
     *    long key press -> decrease ratio
     *    two short key presses -> exit tool
     */

    Test = TestKey(0, CURSOR_NONE);     /* wait for user feedback */
    if (Test == KEY_SHORT)              /* short key press */
    {
      MilliSleep(50);                   /* debounce button a little bit longer */
      Prescaler = TestKey(200, CURSOR_NONE); /* check for second key press */
      if (Prescaler > 0)                /* second key press */
      {
        Test = 0;                         /* end loop */
      }
      else                              /* single key press */
      {
        if (Ratio <= 95) Ratio += 5;      /* +5% and limit to 100% */
      }
    }
    #ifdef HW_KEYS
    else if (Test == KEY_RIGHT)         /* right key */
    {
      if (Ratio <= 99) Ratio += 1;      /* +1% and limit to 100% */
    }
    else if (Test == KEY_LEFT)          /* left key */
    {
      if (Ratio >= 1) Ratio -= 1;       /* -1% and limit to 0% */
    }
    #endif
    else                                /* long key press */
    {
      if (Ratio >= 5) Ratio -= 5;       /* -5% and limit to 0% */
    }

    /* calculate toggle value: top * (ratio / 100) */
    Value = (uint32_t)Top * Ratio;
    Value /= 100;
    OCR1B = (uint16_t)Value;            /* update compare value */
  }


  /*
   *  clean up
   */

  TCCR1B = 0;                 /* disable timer */
  TCCR1A = 0;                 /* reset flags (also frees PB2) */

  #ifndef HW_FIXED_SIGNAL_OUTPUT
  R_DDR = 0;                            /* set HiZ mode */
  #endif

  #ifdef HW_FIXED_SIGNAL_OUTPUT
  SIGNAL_DDR &= ~(1 << SIGNAL_OUT);     /* set HiZ mode */
  #endif
}

#endif



#ifdef SW_PWM_PLUS

/*
 *  PWM generator with improved UI
 *  - uses probe #2 (OC1B) as PWM output
 *    and probe #1 & probe #3 as ground
 *  - alternative: dedicated signal output via OC1B
 *  - max. reasonable PWM frequency for 8MHz MCU clock is 40kHz
 *  - requires additional keys (e.g. rotary encoder) and
 *    display with more than 2 text lines
 *  - requires idle sleep mode to keep timer running when MCU is sleeping
 */

void PWM_Tool(void)
{
  uint8_t           Flag;               /* loop control */
  uint8_t           Mode;               /* UI */
  uint8_t           Test = 0;           /* user feedback */
  uint8_t           Step;               /* step size */
  uint8_t           Ratio;              /* PWM ratio (in %) */
  uint8_t           Index;              /* prescaler table index */
  uint8_t           Bitmask = 0;        /* prescaler bitmask */
  uint16_t          Prescaler;          /* timer prescaler */
  uint16_t          Top;                /* top value */
  uint16_t          Step2;              /* step size */
  uint16_t          Temp;               /* temporary value */
  uint32_t          Value;              /* temporary value */

  /* local constants for Flag (bitmask) */
  #define RUN_FLAG       0b00000001     /* run / otherwise end */
  #define CHANGE_FREQ    0b00000010     /* change frequency */
  #define CHANGE_RATIO   0b00000100     /* change ratio */
  #define DISPLAY_FREQ   0b00001000     /* display frequency */
  #define DISPLAY_RATIO  0b00010000     /* display ratio */

  /* local constants for Mode */
  #define MODE_FREQ               1     /* frequency mode */
  #define MODE_RATIO              2     /* ratio mode */


  /*
   *  Timer1:
   *  - phase & frequency correct PWM:  f_PWM = f_MCU / (2 * prescaler * top)
   *  - available prescalers:           1, 8, 64, 256, 1024
   *  - range of top:                   (2^2 - 1) up to (2^16 - 1)
   *  - ranges for a 8MHz MCU clock:
   *    prescaler  /2pre       top 2^16   top 2^2    top 100
   *    1          4MHz        61Hz       1MHz       40kHz
   *    8          500kHz      7.6Hz      125kHz     5kHz
   *    64         62.5kHz     0.95Hz     15.625kHz  625Hz
   *    256        15625Hz     0.24Hz     3906.25Hz  156.25Hz
   *    1024       3906.25Hz   0.06Hz     976.5Hz    39Hz
   *  - to support a PWM ratio of 1% top should be 100 at least
   */

  ShortCircuit(0);                    /* make sure probes are not shorted */
  LCD_Clear();
  Display_EEString_Space(PWM_str);    /* display: PWM */
  #ifndef HW_FIXED_SIGNAL_OUTPUT
  ProbePinout(PROBES_PWM);            /* show probes used */
  #endif

  #ifndef HW_FIXED_SIGNAL_OUTPUT
  /* probes 1 and 3 are signal ground, probe 2 is signal output */
  ADC_PORT = 0;                         /* pull down directly: */
  ADC_DDR = (1 << TP1) | (1 << TP3);    /* probe 1 & 3 */
  R_DDR = (1 << R_RL_2);                /* enable Rl for probe 2 */
  R_PORT = 0;                           /* pull down probe 2 initially */
  #endif

  #ifdef HW_FIXED_SIGNAL_OUTPUT
  /* dedicated output via OC1B */
  SIGNAL_PORT &= ~(1 << SIGNAL_OUT);    /* low by default */
  SIGNAL_DDR |= (1 << SIGNAL_OUT);      /* enable output */
  #endif


  /*
   *  set up Timer1 for PWM
   *  - phase and frequency correct PWM
   *  - top value by OCR1A
   *  - OC1B non-inverted output
   */

  TCNT1 = 0;                            /* set counter to 0 */

  /* enable OC1B pin and set timer mode */
  TCCR1A = (1 << WGM10) | (1 << COM1B1);
  TCCR1B = (1 << WGM13);


  /*
   *  processing loop
   */

  /* start values */
  Ratio = 50;                      /* 50% PWM ratio */
  Prescaler = 1;
  Index = 0;
  Bitmask = (1 << CS10);           /* prescaler bitmask for 1 */
  Top = (CPU_FREQ / 2000);         /* 1kHz */
  Flag = RUN_FLAG | CHANGE_FREQ | CHANGE_RATIO | DISPLAY_FREQ | DISPLAY_RATIO;
  Mode = MODE_FREQ;                /* frequency mode */

  while (Flag > 0)       /* processing loop */
  {
    /*
     *  change timer settings
     */

    if (Flag & CHANGE_FREQ)        /* change frequency */
    {
      /*
       *  auto-ranging
       */

      Step = Index;

      /* check if we should change the range */
      if (Top > 0x7FFF)            /* more than 15 bits */
      {
        if (Index < (NUM_TIMER1 - 1))   /* don't exceed upper prescaler limit */
        {
          Index++;                 /* increase prescaler */
        }
      }
      else if (Top < 0x0FFF)       /* less than 12 bits */
      {
        if (Index > 0)             /* don't exceed lower prescaler limit */
        {
          Index--;                 /* decrease prescaler */
        }
      }

      /* process changed range */
      if (Index != Step)           /* range has changed */
      {
        Step2 = Prescaler;         /* save old value */

        /* read new prescaler and bitmask from table */
        Prescaler = eeprom_read_word(&T1_Prescaler_table[Index]);
        Bitmask = eeprom_read_byte(&T1_Bitmask_table[Index]);

        if (Index > Step)          /* larger prescaler */
        {
          /* decrease top value by same factor as the prescaler increased */
          Temp = Prescaler / Step2;
          Top /= Temp;
        }
        else                       /* smaller prescaler */
        {
          /* increase top value by same factor as the prescaler decreased */
          Temp = Step2 / Prescaler;
          Top *= Temp;
        }
      }

      /* set frequency */
      OCR1A = Top;                      /* set top value */
      TCCR1B = (1 << WGM13) | Bitmask;  /* (re)start timer */
      Flag &= ~CHANGE_FREQ;             /* clear flag */
      /* a frequency change implies a ratio change */
    }


    if (Flag & CHANGE_RATIO)       /* change ratio */
    {
      /* toggle = top * (ratio / 100) */
      Value = (uint32_t)Top * Ratio;
      Value /= 100;
      OCR1B = (uint16_t)Value;          /* set compare/toggle value */
      Flag &= ~CHANGE_RATIO;            /* clear flag */
    }


    /*
     *  update display
     */

    if (Flag & DISPLAY_FREQ)       /* display frequency */
    {
      LCD_ClearLine2();
      MarkItem(MODE_FREQ, Mode);        /* mark mode if selected */

      /* f_PWM = f_MCU / (2 * prescaler * top) */
      Value = CPU_FREQ * 50;            /* scale to 0.01Hz and /2 */
      Value /= Prescaler;
      Step = 2;                         /* 2 decimal places */

      /*
       *  optimize resolution of frequency without causing an overflow
       *  prescaler       :  1  8  64  256  1024
       *  decimal places  :  2  3   4    4     5
       */

      Temp = Prescaler;
      while (Temp >= 8)         /* loop through prescaler steps */
      {
        Value *= 10;            /* scale by factor 0.1 */
        Step++;                 /* one decimal place more */
        Temp /= 8;              /* next lower prescaler */
      }

      Value /= Top;

      Display_FullValue(Value, Step, 'H');
      Display_Char('z');                /* add z for Hz */

      Flag &= ~DISPLAY_FREQ;            /* clear flag */
    }


    if (Flag & DISPLAY_RATIO)      /* display ratio */
    {
      LCD_ClearLine(3);
      LCD_CharPos(1, 3);
      MarkItem(MODE_RATIO, Mode);       /* mark mode if selected */

      Display_Value(Ratio, 0, '%');     /* show ratio in % */

      Flag &= ~DISPLAY_RATIO;           /* clear flag */
    }

    /* smooth UI after long key press */
    if (Test == KEY_LONG)          /* long key press */
    {
      /* wait until button is released to prevent further actions */
      while (!(CONTROL_PIN & (1 << TEST_BUTTON))) /* as long as key is pressed */
      {
        wait10ms();           /* simply wait */
      }

      MilliSleep(500);        /* smooth UI */
    }


    /*
     *  user feedback
     */

    Test = TestKey(0, CURSOR_NONE);     /* wait for user feedback */

    /* consider rotary encoder's turning velocity */
    Step = UI.KeyStep;             /* get velocity (1-7) */
    Step2 = Step;
    if (Step > 1)                  /* larger step */
    {
      /* increase step size based on turning velocity */
      if (Mode == MODE_FREQ)       /* frequency mode */
      {
        /*
         *  value ranges for each prescaler:
         *  -    1:  100 -> 32767 /  100 <- 32760
         *  -    8: 4095 -> 32767 / 4095 <- 32760
         *  -   64: 4095 -> 32767 / 4095 <- 16380
         *  -  256: 8191 -> 32767 / 4095 <- 16380
         *  - 1024: 8191 -> 65635 / 4095 <- 65635
         */

        /* step^4: 16 81 256 625 1296 2401 */
        Step2 *= Step;             /* ^2 */
        Step2 *= Step2;            /* ^2 */
      }
      else                         /* ratio mode */
      {
        /* 0-100% */
        Step *= 100 / 32;
      }
    }

    /* process user input */
    if (Test == KEY_SHORT)              /* short key press */
    {
      MilliSleep(50);                   /* debounce button a little bit longer */
      Test = TestKey(200, CURSOR_NONE); /* check for second key press */

      if (Test > KEY_TIMEOUT)           /* second key press */
      {
        Flag = 0;                       /* end loop */
      }
      else                              /* single key press */
      {
        /* toggle frequency/ratio mode */
        if (Mode == MODE_FREQ)     /* frequency mode */
        {
          Mode = MODE_RATIO;       /* change to ratio mode */
        }
        else                       /* ratio mode */
        {
          Mode = MODE_FREQ;        /* change to frequency mode */
        }

        Flag |= DISPLAY_FREQ | DISPLAY_RATIO;  /* update display */
      }
    }
    else if (Test == KEY_LONG)          /* long key press */
    {
      if (Mode == MODE_FREQ)       /* frequency mode */
      {
        /* set 1kHz */
        Prescaler = 1;
        Index = 0;
        Bitmask = (1 << CS10);     /* prescaler bitmask for 1 */
        Top = (CPU_FREQ / 2000);   /* 1kHz */
        Flag |= CHANGE_FREQ | DISPLAY_FREQ | CHANGE_RATIO;   /* set flags */
      }
      else                         /* ratio mode */
      {
        /* set 50% */
        Ratio = 50;
        Flag |= CHANGE_RATIO | DISPLAY_RATIO;     /* set flags */
      }
    }
    else if (Test == KEY_RIGHT)    /* right key */
    {
      if (Mode == MODE_FREQ)       /* frequency mode */
      {
        /* increase frequency -> decrease top */
        Temp = Top - Step2;        /* take advantage of underflow */
        if ((Temp > Top) || (Temp < 0x0064))      /* underflow */
        {
          Temp = 0x0064;           /* lower limit */
        }
        Top = Temp;                /* set new value */

        Flag |= CHANGE_FREQ | DISPLAY_FREQ | CHANGE_RATIO;  /* set flags */
      }
      else                         /* ratio mode */
      {
        /* increase ratio */
        Ratio += Step;             /* add (max 200) */
        if (Ratio > 100)           /* limit exceeded */
        {
          Ratio = 100;             /* max. is 100 */
        }

        Flag |= CHANGE_RATIO | DISPLAY_RATIO;     /* set flags */
      }
    }
    else if (Test == KEY_LEFT)     /* left key */
    {
      if (Mode == MODE_FREQ)       /* frequency mode */
      {
        /* decrease frequency -> increase top */
        Temp = Top + Step2;        /* take advantage of overflow */
        if (Temp < Top)            /* overflow */
        {
          Temp = 0xFFFF;           /* upper limit */
        }
        Top = Temp;                /* set new value */

        Flag |= CHANGE_FREQ | DISPLAY_FREQ | CHANGE_RATIO;  /* set flags */
      }
      else                         /* ratio mode */
      {
        /* decrease ratio */
        if (Ratio > Step)          /* no underflow */
        {
          Ratio -= Step;           /* new ratio */
        }
        else                       /* underflow */
        {
          Ratio = 0;               /* lower limit is 0 */
        }

        Flag |= CHANGE_RATIO | DISPLAY_RATIO;     /* set flags */
      }
    }
  }


  /*
   *  clean up
   */

  TCCR1B = 0;                 /* disable timer */
  TCCR1A = 0;                 /* reset flags (also frees PB2) */

  #ifndef HW_FIXED_SIGNAL_OUTPUT
  R_DDR = 0;                            /* set HiZ mode */
  #endif

  #ifdef HW_FIXED_SIGNAL_OUTPUT
  SIGNAL_DDR &= ~(1 << SIGNAL_OUT);     /* set HiZ mode */
  #endif

  /* clean up local constants */
  #undef MODE_RATIO
  #undef MODE_FREQ

  #undef DISPLAY_RATIO
  #undef DISPLAY_FREQ
  #undef CHANGE_RATIO
  #undef CHANGE_FREQ
  #undef RUN_FLAG
}

#endif



#ifdef SW_SERVO

/*
 *  Servo Check, PWM generator for testing servos
 *  - uses probe #2 (OC1B) as PWM output
 *    and probe #1 & probe #3 as ground
 *  - alternative: dedicated signal output via OC1B
 *  - requires additional keys (e.g. rotary encoder) and
 *    display with more than 2 lines
 *  - requires idle sleep mode to keep timers running when MCU is sleeping
 */

void Servo_Check(void)
{
  uint8_t           Flag;               /* loop control */
  uint8_t           Mode;               /* UI */
  uint8_t           Test = 0;           /* user feedback */
  uint8_t           Index;              /* PWM index */
  uint8_t           Period[4] = {200, 80, 40, 30};  /* in 0.1ms */
  uint16_t          Toggle;             /* toggle value */
  uint16_t          Step;               /* step size */
  uint16_t          Temp;               /* temporary value */
  uint32_t          Value;              /* temporary value */

  /* local constants for Flag (bitmask) */
  #define RUN_FLAG       0b00000001     /* run / otherwise end */
  #define SWEEP_MODE     0b00000010     /* sweep mode */
  #define CHANGE_PULSE   0b00000100     /* change pulse width */
  #define CHANGE_FREQ    0b00001000     /* change frequency */
  #define DISPLAY_PULSE  0b00010000     /* display pulse width */
  #define DISPLAY_FREQ   0b00100000     /* display frequency */
  #define TOGGLE_SWEEP   0b01000000     /* enter/leave sweep operation */

  /* local constants for Mode */
  #define MODE_PULSE              1     /* pulse width mode */
  #define MODE_FREQ               2     /* frequency mode */


  /*
   *  MCU clock specific value
   *  - step size for a resolution of about 0.01ms
   *  - 8MHz: 5, 16MHz: 10, 20MHz: 13
   */

  #if (CPU_FREQ >= 16000000)
    #define PULSE_STEP        10
  #elif (CPU_FREQ >= 8000000)
    #define PULSE_STEP        5
  #else
    #define PULSE_STEP        1
  #endif

  /*
   *  PWM for servos:
   *  - frequency
   *    50Hz / 20ms  analog servo
   *    125Hz / 8ms  digital servo
   *    250Hz / 4ms  high speed digital servo
   *    333Hz / 3ms  high speed digital servo
   *  - pulse 1 - 2ms (allow 0.5 - 2.5ms)
   *    left   1.0ms
   *    mid    1.5ms
   *    right  2.0ms
   *  - typical rotation is 90-120° & 180°
   *  - typical speed is 30-500ms/60°
   */

  ShortCircuit(0);                    /* make sure probes are not shorted */
  LCD_Clear();
  Display_EEString_Space(Servo_str);  /* display: Servo */
  #ifndef HW_FIXED_SIGNAL_OUTPUT
  ProbePinout(PROBES_PWM);            /* show probes used */
  #endif

  #ifndef HW_FIXED_SIGNAL_OUTPUT
  /* probes 1 and 3 are signal ground, probe 2 is signal output */
  ADC_PORT = 0;                         /* pull down directly: */
  ADC_DDR = (1 << TP1) | (1 << TP3);    /* probe 1 & 3 */
  R_DDR = (1 << R_RL_2);                /* enable Rl for probe 2 */
  R_PORT = 0;                           /* pull down probe 2 initially */
  #endif

  #ifdef HW_FIXED_SIGNAL_OUTPUT
  /* dedicated output via OC1B */
  SIGNAL_PORT &= ~(1 << SIGNAL_OUT);    /* low by default */
  SIGNAL_DDR |= (1 << SIGNAL_OUT);      /* enable output */
  #endif


  /*
   *  calculate required values for PWM based on MCU clock
   *  - Timer1, phase correct PWM
   *  - top = f_MCU / (2 * prescaler * f_PWM)
   *        = (f_MCU * t_PWM) / (2 * prescaler)
   *  - toggle = (f_MCU * t_pulse) / (2 * prescaler)
   *  - use prescaler 1:8 for best resolution across all MCU clocks
   *    with t_pulse in 0.1ms
   *    = ((f_MCU / 10000) * t_pulse) / 16
   */

  /* PWM toggle values (t_pulse) */
  #define SERVO_LEFT_MAX      (((CPU_FREQ / 10000) * 5) / 16)    /* 0.5ms */
  #define SERVO_LEFT_NORM     (((CPU_FREQ / 10000) * 10) / 16)   /* 1.0ms */
  #define SERVO_MID           (((CPU_FREQ / 10000) * 15) / 16)   /* 1.5ms */
  #define SERVO_RIGHT_NORM    (((CPU_FREQ / 10000) * 20) / 16)   /* 2.0ms */  
  #define SERVO_RIGHT_MAX     (((CPU_FREQ / 10000) * 25) / 16)   /* 2.5ms */

  /* sweep control */
  #define SERVO_STEP_MAX      (SERVO_LEFT_NORM / 10)   /* toggle_1ms / 10 */

  /*
   *  calculate required values for sweep timer
   *  - Timer0, CTC mode
   *  - t_step = 3ms -> f_step = 333Hz
   *  - prescaler = 1024
   *  - top = (f_MCU / (f_step * prescaler)) - 1
   *        = (t_step / (t_MCU_cycle * prescaler)) - 1
   *  - t_step = t_MCU_cycle * prescaler * (top + 1)
   *  - SERVO_STEP_TIME in µs
   */

  #define SERVO_SWEEP_TOP     (((CPU_FREQ / 333) / 1024) - 1)
  #define SERVO_STEP_TIME     ((MCU_CYCLE_TIME * 1024 * (SERVO_SWEEP_TOP + 1)) / 10000)


  /*
   *  set up Timer0 for sweeping
   *  - CTC mode
   *  - top value by OCR0A (double buffered)
   *  - fixed prescaler 1:1024
   */

  TCCR0B = 0;                      /* disable Timer0 */
  TCNT0 = 0;                       /* reset counter */
  OCR0A = SERVO_SWEEP_TOP;         /* set compare value */
  TCCR0A = (1 << WGM01);           /* set timer mode */
  TIMSK0 = (1 << OCIE0A);          /* enable output compare match A interrupt */


  /*
   *  set up Timer1 for PWM
   *  - phase correct PWM
   *  - top value by OCR1A (buffered)
   *  - OC1B non-inverted output
   *  - fixed prescaler 1:8
   */

  TCNT1 = 0;                       /* reset counter to 0 */
  TIMSK1 = 0;                      /* disable all interrupts for Timer1 */

  /* enable OC1B pin and set timer mode */
  TCCR1A = (1 << WGM11) | (1 << WGM10) | (1 << COM1B1);
  TCCR1B = (1 << WGM13) | (1 << CS11);  /* start Timer1 by setting prescaler */

  /* set start values */
  Toggle = SERVO_MID;              /* toggle value (1.5ms) */
  Index = 0;                       /* #0 (20.0ms) */
  SweepStep = 0;
  SweepDir = 0;
  Mode = MODE_PULSE;               /* pulse width mode */
  Flag = RUN_FLAG | MODE_PULSE | CHANGE_PULSE | CHANGE_FREQ | DISPLAY_FREQ | DISPLAY_PULSE | DISPLAY_FREQ;

  /*
   *  todo:
   *  - since the pulse length is displayed with a resolution of 0.01ms
   *    a visible change might need several steps
   *  - improve UI to give visual feedback for each step
   */

  while (Flag > 0)       /* processing loop */
  {
    /*
     *  change timer settings
     */

    /* change pulse width */
    if (Flag & CHANGE_PULSE)
    {
      OCR1B = Toggle;                   /* set toggle value */

      Flag &= ~CHANGE_PULSE;            /* clear flag */
    }

    /* change frequency */
    if (Flag & CHANGE_FREQ)
    {
      /* top = ((f_MCU / 10000) * t_pulse) / 16 */
      Test = Period[Index];             /* get period */
      Value = (CPU_FREQ / 10000);       /* MCU clock in 10kHz */
      Value *= Test;
      Value /= 16; 
      OCR1A = (uint16_t)Value;          /* set top value */

      Flag &= ~CHANGE_FREQ;             /* clear flag */
    }


    /*
     *  update display
     */

    /* display pulse duration / sweep period */
    if (Flag & DISPLAY_PULSE)
    {
      LCD_ClearLine2();
      MarkItem(MODE_PULSE, Mode);       /* mark mode if selected */

      if (Flag & SWEEP_MODE)            /* sweep mode */
      {
        /*
         *  calculate sweep time
         *  - t_sweep = t_step * (toggle_1ms / step)
         */

        Value = SERVO_STEP_TIME;        /* step time in µs (around 3000) */
        Value *= SERVO_LEFT_NORM;       /* * toggle value for 1ms */
        Value /= SweepStep;             /* / step size (in µs) */
      }
      else                              /* normal mode */
      {
        /*
         *  calculate pulse length
         *  - t = (toggle * 2 * prescaler) / f_MCU
         */

        Value = (uint32_t)Toggle;
        Value *= 16000;                 /* * (2 * prescaler) (in 0.001) */
        Value /= (CPU_FREQ / 1000);     /* / f_MCU (in 1µs) */
      }

      /* display value */
      Display_FullValue(Value, 3, 'm');
      Display_Char('s');

      Flag &= ~DISPLAY_PULSE;           /* clear flag */
    }

    /* display PWM frequency/period */
    if (Flag & DISPLAY_FREQ)
    {
      LCD_ClearLine(3);
      LCD_CharPos(1, 3);
      MarkItem(MODE_FREQ, Mode);        /* mark mode if selected */

      Test = Period[Index];             /* get period */
      Value = 10000 / Test;             /* calculate frequency */
      Display_Value(Value, 0, 'H');     /* display frequency */
      Display_Char('z');

      if (Flag & SWEEP_MODE)            /* in sweep mode */
      {
        Display_Space();
        Display_EEString(Sweep_str);    /* display: sweep */
      }

      Flag &= ~DISPLAY_FREQ;            /* clear flag */
    }

    /* smooth UI after long key press */
    if (Test == KEY_LONG)          /* long key press */
    {
      /* wait until button is released to prevent further actions */
      while (!(CONTROL_PIN & (1 << TEST_BUTTON))) /* as long as key is pressed */
      {
        wait10ms();           /* simply wait */
      }

      MilliSleep(500);        /* smooth UI */
    }


    /*
     *  user feedback
     */

    Test = TestKey(0, CURSOR_BLINK);    /* wait for user feedback */

    /* consider rotary encoder's turning velocity (1-7) */
    Step = UI.KeyStep;             /* get velocity */
    if (Step > 1)                  /* larger step */
    {
      /* increase step size based on turning velocity */
      Step--;

      if (Flag & SWEEP_MODE)       /* in sweep mode */
      {
        /*
         *  MCU clock specific value range
         *  - 8MHz: 1-50, 16MHz: 1-100, 20MHz: 1-125
         */

        Step *= (SERVO_STEP_MAX / 32) + 1;
      }
      else                         /* in normal mode */
      {
        /*
         *  MCU clock specific value range
         *  - 8MHz: 250-1250, 16MHz: 500-2500, 20MHz: 625-3125
         *  - use multiples of 0.01ms step size
         */

        Step *= PULSE_STEP * ((SERVO_RIGHT_MAX - SERVO_LEFT_MAX) / 500);
      }
    }
    else                           /* single step */
    {
      if (! (Flag & SWEEP_MODE))   /* in normal mode */
      {
        /*
         *  MCU clock specific value
         *  - change step size for a resolution of about 0.01ms
         */

        Step = PULSE_STEP;
      }
    }

    /* process user input */
    if (Test == KEY_SHORT)              /* short key press */
    {
      MilliSleep(50);                   /* debounce button a little bit longer */
      Test = TestKey(200, CURSOR_NONE); /* check for second key press */

      if (Test > KEY_TIMEOUT)           /* second key press */
      {
        Flag = 0;                       /* end loop */
      }
      else                              /* single key press */
      {
        /* toggle pulse/frequency mode */
        if (Mode == MODE_PULSE)         /* pulse width mode */
        {
          Mode = MODE_FREQ;             /* change to frequency mode */
        }
        else                            /* frequency mode */
        {
          Mode = MODE_PULSE;            /* change to pulse width mode */
        }

        Flag |= DISPLAY_PULSE | DISPLAY_FREQ;     /* update display */
      }
    }
    else if (Test == KEY_LONG)          /* long key press */
    {
      if (Mode == MODE_PULSE)           /* pulse width mode */
      {
        if (Flag & SWEEP_MODE)          /* in sweep mode */
        {
          /* return to slowest sweep speed */
          SweepStep = 1;                /* smallest step */
          Flag |= DISPLAY_PULSE;        /* set flag */
        }
        else                            /* in normal mode */
        {
          /* return to middle position (1.5ms) */
          Toggle = SERVO_MID;           /* set mid */
          Flag |= CHANGE_PULSE | DISPLAY_PULSE;   /* set flags */
        }
      }
      else                              /* frequency mode */
      {
        if (Flag & SWEEP_MODE)          /* in sweep mode */
        {
          /* leave sweep mode */
          Flag &= ~SWEEP_MODE;          /* clear flag */
        }
        else                            /* in normal mode */
        {
          /* enter sweep mode */
          Flag |= SWEEP_MODE;           /* set flag */
        }

        Flag |= DISPLAY_PULSE | DISPLAY_FREQ | TOGGLE_SWEEP;  /* set flags */
      }
    }
    else if (Test == KEY_RIGHT)         /* right key */
    {
      if (Mode == MODE_PULSE)           /* pulse width mode */
      {
        if (Flag & SWEEP_MODE)          /* in sweep mode */
        {
          /* increase sweep speed -> increase sweep step */
          Temp = SweepStep + Step;
          if (Temp > SERVO_STEP_MAX) Temp = SERVO_STEP_MAX;
          SweepStep = (uint8_t)Temp;
          Flag |= DISPLAY_PULSE;        /* set flag */
        }
        else                            /* in normal mode */
        {
          /* increase pulse length -> increase toggle value */
          Temp = Toggle + Step;         /* take advantage of overflow */
          if ((Temp < Toggle) || (Temp > SERVO_RIGHT_MAX))  /* overflow */
          {
            Temp = SERVO_RIGHT_MAX;     /* upper limit */
          }
          Toggle = Temp;                /* set new value */
          Flag |= CHANGE_PULSE | DISPLAY_PULSE;   /* set flags */
        }
      }
      else                              /* frequency mode */
      {
        /* next PWN frequency -> increase index */
        if (Index < 3)                  /* upper limit is 3 */
        {
          Index++;                      /* next one */
          Flag |= DISPLAY_FREQ | CHANGE_FREQ;     /* set flags */
        }       
      }
    }
    else if (Test == KEY_LEFT)          /* left key */
    {
      if (Mode == MODE_PULSE)           /* pulse width mode */
      {
        if (Flag & SWEEP_MODE)          /* in sweep mode */
        {
          /* decrease sweep speed -> decrease sweep step */
          Temp = SweepStep - Step;
          if (Step >= SweepStep) Temp = 1;
          SweepStep = (uint8_t)Temp;
          Flag |= DISPLAY_PULSE;        /* set flag */
        }
        else                            /* in normal mode */
        {
          /* decrease pulse length -> decrease toggle value */
          Temp = Toggle - Step;         /* take advantage of underflow */
          if ((Temp > Toggle) || (Temp < SERVO_LEFT_MAX))   /* underflow */
          {
            Temp = SERVO_LEFT_MAX;      /* lower limit */
          }
          Toggle = Temp;                /* set new value */
          Flag |= CHANGE_PULSE | DISPLAY_PULSE;   /* set flags */
        }
      }
      else                              /* frequency mode */
      {
        /* previous PWM frequency -> decrease index */
        if (Index > 0)                  /* lower limit is 0 */
        {
          Index--;                      /* previous one */
          Flag |= DISPLAY_FREQ | CHANGE_FREQ;     /* set flags */
        }
      }
    }


    /*
     *  enter/leave sweep operation
     *  - use Timer0 as sweep timer
     */

    if (Flag & TOGGLE_SWEEP)
    {
      if (Flag & SWEEP_MODE)            /* enter sweeping */
      {
        /* set start values */
        SweepStep = 1;                  /* forward */
        SweepDir = 1;                   /* lowest speed */

        /* start sweep timer */
        TCNT0 = 0;                      /* reset counter */
        TCCR0B = (1 << CS02) | (1 << CS00);  /* enable timer by setting prescaler */  
      }
      else                              /* exit sweeping */
      {
        /* stop sweep timer */
        TCCR0B = 0;                     /* disable Timer0 */
      }

      Flag &= ~TOGGLE_SWEEP;            /* clear flag */
    }
  }


  /*
   *  clean up
   */

  TCCR0B = 0;                 /* disable Timer0 */
  TIMSK0 = 0;                 /* disable all interrupts for Timer0 */
  TCCR1B = 0;                 /* disable Timer1 */
  TCCR1A = 0;                 /* reset flags (also frees PB2) */

  #ifndef HW_FIXED_SIGNAL_OUTPUT
  R_DDR = 0;                            /* set HiZ mode */
  #endif

  #ifdef HW_FIXED_SIGNAL_OUTPUT
  SIGNAL_DDR &= ~(1 << SIGNAL_OUT);     /* set HiZ mode */
  #endif

  /* clean up local constants */
  #undef SERVO_STEP_TIME
  #undef SERVO_SWEEP_TOP
  #undef SERVO_STEP_MAX
  #undef SERVO_RIGHT_MAX
  #undef SERVO_RIGHT_NORM
  #undef SERVO_MID
  #undef SERVO_LEFT_MAX
  #undef SERVO_LEFT_NORM

  #undef PULSE_STEP

  #undef MODE_FREQ
  #undef MODE_PULSE

  #undef TOGGLE_SWEEP
  #undef DISPLAY_FREQ
  #undef DISPLAY_PULSE
  #undef CHANGE_FREQ
  #undef CHANGE_PULSE
  #undef SWEEP_MODE
  #undef RUN_FLAG
}



/*
 *  ISR for match of Timer0's OCR0A (Output Compare Register A)
 *  - sweep timer for servo check  
 */

ISR(TIMER0_COMPA_vect, ISR_BLOCK)
{
  uint16_t          Temp;     /* temp. value */

  /*
   *  hints:
   *  - the OCF0A interrupt flag is cleared automatically
   *  - interrupt processing is disabled while this ISR runs
   *    (no nested interrupts)
   */

  /* toggle values for PWM */
  #define SERVO_LEFT_NORM     (((CPU_FREQ / 10000) * 10) / 16)   /* 1.0ms */
  #define SERVO_RIGHT_NORM    (((CPU_FREQ / 10000) * 20) / 16)   /* 2.0ms */


  /*
   *  update PWM pulse length (Timer1)
   */

  Temp = OCR1B;               /* get current compare value */

  if (SweepDir == 1)          /* forward */
  {
    Temp += SweepStep;        /* add step */

    if (Temp >= SERVO_RIGHT_NORM)  /* exceeded upper limit */
    {
      Temp = SERVO_RIGHT_NORM;     /* limit */
      SweepDir = 2;                /* change direction */
    }
  }
  else                        /* backward */
  {
    Temp -= SweepStep;        /* substract step */

    if (Temp <= SERVO_LEFT_NORM)   /* exceeded lower limit */
    {
      Temp = SERVO_LEFT_NORM;      /* limit */
      SweepDir = 1;                /* change direction */
    }
  }

  OCR1B = Temp;               /* set new compare value */

  #undef SERVO_LEFT_NORM
  #undef SERVO_RIGHT_NORM
}

#endif



/* ************************************************************************
 *   Signal Generator (just squarewave)
 * ************************************************************************ */


#ifdef SW_SQUAREWAVE

/*
 *  create square wave signal with variable frequency
 *  - uses probe #2 (OC1B) as output
 *    and probe #1 & probe #3 as ground
 *  - alternative: dedicated signal output via OC1B
 *  - requires additional keys (e.g. rotary encoder)
 *  - requires idle sleep mode to keep timer running when MCU is sleeping
 */

void SquareWave_SignalGenerator(void)
{
  uint8_t           Flag = 1;           /* loop control */
  uint8_t           Test;               /* user feedback */
  uint8_t           Index;              /* prescaler table index */
  uint8_t           Bitmask = 0;        /* prescaler bitmask */
  uint16_t          Prescaler;          /* timer prescaler */
  uint16_t          Top;                /* counter's top value */
  uint16_t          Step;               /* step size */
  uint16_t          Temp;               /* temporary value */
  uint32_t          Value;              /* temporary value */

  /*
      fast PWM:             f_PWM = f_MCU / (prescaler * (1 + top))
      available prescalers: 1, 8, 64, 256, 1024
      top:                  (2^2 - 1) up to (2^16 - 1)

      ranges for a 8MHz MCU clock:
      prescaler  /pre       top 2^16     top 2^2
      1          8MHz       122Hz        2MHz
      8          1MHz       15.26Hz      250kHz
      64         125kHz     1.9Hz        31.25kHz
      256        31.25kHz   0.5Hz        7812.5Hz
      1024       7812.5Hz   0.12Hz       1953.125Hz 
  */

  ShortCircuit(0);                      /* make sure probes are not shorted */
  LCD_Clear();
  Display_EEString_Space(SquareWave_str);    /* display: Square Wave */
  #ifndef HW_FIXED_SIGNAL_OUTPUT
  ProbePinout(PROBES_PWM);              /* show probes used */
  #endif

  #ifndef HW_FIXED_SIGNAL_OUTPUT
  /* probes 1 and 3 are signal ground, probe 2 is signal output */
  ADC_PORT = 0;                         /* pull down directly: */
  ADC_DDR = (1 << TP1) | (1 << TP3);    /* probe 1 & 3 */
  R_DDR = (1 << R_RL_2);                /* enable Rl for probe 2 */
  R_PORT = 0;                           /* pull down probe 2 initially */
  #endif

  #ifdef HW_FIXED_SIGNAL_OUTPUT
  /* dedicated output via OC1B */
  SIGNAL_PORT &= ~(1 << SIGNAL_OUT);    /* low by default */
  SIGNAL_DDR |= (1 << SIGNAL_OUT);      /* enable output */
  #endif


  /*
   *  set up Timer1 for PWM with 50% duty cycle 
   *  - fast PWM mode 
   *  - top value by OCR1A
   *  - OC1B non-inverted output
   */

  /* enable OC1B pin and set timer mode */
  TCCR1A = (1 << WGM11) | (1 << WGM10) | (1 << COM1B1) | (1 << COM1B0);
  TCCR1B = (1 << WGM13) | (1 << WGM12);


  /*
   *  processing loop
   */

  /* set values for default frequency: 1kHz */
  Index = 0;                       /* prescaler 1/1 */
  Prescaler = 1;                   /* prescaler 1/1 */
  Bitmask = (1 << CS10);           /* bitmask for prescaler 1 */
  Top = (CPU_FREQ / 1000) - 1;     /* top = f_MCU / (prescaler * f_PWM) - 1 */

  while (Flag > 0)
  {
    /*
     *  auto-ranging
     */

    Test = Index;

    /* check if we should change the range */
    if (Top > 0x7FFF)              /* more than 15 bits */
    {
      if (Index < (NUM_TIMER1 - 1))     /* don't exceed upper prescaler limit */
      {
        Index++;                   /* increase prescaler */
      }
    }
    else if (Top < 0x0FFF)         /* less than 12 bits */
    {
      if (Index > 0)               /* don't exceed lower prescaler limit */
      {
        Index--;                   /* decrease prescaler */
      }
    }

    /* process changed range */
    if (Index != Test)             /* range has changed */
    {
      Step = Prescaler;            /* save old value */

      /* read new prescaler and bitmask from table */
      Prescaler = eeprom_read_word(&T1_Prescaler_table[Index]);
      Bitmask = eeprom_read_byte(&T1_Bitmask_table[Index]);

      /* adjust top value for changed prescaler */
      if (Index > Test)            /* larger prescaler */
      {
        /* decrease top value by same factor as the prescaler increased */
        Temp = Prescaler / Step;
        Top /= Temp;
      }
      else                         /* smaller prescaler */
      {
        /* increase top value by same factor as the prescaler decreased */
        Temp = Step / Prescaler;
        Top *= Temp;  
      }
    }


    /*
     *  change timer settings
     */

    /* update timer */
    TCCR1B = (1 << WGM13) | (1 << WGM12);    /* stop timer */
    TCNT1 = 0;                               /* reset counter */
    OCR1B = Top / 2;                         /* 50% duty cycle */
    OCR1A = Top;                             /* top value for frequency */
    TCCR1B = (1 << WGM13) | (1 << WGM12) | Bitmask;    /* (re)start timer */


    /*
     *  display frequency
     *  - f_PWM = f_MCU / (prescaler * (1 + top))
     */

    Value = CPU_FREQ * 100;        /* scale to 0.01Hz */
    Value /= Prescaler;
    Test = 2;                      /* 2 decimal places */

    /*
     *  optimize resolution of frequency without causing an overflow
     *  prescaler       :  1  8  64  256  1024
     *  decimal places  :  2  3   4    4     5
     */

    Temp = Prescaler;
    while (Temp >= 8)         /* loop through prescaler steps */
    {
      Value *= 10;            /* scale by factor 0.1 */
      Test++;                 /* one decimal place more */
      Temp /= 8;              /* next lower prescaler */
    }

    Value /= Top + 1;
    LCD_ClearLine2();
    Display_FullValue(Value, Test, 'H');
    Display_Char('z');                  /* add z for Hz */


    /*
     *  user feedback
     */

    Test = TestKey(0, CURSOR_NONE);     /* wait for user feedback */

    /* consider rotary encoder's turning velocity */
    Step = UI.KeyStep;             /* get velocity (1-7) */

    if (Step > 1)                  /* larger step */
    {
      /* increase step size based on turning velocity */

      /* step^4: 16 81 256 625 1296 2401 */
      Step *= Step;                /* ^2 */
      Step *= Step;                /* ^2 */
    }

    /* process user input */
    if (Test == KEY_RIGHT)         /* encoder: right turn */
    {
      /* increase frequency -> decrease top value */
      Temp = Top - Step;           /* take advantage of underflow */
      if ((Temp > Top) || (Temp < 0x003))    /* underflow */
      {
        Temp = 0x0003;             /* lower limit */
      }
      Top = Temp;                  /* set new value */
    }
    else if (Test == KEY_LEFT)     /* encoder: left turn */
    {
      /* decrease frequency -> increase top value */
      Temp = Top + Step;           /* take advantage of overflow */
      if ((Temp < Top)  || (Temp == 0xFFFF))      /* overflow */
      {
        Temp = 0xFFFE;             /* upper limit */
      }
      Top = Temp;                  /* set new value */
    }
    else if (Test == KEY_SHORT)         /* short key press */
    {
      MilliSleep(50);              /* debounce button a little bit longer */
      Test = TestKey(200, CURSOR_NONE);   /* check for second key press */

      if (Test > KEY_TIMEOUT)      /* second key press */
      {
        Flag = 0;                  /* end loop */
      }
      /* else: todo: add some function here? */
    }
    else if (Test == KEY_LONG)          /* long key press */
    {
      /* set default frequency: 1kHz */
      Index = 0;                        /* prescaler 1/1 */
      Prescaler = 1;                    /* prescaler 1/1 */
      Bitmask = (1 << CS10);            /* bitmask for prescaler 1 */
      Top = (CPU_FREQ / 1000) - 1;      /* top = f_MCU / (prescaler * f) - 1 */
    }
  }


  /*
   *  clean up
   */

  TCCR1B = 0;                 /* disable timer */
  TCCR1A = 0;                 /* reset flags (also frees PB2) */

  #ifndef HW_FIXED_SIGNAL_OUTPUT
  R_DDR = 0;                  /* set HiZ mode */
  #endif

  #ifdef HW_FIXED_SIGNAL_OUTPUT
  SIGNAL_DDR &= ~(1 << SIGNAL_OUT);     /* set HiZ mode */
  #endif
}

#endif



/* ************************************************************************
 *   ESR tool
 * ************************************************************************ */


#if defined (SW_ESR) || defined (SW_OLD_ESR)

/*
 *  ESR tool
 *  - uses probe #1 (pos) and probe #3 (neg) 
 */

void ESR_Tool(void)
{
  uint8_t           Run = 1;       /* control flag */
  uint8_t           Test;          /* temp. value */
  Capacitor_Type    *Cap;          /* pointer to cap */
  uint16_t          ESR;           /* ESR (in 0.01 Ohms) */

  Check.Diodes = 0;                /* disable diode check in cap measurement */
  Cap = &Caps[0];                  /* pointer to first cap */

  #ifdef HW_DISCHARGE_RELAY
  ADC_DDR = (1 << TP_REF);         /* short circuit probes */
  #endif

  /* show tool info */
  LCD_Clear();
  Display_EEString(ESR_str);       /* display: ESR */
  ProbePinout(PROBES_ESR);         /* show probes used */
  Display_Char('-');               /* display "no value" */

  while (Run > 0)
  {
    /*
     *  short or long key press -> measure
     *  two short key presses -> exit tool
     */

    Test = TestKey(0, CURSOR_BLINK);    /* wait for user feedback */
    if (Test == KEY_SHORT)              /* short key press */
    {
      MilliSleep(50);                   /* debounce button a little bit longer */
      Test = TestKey(200, CURSOR_NONE); /* check for second key press */
      if (Test > KEY_TIMEOUT)           /* second key press */
      {
        Run = 0;                        /* end loop */
      }
    }

    /* measure cap */
    if (Run > 0)                        /* key pressed */
    {
      #ifdef HW_DISCHARGE_RELAY
      ADC_DDR = 0;                      /* remove short circuit */
      #endif

      LCD_ClearLine2();                 /* update line #2 */
      Display_EEString(Probing_str);    /* display: probing... */
      MeasureCap(PROBE_1, PROBE_3, 0);  /* probe-1 = Vcc, probe-3 = Gnd */
      LCD_ClearLine2();                 /* update line #2 */
      
      if (Check.Found == COMP_CAPACITOR)     /* found capacitor */
      {
        /* show capacitance */
        Display_Value(Cap->Value, Cap->Scale, 'F');

        /* show ESR */
        Display_Space();
        ESR = MeasureESR(Cap);
        if (ESR < UINT16_MAX)           /* got valid ESR */
        {
          Display_Value(ESR, -2, LCD_CHAR_OMEGA);
        }
        else                            /* no ESR */
        {
          Display_Char('-');
        }
      }
      else                                   /* no capacitor */
      {
        Display_Char('-');
      }

      #ifdef HW_DISCHARGE_RELAY
      ADC_DDR = (1 << TP_REF);          /* short circuit probes */
      #endif
    }
  }

  #ifdef HW_DISCHARGE_RELAY
  ADC_DDR = 0;                     /* remove short circuit */
  #endif
}

#endif



/* ************************************************************************
 *   Zener tool
 * ************************************************************************ */


#ifdef HW_ZENER

/*
 *  Zener tool:
 *  - Zener diode voltage measurement hardware option
 *  - use dedicated analog input (TP_ZENER) with 10:1 voltage divider
 */

void Zener_Tool(void)
{
  uint8_t                Run = 1;            /* control flag */
  uint8_t                Counter;            /* length of key press */
  uint8_t                Counter2 = 0;       /* time between two key presses */
  uint16_t               Value;              /* current value */
  uint16_t               Min = UINT16_MAX;   /* minimal value */

  /* show info */
  LCD_Clear();
  Display_EEString(Zener_str);     /* display: Zener */
  Display_NextLine();
  Display_Char('-');               /* display "no value" */

  while (Run > 0)             /* processing loop */
  {
    Counter = 0;              /* reset key press time */
    MilliSleep(30);           /* delay */
    Counter2++;               /* increase delay time */

    /*
     *  key press triggers measurement
     *  - also enables boost converter via hardware
     *  two short key presses exit tool
     */

    while (!(CONTROL_PIN & (1 << TEST_BUTTON)))   /* as long as key is pressed */
    {
      /* get voltage (10:1 voltage divider) */
      Value = ReadU(TP_ZENER);     /* special probe pin */
      Value /= 10;                 /* scale to 0.1V */

      /* display voltage */
      if (Counter % 8 == 0)        /* every 8 loop runs (240ms) */
      {
        LCD_ClearLine2();        
        Display_Value(Value, -1, 'V');
      }

      /* data hold */
      if (Counter == 0)            /* first loop run */
      {
        Min = UINT16_MAX;          /* reset to default */
      }
      else if (Counter >= 10)      /* ensure stable DC */
      {
        if (Value < Min) Min = Value;   /* update minimum */
      }

      /* timer */
      MilliSleep(30);                        /* delay next run / also debounce */
      Counter++;                             /* increaye key press time */
      if (Counter > 240) Counter = 201;      /* prevent overflow */
    }


    /*
     *  user interface logic
     */

    if (Counter > 0)                         /* key was pressed */
    {
      /* detect two quick key presses */
      if (Run == 2)                          /* flag for short key press set */
      {
        if (Counter2 <= 8)                   /* short delay between key presses <= 250ms */
        {
          Run = 0;                           /* end loop */
        }
        else                                 /* long delay between key presses */
        {
          Run = 1;                           /* reset flag */
        }
      }
      else                                   /* flag not set */
      {
        if (Counter <= 10)                   /* short key press <= 300ms */
        {
          Run = 2;                           /* set flag */
        }
      }

      /* display hold value */
      LCD_ClearLine2();

      if (Min != UINT16_MAX)       /* got updated value */
      {
        Display_Value(Min, -1, 'V');    /* display minimum */
        Display_Space();
        Display_EEString(Min_str);      /* display: Min */
      }
      else                         /* unchanged default */
      {
        Display_Char('-');              /* display "no value" */
      }

      Counter2 = 0;           /* reset delay time */
    }
  }
}

#endif



/* ************************************************************************
 *   Frequency counter
 * ************************************************************************ */


#ifdef HW_FREQ_COUNTER_BASIC

/*
 *  basic frequency counter
 *  - frequency input: T0
 *  - requires idle sleep mode to keep timers running when MCU is sleeping
 */

void FrequencyCounter(void)
{
  uint8_t           Flag;               /* loop control flag */
  uint8_t           Old_DDR;            /* old DDR state */
  uint8_t           Index;              /* prescaler table index */
  uint8_t           Bitmask;            /* prescaler bitmask */
  uint16_t          GateTime;           /* gate time in ms */
  uint16_t          Top;                /* top value for timer */
  uint32_t          Value;              /* temporary value */

  /* control flags */
  #define EXIT_ALL       0         /* end loop(s) and exit */
  #define PROCEED        1         /* proceed with next step */
  #define WAIT_LOOP      2         /* enter/run waiting loop */
  #define SHOW_FREQ      3         /* display frequency */

  /* show info */
  LCD_Clear();                          /* clear display */
  Display_EEString(FreqCounter_str);    /* display: Freq. Counter */


  /*
   *  We use Timer1 for the gate time and Timer0 to count pulses of the
   *  unknown signal. Max. frequency for Timer0 is 1/4 of the MCU clock.
   */

  Flag = PROCEED;             /* enter measurement loop */

  /*
      auto ranging

      Timer1 top value (gate time)
      - top = gatetime * MCU_cycles / prescaler 
      - gate time in µs
      - MCU cycles per µs
      - top max. 2^16 - 1

      Frequency
      - f = pulses / gatetime
      - pulses = f * gatetime

      range         gate time  prescaler  MCU clock  pulses      
      ----------------------------------------------------------
      -10kHz           1000ms       1024  > 16MHz    -10k
                       1000ms        256  <= 16MHz   -10k      
      10kHz-100kHz      100ms         64  all        1k-10k
      100kHz-            10ms          8  all        1k-(50k)
   */

  /* start values for autoranging (assuming high frequency) */
  GateTime = 10;                   /* gate time 10ms */
  Index = 1;                       /* prescaler table index (prescaler 8:1) */

  /* set up Timer0 (pulse counter) */
  TCCR0A = 0;                      /* normal mode (count up) */
  TIFR0 = (1 << TOV0);             /* clear overflow flag */
  TIMSK0 = (1 << TOIE0);           /* enable overflow interrupt */

  /* set up Timer1 (gate time) */
  TCCR1A = 0;                      /* normal mode (count up) */
  TIFR1 = (1 << OCF1A);            /* clear output compare A match flag */
  TIMSK1 = (1 << OCIE1A);          /* enable output compare A match interrupt */


  /*
   *  measurement loop
   */

  while (Flag > EXIT_ALL)
  {
    /* set up T0 as input (pin might be shared with display) */
    Old_DDR = COUNTER_DDR;              /* save current settings */
    COUNTER_DDR &= ~(1 << COUNTER_IN);  /* signal input */
    wait500us();                        /* settle time */

    /* update prescaler */
    Top = eeprom_read_word(&T1_Prescaler_table[Index]);     /* prescaler value */
    Bitmask = eeprom_read_byte(&T1_Bitmask_table[Index]);   /* prescaler bits */

    /* calculate compare value for Timer1 (gate time) */
    /* top = gatetime * MCU_cycles / timer prescaler */
    Value = GateTime;                   /* gatetime (in ms) */
    /* * MCU cycles per µs and scale gatetime to µs */
    Value *= (MCU_CYCLES_PER_US * 1000);
    Value /= Top;                       /* divide by timer prescaler */
    Top = (uint16_t)Value;              /* use lower 16 bit */

    /* start timers */
    FreqPulses = 0;                     /* reset pulse counter */
    Flag = WAIT_LOOP;                   /* enter waiting loop */
    TCNT0 = 0;                          /* Timer0: reset pulse counter */
    TCNT1 = 0;                          /* Timer1: reset gate time counter */
    OCR1A = Top;                        /* Timer1: set gate time */
    TCCR1B = Bitmask;                   /* start Timer1, prescaler */
    TCCR0B = (1 << CS02) | (1 << CS01); /* start Timer0, clock source: T0 falling edge */

    /* wait for timer1 or key press */
    while (Flag == WAIT_LOOP)
    {
      if (TCCR1B == 0)                  /* Timer1 stopped by ISR */
      {
        Flag = PROCEED;                 /* end loop and signal Timer1 event */
      }
      else                              /* Timer1 still running */
      {
        /* check for key press */
        while (!(CONTROL_PIN & (1 << TEST_BUTTON)))    /* pressed */
        {
          MilliSleep(50);          /* take a nap */
          Flag = EXIT_ALL;         /* end all loops */
        }

        if (Flag > EXIT_ALL)       /* smooth UI */
        {
          MilliSleep(100);         /* sleep for 100ms */
        }
      }
    }

    /* T0 pin might be shared with display */
    COUNTER_DDR = Old_DDR;              /* restore old settings */


    /*
     *  process measurement
     */

    if (Flag == PROCEED)                /* got measurement */
    {
      FreqPulses += TCNT0;              /* add counter of Timer0 */

      /*
       *  calculate frequency
       *  - f = pulses / gatetime
       *  - 20MHz MCU: 5M pulses per second at maximum
       *    with 10ms gate time max. 50k pulses
       */

      Value = FreqPulses;               /* number of pulses */
      Value *= 1000;                    /* scale to ms */
      Value /= GateTime;                /* divide by gatetime (in ms) */
      Flag = SHOW_FREQ;                 /* display frequency */

      /* autoranging */
      if (FreqPulses > 10000)           /* range overrun */
      {
        if (GateTime > 10)              /* upper range limit not reached yet */
        {
          GateTime /= 10;               /* 1000ms -> 100ms -> 10ms */
          Index--;                      /* one prescaler step down */
          #if CPU_FREQ > 16000000 
          if (Index == 3) Index--;      /* skip 256, use 64 */
          #endif
          Flag = PROCEED;               /* don't display frequency */
        }
      }
      else if (FreqPulses < 1000)       /* range underrun */
      {
        if (GateTime < 1000)            /* lower range limit not reached yet */
        {
          GateTime *= 10;               /* 1ms -> 10ms -> 100ms -> 1000ms */
          Index++;                      /* one prescaler step up */
          #if CPU_FREQ > 16000000 
          if (Index == 3) Index++;      /* skip 256, use 1024 */
          #endif
          Flag = PROCEED;               /* don't display frequency */
        }
      }
    }


    /*
     *  display frequency (in line #2)
     */

    LCD_ClearLine2();                   /* clear line #2 */
    Display_Char('f');                  /* display: f */
    Display_Space();

    if (Flag == SHOW_FREQ)              /* valid frequency */
    {
      Display_Value(Value, 0, 'H');     /* display frequency */
      Display_Char('z');                /* append "z" for Hz */
      Flag = 1;                         /* clear flag */
    }
    else                                /* invalid frequency */
    {
      Display_Char('-');                /* display: no value */
    }    
  }


  /*
   *  clean up
   */

  TIMSK0 = 0;                 /* disable all interrupts for Timer0 */
  TIMSK1 = 0;                 /* disable all interrupts for Timer1 */

  /* local constants */
  #undef EXIT_ALL
  #undef PROCEED
  #undef WAIT_LOOP
  #undef SHOW_FREQ
}



/*
 *  ISR for overflow of Timer0
 *  - catch overflows of pulse counter 
 */

ISR(TIMER0_OVF_vect, ISR_BLOCK)
{
  /*
   *  hints:
   *  - the TOV0 interrupt flag is cleared automatically
   *  - interrupt processing is disabled while this ISR runs
   *    (no nested interrupts)
   */

  FreqPulses += 256;          /* add overflow to global counter */
}



/*
 *  ISR for match of Timer1's OCR1A (Output Compare Register A)
 *  - for frequency counter gate time
 */

ISR(TIMER1_COMPA_vect, ISR_BLOCK)
{
  /*
   *  hints:
   *  - the OCF1A interrupt flag is cleared automatically
   *  - interrupt processing is disabled while this ISR runs
   *    (no nested interrupts)
   */

  /* gate time has passed */
  TCCR1B = 0;                 /* disable Timer1 */
  TCCR0B = 0;                 /* disable Timer0 */
}

#endif



#ifdef HW_FREQ_COUNTER_EXT

/*
 *  extended frequency counter
 *  - frequency input: T0 
 *  - control signals
 *    prescaler       - COUNTER_CTRL_DIV
 *    channel addr #0 - COUNTER_CTRL_CH0
 *    channel addr #1 - COUNTER_CTRL_CH1
 *  - prescaler
 *    0 - 1:1
 *    1 - 16:1 (or 32:1)
 *  - source channel address
 *    00 - buffered frequency input
 *    01 - unused
 *    10 - HF crystal oscillator
 *    11 - LF crystal oscillator
 *  - requires idle sleep mode to keep timers running when MCU is sleeping
 */

void FrequencyCounter(void)
{
  uint8_t           Flag;               /* loop control flag */
  uint8_t           Test = 0;           /* user feedback */
  uint8_t           InDir;              /* input DDR state */
  uint8_t           CtrlDir;            /* control DDR state */
  uint8_t           Channel;            /* input channel */
  uint8_t           Range;              /* range ID */
  uint8_t           Div = 0;            /* frequency prescaler */
  uint8_t           Index;              /* prescaler table index */
  uint8_t           Bitmask = 0;        /* prescaler bitmask */
  uint16_t          GateTime = 0;       /* gate time in ms */
  uint16_t          Top = 0;            /* top value for timer */
  unsigned char     *String = NULL;     /* string pointer (EEPROM) */
  uint32_t          MinPulses = 0;      /* minimim pulses for range */
  uint32_t          MaxPulses = 0;      /* maximum pulses for range */
  uint32_t          Value;              /* temporary value */

  /* control flags */
  #define RUN_FLAG            0b00000001     /* run flag */
  #define WAIT_FLAG           0b00000010     /* wait flag */
  #define GATE_FLAG           0b00000100     /* gatetime flag */
  #define UPDATE_CHANNEL      0b00001000     /* update source channel */
  #define UPDATE_RANGE        0b00010000     /* update measurement range */
  #define SHOW_FREQ           0b00100000     /* display frequency */

  /* show info */
  LCD_Clear();                          /* clear display */
  Display_EEString(FreqCounter_str);    /* display: Freq. Counter */


  /*
   *  We use Timer1 for the gate time and Timer0 to count pulses of the
   *  unknown signal. Max. frequency for Timer0 is 1/4 of the MCU clock.
   */

  /*
      auto ranging

      Timer1 top value (gate time)
      - top = gatetime * MCU_cycles / timer prescaler
      - gate time in µs
      - MCU cycles per µs
      - top max. 2^16 - 1

      Frequency
      - f = f-prescaler * pulses / gatetime
      - pulses = f * gatetime / f-prescaler

                    gate    timer      MCU       frequency
      range         time    prescaler  clock     prescaler  pulses     
      ----------------------------------------------------------------
      n/a           3000ms       1024  all
      -100kHz       1000ms       1024  > 16MHz         1:1  -100k
                    1000ms        256  <= 16MHz        1:1  -100k
      100kHz-1MHz    100ms         64  all             1:1  10k-100k
      1MHz-          100ms         64  all            16:1  6250-(500k)
                     100ms         64  all            32:1  3125-(500k)
   */

  /* set up control lines */
  CtrlDir = COUNTER_CTRL_DDR;      /* get current direction */
  /* set to output mode */
  COUNTER_CTRL_DDR |= (1 << COUNTER_CTRL_DIV) | (1 << COUNTER_CTRL_CH0) | (1 << COUNTER_CTRL_CH1);

  /* set up Timer0 (pulse counter) */
  TCCR0A = 0;                      /* normal mode (count up) */
  TIFR0 = (1 << TOV0);             /* clear overflow flag */
  TIMSK0 = (1 << TOIE0);           /* enable overflow interrupt */

  /* set up Timer1 (gate time) */
  TCCR1A = 0;                      /* normal mode (count up) */
  TIFR1 = (1 << OCF1A);            /* clear output compare A match flag */
  TIMSK1 = (1 << OCIE1A);          /* enable output compare A match interrupt */

  /* set start values */
  Channel = 0;                     /* source channel: ext. frequency */
  Range = 2;                       /* start with highest range */
  Flag = RUN_FLAG | UPDATE_CHANNEL | UPDATE_RANGE;     /* set control flags */


  /*
   *  processing loop
   */

  while (Flag > 0)
  {
    /*
     *  settings for ranges
     */

    if (Flag & UPDATE_RANGE)       /* update range settings */
    {
      switch (Range)               /* get range specifics */
      {
        case 0:     /* -100kHz */
          Div = 1;                 /* frequency prescaler 1:1 */
          #if CPU_FREQ <= 16000000
          Index = 3;               /* table index 3: 256:1 */
          #else
          Index = 4;               /* table index 4: 1024:1 */
          #endif
          GateTime = 1000;         /* gate time: 1000ms */
          MinPulses = 0;           /* lower limit: none */
          MaxPulses = 100000;      /* upper limit: 100k */
          break;

        case 1:     /* 100kHz-1MHz */
          Div = 1;                 /* frequency prescaler 1:1 */
          Index = 2;               /* table index 2: 64:1 */
          GateTime = 100;          /* gate time: 100ms */
          MinPulses = 10000;       /* lower limit: 10k */
          MaxPulses = 100000;      /* upper limit: 100k */
          break;

        case 2:     /* 1MHz- */
          Div = FREQ_COUNTER_PRESCALER; /* frequency prescaler 16:1 or 32:1 */
          Index = 2;               /* table index 2: 64:1 */
          GateTime = 100;          /* gate time: 100ms */
          #if FREQ_COUNTER_PRESCALER == 16
          MinPulses = 6250;        /* lower limit: 6250 */
          #elif FREQ_COUNTER_PRESCALER == 32
          MinPulses = 3125;        /* lower limit: 3125 */
          #endif
          MaxPulses = 0;           /* upper limit: none */
          break;
      }

      /* update Timer1 prescaler */
      Top = eeprom_read_word(&T1_Prescaler_table[Index]);   /* prescaler value */
      Bitmask = eeprom_read_byte(&T1_Bitmask_table[Index]); /* prescaler bits */


      /* calculate compare value for Timer1 (gate time) */
      /* top = gatetime * MCU_cycles / timer prescaler */
      Value = GateTime;                 /* gatetime (in ms) */
      /* * MCU cycles per µs and scale gatetime to µs */
      Value *= (MCU_CYCLES_PER_US * 1000);
      Value /= Top;                     /* divide by timer prescaler */
      Top = (uint16_t)Value;            /* use lower 16 bit */


      /* update frequency counter prescaler */
      if (Div == FREQ_COUNTER_PRESCALER)     /* set 16:1 */
      {
        COUNTER_CTRL_PORT |= (1 << COUNTER_CTRL_DIV);       /* set bit */
      }
      else                                   /* set 1:1 */
      {
        COUNTER_CTRL_PORT &= ~(1 << COUNTER_CTRL_DIV);      /* clear bit */
      }

      Flag &= ~UPDATE_RANGE;            /* clear flag */
    }  


    /*
     *  set and display source channel
     */

    if (Flag & UPDATE_CHANNEL)          /* update channel settings */
    {
      switch (Channel)             /* get channel specifics */
      {
        case 0:     /* buffered frequency input */
          String = (unsigned char *)FreqInput_str;
          Test = 0;
          break;

        case 1:     /* high frequency crystal oscillator */
          String = (unsigned char *)HF_Crystal_str;
          Test = (1 << COUNTER_CTRL_CH1);
          break;

        case 2:     /* low frequency crystal oscillator */
          String = (unsigned char *)LF_Crystal_str;
          Test = (1 << COUNTER_CTRL_CH1) | (1 << COUNTER_CTRL_CH0);
          break;
      }

      /* set source channel */
      InDir = COUNTER_CTRL_PORT;        /* get current state */
      InDir &= ~((1 << COUNTER_CTRL_CH1) | (1 << COUNTER_CTRL_CH0));  /* clear channel lines */
      InDir |= Test;                    /* set channel lines */
      COUNTER_CTRL_PORT = InDir;        /* update port */

      /* display source channel (in line #3) */ 
      LCD_ClearLine(3);
      LCD_CharPos(1, 3);
      Display_EEString(CounterChannel_str);  /* display: "Ch" */
      Display_Space();
      Display_EEString(String);              /* display channel name */

      Flag &= ~UPDATE_CHANNEL;          /* clear flag */
    }


    /* set up T0 as input */
    InDir = COUNTER_DDR & (1 << COUNTER_IN);      /* get current direction */
    COUNTER_DDR &= ~(1 << COUNTER_IN);  /* set to input mode */
    wait500us();                        /* settle time */


    /* start timers */
    Flag |= WAIT_FLAG;                  /* enter waiting loop */
    FreqPulses = 0;                     /* reset pulse counter */
    TCNT0 = 0;                          /* Timer0: reset pulse counter */
    TCNT1 = 0;                          /* Timer1: reset gate time counter */
    OCR1A = Top;                        /* Timer1: set gate time */
    TCCR1B = Bitmask;                   /* start Timer1, prescaler */
    TCCR0B = (1 << CS02) | (1 << CS01); /* start Timer0, clock source: T0 falling edge */


    /*
     *  wait for timer1 or user feedback
     */

    while (Flag & WAIT_FLAG)
    {
      if (TCCR1B == 0)                  /* Timer1 stopped by ISR */
      {
        Flag = RUN_FLAG | GATE_FLAG;    /* end waiting loop and signal Timer1 event */
      }
      else                              /* Timer1 still running */
      {
        Test = TestKey(0, CURSOR_NONE);      /* check for user feedback */

        if (Test == KEY_SHORT)          /* short key press */
        {
          MilliSleep(50);               /* debounce button a little bit longer */
          Test = TestKey(200, CURSOR_NONE);  /* check for second key press */

          if (Test > KEY_TIMEOUT)       /* second key press */
          {
            Flag = 0;                   /* end processing loop */
          }
          else                          /* single key press */
          {
            /* select next source channel */
            if (Channel < 2) Channel++;           /* next channel */
            else Channel = 0;                     /* overrun */
            Flag = RUN_FLAG | UPDATE_CHANNEL;     /* update channel */
          }
        }
        #ifdef HW_KEYS
        else if (Test == KEY_RIGHT)     /* right key */
        {
          if (Channel < 2) Channel++;             /* next channel */
          else Channel = 0;                       /* overrun */
          Flag = RUN_FLAG | UPDATE_CHANNEL;       /* update channel */
        }
        else if (Test == KEY_LEFT)      /* left key */
        {
          if (Channel > 0) Channel--;             /* previous channel */
          else Channel = 2;                       /* underrun */
          Flag = RUN_FLAG | UPDATE_CHANNEL;       /* update channel */
        }
        #endif
      }
    }


    /*
     *  process measurement
     */

    if (InDir)                     /* restore old setting for T0 */
    {
      COUNTER_DDR |= (1 << COUNTER_IN);      /* set to output mode */
    }

    Cfg.OP_Control &= ~OP_BREAK_KEY;    /* clear break signal (just in case) */

    if (Flag & GATE_FLAG)               /* got measurement */
    {
      FreqPulses += TCNT0;              /* add counter of Timer0 */

      /* autoranging */
      if (FreqPulses < MinPulses)       /* range underrun */
      {
        if (Range > 0)                  /* not lowest range yet */
        {
          Range--;                      /* change to lower range */
          Flag |= UPDATE_RANGE;         /* set flag for updating range */
        }
      }
      else if (FreqPulses > MaxPulses)  /* range overrun */
      {
        if (Range < 2)                  /* not highest range yet */
        {
          Range++;                      /* change to higher range */
          Flag |= UPDATE_RANGE;         /* set flag for updating range */
        }
      }

      /* show frequency only when not switching ranges */
      if (! (Flag & UPDATE_RANGE)) Flag |= SHOW_FREQ;


      /*
       *  calculate frequency
       *  - f = pulses * f-prescaler / gatetime
       *  - 20MHz MCU: 5M pulses per second at maximum
       *    with 100ms gate time max. 500k pulses
       */

      if (Div == 1)                     /* f-prescaler 1:1 */
      {
        /* no overflow possible */
        FreqPulses *= 1000;             /* scale to ms */
        FreqPulses /= GateTime;         /* divide by gatetime (in ms) */
      }
      else                              /* f-prescaler 16:1 or 32:1 */
      {
        /* prevent overflow */
        FreqPulses *= 100;              /* scale to 10ms */
        FreqPulses *= Div;              /* * f-prescaler */
        FreqPulses /= (GateTime / 10);  /* divide by gatetime (in 10ms) */
      }

      Flag &= ~GATE_FLAG;          /* clear flag */
    }


    /*
     *  display frequency (in line #2)
     */

    LCD_ClearLine2();                 /* clear line #2 */
    Display_Char('f');                /* display: f */
    Display_Space();

    if (Flag & SHOW_FREQ)               /* valid frequency */
    {
      Test = 0;                         /* dot position */
      Index = 0;                        /* unit char */

      if (FreqPulses >= 1000000)        /* f >= 1MHz */
      {
        Test = 6;             /* 10^6 */
        Index = 'M';          /* M for mega */
      }
      else if (FreqPulses >= 1000)      /* f >= 1kHz */
      {
        Test = 3;             /* 10^3 */
        Index = 'k';          /* k for kilo */
      }

      Display_FullValue(FreqPulses, Test, Index);
      Display_EEString(Hertz_str);      /* display: "Hz" */

      Flag &= ~SHOW_FREQ;               /* clear flag */
    }
    else                                /* invalid frequency */
    {
      Display_Char('-');                /* display: no value */
    }
  }


  /*
   *  clean up
   */

  TIMSK0 = 0;                 /* disable all interrupts for Timer0 */
  TIMSK1 = 0;                 /* disable all interrupts for Timer1 */

  /* filter control lines which were in input mode */ 
  CtrlDir ^= (1 << COUNTER_CTRL_DIV) | (1 << COUNTER_CTRL_CH0) | (1 << COUNTER_CTRL_CH1);
  CtrlDir &= (1 << COUNTER_CTRL_DIV) | (1 << COUNTER_CTRL_CH0) | (1 << COUNTER_CTRL_CH1);
  COUNTER_CTRL_DDR &= ~CtrlDir;         /* set former direction */

  #undef SHOW_FREQ
  #undef UPDATE_RANGE
  #undef UPDATE_CHANNEL
  #undef GATE_FLAG
  #undef WAIT_FLAG
  #undef RUN_FLAG
}



/*
 *  ISR for overflow of Timer0
 *  - catch overflows of pulse counter 
 */

ISR(TIMER0_OVF_vect, ISR_BLOCK)
{
  /*
   *  hints:
   *  - the TOV0 interrupt flag is cleared automatically
   *  - interrupt processing is disabled while this ISR runs
   *    (no nested interrupts)
   */

  FreqPulses += 256;          /* add overflow to global counter */
}



/*
 *  ISR for match of Timer1's OCR1A (Output Compare Register A)
 *  - for frequency counter gate time
 */

ISR(TIMER1_COMPA_vect, ISR_BLOCK)
{
  /*
   *  hints:
   *  - the OCF1A interrupt flag is cleared automatically
   *  - interrupt processing is disabled while this ISR runs
   *    (no nested interrupts)
   */

  /* gate time has passed */
  TCCR1B = 0;                 /* disable Timer1 */
  TCCR0B = 0;                 /* disable Timer0 */

  Cfg.OP_Control |= OP_BREAK_KEY;       /* break TestKey() processing */
}

#endif



/* ************************************************************************
 *   rotary encoder check
 * ************************************************************************ */


#ifdef SW_ENCODER

/* local constants */
#define DIR_NONE         0b00000000     /* no turn or error */
#define DIR_RIGHT        0b00000001     /* turned to the right */
#define DIR_LEFT         0b00000010     /* turned to the left */


/*
 *  check rotary encoder
 *
 *  requires:
 *  - pointer to encoder history
 */

uint8_t CheckEncoder(uint8_t *History)
{
  uint8_t           Action = DIR_NONE;       /* return value */
  uint8_t           Old_AB;                  /* old AB state */
  uint8_t           AB = 0;                  /* new AB state */
  uint8_t           Dir;                     /* turning direction */
  uint8_t           Steps;                   /* encoder steps */
  uint8_t           Temp;                    /* temporary value */

  /* we assume: probe-1 = A / probe-2 = B / probe-3 = Common */
  /* set up probes: probe-1 -- Rl -- Vcc / probe-2 -- Rl -- Vcc / Gnd -- probe-3 */
  R_PORT = Probes.Rl_1 | Probes.Rl_2;   /* pullup via Rl */
  R_DDR =  Probes.Rl_1 | Probes.Rl_2;   /* enable pull-up resistors */
  ADC_PORT = 0;                         /* pull down directly */
  ADC_DDR = Probes.Pin_3;               /* enable Gnd for probe-3 */
  wait500us();                          /* settle time */

  /* get A & B signals */
  Temp = ADC_PIN;
  if (Temp & Probes.Pin_1) AB = 0b00000010;
  if (Temp & Probes.Pin_2) AB |= 0b00000001;

  R_DDR = 0;                  /* reset probes */
  ADC_DDR = 0;

  /* unpack history */
  Temp = *History;
  Old_AB = Temp & 0b00000011;      /* old AB state, first 2 bits */
  Temp >>=2 ;                      /* move 2 bits */
  Dir = Temp & 0b00000011;         /* direction, next 2 bits */
  Temp >>= 2;                      /* move 2 bits */
  Steps = Temp;                    /* steps, remaining 4 bits */

  /* update state history */
  if (Dir == (DIR_RIGHT | DIR_LEFT))    /* first scan */
  {
    Old_AB = AB;              /* set as last state */
    Dir = DIR_NONE;           /* reset direction */
  }

  /* process signals */
  if (Old_AB != AB)           /* signals changed */
  {
    /* check if only one bit has changed (Gray code) */
    Temp = AB ^ Old_AB;                 /* get bit difference */
    if (!(Temp & 0b00000001)) Temp >>= 1;
    if (Temp == 1)                      /* valid change */
    {
      /* determine direction */
      /* Gray code: 00 01 11 10 */
      Temp = 0b10001101;                /* expected values for a right turn */
      Temp >>= (Old_AB * 2);            /* get expected value by shifting */
      Temp &= 0b00000011;               /* select value */
      if (Temp == AB)                   /* value matches */
        Temp = DIR_RIGHT;               /* turn to the right */
      else                              /* value mismatches */
        Temp = DIR_LEFT;                /* turn to the left */

      /* detection logic */
      if (Temp == Dir)                  /* turn in same direction */
      {
        Steps++;                        /* got another step */

        /* for proper detection we need 4 Gray code steps */
        if (Steps == 4)                 /* got 4 steps */
        {
          LCD_ClearLine2();

          /*
           *  The turning direction determines A and B:
           *  - right: A = Probe #1 / B = Probe #2
           *  - left:  A = Probe #2 / B = Probe #1
           */

          if (Dir == DIR_RIGHT)         /* right */
          {
            Semi.A = Probes.ID_1;
            Semi.B = Probes.ID_2;
          }
          else                          /* left */
          {
            Semi.A = Probes.ID_2;
            Semi.B = Probes.ID_1;
          }

          Semi.C = Probes.ID_3;         /* Common */

          /* display pinout */
          Show_SemiPinout('A', 'B', 'C');

          Steps = 0;                      /* reset steps */
          Action = Temp;                  /* signal valid step */
        }
      }
      else                         /* turn has changed direction */
      {
        Steps = 1;                 /* first step for new direction */
      }

      Dir = Temp;                  /* update direction */
    }
    else                                /* invalid change */
    {
      Dir = DIR_RIGHT | DIR_LEFT;       /* trigger reset of history */
    }
  }

  /* pack new history */
  Temp = AB;             /* AB state, first 2 bits */
  Dir <<= 2;             /* direction, next 2 bits */
  Temp |= Dir;
  Steps <<= 4;           /* steps, remaining 4 bits */
  Temp |= Steps;
  *History = Temp;       /* save new history */

  return Action;
}


/*
 *  rotary encoder check
 *  - uses standard probes
 */

void Encoder_Tool(void)
{
  uint8_t      Flag;          /* flag/counter */
  uint8_t      History[3];    /* encoder history */

  /*
   *  History:
   *  - 000000xx AB state
   *  - 0000xx00 turning direction
   *  - xxxx0000 steps               
   */

  /* show info */
  LCD_Clear();
  Display_EEString(Encoder_str);   /* display: Rotary Encoder */

  /* init array */
  for (Flag = 0; Flag <= 2; Flag++)
  {
    History[Flag] = (DIR_RIGHT | DIR_LEFT) << 2;
  }

  /* processing loop */
  Flag = 5;
  while (Flag < 10)
  {
    wdt_reset();

    if (Flag == 5)                 /* ask user to turn */
    {
      LCD_ClearLine2();
      Display_EEString(TurnRight_str);    /* display: Turn right! */
      Flag = 0;                           /* reset flag */
    }

    UpdateProbes(PROBE_1, PROBE_2, PROBE_3);      /* check first pinout */
    Flag = CheckEncoder(&History[0]);

    if (Flag == 0)
    {
      UpdateProbes(PROBE_1, PROBE_3, PROBE_2);    /* check second pinout */
      Flag = CheckEncoder(&History[1]);
    }

    if (Flag == 0)
    {    
      UpdateProbes(PROBE_2, PROBE_3, PROBE_1);    /* check third pinout */
      Flag = CheckEncoder(&History[2]);
    }

    if (Flag > 0)             /* detected encoder */
    {
      TestKey(3000, CURSOR_STEADY | UI_OP_MODE);  /* let the user read */
      Flag = 5;                    /* reset flag */
    }
    else                      /* nothing found yet */
    {
      if (!(CONTROL_PIN & (1 << TEST_BUTTON)))   /* if key is pressed */
      {
        MilliSleep(100);           /* smooth UI */
        Flag = 10;                 /* end loop */
      }
    }
  }
}

/* local constants */
#undef DIR_LEFT
#undef DIR_RIGHT
#undef DIR_NONE

#endif



/* ************************************************************************
 *   opto couplers
 * ************************************************************************ */


#ifdef SW_OPTO_COUPLER

/*
 *  check for LED
 *  - simple wrapper for CheckDiode()
 *
 *  requires:
 *  - Probe1: ID of positive probe (anode)
 *  - Probe2: ID of negative probe (cathode)
 */

void Check_LED(uint8_t Probe1, uint8_t Probe2)
{
  uint8_t           Probe3;             /* ID of probe #3 */
  uint16_t          U1;                 /* voltage */

  /* update all three probes */
  Probe3 = GetThirdProbe(Probe1, Probe2);    /* get third one */
  UpdateProbes(Probe1, Probe2, Probe3);      /* update probes */

  /* we assume: probe-1 = A / probe2 = C */
  /* set probes: Gnd -- Rl -- probe-2 / probe-1 -- Vcc */
  R_PORT = 0;                      /* set resistor port to Gnd */
  R_DDR = Probes.Rl_2;             /* pull down probe-2 via Rl */
  ADC_DDR = Probes.Pin_1;          /* set probe-1 to output */
  ADC_PORT = Probes.Pin_1;         /* pull-up probe-1 directly */

  U1 = ReadU_5ms(Probes.ADC_2);    /* voltage at Rl (cathode) */

  if (U1 >= 977)         /*  not just a leakage current (> 1.4mA) */
  {
    CheckDiode();        /* run standard diode check */
  }
}



/*
 *  check opto couplers
 *  - uses standard probes
 *  - pins which have to be connected (common Gnd):
 *    - LED's cathode and BJT's emitter 
 *    - LED's cathode and TRIAC's MT2
 *  - supports:
 *    - BJT
 *    - Triac (with and without zero crossing circuit)
 */

void OptoCoupler_Tool(void)
{
  uint8_t           Run = 1;            /* loop control */
  uint8_t           Test;               /* user input */
  uint16_t          U1, U2;             /* voltages */
  uint16_t          U3, U4;             /* voltages */
  uint32_t          CTR = 0;            /* CTR in % */

  /* init */
  /* next-line mode: keep first line and wait for key/timeout */
  UI.LineMode = LINE_KEEP | LINE_KEY;

  /* display info */
  LCD_Clear();
  Display_EEString(OptoCoupler_str);    /* display: Opto Coupler */
  Display_NL_EEString(Start_str);       /* display: Start */

  while (Run)
  {
    /* user input */
    Test = TestKey(0, CURSOR_BLINK);    /* get user input */

    if (Test == KEY_SHORT)         /* short key press */
    {
      /* a second key press ends tool */
      MilliSleep(50);
      Test = TestKey(300, CURSOR_NONE);
      if (Test > KEY_TIMEOUT) Run = 0;  /* end loop */
    }

    if (Run)                       /* check opto coupler */
    {
      LCD_Clear();
      #ifdef UI_SERIAL_COPY
      SerialCopy_On();             /* enable serial output & NL */
      #endif
      Display_EEString(OptoCoupler_str);     /* display: Opto Coupler */
      Display_NextLine();
      Test = 0;

      /*
       *  scan for LED
       */

      Check.Found = COMP_NONE;          /* reset component search */
      Check.Diodes = 0;                 /* reset number of diodes */

      /* check all possible probe combinations */
      Check_LED(PROBE_1, PROBE_2);
      Check_LED(PROBE_2, PROBE_1);
      Check_LED(PROBE_1, PROBE_3);
      Check_LED(PROBE_3, PROBE_1);
      Check_LED(PROBE_2, PROBE_3);
      Check_LED(PROBE_3, PROBE_2);

      if (Check.Diodes == 1)       /* got one */
      {
        /* update all three probes for remaining checks */
        Test = GetThirdProbe(Diodes[0].A, Diodes[0].C);  /* get third probe */
        UpdateProbes(Diodes[0].A, Diodes[0].C, Test);    /* update probes */

        Test = 50;                      /* proceed with other checks */
      }


      /*
       *  we assume:
       *  probe-1 = LED's anode
       *  probe-2 = LED's cathode & BJT's emitter or TRIAC's MT2
       *  probe-3 = BJT's collector or TRIAC's MT1
       */


      /*
       *  check for BJT and TRIAC
       *  - BJT conducts only while LED is lit.
       *  - TRIAC keeps conducting as long as load current flows.
       *    Some types with zero crossing circuit got an inhibit voltage
       *    of about 5V.
       */

      if (Test == 50)
      {
        /* set probes: probe-2 -- Gnd / probe-3 -- Rl -- Vcc */
        ADC_DDR = Probes.Pin_2;              /* set probe-2 to output */
        ADC_PORT = 0;                        /* pull down probe-2 directly */
        R_DDR = Probes.Rl_1 | Probes.Rl_3;   /* select Rl for probe-1 & Rl for probe-3 */
        R_PORT = Probes.Rl_3;                /* pull up collector via Rl */
        U1 = ReadU_5ms(Probes.ADC_3);        /* voltage at collector when LED is off */

        /* make sure we have no conduction without the LED lit */
        if (U1 > 4000)        /* allow a leakage current of 1.5mA */
        {
          /* simulate zero crossing in case of a TRIAC with zero crossing circuit */
          R_PORT = Probes.Rl_1;                /* turn on LED */
          wait1ms();                           /* wait a tad */
          R_PORT = Probes.Rl_1 | Probes.Rl_3;  /* also pull up collector via Rl */
          U1 = ReadU_5ms(Probes.ADC_3);        /* voltage at collector when LED is on */

          R_PORT = Probes.Rl_3;                /* turn off LED */
          U2 = ReadU_5ms(Probes.ADC_3);        /* voltage at collector when LED is off */

          /* we should have conduction when the LED is lit */
          if (U1 <= 4000)          /* more than 1.5mA */
          {
            if (U2 >= 4000)        /* no conduction, allow some leakage current */
            {
              Test = 100;          /* BJT type */
            }
            else                   /* conduction */
            {
              /* check if both voltages are about the same */
              U3 = U1;
              U3 /= 8;             /* 12.5% */
              U4 = U1 - U3;        /* lower threshold */
              U3 += U1;            /* upper threshold */
              if ((U2 > U4) && (U2 < U3))
              {
                Test = 101;        /* TRIAC type */
              }
            }
          }
        }

        R_DDR = Probes.Rl_1;                 /* set probe-3 to HiZ */
      }


      /*
       *  measure CRT for BJT type
       */

      if (Test == 100)          /* got BJT type */
      {
        /* change probes: probe-3 -- Vcc */
        ADC_DDR = Probes.Pin_2 | Probes.Pin_3;    /* set probe-3 to output */
        ADC_PORT = Probes.Pin_3;                  /* pull up probe-3 directly */

        /* get voltages at current shunts */
        Cfg.Samples = 10;               /* just a few samples for 1ms runtime */
        R_PORT = Probes.Rl_1;           /* turn LED on */
        wait1ms();                      /* time for propagation delay */
        U1 = ReadU(Probes.ADC_1);       /* voltage at LED's anode (Rl) */
        U2 = ReadU(Probes.ADC_2);       /* voltage at emitter (RiL) */
        R_PORT = 0;                     /* turn LED off */
        Cfg.Samples = ADC_SAMPLES;      /* reset samples to default */

        /* calculate LED's If */
        /* If = (Vcc - U1) / (RiH + Rl) */
        U3 = Cfg.Vcc - U1;              /* Vcc - U1 (mV) */
        CTR = (uint32_t)U3;
        CTR *= 10000;                   /* scale to 0.0001 mV */
        U4 = NV.RiH + (R_LOW * 10);     /* RiH + Rl (0.1 Ohms) */
        CTR /= U4;                      /* If = U/R in µA */
        U3 = (uint16_t)CTR;             /* If in µA */

        /* calculate BJT's Ie */
        /* Ie = I_total - If = (U2 / RiL) - If */
        CTR = (uint32_t)U2;             /* U2 (mV) */
        CTR *= 10000;                   /* scale to 0.0001 mV */
        CTR /= NV.RiL;                  /* /RiL in 0.1 Ohms -> I_total (µA) */ 
        CTR -= U3;                      /* Ie = I_total - If (µA) */

        /* calculate CTR */
        /* CTR = Ie / If */
        CTR *= 100;                     /* scale up to % */
        CTR /= U3;                      /* Ie / If (%) */
      }


      /*
       *  Measure turn-on and turn-off times
       *  - Unfortunately we can't use the analog comparator in conjunction
       *    with Timer1, because the 1.1V bandgap reference would limit the
       *    time measurement to opto couplers with a CTR > 200%.
       */

      if (Test == 100)
      {
        U1 = UINT16_MAX;           /* reset value */
        U2 = UINT16_MAX;

        ADC_DDR = Probes.Pin_2;              /* set probe-2 to output */
        ADC_PORT = 0;                        /* pull down probe-2 directly */
        R_DDR = Probes.Rl_1 | Probes.Rl_3;   /* select Rl for probe-1 & Rl for probe-3 */
        R_PORT = Probes.Rl_3;                /* pull up collector via Rl */

        U1 = ReadU_5ms(Probes.ADC_3);        /* voltage at collector when LED is off */

        /* make sure we have no conduction without the LED lit */
        if (U1 > 4000)        /* allow a leakage current of 1.5mA */
        {
          Test = Probes.Pin_3;     /* port pin mask for probe-3 */

          /*
           *  turn-on delay
           */

          Run = 0;                                /* zero counter */
          R_PORT = Probes.Rl_1 | Probes.Rl_3;     /* turn on LED */

          /*
           *  wait for logic low level (<2.0V)
           *  - MCU cycles for full loop run: 7
           */

          while (ADC_PIN & Test)
          {
            Run++;                      /* increase counter */
            if (Run > 250) break;       /* check for overflow */
          }

          if (Run <= 250)          /* no overrun */
          {
            U1 = Run * 70;                   /* delay (0.1 MCU cycles) */
            U1 /= MCU_CYCLES_PER_US;         /* delay (0.1 µs) */
          }


          /*
           *  turn-off delay
           */

          Run = 0;                                /* zero counter */
          R_PORT = Probes.Rl_3;                   /* turn off LED */

          /*
           *  wait for logic high level (>2.5V)
           *  - MCU cycles for full loop run: 7
           */

          while (!(ADC_PIN & Test))
          {
            Run++;                      /* increase counter */
            if (Run > 250) break;       /* check for overflow */
          }

          if (Run <= 250)          /* no overrun */
          {
            U2 = Run * 70;                   /* delay (0.1 MCU cycles) */
            U2 /= MCU_CYCLES_PER_US;         /* delay (0.1 µs) */
          }

          Run = 1;            /* reset value */
          Test = 100;         /* reset value */
        }
      }


      /*
       *  display result
       */

      if (Test == 100)          /* got BJT type */
      {
        Display_EEString(BJT_str);      /* display: BJT */

        Display_NL_EEString_Space(CTR_str);       /* display: CTR */
        Display_Value(CTR, 0, '%');               /* display CTR */

        Display_NL_EEString_Space(If_str);        /* display: If */
        Display_Value(U3, -6, 'A');               /* display If */

        if (U1 < UINT16_MAX)       /* valid t_on */
        {
          Display_NL_EEString_Space(t_on_str);    /* display: t_on */
          if (U1 < 10)        /* < 1µs */
          {
            Display_Char('<');
            U1 = 10;          /* 1µs */
          }
          Display_Value(U1, -7, 's');
        }

        if (U2 < UINT16_MAX)       /* valid t_off */
        {
          Display_NL_EEString_Space(t_off_str);   /* display: t_off */
          if (U2 < 10)        /* < 1µs */
          {
            Display_Char('<');
            U2 = 10;          /* 1µs */
          }
          Display_Value(U2, -7, 's');
        }

        Display_NL_EEString_Space(Vf_str);        /* display: Vf */
        Display_Value(Diodes[0].V_f, -3, 'V');    /* display Vf */
      }
      else if (Test == 101)     /* got TRIAC type */
      {
        Display_EEString(Triac_str);    /* display: TRIAC */

        Display_NL_EEString_Space(Vf_str);        /* display: Vf */
        Display_Value(Diodes[0].V_f, -3, 'V');    /* display Vf */
      }
      else                      /* none found */
      {
        Display_EEString(None_str);     /* display: None */
      }

      #ifdef UI_SERIAL_COPY
      SerialCopy_Off();            /* disable serial output & NL */
      #endif
    }
  }
}

#endif



/* ************************************************************************
 *   OneWire
 * ************************************************************************ */


#ifdef SW_DS18B20

/*
 *  temperature sensor DS18B20
 */

void DS18B20_Tool(void)
{
  uint8_t           Flag;     /* control flag */
  uint8_t           Test;     /* key / feedback */
  uint8_t           Bits;     /* bit depth */
  int8_t            Scale;    /* temperature scale 10^x */
  int32_t           Value;    /* temperature value */
  #ifdef UI_FAHRENHEIT
  int32_t           Temp;     /* temporary value */
  #endif

  /* inform user about pinout and check for client */
  Flag = OneWire_Probes(DS18B20_str);
  LCD_ClearLine2();                     /* clear line #2 */
  Display_EEString(Start_str);          /* display: Start */


  /*
   *  processing loop
   */

  while (Flag)
  {
    /* user input */
    Test = TestKey(0, CURSOR_BLINK);    /* get user input */

    if (Test == KEY_SHORT)         /* short key press */
    {
      /* a second key press ends tool */
      MilliSleep(50);
      Test = TestKey(300, CURSOR_NONE);
      if (Test > KEY_TIMEOUT)           /* any key */
      {
        Flag = 0;                       /* end loop */
      }
    }

    LCD_ClearLine2();                   /* clear line #2 */

    if (Flag)            /* ok to proceed */
    {
      /* get temperature from DS18B20 (in °C) */
      Test = DS18B20_ReadTemperature(&Value, &Scale, &Bits);

      if (Test)                    /* got temperature */
      {
        /* Scale should be -1 to -4: 1-4 decimal places */
        Scale = -Scale;

        #ifdef UI_FAHRENHEIT
        /*
         *  convert °C to °F
         *  - T[°F] = T[°C] * 9/5 + 32
         */

        Value *= 9;
        Value /= 5;
        Temp = 32;            /* offset */

        /* scale offset to match temperature's scale */
        Test = Scale;         /* decimal places */
        while (Test > 0)
        {
          Temp *= 10;         /* scale by 10^1 */
          Test--;             /* next digit */
        }
        Value += Temp;        /* add scaled offset */
        #endif

        /* todo: add degree symbol to bitmap fonts */ 
        Display_SignedFullValue(Value, Scale, '°');

        #ifdef UI_FAHRENHEIT
          Display_Char('F');       /* display: F (Fahrenheit) */
        #else
          Display_Char('C');       /* display: C (Celsius) */
        #endif
      }
      else                         /* some error */
      {
        Display_Char('-');         /* display n/a */
      }
    }
  }
}

#endif



/* ************************************************************************
 *   capacitor leakage current
 * ************************************************************************ */


#ifdef SW_CAP_LEAKAGE

/*
 *  tool for measuring the leakage current of a capacitor
 *  - uses probe #1 (pos) and probe #3 (neg)
 *  - requires display with more than 2 lines
 */

void Cap_Leakage(void)
{
  uint8_t           Flag;               /* loop control flag */
  uint8_t           Test = 0;           /* user feedback */
  uint8_t           Mode;               /* mode */
  uint16_t          U1;                 /* voltage #1 */
  uint32_t          Value;              /* temp. value */

  /* control flags */
  #define RUN_FLAG            0b00000001     /* run flag */
  #define CHANGED_MODE        0b00000100     /* mode has changed */

  /* mode */
  #define MODE_NONE           0         /* no mode (show pinout) */
  #define MODE_HIGH           1         /* charge cap: high current */
  #define MODE_LOW            2         /* charge cap: low current */
  #define MODE_DISCHARGE      3         /* discharge cap */

  /* show info */
  LCD_Clear();                          /* clear display */
  Display_EEString(CapLeak_str);        /* display: cap leakage */

  /* set start values */
  Flag = RUN_FLAG | CHANGED_MODE;
  Mode = MODE_NONE;

  UpdateProbes(PROBE_1, 0, PROBE_3);    /* update bitmasks and probes */

  while (Flag > 0)       /* processing loop */
  {
    /*
     *  display mode and set probes
     */

    if (Flag & CHANGED_MODE)       /* mode has changed */
    {
      LCD_ClearLine2();            /* clear line #2 */

      switch (Mode)                /* based on mode */
      {
        case MODE_NONE:            /* display pinout */
          /* probe-1: Vcc / probe-3: Gnd */
          Show_SimplePinout('+', 0, '-');
          LCD_ClearLine(3);             /* clear line #3 */
          break;

        case MODE_HIGH:            /* charge cap with high current (Rl) */
          Display_EEString_Space(CapCharge_str);
          Display_EEString(CapHigh_str);

          /* set probes: probe-3 -- Rl -- Gnd / probe-1 -- Vcc */
          ADC_DDR = 0;                  /* set to HiZ */
          R_DDR = Probes.Rl_3;          /* select Rl for probe-3 */
          R_PORT = 0;                   /* pull down probe-3 via Rl */
          ADC_PORT = Probes.Pin_1;      /* pull up probe-1 directly */
          ADC_DDR = Probes.Pin_1;       /* enable pull-up of probe-1 */
          break;

        case MODE_LOW:             /* charge cap with low current (Rh) */
          /* max. charge current I = 5V/Rh = 10.6µA */
          Display_EEString_Space(CapCharge_str);
          Display_EEString(CapLow_str);

          /* set probes: probe-3 -- Rh -- Gnd / probe-1 -- Vcc */
          /* simply switch pull-down resistor Rl to Rh */
          R_DDR = Probes.Rh_3;          /* select Rh for probe-3 */
          break;

        case MODE_DISCHARGE:       /* discharge cap */
          Display_EEString(CapDischarge_str);
          /* set probes: probe-3 -- Gnd / probe-1 -- Rl -- Gnd */
          ADC_DDR = 0;                  /* set to HiZ */
          R_DDR = Probes.Rl_1;          /* select Rl for probe-1 */
          /* R_PORT set to 0 already: pull down probe-1 via Rl */
          ADC_DDR = Probes.Pin_3;       /* set probe-3 to output */
          ADC_PORT = 0;                 /* pull down probe-3 directly */
          break;
      }

      Flag &= ~CHANGED_MODE;       /* clear flag */
    }


    /*
     *  manage modes
     */

    if (Mode != MODE_NONE)
    {
      LCD_ClearLine(3);            /* clear line #3 */
      LCD_CharPos(1, 3);           /* move to line #3 */

      switch (Mode)                /* based on mode */
      {
        case MODE_HIGH:            /* charge cap with high current (Rl) */
          /* voltage across Rl and RiL at probe-3 */
          U1 = ReadU(Probes.ADC_3);          /* read voltage at probe-3 */

          /* calculate current: I = U / R (ignore R_Zero) */
          Value = U1;                        /* U across Rl and RiL in mV */
          Value *= 100000;                   /* scale to 0.01 µV */
          Value /= ((R_LOW * 10) + NV.RiL);  /* 0.01 µV / 0.1 Ohms = 0.1 µA */
          Display_Value(Value, -7, 'A');     /* display current */

          /* change to low current mode when current is quite low */
          if (U1 <= 3)                       /* I <= 4.2µA */
          {
            Mode = MODE_LOW;                 /* low current mode */
            Flag |= CHANGED_MODE;            /* set flag for changed mode */
          }
          break;

        case MODE_LOW:             /* charge cap with low current (Rh) */
          /* voltage across Rh at probe-3 (ignore RiL) */
          U1 = ReadU(Probes.ADC_3);          /* read voltage at probe-3 */

          if (U1 > CAP_DISCHARGED)      /* minimum exceeded */
          {
            /* calculate current: I = U / R */
            Value = U1;                        /* in mV */
            Value *= 10000;                    /* scale to 0.1 µV */
            Value /= (R_HIGH / 1000);          /* 0.1 µV / kOhms = 0.1 nA */
            Display_Value(Value, -10, 'A');    /* display current */
          }
          else                          /* in the noise floor */
          {
            Display_Char('-');
          }
          break;

        case MODE_DISCHARGE:       /* discharge cap */
          /* voltage at cap (probe-1) */
          U1 = ReadU(Probes.ADC_1);          /* read voltage at probe-1 */
          Display_Value(U1, -3, 'V');        /* display voltage */

          /* check if cap is discharged */
          if (U1 <= CAP_DISCHARGED)          /* < threshold */
          {
            /* start new check cycle */
            Mode = MODE_NONE;                /* show pinout */
            Flag |= CHANGED_MODE;            /* set flag for changed mode */
          }
          break;
      }

      /* common display output */
      if ((Mode == MODE_HIGH) || (Mode == MODE_LOW))
      {
        /* display voltage across current shunt (Rl or Rh) */
        Display_Space();
        Display_Char('(');
        Display_Value(U1, -3, 'V');          /* display voltage */
        Display_Char(')');
      }
    }


    /*
     *  user feedback
     *  - short key press -> next step
     *  - two short key presses -> exit tool
     */

    if (! (Flag & CHANGED_MODE))        /* skip when mode has changed */
    {
      Test = TestKey(2000, CURSOR_NONE);   /* check for user feedback */

      if (Test == KEY_SHORT)            /* short key press */
      {
        MilliSleep(50);                 /* debounce button a little bit longer */
        Test = TestKey(200, CURSOR_NONE);  /* check for second key press */

        if (Test > KEY_TIMEOUT)         /* second key press */
        {
          Flag = 0;                     /* end processing loop */
        }
        else                            /* single key press */
        {
          Test = 100;                   /* next mode */
        }
      }
      #ifdef HW_KEYS
      else if (Test == KEY_RIGHT)       /* right key */
      {
        Test = 100;                     /* next mode */
      }
      #endif

      if (Test == 100)                  /* next mode */
      {
        /* change mode */
        if (Mode == MODE_NONE)          /* pinout mode */
        {
          Mode = MODE_HIGH;             /* charge cap with high current */
          Flag |= CHANGED_MODE;         /* set flag for changed mode */
        }
        else                            /* any other mode */
        {
          Mode = MODE_DISCHARGE;        /* discharge cap */
          Flag |= CHANGED_MODE;         /* set flag for changed mode */
        }
      }
    }
  }

  /* clean up */
  #undef MODE_NONE
  #undef MODE_CHARGE
  #undef MODE_LEAK
  #undef MODE_DISCHARGE

  #undef RUN_FLAG
  #undef CHANGED_MODE
}

#endif



/* ************************************************************************
 *   clean-up of local constants
 * ************************************************************************ */


/* probes */
#undef PROBES_ESR
#undef PROBES_PWM

/* source management */
#undef EXTRAS_C



/* ************************************************************************
 *   EOF
 * ************************************************************************ */
