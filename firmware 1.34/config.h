/* ************************************************************************
 *
 *   global configuration, setup and settings
 *
 *   (c) 2012-2018 by Markus Reschke
 *   based on code from Markus Frejek and Karl-Heinz Kübbeler
 *
 * ************************************************************************ */


/* source management */
#define CONFIG_H


/*
 *  For MCU specific settings (port and pin assignments) and LCD display
 *  settings please edit:
 *  - ATmega328:           config_328.h
 *  - ATmega324/644/1284:  config_644.h
 */



/* ************************************************************************
 *   Hardware options
 * ************************************************************************ */


/*
 *  rotary encoder for user interface
 *  - default pins: PD2 & PD3 (ATmega 328)
 *  - could be in parallel with LCD module
 *  - see ENCODER_PORT for port pins (config-<MCU>.h)
 *  - uncomment to enable and also set ENCODER_PULSES & ENCODER_STEPS below
 *    to match your rotary encoder
 */

//#define HW_ENCODER


/*
 *  Number of Gray code pulses per step or detent for the rotary encoder
 *  - typical values: 2 or 4, rarely 1
 *  - a rotary encoder's pulse is the complete sequence of 4 Gray code pulses
 *  - adjust value to match your rotary encoder
 */

#define ENCODER_PULSES   4


/*
 *  Number of detents or steps
 *  - this is used by the detection of the rotary encoder's turning velocity
 *  - it doesn't have to match exactly and also allows you to finetune the
 *    the feedback (higher: slow down, lower: speed up)
 *  - typical values: 20, 24 or 30 
 *  - adjust value to match your rotary encoder
 */

#define ENCODER_STEPS    24


/*
 *  increase/decrease push buttons for user interface
 *  - alternative for rotary encoder
 *  - see KEY_PORT for port pins (config-<MCU>.h)
 *  - uncomment to enable
 */

//#define HW_INCDEC_KEYS


/*
 *  2.5V voltage reference for Vcc check
 *  - default pin: PC4 (ATmega 328)
 *  - should be at least 10 times more precise than the voltage regulator
 *  - see TP_REF for port pin (config-<MCU>.h)
 *  - uncomment to enable and also adjust UREF_25 below for your voltage
 *    reference
 */

//#define HW_REF25


/*
 *  Typical voltage of 2.5V voltage reference (in mV)
 *  - see datasheet of the voltage reference
 *  - or use >= 5.5 digit DMM to measure the voltage
 */

#define UREF_25           2495


/*
 *  Probe protection relay for discharging caps
 *  - default pin: PC4 (ATmega 328)
 *  - low signal: short circuit probe pins
 *    high signal via external reference: remove short circuit 
 *  - uncomment to enable
 */

//#define HW_DISCHARGE_RELAY


/*
 *  voltage measurement up to 50V DC
 *  - default pin: PC3 (ATmega 328)
 *  - 10:1 voltage divider
 *  - for Zener diodes
 *  - DC-DC boost converter controled by test push button
 *  - see TP_BAT for port pin
 *  - uncomment to enable
 */

//#define HW_ZENER


/*
 *  fixed signal output
 *  - in case MCU's OC1B pin is wired as dedicated signal output
 *    instead of driving Rl probe resistor for test pin #2
 *  - uncomment to enable
 */

//#define HW_FIXED_SIGNAL_OUTPUT


/*
 *  basic frequency counter
 *  - default pin: T0 (PD4 ATmega 328)
 *  - uses T0 directly as frequency input
 *  - counts up to 1/4 of MCU clock rate
 *  - might be in parallel with LCD module
 *  - uncomment to enable
 */

//#define HW_FREQ_COUNTER_BASIC


/*
 *  extended frequency counter
 *  - low and high frequency crystal oscillators
 *    and buffered frequency input
 *  - prescalers 1:1 and 16:1 (32:1)
 *  - see COUNTER_PORT for port pins (config-<MCU>.h)
 *  - requires a display with more than 2 text lines
 *  - uncomment to enable (not supported yet)
 *  - select the circuit's prescaler setting: either 16:1 or 32:1 
 */

//#define HW_FREQ_COUNTER_EXT
#define FREQ_COUNTER_PRESCALER   16   /* 16:1 */
//#define FREQ_COUNTER_PRESCALER   32   /* 32:1 */


/*
 *  IR remote control detection/decoder (via dedicated MCU pin)
 *  - requires IR receiver module, e.g. TSOP series
 *  - module is connected to fixed I/O pin
 *  - see IR_PORT for port pin (config-<MCU>.h)
 *  - uncomment to enable
 */

//#define HW_IR_RECEIVER


/*
 *  fixed cap for self-adjustment
 *  - see TP_CAP and ADJUST_PORT for port pins (config-<MCU>.h)
 *  - uncomment to enable
 */

//#define HW_ADJUST_CAP


/*
 *  relay for parallel cap (sampling ADC)
 *  - uncomment to enable (not implemented yet)
 */

//#define HW_CAP_RELAY



/* ************************************************************************
 *   software options
 * ************************************************************************ */


/*
 *  PWM generator with simple user interface
 *  - uncomment to enable
 */

#define SW_PWM_SIMPLE


/*
 *  PWM generator with fancy user interface
 *  - requires additional keys and display with more than 2 text lines
 *  - uncomment to enable
 */

//#define SW_PWM_PLUS


/*
 *  Inductance measurement
 *  - uncomment to enable
 */

#define SW_INDUCTOR


/*
 *  ESR measurement and in-circuit ESR measurement
 *  - requires MCU clock >= 8 MHz
 *  - choose SW_OLD_ESR for old method starting at 180nF
 *  - uncomment to enable
 */

#define SW_ESR
//#define SW_OLD_ESR


/*
 *  check for rotary encoders
 *  - uncomment to enable
 */

//#define SW_ENCODER


/*
 *  squarewave signal generator
 *  - requires additional keys
 *  - uncomment to enable
 */

#define SW_SQUAREWAVE


/*
 *  IR remote control detection/decoder (via probes)
 *  - requires IR receiver module, e.g. TSOP series
 *  - module will be connected to probe leads
 *  - uncomment to enable
 */

#define SW_IR_RECEIVER


/*
 *  current limiting resistor for IR receiver module
 *  - for 5V only modules
 *  - Warning: any short circuit may destroy your MCU
 *  - uncomment to disable resistor
 */

//#define SW_IR_DISABLE_RESISTOR


/*
 *  IR remote control sender
 *  - requires additional keys and display with more than 4 text lines
 *  - also requires an IR LED with a simple driver
 *  - uncomment to enable
 */

//#define SW_IR_TRANSMITTER


/*
 *  additional protocols for IR remote control detection/decoder
 *  and sender
 *  - uncommon protocols which increase flash memory usage ;)
 *  - uncomment to enable
 */

//#define SW_IR_EXTRA


/*
 *  check for opto couplers
 *  - uncomment to enable
 */

#define SW_OPTO_COUPLER


/*
 *  check for Unijunction Transistors
 *  - uncomment to enable
 */

//#define SW_UJT


/*
 *  color coding for probes
 *  - requires color graphics LCD
 *  - uncomment to enable
 *  - edit colors.h to select correct probe colors
 */

#define SW_PROBE_COLORS


/*
 *  Servo Check
 *  - requires additional keys and display with more than 2 text lines
 *  - uncomment to enable
 */

//#define SW_SERVO


/*
 *  DS18B20
 *  - uncomment to enable
 *  - also enable ONEWIRE_PROBES or ONEWIRE_IO_PIN (see section 'Busses')
 */

//#define SW_DS18B20


/*
 *  capacitor leakage check
 *  - requires display with more than two lines
 *  - uncomment to enable
 */

//#define SW_CAP_LEAKAGE



/* ************************************************************************
 *   Makefile workaround for some IDEs 
 * ************************************************************************ */


/*
 *  Oscillator startup cycles (after wakeup from power-safe mode):
 *  - typical values
 *    - internal RC:              6
 *    - full swing crystal:   16384 (also 256 or 1024 based on fuse settings)
 *    - low power crystal:    16384 (also 256 or 1024 based on fuse settings)
 *  - Please change value if it doesn't match your tester!
 */

#ifndef OSC_STARTUP
  #define OSC_STARTUP    16384
#endif



/* ************************************************************************
 *   misc settings
 * ************************************************************************ */


/*
 *  Languange of user interface. Available languages:
 *  - English (default)
 *  - Czech
 *  - Danish
 *  - German
 *  - Polish
 *  - Spanish
 *  - Russian (only 8x16 font horizontally aligned)
 */

#define UI_ENGLISH
//#define UI_CZECH
//#define UI_DANISH
//#define UI_GERMAN
//#define UI_ITALIAN
//#define UI_POLISH
//#define UI_SPANISH
//#define UI_RUSSIAN


/*
 *  Use comma instead of dot to indicate a decimal fraction.
 *  - uncomment to enable
 */

//#define UI_COMMA


/*
 *  Display temperatures in Fahrenheit instead of Celsius.
 *  - uncomment to enable
 */

//#define UI_FAHRENHEIT


/*
 *  Set the default operation mode to auto-hold.
 *  - instead of continous mode
 *  - uncomment to enable
 */

//#define UI_AUTOHOLD


/*
 *  Trigger the menu also by a short circuit of all three probes.
 *  - former default behaviour
 *  - uncomment to enable
 */

//#define UI_SHORT_CIRCUIT_MENU


/*
 *  Output components found also via TTL serial interface.
 *  - uncomment to enable
 *  - also enable SERIAL_BITBANG or SERIAL_HARDWARE (see section 'Busses')
 */

//#define UI_SERIAL_COPY


/*
 *  Control tester via TTL serial interface.
 *  - uncomment to enable
 *  - also enable SERIAL_BITBANG or SERIAL_HARDWARE, plus SERIAL_RW
 *    (see section 'Busses') 
 */

//#define UI_SERIAL_COMMANDS


/*
 *  Maximum time to wait after probing in continous mode (in ms).
 *  - Time between printing the result and starting a new probing cycle.
 */

#define CYCLE_DELAY      3000


/*
 *  Maximum number of tests without any component found in a row.
 *  - If this number is reached the tester powers off.
 */

#define CYCLE_MAX        5


/*
 *  Battery monitoring mode
 *  - BAT_NONE     disable battery monitoring completely
 *  - BAT_DIRECT   direct measurement of battary voltage (< 5V)
 *  - BAT_DIVIDER  measurement via voltage divider
 *  - uncomment one of the modes
 */

//#define BAT_NONE
//#define BAT_DIRECT
#define BAT_DIVIDER


/*
 *  Unmonitored optional external power supply
 *  - Some circuits supporting an additional external power supply are designed
 *    in a way that prevents the battery monitoring to measure the voltage of
 *    the external power supply. This would trigger the low battery shut-down.
 *    The switch below will prevent the shut-down when the measured voltage is
 *    below 0.9V (caused by the diode's leakage current).
 *  - uncomment to enable
 */

//#define BAT_EXT_UNMONITORED


/*
 *  Voltage divider for battery monitoring
 *  - BAT_R1: top resistor in Ohms
 *  - BAT_R2: bottom resistor in Ohms
 */

#define BAT_R1           10000
#define BAT_R2           3300


/*
 *  Voltage drop by reverse voltage protection diode and power management
 *  transistor (in mV):
 *  - Schottky diode about 200mV / PNP BJT about 100mV.
 *  - Get your DMM and measure the voltage drop!
 */  

#define BAT_OFFSET       290


/*
 *  Battery weak voltage (in mV).
 *  - Tester warns if BAT_WEAK is reached.
 *  - Voltage drop (BAT_OUT) is considered in calculation.
 */

#define BAT_WEAK         7400


/*
 *  Battery low voltage (in mV).
 *  - Tester powers off if BAT_LOW is reached.
 *  - Voltage drop (BAT_OFFSET) is considered in calculation.
 */

#define BAT_LOW          6400 


/*
 *  Enter sleep mode when idle to save power.
 *  - uncomment to enable
 */

#define SAVE_POWER



/* ************************************************************************
 *   measurement settings and offsets
 * ************************************************************************ */


/*
 *  ADC voltage reference based on Vcc (in mV). 
 */

#define UREF_VCC         5001


/*
 * Offset for the internal bandgap voltage reference (in mV): -100 up to 100
 *  - To compensate any difference between real value and measured value.
 *  - The ADC has a resolution of about 4.88mV for V_ref = 5V (Vcc) and
 *    1.07mV for V_ref = 1.1V (bandgap).
 *  - Will be added to measured voltage of bandgap reference.
 */

#define UREF_OFFSET      0


/*
 *  Exact values of probe resistors.
 *  - Standard value for Rl is 680 Ohms.
 *  - Standard value for Rh is 470k Ohms.
 */

/* Rl in Ohms */
#define R_LOW            680

/* Rh in Ohms */
#define R_HIGH           470000


/*
 *  Offset for systematic error of resistor measurement with Rh (470k) 
 *  in Ohms.
 *  - if resistors >20k measure too high or low adjust the offset accordingly
 *  - standard offset is 350 Ohms
 */

#define RH_OFFSET        350



/*
 *  Resistance of probe leads (in 0.01 Ohms).
 *  - Resistance of two probe leads in series.
 *  - Assuming all probe leads got same/similar resistance.
 */

#define R_ZERO           20



/* 
 *  Capacitance of the wires between PCB and terminals (in pF).
 *  Examples:
 *  - 2pF for wires 10cm long
 */

#define CAP_WIRES        2


/* 
 *  Capacitance of the probe leads connected to the tester (in pF).
 *  Examples:
 *    capacity  length of probe leads
 *    -------------------------------
 *     3pF      about 10cm
 *     9pF      about 30cm
 *    15pF      about 50cm
 */

#define CAP_PROBELEADS   9


/*
 *  Maximum voltage at which we consider a capacitor being
 *  discharged (in mV)
 */

#define CAP_DISCHARGED   2


/*
 *  Correction factors for capacitors (in 0.1%)
 *  - positive factor increases capacitance value
 *    negative factor decreases capacitance value
 *  - CAP_FACTOR_SMALL for caps < 4.7µF
 *  - CAP_FACTOR_MID for caps 4.7 - 47µF
 *  - CAP_FACTOR_LARGE for caps > 47µF
 */

#define CAP_FACTOR_SMALL      0      /* no correction */ 
#define CAP_FACTOR_MID        -40    /* -4.0% */
#define CAP_FACTOR_LARGE      -90    /* -9.0% */


/*
 *  Number of ADC samples to perform for each mesurement.
 *  - Valid values are in the range of 1 - 255.
 */

#define ADC_SAMPLES      25



/* ************************************************************************
 *   MCU specific setup to support different AVRs
 * ************************************************************************ */


/* MCU clock */
#define CPU_FREQ    F_CPU


/*
 *  ATmega 328/328P
 */

#if defined(__AVR_ATmega328__)

  #include "config_328.h"


/*
 *  ATmega 324P/324PA/644/644P/644PA/1284/1284P
 */

#elif defined(__AVR_ATmega324P__) || defined(__AVR_ATmega644__) || defined(__AVR_ATmega1284__)

  #include "config_644.h"


/*
 *  missing or unsupported MCU
 */

#else
  #error <<< No or wrong MCU type selected! >>>
#endif



/* ************************************************************************
 *   Busses
 * ************************************************************************ */


/*
 *  I2C bus
 *  - might be required by some hardware
 *  - could already be enabled in display section (config_<MCU>.h)
 *  - for bit-bang I2C port and pins see I2C_PORT (config_<MCU>.h)
 *  - hardware I2C (TWI) uses automatically the proper MCU pins
 *  - uncomment either I2C_BITBANG or I2C_HARDWARE to enable
 *  - uncomment one of the bus speed modes
 */

//#define I2C_BITBANG                /* bit-bang I2C */
//#define I2C_HARDWARE               /* MCU's hardware TWI */
//#define I2C_STANDARD_MODE          /* 100kHz bus speed */
//#define I2C_FAST_MODE              /* 400kHz bus speed */
//#define I2C_RW                     /* enable I2C read support (untested) */


/*
 *  SPI bus
 *  - might be required by some hardware
 *  - could already be enabled in display section (config_<MCU>.h)
 *  - for bit-bang SPI port and pins see SPI_PORT (config_<MCU>.h)
 *  - hardware SPI uses automatically the proper MCU pins
 *  - uncomment either SPI_BITBANG or SPI_HARDWARE to enable
 */

//#define SPI_BITBANG                /* bit-bang SPI */
//#define SPI_HARDWARE               /* hardware SPI */
//#define SPI_RW                     /* enable SPI read support */


/*
 *  TTL serial interface
 *  - could already be enabled in display section (config_<MCU>.h)
 *  - for bit-bang serial port and pins see SERIAL_PORT (config_<MCU>.h)
 *  - hardware serial uses automatically the proper MCU pins
 *  - uncomment either SERIAL_BITBANG or SERIAL_HARDWARE to enable
 */

//#define SERIAL_BITBANG             /* bit-bang serial */
//#define SERIAL_HARDWARE            /* hardware serial */
//#define SERIAL_RW                  /* enable serial read support */


/*
 *  OneWire bus
 *  - for dedicated I/O pin please see ONEWIRE_PORT (config_<MCU>.h)
 *  - uncomment either ONEWIRE_PROBES or ONEWIRE_ to enable
 */

//#define ONEWIRE_PROBES             /* via probes */
//#define ONEWIRE_IO_PIN             /* via dedicated I/O pin */



/* ************************************************************************
 *   ADC clock
 * ************************************************************************ */


/*
 *  ADC clock 
 *  - The ADC clock is 125000Hz by default.
 *  - You could also set 250000Hz, but that exceeds the max. ADC clock
 *    of 200kHz for 10 bit resolution!
 *  - Special case for 20MHz MCU clock: 156250Hz
 */

#if CPU_FREQ == 20000000
  /* 20MHz MCU clock */
  #define ADC_FREQ    156250
#else
  /* all other MCU clocks */
  #define ADC_FREQ    125000
#endif


/*
 *  define clock divider
 *  - supports 1MHz, 2MHz, 4MHz, 8MHz and 16MHz MCU clocks
 *  - we got only 7 fixed prescalers from 2 up to 128
 */

/* 1MHz/250kHz */
#if CPU_FREQ / ADC_FREQ == 4
  #define ADC_CLOCK_DIV (1 << ADPS1) 
#endif

/* 1MHz/125kHz 2MHz/250kHz */
#if CPU_FREQ / ADC_FREQ == 8
  #define ADC_CLOCK_DIV (1 << ADPS1) | (1 << ADPS0)
#endif

/* 2MHz/125kHz 4MHz/250kHz */
#if CPU_FREQ / ADC_FREQ == 16
  #define ADC_CLOCK_DIV (1 << ADPS2)
#endif

/* 4MHz/125kHz 8MHz/250kHz */
#if CPU_FREQ / ADC_FREQ == 32
  #define ADC_CLOCK_DIV (1 << ADPS2) | (1 << ADPS0)
#endif

/* 8MHz/125kHz 16MHz/250kHz */
#if CPU_FREQ / ADC_FREQ == 64
  #define ADC_CLOCK_DIV (1 << ADPS2) | (1 << ADPS1)
#endif

/* 16MHz/125kHz 20MHz/156.25kHz */
#if CPU_FREQ / ADC_FREQ == 128
  #define ADC_CLOCK_DIV (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0)
#endif



/* ************************************************************************
 *   derived values
 * ************************************************************************ */


/*
 *  total default capacitance (in pF)
 *  - max. 255
 */

#define C_ZERO           CAP_PCB + CAP_WIRES + CAP_PROBELEADS


/*
 *  number of MCU cycles per µs
 *  - min. 1 (for 1MHz)
 *  - max. 20 (for 20MHz)
 */

#define MCU_CYCLES_PER_US     (CPU_FREQ / 1000000)


/*
 *  number of MCU cycles per ADC cycle
 *  - min. 4
 *  - max. 128
 */ 

#define MCU_CYCLES_PER_ADC    (CPU_FREQ / ADC_FREQ)


/*
 *  time of a MCU cycle (in 0.1 ns)
 */

#define MCU_CYCLE_TIME        (10000 / (CPU_FREQ / 1000000))



/* ************************************************************************
 *   options management
 * ************************************************************************ */


/*
 *  hardware/software options
 */


/* additional keys */
/* rotary encoder, increase/decrease push buttons or touch screen */
#if defined (HW_ENCODER) || defined (HW_INCDEC_KEYS) | defined (HW_TOUCH)
  #define HW_KEYS
#endif

/* options which require additional keys */
#ifndef HW_KEYS

  /* PWM+ */
  #ifdef SW_PWM_PLUS
    #undef SW_PWM_PLUS
    #define SW_PWM_SIMPLE   
  #endif

  /* squarewave generator */
  #ifdef SW_SQUAREWAVE
    #undef SW_SQUAREWAVE
  #endif

  /* Servo Check */
  #ifdef SW_SERVO
    #undef SW_SERVO
  #endif

#endif


/* options which require a MCU clock >= 8MHz */
#if CPU_FREQ < 8000000

  /* ESR measurement */
  #ifdef SW_ESR
    #undef SW_ESR
  #endif

  /* old ESR measurement */
  #ifdef SW_OLD_ESR
    #undef SW_OLD_ESR
  #endif

#endif


/* SPI */
#if defined (SPI_BITBANG) || defined (SPI_HARDWARE)
  #define HW_SPI
#endif


/* I2C */
#if defined (I2C_BITBANG) || defined (I2C_HARDWARE)
  #define HW_I2C
#endif


/* TTL serial */
#if defined (SERIAL_BITBANG) || defined (SERIAL_HARDWARE)
  #define HW_SERIAL
#endif

/* VT100 display driver disables other options for serial interface */
#ifdef LCD_VT100
  #ifdef UI_SERIAL_COPY
    #undef UI_SERIAL_COPY
  #endif
  #ifdef UI_SERIAL_COMMANDS
    #undef UI_SERIAL_COMMANDS
  #endif  
#endif

/* options which require TTL serial */
#ifndef HW_SERIAL
  #ifdef LCD_VT100
    #undef LCD_VT100
  #endif
  #ifdef UI_SERIAL_COPY
    #undef UI_SERIAL_COPY
  #endif
  #ifdef UI_SERIAL_COMMANDS
    #undef UI_SERIAL_COMMANDS
  #endif
#endif

/* options which require TTL serial RW */
#ifndef SERIAL_RW
  #ifdef UI_SERIAL_COMMANDS
    #undef UI_SERIAL_COMMANDS
  #endif
#endif


/* OneWire: probe leads prevail */
#ifdef ONEWIRE_PROBES
  #undef ONEWIRE_IO_PIN
#endif
#ifdef ONEWIRE_IO_PIN
  #undef ONEWIRE_PROBES
#endif

/* options which require OneWire */
#if ! defined (ONEWIRE_PROBES) && ! defined (ONEWIRE_IO_PIN)
  #ifdef SW_DS18B20
    #undef SW_DS18B20
  #endif
#endif


/* touchscreen */
#ifdef TOUCH_PORT
  #define HW_TOUCH
#endif


/* LCD module */
#ifdef LCD_CONTRAST
  #define SW_CONTRAST
#else
  #define LCD_CONTRAST        0
#endif


/* color coding for probes requires a color graphics display */
#ifdef SW_PROBE_COLORS
  #ifndef LCD_COLOR
    #undef SW_PROBE_COLORS
  #endif
#endif


/* component symbols for fancy pinout */
#if defined (SYMBOLS_24X24_H)
  #define SW_SYMBOLS
#endif
#if defined (SYMBOLS_24X24_HF) || defined (SYMBOLS_30X32_HF) || defined (SYMBOLS_32X32_HF)
  #define SW_SYMBOLS
#endif
#if defined (SYMBOLS_24X24_VFP)
  #define SW_SYMBOLS
#endif
#if defined (SYMBOLS_24X24_VP_F)
  #define SW_SYMBOLS
#endif

/* symbols require graphic display */
#ifdef SW_SYMBOLS
  #ifndef LCD_GRAPHIC
    #undef SW_SYMBOLS
  #endif
#endif


/* frequency counter */
#if defined (HW_FREQ_COUNTER_BASIC) || defined (HW_FREQ_COUNTER_EXT)
  #define HW_FREQ_COUNTER
#endif


/* IR detector/decoder: probe lead based decoder prevails */
#ifdef SW_IR_RECEIVER
  #undef HW_IR_RECEIVER
#endif
#ifdef HW_IR_RECEIVER
  #undef SW_IR_RECEIVER
#endif



/* ************************************************************************
 *   EOF
 * ************************************************************************ */
