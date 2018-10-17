/* ************************************************************************
 *
 *   ST7920 graphic display controller with built-in fonts
 *
 *   (c) 2017 by Markus Reschke
 *
 * ************************************************************************ */


/* ************************************************************************
 *   addresses
 * ************************************************************************ */


/*
 *  CGROM
 *  - 8192 Chinese characters 16x16
 *  - BIG5: traditional character set
 *  - GB: simplified character set
 */

#define CGROM_BIG5_START      0xA140
#define CGROM_BIG5_END        0xD75F
#define CGROM_GB_START        0xA1A0
#define CGROM_GB_END          0xF7FF


/*
 *  HCGROM
 *  - 126 alpha numeric characters 8x16
 */

#define HCGROM_START          0x02
#define HCGROM_END            0x7F


/*
 *  CGRAM
 *  - 4 user-defined characters 16x16
 *  - address range 0x0000 - 0x0006
 */

#define CGRAM_1               0x0000    /* char #1 */
#define CGRAM_2               0x0002    /* char #2 */
#define CGRAM_3               0x0004    /* char #3 */
#define CGRAM_4               0x0006    /* char #4 */


/*
 *  DDRAM
 *  - 64x2 bytes
 *  - 16 characters (16x16) or 32 characters (8x16) by 4 lines
 *  - only 2 lines can be displayed at a time
 */

#define ADDR_LINE_1           0x80    /* line #1 (0x80 - 0x8F) */
#define ADDR_LINE_2           0x90    /* line #2 (0x90 - 0x9F) */
#define ADDR_LINE_3           0xA0    /* line #3 (0xA0 - 0xAF) */
#define ADDR_LINE_4           0xB0    /* line #4 (0xB0 - 0xBF) */


/*
 *  GDRAM
 *  - x: 0-15 banks with 16 dots each (MSB/LSB)
 *  - y: 0-63 rows
 */



/* ************************************************************************
 *   SPI
 * ************************************************************************ */


/*
 *  start byte for SPI
 *  - for synchronizing and selecting RW & RS
 */

#define START_BYTE            0b11111000     /* start byte */
#define FLAG_WRITE            0b00000000     /* write mode */
#define FLAG_READ             0b00000100     /* read mode (not supported) */
#define FLAG_CMD              0b00000000     /* command mode */
#define FLAG_DATA             0b00000010     /* data mode */



/* ************************************************************************
 *   basic instruction set
 * ************************************************************************ */


/*
 *  clear display (DDRAM)
 *  - fill with spaces and go to first character in first line
 *  - exec time: 1.6ms
 */

#define CMD_CLEAR             0b00000001     /* clear display */


/*
 *  return home (DDRAM)
 *  - go to first character in first line
 *  - exec time: 72탎
 */

#define CMD_HOME              0b00000010     /* return home */


/*
 *  set entry mode
 *  - set cursor movement and display shifting
 *  - exec time: 72탎
 */

#define CMD_ENTRY_MODE        0b00000100     /* set entry mode */

#define FLAG_DECREASE         0b00000000     /* decrease address (move left) */
#define FLAG_INCREASE         0b00000010     /* increase address (move right) */
#define FLAG_DISPLAY_NOSHIFT  0b00000000     /* auto shifting off */
#define FLAG_DISPLAY_SHIFT    0b00000001     /* auto shifting on */


/*
 *  display status
 *  - exec time: 72탎
 */

#define CMD_DISPLAY           0b00001000     /* display status */

#define FLAG_DISPLAY_OFF      0b00000000     /* display off */
#define FLAG_DISPLAY_ON       0b00000100     /* display on */
#define FLAG_CURSOR_OFF       0b00000000     /* cursor off */
#define FLAG_CURSOR_ON        0b00000010     /* cursor on */
#define FLAG_BLINK_OFF        0b00000000     /* blinking off */
#define FLAG_BLINK_ON         0b00000001     /* blinking on */


/*
 *  cursor and display shift control
 *  - exec time: 72탎
 */

#define CMD_CURSOR            0b00010000     /* cursor control */

#define FLAG_SHIFT_CURSOR     0b00000000     /* shift cursor */
#define FLAG_SHIFT_DISPLAY    0b00001000     /* shift display */
#define FLAG_SHIFT_LEFT       0b00000000     /* shift left */
#define FLAG_SHIFT_RIGHT      0b00000100     /* shift right */


/*
 *  function set
 *  - exec time: 72탎
 *  - can't set interface and instruction set at once
 */

#define CMD_FUNCTION_SET      0b00100000     /* function set */

#define FLAG_INTERFACE_4BIT   0b00000000     /* enable 4 bit data interface */
#define FLAG_INTERFACE_8BIT   0b00010000     /* enable 8 bit data interface */
#define FLAG_CMD_BASIC        0b00000000     /* basic instruction set */
#define FLAG_CMD_EXT          0b00000100     /* extended instruction set */


/*
 *  set CGRAM address
 *  - exec time: 72탎
 *  - valid range: 0x00 - 0x3F
 */

#define CMD_SET_CGRAM_ADDR    0b01000000     /* set CGRAM address */


/*
 *  set DDRAM address
 *  - exec time: 72탎
 */

#define CMD_SET_DDRAM_ADDR    0b10000000     /* set DDRAM address */



/* ************************************************************************
 *   extended instruction set
 * ************************************************************************ */


/*
 *  stand-by mode
 *  - any other new command disables stand-by
 *  - exec time: 72탎
 */

#define CMD_STAND_BY          0b00000001     /* enter stand-by */


/*
 *  vertical scroll or RAM address select
 *  - exec time: 72탎
 */

#define CMD_SELECT            0b00000010     /* address select */

#define FLAG_CGRAM            0b00000000     /* CGRAM address */
#define FLAG_VSCROLL          0b00000001     /* vertical scroll address */


/*
 *  reverse
 *  - another reverse command toggles the reverse condition
 *  - exec time: 72탎
 */

#define CMD_REVERSE           0b00000100     /* reverse */

#define FLAG_LINE_1           0b00000000     /* line #1 */
#define FLAG_LINE_2           0b00000001     /* line #2 */
#define FLAG_LINE_3           0b00000010     /* line #3 */
#define FLAG_LINE_4           0b00000011     /* line #4 */


/*
 *  extended function set
 *  - exec time: 72탎
 *  - can't set interface, instruction set or graphic display at once
 */

#define CMD_EXT_FUNC_SET      0b00100000     /* extended function set */

#define FLAG_INTERFACE_4BIT   0b00000000     /* enable 4 bit data interface */
#define FLAG_INTERFACE_8BIT   0b00010000     /* enable 8 bit data interface */
#define FLAG_CMD_BASIC        0b00000000     /* basic instruction set */
#define FLAG_CMD_EXT          0b00000100     /* extended instruction set */
#define FLAG_GFX_OFF          0b00000000     /* graphic display off */
#define FLAG_GFX_ON           0b00000010     /* graphic display on */


/*
 *  set vertical scroll address
 *  - exec time: 72탎
 *  - 6 bits
 */

#define CMD_SET_SCROLL_ADDR   0b01000000     /* set scroll address */


/*
 *  set graphic RAM address
 *  - exec time: 72탎
 *  - 2 byte command
 */

#define CMD_SET_GDRAM_ADDR    0b10000000    /* set GDRAM address */

/* byte #1: vertical address (7 bits) */
/* byte #2: horizontal address / bank (4 bits) */



/* ************************************************************************
 *   EOF
 * ************************************************************************ */
