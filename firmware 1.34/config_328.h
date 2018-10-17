/* ************************************************************************
 *
 *   ATmega 328 specific global configuration, setup and settings
 *
 *   (c) 2012-2018 by Markus Reschke
 *   based on code from Markus Frejek and Karl-Heinz Kübbeler
 *
 * ************************************************************************ */


/* source management */
#define CONFIG_328_H



/* ************************************************************************
 *   LCD module
 * ************************************************************************ */


/*
 *  LCD module / controller
 *
 *  Please uncomment the package matching your LCD module
 *  and adjust settings.
 *
 *  To uncomment, remove the enclosing "#if 0" and "#endif" or
 *  put a "//" in front of both.
 */


/*
 *  HD44780, 4 bit parallel interface
 *  - if you change LCD_DB4/5/6/7 comment out LCD_DB_STD!
 */

#if 0
#define LCD_HD44780
#define LCD_TEXT                        /* character display */
#define LCD_PAR_4                       /* 4 bit parallel interface */
#define LCD_PORT         PORTD          /* port data register */
#define LCD_DDR          DDRD           /* port data direction register */
#define LCD_DB_STD                      /* use standard pins 0-3 for DB4-7 */
#define LCD_DB4          PD0            /* port pin used for DB4 */
#define LCD_DB5          PD1            /* port pin used for DB5 */
#define LCD_DB6          PD2            /* port pin used for DB6 */
#define LCD_DB7          PD3            /* port pin used for DB7 */
#define LCD_RS           PD4            /* port pin used for RS */
#define LCD_EN1          PD5            /* port pin used for E */
#define LCD_CHAR_X       16             /* characters per line */
#define LCD_CHAR_Y       2              /* number of lines */
#define FONT_HD44780_INT                /* internal 5x7 font: international */
//#define FONT_HD44780_CYR                /* internal 5x7 font: Cyrillic */
#endif



/*
 *  HD44780, PCF8574 based backpack (bit-bang I2C)
 *  - if you change LCD_DB4/5/6/7 comment out LCD_DB_STD!
 *  - PCF8574T is 0x27, PCF8574AT is 0x3f
 */

#if 0
#define LCD_HD44780
#define LCD_TEXT                        /* character display */
#define LCD_PCF8574                     /* PCF8574 backpack */
#define LCD_I2C_ADDR     0x3f           /* PCF8574's I2C address */
#define LCD_DB_STD                      /* use standard pins 4-7 for DB4-7 */
#define LCD_DB4          PCF8574_P4     /* port pin used for DB4 */
#define LCD_DB5          PCF8574_P5     /* port pin used for DB5 */
#define LCD_DB6          PCF8574_P6     /* port pin used for DB6 */
#define LCD_DB7          PCF8574_P7     /* port pin used for DB7 */
#define LCD_RS           PCF8574_P0     /* port pin used for RS */
#define LCD_RW           PCF8574_P1     /* port pin used for RW */
#define LCD_EN1          PCF8574_P2     /* port pin used for E */
#define LCD_LED          PCF8574_P3     /* port pin used for backlight */
#define LCD_CHAR_X       16             /* characters per line */
#define LCD_CHAR_Y       2              /* number of lines */
#define FONT_HD44780_INT                /* internal 5x7 font: international */
//#define FONT_HD44780_CYR                /* internal 5x7 font: Cyrillic */
#define I2C_BITBANG                     /* bit-bang I2C */
#define I2C_STANDARD_MODE               /* 100kHz bus speed */
#define I2C_PORT         PORTD          /* I2C port data register */
#define I2C_DDR          DDRD           /* I2C port data direction register */
#define I2C_PIN          PIND           /* I2C port input pins register */
#define I2C_SDA          PD0            /* port pin used for SDA */
#define I2C_SCL          PD1            /* port pin used for SCL */
#endif



/*
 *  ST7565R, SPI interface (bit-bang)
 *  - settings for Electronic Assembly EA DOGM/DOGL128-6
 *  - uses LCD_CS to support rotary encoder in parallel at PD2/3
 */

//#if 0
#define LCD_ST7565R
#define LCD_GRAPHIC                     /* graphic display */
#define LCD_SPI                         /* SPI interface */
#define LCD_PORT         PORTD          /* port data register */
#define LCD_DDR          DDRD           /* port data direction register */
#define LCD_RESET        PD0            /* port pin used for /RES (optional) */
#define LCD_CS           PD5            /* port pin used for /CS1 (optional) */
#define LCD_A0           PD1            /* port pin used for A0 */
#define LCD_SCL          PD2            /* port pin used for SCL */
#define LCD_SI           PD3            /* port pin used for SI (LCD's data input) */
#define LCD_DOTS_X       128            /* number of horizontal dots */
#define LCD_DOTS_Y       64             /* number of vertical dots */
//#define LCD_FLIP_X                      /* enable horizontal flip */
#define LCD_OFFSET_X                    /* enable x offset of 4 dots */
#define LCD_FLIP_Y                      /* enable vertical flip */
#define LCD_START_Y      0              /* start line (0-63) */
#define LCD_CONTRAST     22             /* default contrast (0-63) */
#define FONT_8X8_VF                     /* 8x8 font, vertically aligned & flipped */
//#define FONT_8X16_VFP                   /* 8x16 font, vertically aligned & flipped */
#define SYMBOLS_24X24_VFP               /* 24x24 symbols, vertically aligned & flipped */
#define SPI_BITBANG                     /* bit-bang SPI */
#define SPI_PORT         LCD_PORT       /* SPI port data register */
#define SPI_DDR          LCD_DDR        /* SPI port data direction register */
#define SPI_SCK          LCD_SCL        /* port pin used for SCK */
#define SPI_MOSI         LCD_SI         /* port pin used for MOSI */
//#endif



/*
 *  ILI9342, SPI interface (bit-bang)
 */

#if 0
#define LCD_ILI9341
#define LCD_GRAPHIC                     /* graphic display */
#define LCD_COLOR                       /* color display */
#define LCD_SPI                         /* SPI interface */
#define LCD_PORT         PORTD          /* port data register */
#define LCD_DDR          DDRD           /* port data direction register */
#define LCD_RES          PD4            /* port pin used for /RES (optional) */
#define LCD_CS           PD5            /* port pin used for /CS (optional) */
#define LCD_DC           PD3            /* port pin used for D/C */
#define LCD_SCK          PD2            /* port pin used for SCK */
#define LCD_SDI          PD1            /* port pin used for SDI (LCD's data input) */
//#define LCD_SDO          PD0            /* port pin used for SDO (LCD's data output) */
#define LCD_DOTS_X       320            /* number of horizontal dots */
#define LCD_DOTS_Y       240            /* number of vertical dots */
//#define LCD_FLIP_X                      /* enable horizontal flip */
//#define LCD_FLIP_Y                      /* enable vertical flip */
//#define LCD_ROTATE                      /* switch X and Y (rotate by 90°) */
#define FONT_16X26_HF                   /* 16x26 font, horizontally aligned & flipped */
#define SYMBOLS_32X32_HF                /* 32x32 symbols, horizontally aligned & flipped */
#define SPI_BITBANG                     /* bit-bang SPI */
#define SPI_PORT         LCD_PORT       /* SPI port data register */
#define SPI_DDR          LCD_DDR        /* SPI port data direction register */
#define SPI_SCK          LCD_SCK        /* port pin used for SCK */
#define SPI_MOSI         LCD_SDI        /* port pin used for MOSI */
#endif



/*
 *  ST7735, SPI interface (bit-bang, 4 wire)
 */

#if 0
#define LCD_ST7735
#define LCD_GRAPHIC                     /* graphic display */
#define LCD_COLOR                       /* color display */
#define LCD_SPI                         /* SPI interface */
#define LCD_PORT         PORTD          /* port data register */
#define LCD_DDR          DDRD           /* port data direction register */
#define LCD_RES          PD4            /* port pin used for /RESX (optional) */
#define LCD_CS           PD5            /* port pin used for /CSX (optional) */
#define LCD_DC           PD3            /* port pin used for D/CX */
#define LCD_SCL          PD2            /* port pin used for SCL */
#define LCD_SDA          PD1            /* port pin used for SDA */
#define LCD_DOTS_X       128            /* number of horizontal dots */
#define LCD_DOTS_Y       160            /* number of vertical dots */
//#define LCD_FLIP_X                      /* enable horizontal flip */
#define LCD_FLIP_Y                      /* enable vertical flip */
#define LCD_ROTATE                      /* switch X and Y (rotate by 90°) */
//#define LCD_OFFSET_X     4               /* enable x offset of 2 or 4 dots */
//#define LCD_OFFSET_Y     2               /* enable y offset of 1 or 2 dots */
//#define LCD_LATE_ON                     /* turn on LCD after clearing it */
#define FONT_10X16_HF                   /* 10x16 font, horizontally aligned & flipped */
//#define FONT_8X16_CYRILLIC_HF           /* 8x16 cyrillic font, horizontally aligned & flipped */
#define SYMBOLS_30X32_HF                /* 30x32 symbols, horizontally aligned & flipped */
#define SPI_BITBANG                     /* bit-bang SPI */
#define SPI_PORT         LCD_PORT       /* SPI port data register */
#define SPI_DDR          LCD_DDR        /* SPI port data direction register */
#define SPI_SCK          LCD_SCL        /* port pin used for SCK */
#define SPI_MOSI         LCD_SDA        /* port pin used for MOSI */
#endif



/*
 *  PCD8544, SPI interface (bit-bang)
 *  - for a 180° rotated display (LCD_ROT180)
 *    - comment out "_VF" font and "_VFP" symbols
 *    - uncomment "_V_F" font and "_VP_F" symbols
 */

#if 0
#define LCD_PCD8544
#define LCD_GRAPHIC                     /* graphic display */
#define LCD_SPI                         /* SPI interface */
#define LCD_PORT         PORTD          /* port data register */
#define LCD_DDR          DDRD           /* port data direction register */
#define LCD_RES          PD4            /* port pin used for /RES (optional) */
#define LCD_SCE          PD5            /* port pin used for /SCE (optional) */
#define LCD_DC           PD3            /* port pin used for D/C */
#define LCD_SCLK         PD2            /* port pin used for SCLK */
#define LCD_SDIN         PD1            /* port pin used for SDIN (LCD's data input) */
#define LCD_DOTS_X       84             /* number of horizontal dots */
#define LCD_DOTS_Y       48             /* number of vertical dots */
#define LCD_CONTRAST     66             /* default contrast (1-127) */
#define FONT_6X8_VF                     /* 6x8 font, vertically aligned & flipped */
#define SYMBOLS_24X24_VFP               /* 24x24 symbols, vertically aligned & flipped */
//#define LCD_ROT180                      /* rotate output by 180° (not supported yet) */
//#define FONT_6X8_V_F                    /* 6x8 font, vertically aligned, hor. flipped */
//#define SYMBOLS_24X24_VP_F              /* 24x24 symbols, vertically aligned, hor. flipped */
#define SPI_BITBANG                     /* bit-bang SPI */
#define SPI_PORT         LCD_PORT       /* SPI port data register */
#define SPI_DDR          LCD_DDR        /* SPI port data direction register */
#define SPI_SCK          LCD_SCLK       /* port pin used for SCK */
#define SPI_MOSI         LCD_SDIN       /* port pin used for MOSI */
#endif



/*
 *  ST7920, SPI interface (bit-bang)
 *  - for a 180° rotated display (LCD_ROT180)
 *    - comment out "_H" font and symbol
 *    - uncomment "_HF" font and symbol
 */

#if 0
#define LCD_ST7920
#define LCD_GRAPHIC                     /* graphic display */
#define LCD_SPI                         /* SPI interface */
#define LCD_PORT         PORTD          /* port data register */
#define LCD_DDR          DDRD           /* port data direction register */
#define LCD_RESET        PD4            /* port pin used for /RESET (optional) */
#define LCD_CS           PD5            /* port pin used for CS (optional) */
#define LCD_SCLK         PD2            /* port pin used for SCLK */
#define LCD_SID          PD1            /* port pin used for SID (LCD's data input) */
#define LCD_DOTS_X       128            /* number of horizontal dots */
#define LCD_DOTS_Y       64             /* number of vertical dots */
#define FONT_8X8_H                      /* 8x8 font, horizonally aligned */
#define SYMBOLS_24X24_H                 /* 24x24 symbols, horizonally aligned */
//#define LCD_ROT180                      /* rotate output by 180° */
//#define FONT_8X8_HF                     /* 8x8 font, horizonally aligned & flipped */
//#define SYMBOLS_24X24_HF                /* 24x24 symbols, horizonally aligned & flipped */
#define SPI_BITBANG                     /* bit-bang SPI */
#define SPI_PORT         LCD_PORT       /* SPI port data register */
#define SPI_DDR          LCD_DDR        /* SPI port data direction register */
#define SPI_SCK          LCD_SCLK       /* port pin used for SCK */
#define SPI_MOSI         LCD_SID        /* port pin used for MOSI */
#endif



/*
 *  ST7920, 4 bit parallel interface
 *  - if you change LCD_DB4/5/6/7 comment out LCD_DB_STD!
 *  - for a 180° rotated display (LCD_ROT180)
 *    - comment out "_H" font and symbol
 *    - uncomment "_HF" font and symbol
 */

#if 0
#define LCD_ST7920
#define LCD_GRAPHIC                     /* graphic display */
#define LCD_PAR_4                       /* 4 bit parallel interface */
#define LCD_PORT         PORTD          /* port data register */
#define LCD_DDR          DDRD           /* port data direction register */
#define LCD_DB_STD                      /* use standard pins 0-3 for DB4-7 */
#define LCD_DB4          PD0            /* port pin used for DB4 */
#define LCD_DB5          PD1            /* port pin used for DB5 */
#define LCD_DB6          PD2            /* port pin used for DB6 */
#define LCD_DB7          PD3            /* port pin used for DB7 */
#define LCD_RS           PD4            /* port pin used for RS */
#define LCD_EN           PD5            /* port pin used for E */
#define LCD_DOTS_X       128            /* number of horizontal dots */
#define LCD_DOTS_Y       64             /* number of vertical dots */
#define FONT_8X8_H                      /* 8x8 font, horizonally aligned */
#define SYMBOLS_24X24_H                 /* 24x24 symbols, horizonally aligned */
//#define LCD_ROT180                      /* rotate output by 180° */
//#define FONT_8X8_HF                     /* 8x8 font, horizonally aligned & flipped */
//#define SYMBOLS_24X24_HF                /* 24x24 symbols, horizonally aligned & flipped */
#endif


/*
 *  ILI9163, SPI interface (bit-bang, 4 wire)
 */

#if 0
#define LCD_ILI9163
#define LCD_GRAPHIC                     /* graphic display */
#define LCD_COLOR                       /* color display */
#define LCD_SPI                         /* SPI interface */
#define LCD_PORT         PORTD          /* port data register */
#define LCD_DDR          DDRD           /* port data direction register */
#define LCD_RES          PD4            /* port pin used for /RESX (optional) */
#define LCD_CS           PD5            /* port pin used for /CSX (optional) */
#define LCD_DC           PD3            /* port pin used for D/CX */
#define LCD_SCL          PD2            /* port pin used for SCL */
#define LCD_SDA          PD1            /* port pin used for SDA/SDIO */
#define LCD_DOTS_X       128            /* number of horizontal dots */
#define LCD_DOTS_Y       128            /* number of vertical dots */
#define LCD_OFFSET_X      32            /* x offset of 32 dots (160-128) */
//#define LCD_FLIP_X                      /* enable horizontal flip */
#define LCD_FLIP_Y                      /* enable vertical flip */
#define LCD_ROTATE                      /* switch X and Y (rotate by 90°) */
//#define LCD_LATE_ON                     /* turn on LCD after clearing it */
#define FONT_8X8_HF                     /* 8x8 font, horizonally aligned & flipped */
//#define FONT_10X16_HF                   /* 10x16 font, horizontally aligned & flipped */
//#define FONT_8X16_CYRILLIC_HF           /* 8x16 cyrillic font, horizontally aligned & flipped */
#define SYMBOLS_30X32_HF                /* 30x32 symbols, horizontally aligned & flipped */
#define SPI_BITBANG                     /* bit-bang SPI */
#define SPI_PORT         LCD_PORT       /* SPI port data register */
#define SPI_DDR          LCD_DDR        /* SPI port data direction register */
#define SPI_SCK          LCD_SCL        /* port pin used for SCK */
#define SPI_MOSI         LCD_SDA        /* port pin used for MOSI */
#endif



/*
 *  SSD1306, SPI interface (bit-bang, 4 wire)
 */

#if 0
#define LCD_SSD1306
#define LCD_GRAPHIC                     /* graphic display */
#define LCD_SPI                         /* SPI interface */
#define LCD_PORT         PORTD          /* port data register */
#define LCD_DDR          DDRD           /* port data direction register */
#define LCD_RESET        PD4            /* port pin used for /RES (optional) */
#define LCD_CS           PD5            /* port pin used for /CS (optional) */
#define LCD_DC           PD3            /* port pin used for D/C */
#define LCD_SCLK         PD2            /* port pin used for SCLK */
#define LCD_SDIN         PD1            /* port pin used for SDIN (LCD's data input) */
#define LCD_DOTS_X       128            /* number of horizontal dots */
#define LCD_DOTS_Y       64             /* number of vertical dots */
#define LCD_FLIP_X                      /* enable horizontal flip */
#define LCD_FLIP_Y                      /* enable vertical flip */
#define LCD_CONTRAST     127            /* default contrast (0-255) */
#define FONT_8X8_VF                     /* 8x8 font, vertically aligned & flipped */
#define SYMBOLS_24X24_VFP               /* 24x24 symbols, vertically aligned & flipped */
#define SPI_BITBANG                     /* bit-bang SPI */
#define SPI_PORT         LCD_PORT       /* SPI port data register */
#define SPI_DDR          LCD_DDR        /* SPI port data direction register */
#define SPI_SCK          LCD_SCLK       /* port pin used for SCK */
#define SPI_MOSI         LCD_SDIN       /* port pin used for MOSI */
#endif



/*
 *  SSD1306, I2C interface (bit-bang)
 */

#if 0
#define LCD_SSD1306
#define LCD_GRAPHIC                     /* graphic display */
#define LCD_I2C                         /* I2C interface */
#define LCD_I2C_ADDR     0x3c           /* SSD1306's I2C address */
#define LCD_PORT         PORTD          /* port data register */
#define LCD_DDR          DDRD           /* port data direction register */
#define LCD_RESET        PD4            /* port pin used for /RES (optional) */
#define LCD_DOTS_X       128            /* number of horizontal dots */
#define LCD_DOTS_Y       64             /* number of vertical dots */
#define LCD_FLIP_X                      /* enable horizontal flip */
#define LCD_FLIP_Y                      /* enable vertical flip */
#define LCD_CONTRAST     127            /* default contrast (0-255) */
#define FONT_8X8_VF                     /* 8x8 font, vertically aligned & flipped */
#define SYMBOLS_24X24_VFP               /* 24x24 symbols, vertically aligned & flipped */
#define I2C_BITBANG                     /* bit-bang I2C */
#define I2C_FAST_MODE                   /* 400kHz bus speed */
#define I2C_PORT         PORTD          /* I2C port data register */
#define I2C_DDR          DDRD           /* I2C port data direction register */
#define I2C_PIN          PIND           /* I2C port input pins register */
#define I2C_SDA          PD0            /* port pin used for SDA */
#define I2C_SCL          PD1            /* port pin used for SCL */
#endif



/*
 *  VT100 serial terminal, TTL serial
 */

#if 0
#define LCD_VT100
#define LCD_TEXT                        /* character display */
#define LCD_COLOR                       /* color display */
#define LCD_CHAR_X       40             /* characters per line */
#define LCD_CHAR_Y       24             /* number of lines */
#define SERIAL_BITBANG                  /* bit-bang serial */
//#define SERIAL_HARDWARE                 /* hardware serial */
#endif



/*
 *  check if a LCD module is specified
 */

#if !defined(LCD_TEXT) && !defined(LCD_GRAPHIC)
  #error <<< No LCD module specified! >>>
#endif



/* ************************************************************************
 *   port and pin assignments
 * ************************************************************************ */


/*
 *  Test probes:
 *  - Must be an ADC port :-)
 *  - Lower 3 pins of the port must be used for probe pins.
 *  - Please don't change the definitions of TP1, TP2 and TP3!
 */

#define ADC_PORT         PORTC     /* ADC port data register */
#define ADC_DDR          DDRC      /* ADC port data direction register */
#define ADC_PIN          PINC      /* port input pins register */
#define TP1              PC0       /* test pin 1 */
#define TP2              PC1       /* test pin 2 */
#define TP3              PC2       /* test pin 3 */

#define TP_ZENER         PC3       /* test pin with 10:1 voltage divider */
#define TP_REF           PC4       /* test pin with 2.5V reference and relay */
#define TP_BAT           PC5       /* test pin with 4:1 voltage divider */


/*
 *  Probe resistors
 *  - For PWM/squarewave output via probe #2 R_RL_2 has to be PB2/OC1B.
 */

#define R_PORT           PORTB     /* port data register */
#define R_DDR            DDRB      /* port data direction register */
#define R_RL_1           PB0       /* Rl (680R) for test pin #1 */
#define R_RH_1           PB1       /* Rh (470k) for test pin #1 */
#define R_RL_2           PB2       /* Rl (680R) for test pin #2 */
#define R_RH_2           PB3       /* Rh (470k) for test pin #2 */
#define R_RL_3           PB4       /* Rl (680R) for test pin #3 */
#define R_RH_3           PB5       /* Rh (470k) for test pin #3 */


/*
 *  dedicated signal output via OC1B
 *  - don't change this!
 */

#define SIGNAL_PORT      PORTB     /* port data register */
#define SIGNAL_DDR       DDRB      /* port data direction register */
#define SIGNAL_OUT       PB2       /* MCU's OC1B pin */


/*
 *  test push button and power management
 */

#define CONTROL_PORT     PORTD     /* port data register */
#define CONTROL_DDR      DDRD      /* port data direction register */
#define CONTROL_PIN      PIND      /* port input pins register */
#define POWER_CTRL       PD6       /* controls power (1: on / 0: off) */
#define TEST_BUTTON      PD7       /* test/start push button (low active) */


/*
 *  rotary encoder
 */

#define ENCODER_PORT     PORTD     /* port data register */
#define ENCODER_DDR      DDRD      /* port data direction register */
#define ENCODER_PIN      PIND      /* port input pins register */
#define ENCODER_A        PD2       /* rotary encoder A signal */
#define ENCODER_B        PD3       /* rotary encoder B signal */


/*
 *  increase/decrease push buttons
 */

#define KEY_PORT         PORTD     /* port data register */
#define KEY_DDR          DDRD      /* port data direction register */
#define KEY_PIN          PIND      /* port input pins register */
#define KEY_INC          PD2       /* increase push button */
#define KEY_DEC          PD3       /* decrease push button */


/*
 *  frequency counter
 *  - input must be pin PD4/T0
 */

#define COUNTER_PORT     PORTD     /* port data register */
#define COUNTER_DDR      DDRD      /* port data direction register */
#define COUNTER_IN       PD4       /* signal input T0 */


/*
 *  IR detector/decoder
 *  - fixed module connected to dedicated I/O pin
 */

#define IR_PORT          PORTC     /* port data register */
#define IR_DDR           DDRC      /* port data direction register */
#define IR_PIN           PINC      /* port input pins register */
#define IR_DATA          PC6       /* data signal */


/*
 *  SPI
 *  - hardware SPI uses PB5, PB3 and PB4
 *  - could be already set in display section
 */

/* for bit-bang SPI */
#ifndef SPI_PORT
#define SPI_PORT         PORTB     /* port data register */
#define SPI_DDR          DDRB      /* port data direction register */
#define SPI_PIN          PINB      /* port input pins register */
#define SPI_SCK          PB5       /* pin for SCK */
#define SPI_MOSI         PB3       /* pin for MOSI */
#define SPI_MISO         PB4       /* pin for MISO */
#endif


/*
 *  I2C
 *  - hardware I2C (TWI) uses PC4 & PC5
 *  - could be already set in display section
 */

/* for bit-bang I2C */
#ifndef I2C_PORT
#define I2C_PORT         PORTD     /* port data register */
#define I2C_DDR          DDRD      /* port data direction register */
#define I2C_PIN          PIND      /* port input pins register */
#define I2C_SDA          PC4       /* pin for SDA */
#define I2C_SCL          PC5       /* pin for SCL */
#endif


/*
 *  TTL serial interface
 *  - hardware USART0 uses PD0 & PD1
 */

/* for hardware RS232 */
#define SERIAL_USART     0         /* use USART0 */
/* for bit-bang RS232 */
#define SERIAL_PORT      PORTD     /* port data register */
#define SERIAL_DDR       DDRD      /* port data direction register */
#define SERIAL_PIN       PIND      /* port input pins register */
#define SERIAL_TX        PD1       /* pin for Tx (transmit) */
#define SERIAL_RX        PD0       /* pin for Rx (receive) */
#define SERIAL_PCINT     16        /* PCINT# for Rx pin */


/*
 *  OneWire
 *  - dedicated I/O pin
 */

#define ONEWIRE_PORT     PORTC     /* port data register */
#define ONEWIRE_DDR      DDRC      /* port data direction register */
#define ONEWIRE_PIN      PINC      /* port input pins register */
#define ONEWIRE_DQ       PC6       /* DQ (data line) */



/* ************************************************************************
 *   internal stuff
 * ************************************************************************ */


/* ADC reference selection: AVcc */
#define ADC_REF_VCC           (1 << REFS0)

/* ADC reference selection: internal 1.1V bandgap */
#define ADC_REF_BANDGAP       ((1 << REFS1) | (1 << REFS0))

/* ADC reference selection bit mask */
#define ADC_REF_MASK          ((1 << REFS1) | (1 << REFS0))

/* ADC MUX channel for internal 1.1V bandgap reference */
#define ADC_BANDGAP      0x0e      /* 1110 */



/* ************************************************************************
 *   MCU specific setup to support different AVRs
 * ************************************************************************ */


/*
 *  ATmega 328/328P
 */

#if defined(__AVR_ATmega328__)

  /* estimated internal resistance of port to GND (in 0.1 Ohms) */
  #define R_MCU_LOW           200

  /* estimated internal resistance of port to VCC (in 0.1 Ohms) */
  #define R_MCU_HIGH          220

  /* voltage offset of MCU's analog comparator (in mV): -50 up to 50 */
  #define COMPARATOR_OFFSET   0

  /* capacitance of the probe tracks of the PCB and the MCU (in pF) */
  #define CAP_PCB             32

  /* this MCU has 32kB Flash, 1kB EEPROM and 2kB RAM (enable extra features) */
  #define RES_FLASH           32
  #define RES_EEPROM          1
  #define RES_RAM             2


/*
 *  missing or unsupported MCU
 */

#else
  #error <<< No or wrong MCU type selected! >>>
#endif



/* ************************************************************************
 *   EOF
 * ************************************************************************ */
