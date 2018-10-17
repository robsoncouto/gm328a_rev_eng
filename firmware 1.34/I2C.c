/* ************************************************************************
 *
 *   I2C (bit-bang & hardware TWI)
 *
 *   (c) 2017 by Markus Reschke
 *
 * ************************************************************************ */

/*
 *  hints:
 *  - port and pins for bit-bang I2C
 *    I2C_PORT      port data register
 *    I2C_DDR       port data direction register
 *    I2C_PIN       port input pins register
 *    I2C_SDA       pin for SDA
 *    I2C_SCL       pin for SCL
 *  - For hardware I2C (TWI) the MCU specific pins are used:
 *    ATmega 328: SDA PC4 / SCL PC5 
 *    ATmega 644: SDA PC1 / SCL PC0
 *  - bus speed modes
 *    I2C_STANDARD_MODE  100kHz
 *    I2C_FAST_MODE      400kHz
 *  - Don't forget the pull up resistors for SDA and SCL!
 *    Usually 2-10kOhms for 5V.
 */


/* local includes */
#include "config.h"           /* global configuration */

#ifdef HW_I2C


/*
 *  local constants
 */

/* source management */
#define I2C_C


/*
 *  include header files
 */

/* local includes */
#include "common.h"           /* common header file */
#include "variables.h"        /* global variables */
#include "functions.h"        /* external functions */



/*
 *  bus speed modes:
 *  - standard mode      up to 100kbit/s
 *  - fast mode          up to 400kbit/s
 *  - fast mode plus     up to 1Mbit/s      
 *  - high-speed mode
 *    400pF load         up to 1.7Mbit/s
 *    100pF load         up to 3.4Mbit/s
 *
 *  address modes:
 *  - 7 bit + R/W   ->   address byte
 *    R/W (1/0)          bit 0
 *    bit 0 (A0)         bit 1
 *    bit 1 (A1)         bit 2
 *    bit 2 (A2)         bit 3
 *    bits 3-6           bits 4-7
 *  - 10 bit + R/W  ->   2 adress bytes
 *    R/W (1/0)          MSB bit 0
 *    bits 0-7           LSB bits 0-7
 *    bits 8-9           MSB bits 1-2
 *    11110              MSB bits 3-7
 */



/* ************************************************************************
 *   functions for bit-bang I2C
 * ************************************************************************ */


#ifdef I2C_BITBANG

/*
 *  set up SDA and SCL lines
 *
 *  returns:
 *  - I2C_ERROR on bus error
 *  - I2C_OK on success
 */

uint8_t I2C_Setup(void)
{
  uint8_t           Flag = I2C_ERROR;   /* return value */
  uint8_t           Bits;               /* bits/bitmask */

  /* default is SDA and SCL pulled up by external resistors */
  /* set both pins to HiZ input mode */
  I2C_DDR &= ~((1 << I2C_SDA) | (1 << I2C_SCL));

  /* preset pins to 0 */
  I2C_PORT &= ~((1 << I2C_SDA) | (1 << I2C_SCL));

  /* check if SDA and SCL are pulled up externally */
  Bits = I2C_PIN;                  /* get state */
  Bits &= (1 << I2C_SDA) | (1 << I2C_SCL);        /* filter lines */
  if (Bits == ((1 << I2C_SDA) | (1 << I2C_SCL)))  /* lines are high */
  {
    Flag = I2C_OK;       /* signal success */
  }

  /* current state: SDA high / SCL high */

  return Flag;
}



/*
 *  create start condition
 *  - Type
 *    I2C_START for start condition
 *    I2C_REPEATED_START for repeated start condition
 *
 *  returns:
 *  - I2C_ERROR on bus error
 *  - I2C_OK on success
 */

uint8_t I2C_Start(uint8_t Type)
{
  uint8_t           Flag = I2C_OK;      /* return value */
  uint8_t           Bits;               /* bits/bitmask */

  if (Type == I2C_START)      /* start */
  {
    /* expected state: SDA high / SCL high */

    /*
     *  todo: clock hold
     *  - check if SCL is pulled down by slave
     *  - wait until slave releases SCL
     */

    Bits = I2C_DDR;

    /* pull down SDA (SDA low) */
    Bits |= (1 << I2C_SDA);        /* set output mode */
    I2C_DDR = Bits;                /* enable pull down */

    /* SCL will follow after t_HD;STA */
    #ifdef I2C_FAST_MODE
      /* fast mode: min. 0.6탎 */
      wait1us();
    #else
      /* standard mode: min. 4탎 */
      wait4us();
    #endif

    /* pull down SCL (SCL low) */
    Bits |= (1 << I2C_SCL);        /* set output mode */
    I2C_DDR = Bits;                /* enable pull down */

    /* current state: SDA low / SCL low */
  }
#if 0
  else                        /* repeated start */
  {
    /* expected state: SDA undefined / SCL low */

    /* set SDA high */
    I2C_DDR &= ~(1 << I2C_SDA);

    /* SCL has to stay low for t_LOW */
    #ifdef I2C_FAST_MODE
      /* fast mode: min. 1.3탎 */
      wait2us();
    #else
      /* standard mode: min. 4.7탎 */
      wait5us();
    #endif

    /* set SCL high */
    I2C_DDR &= ~(1 << I2C_SCL);

    /* SDA has to stay high for t_SU;STA after SCL rises */
    #ifdef I2C_FAST_MODE
      /* fast mode: min. 0.6탎 */
      wait1us();
    #else
      /* standard mode: min. 4.7탎 */
      wait5us();
    #endif

    /* follow common start condition */
    I2C_Start();

    /* current state: SDA low / SCL low */
  }
#endif

  /* current state: SDA low / SCL low */

  return Flag;
}



/*
 *  write byte (master to slave)
 *  - send byte (address or data) to slave
 *    bit-bang 8 bits, MSB first, LSB last
 *  - get ACK/NACK from slave
 *  - byte to be sent is taken from global I2C.Byte
 *
 *  requires:
 *  - Type
 *    I2C_DATA for data byte
 *    I2C_ADDRESS for address byte
 *
 *  returns:
 *  - I2C_ERROR for bus error
 *  - I2C_ACK for ACK
 *  - I2C_NACK NACK
 */

uint8_t I2C_WriteByte(uint8_t Type)
{
  uint8_t           Flag = I2C_ERROR;   /* return value */
  uint8_t           Byte;               /* byte */
  uint8_t           n = 8;              /* counter */

  /* expected state: SDA undefined / SCL low */

  /*
   *  send byte
   */

  Byte = I2C.Byte;            /* get byte */

  /* bit-bang 8 bits */
  while (n > 0)               /* 8 bits */
  {
    /*
     *  set SDA
     */

    /* SDA has to be set t_SU;DAT before SCL rises */
    /* fast mode: min. 100ns */
    /* standard mode: min. 250ns */

    /* get current MSB and set SDA */
    if (Byte & 0b10000000)    /* 1 */
    {
      /* set SDA high by releasing it */
      I2C_DDR &= ~(1 << I2C_SDA);
    }
    else                      /* 0 */
    {
      /* set SDA low by pulling it down */
      I2C_DDR |= (1 << I2C_SDA);
    }


    /*
     *  run SCL cycle
     */

    /* SCL has to stay low for t_LOW */
    #ifdef I2C_FAST_MODE
      /* fast mode: min. 1.3탎 */
      wait2us();
    #else
      /* standard mode: min. 4.7탎 */
      wait5us();
    #endif

    /* set SCL high */
    I2C_DDR &= ~(1 << I2C_SCL);

    /* 
     *  todo: clock stretching (inter-bit)
     *  - check if SCL is pulled down by slave
     *  - wait until slave releases SCL
     */

    /* SCL has to stay high for t_HIGH */
    #ifdef I2C_FAST_MODE
      /* fast mode: min. 0.6탎 */
      wait1us();
    #else
      /* standard mode: min. 4탎 */
      wait4us();
    #endif

    /* set SCL low */
    I2C_DDR |= (1 << I2C_SCL);

    /* SDA has to stay valid for t_HD;DAT after SCL falls */
    /* fast mode: min. 300ns */
    /* standard mode: min. 300ns */
    wait1us();

    /* new SDA has to become valid t_VD;DAT after SCL falls */
    /* fast mode: max. 0.9탎 */
    /* standard mode: max. 3.45탎 */

    Byte <<= 1;               /* shift bits one step left */
    n--;                      /* next bit */
  }

  /* current state: SDA undefined / SCL low */


  /*
   *  get ACK/NACK
   */

  /* expected state: SDA undefined / SCL low */

  /* release SDA (SDA high) for slave */
  I2C_DDR &= ~(1 << I2C_SDA);


  /*
   *  start SCL cycle for ACK/NACK
   */

  /* SCL has to stay low for t_LOW */
  #ifdef I2C_FAST_MODE
    /* fast mode: min. 1.3탎 */
    wait2us();
  #else
    /* standard mode: min. 4.7탎 */
    wait5us();
  #endif

  /* set SCL high */
  I2C_DDR &= ~(1 << I2C_SCL);


  /*
   *  we expect slave to pull down SDA (SDA low) to ack
   *  - some slaves might need some time, like an EEPROM for writing
   *    or an ADC for the conversion
   */

  Byte = I2C.Timeout;         /* get timeout */
  Byte *= 2;                  /* convert into 5탎 steps */
  if (Byte == 0)              /* prevent zero timeout */
  {
    Byte++;                   /* set to 1 (5탎) */
  }

  while (Byte > 0)            /* timeout loop */
  {
    Byte--;                   /* decrease timeout counter */

    n = I2C_PIN;              /* get current state */
    n &= (1 << I2C_SDA);      /* filter SDA pin */

    if (n == 0)               /* SDA low */
    {
      Flag = I2C_ACK;                /* signal ACK */
      Byte = 0;                      /* end loop */
    }
    else                             /* SDA high */
    {
      if (Byte > 0)                  /* no timeout yet */
      {
        wait5us();                   /* wait */
      }
    }
  }

  if (Flag == I2C_ERROR)      /* got no ACK (still default) */
  {
    Flag = I2C_NACK;          /* signal NACK */
  }


  /* for an ACK the slave has to keep SDA during the clock pulse */

  /* 
   *  todo: clock stretching (inter-byte)
   *  - check if SCL is pulled down by slave
   *  - wait until slave releases SCL
   */  

  /* SCL has to stay high for t_HIGH */
  #ifdef I2C_FAST_MODE
    /* fast mode: min. 0.6탎 */
    wait1us();
  #else
    /* standard mode: min. 4탎 */
    wait4us();
  #endif

  /* todo: should we check if SDA is still low? */

  /* set SCL low */
  I2C_DDR |= (1 << I2C_SCL);

  /* SDA has to stay valid for t_HD;DAT after SCL falls */
  /* fast mode: min. 300ns */
  /* standard mode: min. 300ns */
  wait1us();

  /* slave has to release SDA (SDA high) */
  /* so we check this */
  n = I2C_PIN;                     /* get current state */
  n &= (1 << I2C_SDA);             /* filter SDA pin */
  if (n == 0)                      /* SDA low */
  {
    /* slave hasn't released SDA */
    Flag = I2C_ERROR;              /* signal error */
  }

  /* current state: SDA high / SCL low */

  return Flag;
}



#ifdef I2C_RW

/*
 *  read byte (master from slave)
 *  - read byte (data) from slave
 *    bit-bang 8 bits, MSB first, LSB last
 *  - ACK/NACK to slave
 *  - byte to be read is stored in global I2C.Byte
 *
 *  requires:
 *  - Type
 *    I2C_ACK for sending acknowledge
 *    I2C_NACK for sending not-acknowledge
 *
 *  returns:
 *  - I2C_ERROR on bus error
 *  - I2C_OK on success
 */

uint8_t I2C_ReadByte(uint8_t Type)
{
  uint8_t           Flag = I2C_OK;      /* return value */
  uint8_t           Byte = 0;           /* byte */
  uint8_t           n = 8;              /* counter */

  /* expected state: SDA undefined / SCL low */

  /*
   *  read byte
   */

  /* release SDA for slave (SDA high) */
  I2C_DDR &= ~(1 << I2C_SCA);

  /* bit-bang 8 bits */
  while (n > 0)               /* 8 bits */
  {
    /*
     *  run SCL cycle
     */

    /* SCL has to stay low for t_LOW */
    #ifdef I2C_FAST_MODE
      /* fast mode: min. 1.3탎 */
      wait2us();
    #else
      /* standard mode: min. 4.7탎 */
      wait5us();
    #endif

    /* set SCL high */
    I2C_DDR &= ~(1 << I2C_SCL);

    /* SCL has to stay high for t_HIGH */
    #ifdef I2C_FAST_MODE
      /* fast mode: min. 0.6탎 */
      wait1us();
    #else
      /* standard mode: min. 4탎 */
      wait4us();
    #endif

    /* set SCL low */
    I2C_DDR |= (1 << I2C_SCL);

    /* SDA has to stay valid for t_HD;DAT after SCL falls */
    /* fast mode: min. 300ns */
    /* standard mode: min. 300ns */

    /* new SDA has to become valid t_VD;DAT after SCL falls */
    /* fast mode: max. 0.9탎 */
    /* standard mode: max. 3.45탎 */

    /* read SDA */
    if (I2C_PIN & (1 << I2C_SDA))       /* SDA high */
    {
      Byte |= 0b00000001;               /* set bit */
    }

    n--;                      /* next bit */

    if (n > 0)
    {
      Byte <<= 1;             /* shift 1 bit left */
    }
  }

  I2C.Byte = Byte;            /* save byte */

  /* current state: SDA undefined / SCL low */


  /*
   *  send ACK/NACK
   */

  uint8_t           Flag = I2C_OK;      /* return value */


  /* expected state: SDA undefined / SCL low */

  /* check if slave has released SDA */
  n = I2C_PIN;                     /* get current state */
  n &= (1 << I2C_SDA);             /* filter SDA pin */
  if (n == 0)                      /* SDA low */
  {
    /* slave hasn't released SDA */
    Flag = I2C_ERROR;              /* signal error */
    return Flag;
  }  

  /* SDA has to become valid t_VD;ACK after read's last SCL fall */
  /* fast mode: max. 0.9탎 */
  /* standard mode: max. 3.45탎 */

  /* set SDA */
  if (Type == I2C_ACK)        /* ACK */
  {
    /* pull down SDA (SDA low) to signal ack to slave */
    I2C_DDR |= (1 << I2C_SDA);
  }

  /* else: NACK */
  /* we keep SDA high to signal nack to slave */


  /*
   *  run SCL cycle for ACK
   */

  /* SCL has to stay low for t_LOW */
  #ifdef I2C_FAST_MODE
    /* fast mode: min. 1.3탎 */
    wait2us();
  #else
    /* standard mode: min. 4.7탎 */
    wait5us();
  #endif

  /* set SCL high */
  I2C_DDR &= ~(1 << I2C_SCL);

  /* SCL has to stay high for t_HIGH */
  #ifdef I2C_FAST_MODE
    /* fast mode: min. 0.6탎 */
    wait1us();
  #else
    /* standard mode: min. 4탎 */
    wait4us();
  #endif

  /* set SCL low */
  I2C_DDR |= (1 << I2C_SCL);

  /* SDA has to stay valid for t_HD;DAT after SCL falls */
  /* fast mode: min. 300ns */
  /* standard mode: min. 300ns */
  wait1us();

  /* current state: SDA undefined / SCL low */

  return Flag;
}

#endif



/*
 *  create stop condition
 */

void I2C_Stop(void)
{
  uint8_t           Bits;     /* bitmask */

  /* expected state: SDA undefined / SCL low */

  Bits = I2C_DDR;        /* get current state */

  /* pull down SDA (SDA low) */
  Bits |= (1 << I2C_SDA);          /* set output mode */
  I2C_DDR = Bits;                  /* enable pull-down */

  /* SCL has to stay low for t_LOW */
  /* actually t_LOW - t_HD;DAT after ACK */
  #ifdef I2C_FAST_MODE
    /* fast mode: min. 1.3탎 */
    wait2us();
  #else
    /* standard mode: min. 4.7탎 */
    wait5us();
  #endif

  /* release SCL (SCL high) */
  Bits &= ~(1 << I2C_SCL);         /* set input mode */
  I2C_DDR = Bits;                  /* enable HiZ */

  /* SDA will follow after t_SU;STO */
  #ifdef I2C_FAST_MODE
    /* fast mode: min. 0.6탎 */
    wait1us();
  #else
    /* standard mode: min. 4탎 */
    wait4us();
  #endif

  /* release SDA (SDA high) */
  Bits &= ~(1 << I2C_SDA);         /* set input mode */
  I2C_DDR = Bits;                  /* enable HiZ */

  /*
   *  bus free time between Stop and next Start (T_BUF)
   *  - standard mode: min. 4.7탎
   *  - fast mode: min. 1.3탎
   */

  /* current state: SDA high / SCL high */
}

#endif



/* ************************************************************************
 *   functions for hardware TWI
 * ************************************************************************ */


#ifdef I2C_HARDWARE

/*
 *  set up TWI
 *
 *  returns:
 *  - I2C_ERROR on bus error
 *  - I2C_OK on success
 */

uint8_t I2C_Setup(void)
{
  uint8_t           Flag = I2C_OK;      /* return value */

  /*
   *  set bus speed:
   *  - SCL clock = MCU clock / (16 + 2*TWBR * prescaler)
   *  - available prescalers: 1, 4, 16 & 64
   *  - TWBR register: 1-255
   */

  /* set prescaler to 1 */
  TWSR = 0;           /* TWPS1=0 / TWPS0=0 */

  #ifdef I2C_FAST_MODE
    /* 400kHz */
    TWBR = CPU_FREQ / 400000;
  #else
    /* 100kHz */
    TWBR = CPU_FREQ / 100000;
  #endif

  return Flag;
}



/*
 *  create start condition
 *
 *  requires:
 *  - Type
 *    I2C_START for start condition
 *    I2C_REPEATED_START for repeated start condition
 *
 *  returns:
 *  - I2C_ERROR on bus error
 *  - I2C_OK on success
 */

uint8_t I2C_Start(uint8_t Type)
{
  uint8_t           Flag = I2C_ERROR;   /* return value */
  uint8_t           Bits;               /* bits */

  /*
   *  MCU's state maschine for TWI:
   *  - Start for new communication
   *  - Repeated Start after Start (no Stop yet)
   */

  /* set expected status flag */
  if (Type == I2C_START)      /* start */
  {
    Type = (1 << TWS3);       /* 0x08 */
  }
  else                        /* repeated start */
  {
    Type = (1 << TWS4);       /* 0x10 */
  }

  /*
   *  (repeated) start condition:
   *  - set TWINT to clear interrupt flag
   *  - set TWEN to enable TWI
   *  - set TWSTA for "start"
   */

  TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTA);

  /* wait for job done */
  while (!(TWCR & (1 << TWINT)));       /* wait for flag */

  /*
   *  check result
   */

  Bits = TWSR;                /* get status */
  /* filter status bits */
  Bits &= (1 << TWS7) | (1 << TWS6) | (1 << TWS5) | (1 << TWS4) | (1 << TWS3);

  if (Bits == Type)           /* got expected flag */
  {
    Flag = I2C_OK;            /* signal success */
  }

  return Flag;
}



/*
 *  write byte (master to slave)
 *  - send byte (address or data) to slave
 *  - get ACK/NACK from slave
 *  - byte to be sent is taken from global I2C.Byte
 *
 *  requires:
 *  - Type
 *    I2C_DATA for data byte
 *    I2C_ADDRESS for address byte
 *
 *  returns:
 *  - I2C_ERROR for bus error
 *  - I2C_ACK for ACK
 *  - I2C_NACK NACK
 */

uint8_t I2C_WriteByte(uint8_t Type)
{
  uint8_t           Flag = I2C_ERROR;   /* return value */
  uint8_t           Bits;               /* bits/bitmask */
  uint8_t           AckMask;            /* bitmask for ACK */
  uint8_t           NackMask;           /* bitmask for NACK */

  /*
   *  MCU's state maschine for TWI:
   *  - after Start the slave has to be addressed
   *  - after addressing the slave (write) data can be sent
   */


  Bits = I2C.Byte;            /* get byte */

  /* set status bitmasks */
  if (Type == I2C_DATA)            /* data byte */
  {
    AckMask = (1 << TWS5) | (1 << TWS3);     /* 0x28 data & ACK */
    NackMask = (1 << TWS5) | (1 << TWS4);    /* 0x30 data & NACK */
  }
  else                             /* address byte */
  {
    if (Bits & 0b00000001)         /* read address (bit #0 = 1) */
    {
      AckMask = (1 << TWS6);                 /* 0x40 SLA+R & ACK */
      NackMask = (1 << TWS6) | (1 << TWS3);  /* 0x48 SLA+R & NACK */
    }
    else                           /* write address (bit #0 = 0) */
    {
      AckMask = (1 << TWS4) | (1 << TWS3);   /* 0x18 SLA+W & ACK */
      NackMask = (1 << TWS5);                /* 0x20 SLA+W & NACK */
    }
  }

  /* load byte into data register */
  TWDR = Bits;

  /* send by clearing TWINT */
  TWCR = (1 << TWINT) | (1 << TWEN);

  /* wait for job done */
  while (!(TWCR & (1 << TWINT)));       /* wait for flag */

  /* check result */
  Bits = TWSR;                /* get status */
  /* filter status bits */
  Bits &= (1 << TWS7) | (1 << TWS6) | (1 << TWS5) | (1 << TWS4) | (1 << TWS3);

  if (Bits == AckMask)        /* got expected bits for ACK */
  {
    Flag = I2C_ACK;           /* signal ACK */
  }
  else if (Bits == NackMask)  /* got expected bits for NACK */
  {
    Flag = I2C_NACK;          /* signal NACK */
  }
  /* else 0x38: keep default "bus error" */

  return Flag;
}



#ifdef I2C_RW

/*
 *  read byte (master from slave)
 *  - read byte (data) from slave
 *  - ACK/NACK to slave
 *  - byte to be read is stored in global I2C.Byte
 *
 *  requires:
 *  - Type
 *    I2C_ACK for sending acknowledge
 *    I2C_NACK for sending not-acknowledge
 *
 *  returns:
 *  - I2C_ERROR on bus error
 *  - I2C_OK on success
 */

uint8_t I2C_ReadByte(uint8_t Type)
{
  uint8_t           Flag = I2C_ERROR;   /* return value */
  uint8_t           Bits;               /* bits/bitmask */
  uint8_t           StatusMask;         /* bitmask */

  /*
   *  MCU's state maschine for TWI:
   *  - after addressing slave (read) data can be read 
   */

  /* receive by clearing TWINT */
  if (Type == I2C_ACK)             /* ACK */
  {
    /* read and respond with ACK */
    TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);

    StatusMask = (1 << TWS6) | (1 << TWS4);  /* 0x50 data read & ACK */
  }
  else                             /* NACK */
  {
    /* read and respond with NACK */
    TWCR = (1 << TWINT) | (1 << TWEN);

    StatusMask = (1 << TWS6) | (1 << TWS4) | (1 << TWS3);  /* 0x58 data read & NACK */
  }

  /* wait for job done */
  while (!(TWCR & (1 << TWINT)));       /* wait for flag */

  /* check result */
  Bits = TWSR;                /* get status */
  /* filter status bits */
  Bits &= (1 << TWS7) | (1 << TWS6) | (1 << TWS5) | (1 << TWS4) | (1 << TWS3);

  if (Bits == StatusMask)     /* got expected bits */
  {
    Flag = I2C_OK;            /* signal success */
  }
  /* else 0x38: keep default "bus error" */

  return Flag;
}

#endif



/*
 *  create stop condition
 */

void I2C_Stop(void)
{
  /*
   *  stop condition:
   *  - set TWINT to clear interrupt flag
   *  - set TWEN to enable TWI
   *  - set TWSTO for "stop"
   */

  TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);

  /* in this case TWINT won't be set after the job is done */


  /*
   *  bus free time between Stop and next Start (T_BUF)
   *  - standard mode: min. 4.7탎
   *  - fast mode: min. 1.3탎
   */

  #ifdef I2C_FAST_MODE
    /* fast mode: min. 1.3탎 */
    wait5us();
  #else
    /* standard mode: min. 4.7탎 */
    wait20us();
  #endif
}


#endif



/* ************************************************************************
 *   clean-up of local constants
 * ************************************************************************ */

/* source management */
#undef I2C_C

#endif

/* ************************************************************************
 *   EOF
 * ************************************************************************ */
