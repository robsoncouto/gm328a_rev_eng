/* ************************************************************************
 *
 *   OneWire communication
 *
 *   (c) 2018 by Markus Reschke
 *
 * ************************************************************************ */

/*
 *  hints:
 *  - DQ (data line) requires a 4.7kOhms pull-up resistor to Vcc
 *  - pin assignment for probes (ONEWIRE_PROBES)
 *    probe #1      Gnd
 *    probe #2      Vcc (via Rl to limit current)
 *    probe #3      DQ (requires 4.7kOhms pull-up resistor to Vcc)
 *  - port and pins for dedicated MCU pin (ONEWIRE_IO_PIN)
 *    ONEWIRE_PORT  port data register
 *    ONEWIRE_DDR   port data direction register
 *    ONEWIRE_PIN   port input pins register
 *    ONEWIRE_DQ    pin for DQ
 *  - standard-speed (accurate 1탎 delay)
 *  - external power for clients (no parasite power)
 */


/* local includes */
#include "config.h"           /* global configuration */

#if defined (ONEWIRE_IO_PIN) || defined (ONEWIRE_PROBES)


/*
 *  local constants
 */

/* source management */
#define ONEWIRE_C


/*
 *  include header files
 */

/* local includes */
#include "common.h"           /* common header file */
#include "variables.h"        /* global variables */
#include "functions.h"        /* external functions */
#include "OneWire.h"          /* OneWire specifics */


/*
 *  local variables
 */

/* CRC */
uint8_t        CRC8;          /* current CRC-8 */



/* ************************************************************************
 *   low level functions
 * ************************************************************************ */


#ifdef ONEWIRE_IO_PIN

/*
 *  set up OneWire bus
 *  - DQ line
 */

void OneWire_Setup(void)
{
  /*
   *  DQ is driven as open-drain output
   *  and pulled up by external 4.7kOhm resistor
   */

  /* set DQ to input mode */
  ONEWIRE_DDR &= ~(1 << ONEWIRE_DQ);         /* clear bit */

  /* preset DQ to low for output mode */
  ONEWIRE_PORT &= ~(1 << ONEWIRE_DQ);        /* clear bit */
}

#endif



#ifdef ONEWIRE_PROBES

/*
 *  set up probes for OneWire bus
 *  - probe-1: Gnd
 *  - probe-2: Vcc (current limited by Rl)
 *  - probe-3: DQ
 *
 *  requires:
 *  - String: string stored in EEPROM for first display line 
 *
 *  returns:
 *  - 1 on success
 *  - 0 on any problem
 */

uint8_t OneWire_Probes(const unsigned char *String)
{
  uint8_t           Flag = 0;      /* return value */
  uint8_t           Run = 1;       /* loop control */

  /* inform user */
  ShortCircuit(0);                      /* make sure probes are not shorted */
  LCD_Clear();
  Display_EEString(String);             /* display String */
  /* display module pinout (1: Gnd / 2: Vcc / 3: Data) */
  Display_NextLine();
  Show_SimplePinout('-', '+', 'd');

  /* set probes: probe-1 -- Gnd / probe-2 -- Rl -- Vcc / probe-3 (HiZ) -- Rh -- Gnd */
  ADC_PORT = 0;                              /* pull down directly: */
  ADC_DDR = (1 << TP1);                      /* probe-1 */
  /* pull up probe-2 via Rl, pull down probe-3 via Rh */
  R_DDR = (1 << R_RL_2) | (1 << R_RH_3);     /* enable resistors */
  R_PORT = (1 << R_RL_2);                    /* pull up probe-2, pull down probe-3 */

  /* wait for external pull-up resistor or key press */
  while (Run)
  {
    if (ADC_PIN & (1 << TP3))      /* check for high level */
    {
      Flag = 1;                              /* signal "bus ok" */
      Run = 0;                               /* end loop */
    }
    else                           /* check test key */
    {
      Flag = TestKey(100, CURSOR_NONE);      /* wait 100ms for key press */
      if (Flag)                              /* key pressed */
      {
        Flag = 0;                            /* signed "skipped" */
        Run = 0;                             /* end loop */
      }
    }
  }

  /* remove pull-down via Rh for probe-3 */
  R_DDR = (1 << R_RL_2);      /* disable Rh for probe-3 */

  return Flag;
}

#endif



/*
 *  reset bus and check for presence pulse from client(s)
 *
 *  returns:
 *  - 0: no presence pulse from client
 *  - 1: presence pulse from client
 */

uint8_t OneWire_ResetBus(void)
{
  uint8_t           Flag = 0;      /* return value */

  /*
   *  pull down DQ for >=480탎 and release it again
   */

  #ifdef ONEWIRE_IO_PIN
  /* change DQ to output mode (port pin is low) */
  ONEWIRE_DDR |= (1 << ONEWIRE_DQ);          /* set bit */
  #endif

  #ifdef ONEWIRE_PROBES
  /* change probe-3 (DQ) to output mode (port pin is low) */
  ADC_DDR |= (1 << TP3);                     /* set bit */
  #endif

  wait500us();                /* delay of 500탎 */

  #ifdef ONEWIRE_IO_PIN
  /* change DQ back to input mode */
  ONEWIRE_DDR &= ~(1 << ONEWIRE_DQ);         /* clear bit */
  #endif

  #ifdef ONEWIRE_PROBES
  /* change probe-3 (DQ) back to input mode */
  ADC_DDR &= ~(1 << TP3);                    /* clear bit */
  #endif


  /*
   *  check for presence pulse from client(s)
   *  - time slot >=480탎
   *  - client responds after 15-60탎 with a low pulse of 60-240탎
   *  - we check DQ after 70탎
   */

  wait50us();                 /* read delay of 70탎 */
  wait20us();

  #ifdef ONEWIRE_IO_PIN
  /* read DQ */
  if (! (ONEWIRE_PIN & (1 << ONEWIRE_DQ)))   /* low */
  {
    Flag = 1;                 /* signal ok */
  }
  #endif

  #ifdef ONEWIRE_PROBES
  /* read DQ */
  if (! (ADC_PIN & (1 << TP3)))              /* low */
  {
    Flag = 1;                 /* signal ok */
  }
  #endif

  /* end the time slot */
  wait400us();                /* delay of 430탎 (500탎 - 70탎) */
  wait30us();

  return Flag;
}



/*
 *  send bit
 *
 *  requires:
 *  - Bit: 0/1
 */

void OneWire_SendBit(uint8_t Bit)
{
  /*
   *  - write time slot: 60-120탎
   *  - recovery time after write: >=1탎
   *  - write "1":
   *    - 1-15탎 low pulse
   *    - release DQ and wait until end of time slot
   *  - write "0":
   *    - 60-120탎 low pulse (complete time slot)
   *    - release DQ
   */

  /*
   *  pull down DQ
   */

  #ifdef ONEWIRE_IO_PIN
  /* change DQ to output mode (port pin is low) */
  ONEWIRE_DDR |= (1 << ONEWIRE_DQ);     /* set bit */
  #endif    

  #ifdef ONEWIRE_PROBES
  /* change probe-3 (DQ) to output mode (port pin is low) */
  ADC_DDR |= (1 << TP3);                /* set bit */
  #endif


  if (Bit)          /* 1 */
  {
    /*
     *  pull down DQ for 5탎 and release it again
     */

    wait5us();                /* pulse delay of 5탎 */

    #ifdef ONEWIRE_IO_PIN
    /* change DQ back to input mode */
    ONEWIRE_DDR &= ~(1 << ONEWIRE_DQ);       /* clear bit */
    #endif

    #ifdef ONEWIRE_PROBES
    /* change probe-3 (DQ) back to input mode */
    ADC_DDR &= ~(1 << TP3);                  /* clear bit */
    #endif
 
    /* end the time slot + recovery time */
    wait50us();               /* delay of 65탎 */
    wait5us();
    wait10us();
  }
  else              /* 0 */
  {
    /*
     *  pull down DQ for 60탎 and release it again
     */

    wait50us();               /* pulse delay of 60탎 */
    wait10us();

    #ifdef ONEWIRE_IO_PIN
    /* change DQ back to input mode */
    ONEWIRE_DDR &= ~(1 << ONEWIRE_DQ);       /* clear bit */
    #endif

    #ifdef ONEWIRE_PROBES
    /* change probe-3 (DQ) back to input mode */
    ADC_DDR &= ~(1 << TP3);                  /* clear bit */
    #endif

    /* recovery time */
    wait10us();               /* delay of 10탎 */
  }
}



/*
 *  read bit
 *
 *  returns:
 *  - Bit: 0/1
 */

uint8_t OneWire_ReadBit(void)
{
  uint8_t           Bit = 0;       /* return value */

  /*
   *  - read time slot: 60-120탎
   *  - recovery time after read: >=1탎
   *  - master starts read process by a low pulse >=1탎
   *  - client's response is valid for 15탎 after falling edge of
   *    the master's low pulse
   *  - we read DQ after 13탎
   */


  /*
   *  pull down DQ for 5탎 and release it again
   */

  #ifdef ONEWIRE_IO_PIN
  /* change DQ to output mode (port pin is low) */
  ONEWIRE_DDR |= (1 << ONEWIRE_DQ);     /* set bit */
  #endif    

  #ifdef ONEWIRE_PROBES
  /* change probe-3 (DQ) to output mode (port pin is low) */
  ADC_DDR |= (1 << TP3);                /* set bit */
  #endif

  wait5us();                  /* pulse delay of 5탎 */

  #ifdef ONEWIRE_IO_PIN
  /* change DQ back to input mode */
  ONEWIRE_DDR &= ~(1 << ONEWIRE_DQ);    /* clear bit */
  #endif

  #ifdef ONEWIRE_PROBES
  /* change probe-3 (DQ) back to input mode */
  ADC_DDR &= ~(1 << TP3);               /* clear bit */
  #endif


  /*
   *  read client's response
   *  - data bit valid for 15탎 starting with master's read pulse
   *  - high -> 1
   *  - low  -> 0
   */

  wait5us();                /* read delay of 8탎 */
  wait3us();

  #ifdef ONEWIRE_IO_PIN
  /* read DQ */
  if (ONEWIRE_PIN & (1 << ONEWIRE_DQ))  /* high */
  {
    Bit = 1;                /* 1 */
  }
  #endif

  #ifdef ONEWIRE_PROBES
  /* read DQ */
  if (ADC_PIN & (1 << TP3))             /* high */
  {
    Bit = 1;                /* 1 */
  }
  #endif

  /* end the time slot + recovery time */
  wait50us();               /* delay of 57탎 */
  wait5us();
  wait2us();

  return Bit;
}



/*
 *  send byte
 *  - bit order: LSB
 *
 *  requires:
 *  - Byte: data byte
 */

void OneWire_SendByte(uint8_t Byte)
{
  uint8_t           n = 0;         /* counter */

  while (n < 8)          /* 8 bits */
  {
    OneWire_SendBit(Byte & 0x01);       /* send LSB */
    Byte >>= 1;                         /* shift to right */
    n++;                                /* next bit */
  }
}



/*
 *  read byte
 *  - bit order: LSB
 *
 *  returns:
 *  - Byte: data byte
 */

uint8_t OneWire_ReadByte(void)
{
  uint8_t           Byte = 0;      /* return value */
  uint8_t           n = 0;         /* counter */

  while (n < 8)          /* 8 bits */
  {
    Byte >>= 1;                         /* shift to right */

    if (OneWire_ReadBit())              /* read "1" */
    {
      Byte |= 0b10000000;               /* set bit */
    }

    n++;                                /* next bit */
  }

  return Byte;
}



/* ************************************************************************
 *   high level functions
 * ************************************************************************ */


#if 0

/*
 *  address client
 *  - includes reset of the bus
 *
 *  requires:
 *  - ROM_Code: pointer to ROM code stored in an array of 8 bytes
 *
 *  returns:
 *  - 1
 *  - 0
 */

uint8_t OneWire_AddressClient(uint8_t *ROM_Code)
{
  uint8_t           Flag = 0;      /* return value */
  uint8_t           n = 0;         /* counter */

  /* reset bus and check for presence pulse */
  Flag = OneWire_ResetBus();

  if (Flag)              /* detected client(s) */
  {
    if (ROM_Code)            /* valid pointer */
    {
      /* address specific client on the bus */
      OneWire_SendByte(CMD_MATCH_ROM);  /* select client with ROM code */

      /* send client's ROM code */
      while (n < 8)          /* 8 bytes */
      {
        OneWire_SendByte(*ROM_Code);   /* send one byte of ROM code */
        ROM_Code++;                    /* next byte */
        n++;                           /* next byte */
      }
    }
    else                     /* NULL pointer */
    {
      /* assumes a single client on the bus */
      OneWire_SendByte(CMD_SKIP_ROM);   /* select all clients */
    }
  }

  return Flag;
}

#endif



/*
 *  calculate CRC-8
 *  - CRC = X^8 + X^5 + X^4 + 1
 *  - start value: 0x00
 *  - uses variable CRC8 to track current CRC
 *
 *  requires:
 *  - Byte: new input byte
 */

void OneWire_CRC8(uint8_t Byte)
{
  uint8_t           n = 0;         /* counter */
  uint8_t           Bit;           /* LSB */ 

  while (n < 8)          /* 8 bits */
  {
    /* XOR current LSB of input with CRC8's current X^8 */
    Bit = CRC8 ^ Byte;        /* XOR */
    Bit &= 0b00000001;        /* filter LSB */

    /* shift CRC right */
    CRC8 >>= 1;               /* for next bit */

    if (Bit)                  /* XORed LSB is 1 */
    {
      /*
       *  XOR CRC's X^5 and X^4 with 1
       *  - XOR with 0b00011000
       *  - since CRC is already shifted right: XOR with 0b00001100
       *  - since we have to feed the XORed LSB back into the CRC
       *    and the MSB is 0 after shifting: XOR with 0b10001100
       */

      CRC8 ^= 0b10001100;     /* XOR */
    }
    /*  when 0:
     *  - XOR would keep the original bits
     *  - MSB will be 0 after a right shift anyway
     */

    /* shift input right */
    Byte >>= 1;               /* for next input bit */

    n++;                      /* next bit */
  }
}



/* ************************************************************************
 *   DS18B20
 * ************************************************************************ */


#ifdef SW_DS18B20

/*
 *  DS18B20: read temperature
 *  - single client on the bus
 *
 *  requires:
 *  - T: for returning temperature in 캜
 *  - Depth: for returning bit depth (9-12) 
 *
 *  returns:
 *  - 1 on success
 *  - 0 on any problem
 */

uint8_t DS18B20_ReadTemperature(int32_t *Value, int8_t *Scale, uint8_t *Bits)
{
  uint8_t           Flag = 0;           /* return value */
  uint8_t           Run = 0;            /* loop control */
  uint8_t           n;                  /* counter */
  uint8_t           ScratchPad[9];      /* scratchpad */
  uint8_t           Sign;               /* sign flag */
  int16_t           Temp;               /* temperature */

  /* reset bus and check for presence pulse */
  Flag = OneWire_ResetBus();

  if (Flag)                   /* detected client */
  {
    OneWire_SendByte(CMD_SKIP_ROM);     /* select all clients */

    /* start conversion */
    OneWire_SendByte(CMD_DS18B20_CONVERT_T);

    /*
     *  maximum conversion time
     *  -  9 bits  93.75ms  (t_conv/8)
     *  - 10 bits  187.5ms  (t_conv/4)
     *  - 11 bits  375ms    (t_conv/2)
     *  - 12 bits  750ms    (t_conv)
     */

    #if 0
    /* fixed delay for conversion (required when parasitic-powered) */
    MilliSleep(750);          /* 750ms */
    Run = 3;                  /* three read attempts */
    #endif

    /*
     *  check the conversion status to minimize delay
     *  - requires external power
     *  - this way we don't need to know the bit depth in advance
     *    to determine the conversion time
     */

    Flag = 0;                           /* reset flag */
    n = 50;                             /* 750ms / 15ms = 50 */
    while (n > 0)
    {
      MilliSleep(15);                   /* wait 15ms */

      /* check conversion state */
      Sign = OneWire_ReadBit();         /* single read slot */
      if (Sign == FLAG_CONV_DONE)       /* conversion finished */
      {
        Flag = 1;                       /* signal "ok" */
        Run = 3;                        /* three read attempts */
        n = 1;                          /* end loop */
      }

      n--;                              /* next round */
    }
  }


  /*
   *  read scratchpad
   */

  while (Run)
  {
    /* reset bus and check for presence pulse */
    Flag = OneWire_ResetBus();    

    if (Flag)                 /* detected client */
    {
      OneWire_SendByte(CMD_SKIP_ROM);        /* select all clients */

      /* read scratchpad to get temperature */
      OneWire_SendByte(CMD_DS18B20_READ_SCRATCHPAD);
      n = 0;
      while (n < 9)           /* 9 bytes */
      {
        ScratchPad[n] = OneWire_ReadByte();  /* read byte */
        n++;                                 /* next byte */
      }

      /* check CRC of scratchpad */
      CRC8 = 0x00;            /* reset CRC to start value */
      n = 0;
      while (n < 8)           /* 8 data bytes */
      {
        OneWire_CRC8(ScratchPad[n]);    /* process byte */
        n++;                            /* next byte */
      }

      if (ScratchPad[8] == CRC8)        /* CRC matches */
      {
        Run = 1;                        /* end loop */
      }
      else                              /* mismatch */
      {
        Flag = 0;                       /* signal error */
      }
    }
    else                      /* no client detected */
    {
      Run = 1;                /* end loop */
    }

    Run--;                    /* another try */
  }


  /*
   *  get temperature from scratchpad (in 캜)
   */

  if (Flag)                   /* valid scratchpad */
  {
    /*
     *  get bit depth (resolution)
     *  - 9-12 bits mean 1-4 binary places after the dot
     */

    Run = 0;                       /* reset */
    n = ScratchPad[4];             /* get configuration register */
    if (n & FLAG_DS18B20_R0)       /* R0 set */
    {
      Run |= 0b00000001;           /* set lower bit */
    }
    if (n & FLAG_DS18B20_R1)       /* R1 set */
    {
      Run |= 0b00000010;           /* set upper bit */
    }
    *Bits = Run + 9;               /* save bit depth */
                                   /* 0-3 -> 9-12 bits */


    /*
     *  build signed integer from LSB and MSB
     *  - two's complement
     *  - t_LSB: Byte #0 = ScratchPad[0]
     *  - t_MSB: Byte #1 = ScratchPad[1]
     */

    int16_t         *Ptr;          /* pointer to mitigate compiler warning */

    Ptr = (int16_t *)&ScratchPad[0];    /* combine LSB and MSB */
    Temp = *Ptr;                        /* copy value */  


    /*
     *  remove undefined bits based on bit depth and 
     *  also calculate scaling factors
     */

    n = 3 - Run;                        /* number of bits to ignore */
    *Value = 10000;                     /* 4 decimal places */  
    *Scale = -4;                        /* 4 decimal places */
    Run = 16;                           /* 4 binary places (*16) */
    Sign = ScratchPad[1] & 0b11111000;  /* set flag when negative */
    while (n)
    {
      /* remove undefined bit */
      Temp >>= 1;                       /* shift right */
      if (Sign)                         /* negative */
      {
        /* keep signed integer negative */
        Temp |= 0b1000000000000000;     /* set MSB */
      }

      /* scale by one digit */
      *Value /= 10;                /* one decimal place less */
      (*Scale)++;                  /* update scale too */
      Run >>=1;                    /* one binary place less */

      n--;                         /* next bit */
    }

    *Value *= Temp;                /* scale temperature */ 
    *Value /= Run;                 /* and adjust for binary scaling */
  }

  return Flag;
}

#endif



/* ************************************************************************
 *   clean-up of local constants
 * ************************************************************************ */


/* source management */
#undef ONEWIRE_C

#endif

/* ************************************************************************
 *   EOF
 * ************************************************************************ */
