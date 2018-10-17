/* ************************************************************************
 *
 *   TTL serial interface (hardware USART)
 *
 *   (c) 2018 by Markus Reschke
 *
 * ************************************************************************ */

/*
 *  hints:
 *  - port and pins for bit-bang serial (software USART)
 *    SERIAL_PORT   port data register
 *    SERIAL_DDR    port data direction register
 *    SERIAL_PIN    port input pins register
 *    SERIAL_TX     pin for Tx (transmit)
 *    SERIAL_RX     pin for Rx (receive) - not supported
 *  - For hardware USART the MCU specific pins are used:
 *    ATmega 328:        RxD PD0 / TxD PD1
 *    ATmega 644: USART0 RxD PD0 / TxD PD1
 *                USART1 RxD PD2 / TxD PD3
 */


/* local includes */
#include "config.h"           /* global configuration */

#ifdef HW_SERIAL


/*
 *  local constants
 */

/* source management */
#define SERIAL_C


/*
 *  include header files
 */

/* local includes */
#include "common.h"           /* common header file */
#include "variables.h"        /* global variables */
#include "functions.h"        /* external functions */


/*
 *  local constants
 */

/* registers, bits and stuff for hardware USART */

/* USART0 */
#if SERIAL_USART == 0
  #define REG_UDR        UDR0      /* USART I/O Data Register 0 */

  #define REG_UCSR_A     UCSR0A    /* USART Control and Status Register A */
  #define BIT_RXC        RXC0      /* USART Receive Complete */
  #define BIT_TXC        TXC0      /* USART Transmit Complete */
  #define BIT_UDRE       UDRE0     /* USART Data Register Empty */
  #define BIT_FE         FE0       /* Frame Error */
  #define BIT_DOR        DOR0      /* Data OverRun */
  #define BIT_UPE        UPE0      /* USART Parity Error */

  #define REG_UCSR_B     UCSR0B    /* USART Control and Status Register B */
  #define BIT_RXCIE      RXCIE0    /* RX Complete Interrupt Enable */
  #define BIT_RXEN       RXEN0     /* Receiver Enable */
  #define BIT_TXEN       TXEN0     /* Transmitter Enable */
  #define BIT_UCSZ_2     UCSZ02    /* USART Character Size 2 */

  #define REG_UCSR_C     UCSR0C    /* USART Control and Status Register C */
  #define BIT_UMSEL_0    UMSEL00   /* USART Mode Select 0 */
  #define BIT_UMSEL_1    UMSEL01   /* USART Mode Select 1 */
  #define BIT_UPM_0      UPM00     /* USART Parity Mode 0 */
  #define BIT_UPM_1      UPM01     /* USART Parity Mode 1 */
  #define BIT_USBS       USBS0     /* USART Stop Bit Select */
  #define BIT_UCSZ_0     UCSZ00    /* USART Character Size 0 */
  #define BIT_UCSZ_1     UCSZ01    /* USART Character Size 1 */

  #define REG_UBRR_L     UBRR0L    /* USART Baud Rate Register Low */
  #define REG_UBRR_H     UBRR0H    /* USART Baud Rate Register High */
  #define REG_UBRR       UBRR0     /* USART Baud Rate Register combined */

  #define ISR_USART_RX   USART0_RX_vect      /* ISR */
#endif

/* USART1 */
#if SERIAL_USART == 1
  #define REG_UDR        UDR1      /* USART I/O Data Register 1 */

  #define REG_UCSR_A     UCSR1A    /* USART Control and Status Register A */
  #define BIT_RXC        RXC1      /* USART Receive Complete */
  #define BIT_TXC        TXC1      /* USART Transmit Complete */
  #define BIT_UDRE       UDRE1     /* USART Data Register Empty */
  #define BIT_FE         FE1       /* Frame Error */
  #define BIT_DOR        DOR1      /* Data OverRun */
  #define BIT_UPE        UPE1      /* USART Parity Error */

  #define REG_UCSR_B     UCSR1B    /* USART Control and Status Register B */
  #define BIT_RXCIE      RXCIE1    /* RX Complete Interrupt Enable */
  #define BIT_RXEN       RXEN1     /* Receiver Enable */
  #define BIT_TXEN       TXEN1     /* Transmitter Enable */
  #define BIT_UCSZ_2     UCSZ12    /* Character Size 0 */

  #define REG_UCSR_C     UCSR1C    /* USART Control and Status Register C */
  #define BIT_UMSEL_0    UMSEL10   /* USART Mode Select 0 */
  #define BIT_UMSEL_1    UMSEL11   /* USART Mode Select 1 */
  #define BIT_UPM_0      UPM10     /* USART Parity Mode 0 */
  #define BIT_UPM_1      UPM11     /* USART Parity Mode 1 */
  #define BIT_USBS       USBS1     /* USART Stop Bit Select */
  #define BIT_UCSZ_0     UCSZ10    /* USART Character Size 0 */
  #define BIT_UCSZ_1     UCSZ11    /* USART Character Size 1 */

  #define REG_UBRR_L     UBRR1L    /* USART Baud Rate Register Low */
  #define REG_UBRR_H     UBRR1H    /* USART Baud Rate Register High */
  #define REG_UBRR       UBRR1     /* USART Baud Rate Register combined */

  #define ISR_USART_RX   USART1_RX_vect      /* ISR */
#endif


/* registers, bits and stuff for bit-bang USART */

/* PCINT0-7 */
#if (SERIAL_PCINT >= 0) && (SERIAL_PCINT <= 7)
  #define BIT_PC_IRQ     PCIE0          /* Pin Change Interrupt Enable 0 */
  #define BIT_PC_FLAG    PCIF0          /* Pin Change Interrupt Flag 0 */
  #define REG_PC_MASK    PCMSK0         /* Pin Change Mask Register 0 */
  #define ISR_PINCHANGE  PCINT0_vect    /* ISR */
#endif

/* PCINT8-15 */
#if (SERIAL_PCINT >= 8) && (SERIAL_PCINT <= 15)
  #define BIT_PC_IRQ     PCIE1          /* Pin Change Interrupt Enable 1 */
  #define BIT_PC_FLAG    PCIF1          /* Pin Change Interrupt Flag 1 */
  #define REG_PC_MASK    PCMSK1         /* Pin Change Mask Register 1 */
  #define ISR_PINCHANGE  PCINT1_vect    /* ISR */
#endif

/* PCINT16-23 */
#if (SERIAL_PCINT >= 16) && (SERIAL_PCINT <= 23)
  #define BIT_PC_IRQ     PCIE2          /* Pin Change Interrupt Enable 2 */
  #define BIT_PC_FLAG    PCIF2          /* Pin Change Interrupt Flag 2 */
  #define REG_PC_MASK    PCMSK2         /* Pin Change Mask Register 2 */
  #define ISR_PINCHANGE  PCINT2_vect    /* ISR */
#endif

/* PCINT24-31 */
#if (SERIAL_PCINT >= 24) && (SERIAL_PCINT <= 31)
  #define BIT_PC_IRQ     PCIE3          /* Pin Change Interrupt Enable 3 */
  #define BIT_PC_FLAG    PCIF3          /* Pin Change Interrupt Flag 3 */
  #define REG_PC_MASK    PCMSK3         /* Pin Change Mask Register 3 */
  #define ISR_PINCHANGE  PCINT3_vect    /* ISR */
#endif



/* ************************************************************************
 *   functions for software USART (bit-banging)
 * ************************************************************************ */


#ifdef SERIAL_BITBANG

/*
 *  set up serial interface
 *  - TX & RX line
 */

void Serial_Setup(void)
{
  SERIAL_DDR |= (1 << SERIAL_TX);       /* set TX pin to output mode */
  SERIAL_PORT |= (1 << SERIAL_TX);      /* preset TX to idle state (high) */

  #ifdef SERIAL_RW
  SERIAL_DDR &= ~(1 << SERIAL_RX);      /* set RX pin to input mode */
  REG_PC_MASK |= (1 << SERIAL_RX);      /* set up pin change mask for RX pin */
  /* hint: use Serial_Ctrl() to enable pin change interrupt later on */

    #ifdef SAVE_POWER
    /* we have to keep clk_IO running when sleeping */
    Cfg.SleepMode = SLEEP_MODE_IDLE;      /* default sleep mode: power idle */
    #endif

  #endif
}



/*
 *  send byte
 *  - 9600 8N1
 *
 *  requires:
 *  - Byte: byte to send
 */

void Serial_WriteByte(uint8_t Byte)
{
  uint8_t           n = 8;    /* bit counter */

  /* R_PORT & R_DDR / ADC_PORT & ADC_DDR can interfere (input/HiZ) */
  /* idea: add external pull-up resistor to TX to keep TX high while set to HiZ */
  Serial_Setup();        /* quick and dirty */

  /* start bit (0/low) */
  SERIAL_PORT &= ~(1 << SERIAL_TX);     /* clear TX */
  wait100us();                          /* delay for 9600bps */
  wait3us();

  /* 8 data bits (LSB first) */
  while (n > 0)
  {
    if (Byte & 0b00000001)    /* 1 */
    {
      SERIAL_PORT |= (1 << SERIAL_TX);    /* set TX */
    }
    else                      /* 0 */
    {
      SERIAL_PORT &= ~(1 << SERIAL_TX);   /* clear TX */
    }

    wait100us();              /* delay for 9600bps */
    wait3us();

    Byte >>= 1;               /* shift right */
    n--;                      /* next bit */
  }

  /* 1 stop bit (1/high) and stay idle (high) */
  SERIAL_PORT |= (1 << SERIAL_TX);      /* set TX */
  wait100us();                          /* delay for 9600bps */
  wait3us();
}



#ifdef SERIAL_RW

/*
 *  control serial interface
 *
 *  requires:
 *  - Control: control flag/switch
 *    SER_RX_PAUSE  - pause RX (disable interrupt)
 *    SER_RX_RESUME - resume RX (enable interrupt)
 */

void Serial_Ctrl(uint8_t Control)
{
  switch (Control)
  {
    case SER_RX_PAUSE:        /* pause RX */
      /* todo: send XOFF? */
      PCICR &= ~(1 << BIT_PC_IRQ);      /* disable pin change interrupt */
      break;

    case SER_RX_RESUME:       /* resume RX */
      PCIFR |= (1 << BIT_PC_FLAG);      /* clear interrupt flag */
      PCICR |= (1 << BIT_PC_IRQ);       /* enable pin change interrupt */
      /* todo: send XON? */
      break;
  }
}



/*
 *  ISR for PCIn (Pin Change Interrupt n)
 *  - 9600 8N1
 *  - for detecting start bit of RX
 *  - starts bit sampling by setting up timer
 *  - can't be used when by some measurement/tool needs Timer0
 */

ISR(ISR_PINCHANGE, ISR_BLOCK)
{
  /*
   *  hints:
   *  - the PCIFn flag is cleared automatically
   *  - interrupt processing is disabled while this ISR runs
   *    (no nested interrupts)
   */

  /* check if RX pin has changed to 0/low (start bit) */
  if (! (SERIAL_PIN & (1 << SERIAL_RX)))     /* low state */
  {
    PCICR &= ~(1 << BIT_PC_IRQ);        /* disable pin change interrupt */

    /* reset variables for new char */
    RX_Char = 0;
    RX_Bits = 0;

    /*
     *  set up Timer0 for sampling first bit (start)
     *  - normal mode
     *  - use OCR0B for triggering interrupt
     *  - 1/2 bit period of 9600bps = 52.08탎
     *  - fixed prescaler = 8
     *    MCU clock [MHz]:    8    16    20
     *    timer cycle [탎]:   1    0.5   0.4
     *    cycles for 52탎:    52   104   130
     *  - we don't need high precision, just good enough to sample 10 bits
     *  - MCU_CYCLE_TIME in 0.1ns
     *    timer cycles = (52탎 * 10000) / (MCU_CYCLE_TIME * 8)
     *  - use delay of 48탎 for early sampling and to allow some time jittering
     */

    #define HALF_BIT     480000 / (MCU_CYCLE_TIME * 8)

    //TCCR0B = 0;                 /* stop timer */
    TCNT0 = 0;                  /* reset counter to 0 */
    OCR0B = HALF_BIT;           /* set match value for 1/2 bit period */
    TCCR0A = 0;                 /* normal mode (count up) */
    TIFR0 = (1 << OCF0B);       /* clear Output Compare B Match flag */
    TIMSK0 = (1 << OCIE0B);     /* enable Output Compare B Match interrupt */
    TCCR0B = (1 << CS01);       /* start timer by setting prescaler */

    #undef HALF_BIT
  }
  /* else: ignore pin change to 1/high */
}



/*
 *  ISR for match of Timer0's OCR0B (Output Compare Register B)
 *  - 9600 8N1
 *  - sample RX bits
 *  - can't be used when by some measurement/tool needs Timer0
 *  - puts received char into a buffer
 *  - collects full text line and manages the buffer
 */

ISR(TIMER0_COMPB_vect, ISR_BLOCK)
{
  uint8_t           Bit;           /* bit flag */

  /*
   *  hints:
   *  - the OCF0B interrupt flag is cleared automatically
   *  - interrupt processing is disabled while this ISR runs
   *    (no nested interrupts)
   */

  TCCR0B = 0;                 /* stop Timer0 */


  /*
   *  sample RX bit
   *  - LSB first
   */

  Bit = SERIAL_PIN & (1 << SERIAL_RX);       /* get bit */
  RX_Bits++;                                 /* got another bit */

  if (RX_Bits == 1)           /* start bit */
  {
    /* start bit should be low */
    if (Bit)                  /* high */
    {
      RX_Bits = 0;            /* end RX */
    }
  }
  else if (RX_Bits == 10)     /* stop bit */
  {
    /* stop bit should be high */
    if (Bit == 0)             /* low */
    {
      RX_Bits = 0;            /* end RX */
    }
  }
  else                        /* data bit */
  {
    RX_Char >>= 1;                      /* shift right */
    if (Bit) RX_Char |= 0b10000000;     /* set MSB */   
  }


  /*
   *  manage buffer
   */

  if (RX_Bits == 10)          /* got all bits */
  {
    if (! (Cfg.OP_Control & OP_RX_LOCKED))   /* buffer unlocked */
    {
      if (RX_Char == '\r')              /* CR (carriage return) */
      {
        RX_Char = 0;                    /* terminate string */
      }
      else if (RX_Char == '\n')         /* NL (new line) */
      {
        RX_Char = 0;                    /* terminate string */
        if (RX_Pos > 0)                 /* not an empty line */
        {
          Cfg.OP_Control |= OP_RX_LOCKED;    /* lock buffer for processing */
        }
      }
      else if (RX_Pos == (RX_BUFFER_SIZE - 1))    /* no control char & overflow */
      {
        RX_Char = 0;                              /* terminate string */
        /* lock buffer & signal overflow */
        Cfg.OP_Control |= OP_RX_LOCKED | OP_RX_OVERFLOW;
      }

      RX_Buffer[RX_Pos] = RX_Char;      /* copy to buffer */

      if (RX_Char > 0)                  /* no control char */
      {
        RX_Pos++;                       /* next char */
      }
    }
    /* else: drop char */

    RX_Bits = 0;              /* end RX */
  }


  /*
   *  rearm pin-change interrupt for next char
   */

  if (RX_Bits == 0)      /* char done, no more bits expected */
  {
    TIMSK0 = 0;                    /* disable Timer0 interrupts */
    PCIFR |= (1 << BIT_PC_FLAG);   /* clear pin change interrupt flag */
    PCICR |= (1 << BIT_PC_IRQ);    /* enable pin change interrupt */
  }
  else                   /* more bits expected */
  {
    /*
     *  set up Timer0 for sampling next bit (data or stop)
     *  - keep basic settings from RX pin change ISR 
     *  - bit period of 9600bps = 104.1탎
     *  - fixed prescaler = 8 (<=16MHz) or 64 (>16MHz)
     *    MCU clock [MHz]:    8    16    20
     *    timer cycle [탎]:   1    0.5   3.2
     *    cycles for 104탎:   104  208   32.5
     *  - we don't need high precision, just good enough to sample 10 bits
     *  - MCU_CYCLE_TIME in 0.1ns
     *    timer cycles = (104탎 * 10000) / (MCU_CYCLE_TIME * prescaler)
     *  - use a delay of 102탎 to compensate for the runtime of this ISR
     */

    #if CPU_FREQ <= 16000000       /* <= 16MHz */
      /* prescaler 8 */
      #define FULL_BIT        1020000 / (MCU_CYCLE_TIME * 8)
      #define PRESCALER       (1 << CS01)
    #else                          /* > 16MHz */
      /* prescaler 64 */
      #define FULL_BIT        1020000 / (MCU_CYCLE_TIME * 64)
      #define PRESCALER       (1 << CS01) | (1 << CS00)
    #endif

    TCNT0 = 0;                /* reset counter to 0 */
    OCR0B = FULL_BIT;         /* set match value for bit period */
    TCCR0B = PRESCALER;       /* start timer by setting prescaler */

    #undef FULL_BIT
    #undef PRESCALER
  }
}

#endif

#endif



/* ************************************************************************
 *   functions for hardware USART
 * ************************************************************************ */


#ifdef SERIAL_HARDWARE

/*
 *  set up serial interface
 *  - 9600 8N1
 *  - TX & RX line
 */

void Serial_Setup(void)
{
  /*
   *  asynchronous normal mode
   *  - prescaler = (f_MCU / (16 * bps)) - 1
   *  - 2400bps, 8 data bits, no parity, 1 stop bit
   *  - overrides normal port operation of Tx pin and Rx pin (if enabled)
   */

  REG_UBRR = (CPU_FREQ / (16UL * 9600)) - 1;
  REG_UCSR_C = (1 << BIT_UCSZ_1) | (1 << BIT_UCSZ_0);

  #ifdef SERIAL_RW
    /* enable TX and RX */
    REG_UCSR_B = (1 << BIT_RXEN) | (1 << BIT_TXEN);
    /* hint: use Serial_Ctrl() to enable interrupt for RX Complete later on */
  #else
    /* enable TX */
    REG_UCSR_B = (1 << BIT_TXEN);
  #endif

  #ifdef SAVE_POWER
  /* we have to keep clk_IO running when sleeping */
  Cfg.SleepMode = SLEEP_MODE_IDLE;      /* default sleep mode: power idle */
  #endif
}



/*
 *  send byte
 *  - 9600 8N1 (set by Serial_Setup())
 *
 *  requires:
 *  - Byte: byte to send
 */

void Serial_WriteByte(uint8_t Byte)
{
  /* wait for empty Tx buffer */
  while (! (REG_UCSR_A & (1 << BIT_UDRE)));

  /* clear USART Transmit Complete flag */
  //REG_UCSR_A = (1 << BIT_TXC);

  /* copy byte to Tx buffer, triggers sending */
  REG_UDR = Byte;
}



#ifdef SERIAL_RW

/*
 *  control serial interface
 *
 *  requires:
 *  - Control: control flag/switch
 *    SER_RX_PAUSE  - pause RX (disable interrupt)
 *    SER_RX_RESUME - resume RX (enable interrupt)
 */

void Serial_Ctrl(uint8_t Control)
{
  switch (Control)
  {
    case SER_RX_PAUSE:        /* pause RX */
      /* todo: send XOFF? */
      REG_UCSR_B &= ~(1 << BIT_RXCIE);       /* disable RXC interrupt */
      break;

    case SER_RX_RESUME:       /* resume RX */
      REG_UCSR_B |= (1 << BIT_RXCIE);        /* enable RXC interrupt */
      /* todo: send XON? */
      break;
  }
}



/*
 *  ISR for RXCn (Receive Complete n)
 *  - 9600 8N1 (set by Serial_Setup())
 *  - retrieves received character and puts it into a buffer
 *  - collects full text line and manages the buffer
 */

ISR(ISR_USART_RX, ISR_BLOCK)
{
  unsigned char     Char;          /* character */

  /*
   *  hints:
   *  - the RXCn flag is cleared by reading UDRn
   *    if not cleared it will retrigger the interrupt
   *  - interrupt processing is disabled while this ISR runs
   *    (no nested interrupts)
   */

  /* todo: shall be allow nested interrupts for more critical things? */
  /* todo: check receiver error flags? UCSRnA must be read before UDRn! */

  Char = REG_UDR;                       /* get received char & clear flag */

  if (! (Cfg.OP_Control & OP_RX_LOCKED))     /* buffer unlocked */
  {
    if (Char == '\r')                   /* CR (carriage return) */
    {
      Char = 0;                         /* terminate string */
    }
    else if (Char == '\n')              /* NL (new line) */
    {
      Char = 0;                         /* terminate string */
      if (RX_Pos > 0)                   /* not an empty line */
      {
        Cfg.OP_Control |= OP_RX_LOCKED;      /* lock buffer for processing */
      }
    }
    else if (RX_Pos == (RX_BUFFER_SIZE - 1))  /* no control char & overflow */
    {
      Char = 0;                               /* terminate string */
      /* lock buffer & signal overflow */
      Cfg.OP_Control |= OP_RX_LOCKED | OP_RX_OVERFLOW;
    }

    RX_Buffer[RX_Pos] = Char;           /* copy to buffer */

    if (Char > 0)                       /* no control char */
    {
      RX_Pos++;                         /* next char */
    }
  }
  /* else: drop char, otherwise it would block the firmware */
}

#endif

#endif



/* ************************************************************************
 *   high level functions for TX
 * ************************************************************************ */



/*
 *  write one char
 *  - converts custom LCD characters to standard ones
 *
 *  requires:
 *  - Char: character to output
 */

void Serial_Char(unsigned char Char)
{
  switch (Char)
  {
    case LCD_CHAR_DIODE_AC:        /* diode icon '>|' */
      Serial_WriteByte('>');
      Serial_WriteByte('|');
      break;

    case LCD_CHAR_DIODE_CA:        /* diode icon '|<' */
      Serial_WriteByte('|');
      Serial_WriteByte('<');
      break;

    case LCD_CHAR_CAP:             /* capacitor icon '||' */
      Serial_WriteByte('|');
      Serial_WriteByte('|');
      break;

    case LCD_CHAR_OMEGA:           /* omega */
      Serial_WriteByte('R');
      break;

    case LCD_CHAR_MICRO:           /* � / micro */
      Serial_WriteByte('u');
      break;

    case LCD_CHAR_RESISTOR_L:      /* resistor left icon '[' */
      Serial_WriteByte('[');       /* or maybe 'V'? */
      break;

    case LCD_CHAR_RESISTOR_R:      /* resistor right icon ']' */
      Serial_WriteByte(']');       /* or maybe 'V'? */
      break;

    default:                       /* standard char */
      Serial_WriteByte(Char);
  }
}



#if defined (UI_SERIAL_COPY) || defined (UI_SERIAL_COMMANDS)

/*
 *  send carriage return and linefeed
 */

void Serial_NewLine(void)
{
  Serial_WriteByte('\r');          /* send CR */
  Serial_WriteByte('\n');          /* send LF */
}

#endif



/* ************************************************************************
 *   high level functions for RX
 * ************************************************************************ */



/* ************************************************************************
 *   clean-up of local constants
 * ************************************************************************ */

/* source management */
#undef SERIAL_C

#endif

/* ************************************************************************
 *   EOF
 * ************************************************************************ */
