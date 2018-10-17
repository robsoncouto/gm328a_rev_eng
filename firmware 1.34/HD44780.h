/* ************************************************************************
 *
 *   HD44780 compatible character display
 *
 *   (c) 2012-2017 by Markus Reschke
 *   based on code from Markus Frejek and Karl-Heinz Kübbeler
 *
 * ************************************************************************ */


/* ************************************************************************
 *   addresses
 * ************************************************************************ */


/*
 *  CGROM
 *  - 1240 bytes
 *  - 208 characters 5x8 and 32 characters 5x10
 */


/*
 *  CGRAM
 *  - 64 bytes
 *  - used-defined characters
 *  - 8 characters 5x8 or 4 characters 5x10
 */


/*
 *  DDRAM
 *  - 80 bytes
 */

/* start adddresses of lines */
#define ADDR_LINE_1           0x00    /* line #1 */
#define ADDR_LINE_2           0x40    /* line #2 */
#define ADDR_LINE_3           0x14    /* line #3 */
#define ADDR_LINE_4           0x54    /* line #4 */



/* ************************************************************************
 *   LCD commands
 * ************************************************************************ */


/*
 *  clear display
 */

#define CMD_CLEAR_DISPLAY     0x01    /* clear display */


/*
 *  return home
 *  - exec time: 1.52ms
 */

#define CMD_RETURN_HOME       0x02    /* return home */


/*
 *  entry mode set
 *  - exec time: 37µs
 */

#define CMD_ENTRY_MODE_SET    0x04    /* entry mode set */

#define FLAG_CURSOR_DECREASE  0b00000000     /* auto-decrease cursor position */
#define FLAG_CURSOR_INCREASE  0b00000010     /* auto-increase cursor position */
#define FLAG_DISPLAY_NOSHIFT  0b00000000     /* enable display auto-shift */
#define FLAG_DISPLAY_SHIFT    0b00000001     /* disable display auto-shift */


/*
 *  display control
 */

#define CMD_DISPLAY_CONTROL   0x08    /* display control */

#define FLAG_DISPLAY_OFF      0b00000000     /* display off */
#define FLAG_DISPLAY_ON       0b00000100     /* display on */
#define FLAG_CURSOR_OFF       0b00000000     /* cursor off */
#define FLAG_CURSOR_ON        0b00000010     /* cursor on */
#define FLAG_BLINK_OFF        0b00000000     /* blinking off */
#define FLAG_BLINK_ON         0b00000001     /* blinking on */


/*
 *  shift
 */

#define CMD_SHIFT             0x10    /* shift */

#define FLAG_SHIFT_CURSOR     0b00000000     /* shift cursor */
#define FLAG_SHIFT_DISPLAY    0b00001000     /* shift display */
#define FLAG_SHIFT_LEFT       0b00000000     /* shift left */
#define FLAG_SHIFT_RIGHT      0b00000100     /* shift right */


/*
 *  function set
 */

#define CMD_FUNCTION_SET      0x20    /* function set */

#define FLAG_INTERFACE_4BIT   0b00000000     /* enable 4 bit data interface */
#define FLAG_INTERFACE_8BIT   0b00010000     /* enable 8 bit data interface */
#define FLAG_LINES_1          0b00000000     /* display one line */
#define FLAG_LINES_2          0b00001000     /* display two lines */
#define FLAG_FONT_5X7         0b00000000     /* select 5x7 font */
#define FLAG_FONT_5X10        0b00000100     /* select 5x10 font */


/*
 *  set CG RAM address
 */

#define CMD_SET_CG_RAM_ADDR   0x40    /* set CG RAM address (custom character) */


/*
 *  set DD RAM address
 */

#define CMD_SET_DD_RAM_ADDR   0x80    /* set DD RAM address (cursor position) */



/* ************************************************************************
 *   EOF
 * ************************************************************************ */
