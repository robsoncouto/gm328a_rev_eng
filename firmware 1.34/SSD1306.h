/* ************************************************************************
 *
 *   SSD1306 OLED graphic display controller
 *
 *   (c) 2017 by Markus Reschke
 *
 * ************************************************************************ */


/* ************************************************************************
 *   I2C
 * ************************************************************************ */


/*
 *  I2C control byte
 */

#define LCD_CONTROL_BYTE      0b00000000     /* 0x00 */
/* continuation flag */
#define FLAG_CTRL_MULTI       0b00000000     /* stream bytes follows */
#define FLAG_CTRL_SINGLE      0b10000000     /* only 1 byte follows */
/* D/C */
#define FLAG_CTRL_CMD         0b00000000     /* command follows */
#define FLAG_CTRL_DATA        0b01000000     /* data follows */



/* ************************************************************************
 *   fundamental commands
 * ************************************************************************ */


/*
 *  contrast control
 *  - 2 byte command
 *  - valid range: 0 - 255
 *    default: 127
 */

/* byte #1: command */
#define CMD_CONTRAST          0b10000001     /* set contrast */

/* byte #2: contrast (0-255) */


/*
 *  entire display on
 */

#define CMD_PIXEL_MODE        0b10100100     /* set pixel mode */

#define FLAG_PIXEL_RAM        0b00000000     /* based on RAM (default) */
#define FLAG_PIXEL_ALL        0b00000001     /* all pixels on */


/*
 *  set normal/inverse display
 */

#define CMD_DISP_MODE         0b10100110     /* set display mode */

#define FLAG_DISP_NORMAL      0b00000000     /* normal display (1 = pixel on, default) */
#define FLAG_DISP_INVERSE     0b00000001     /* inversed display (0 = pixel on) */


/*
 *  set display on/off
 */

#define CMD_DISPLAY           0b10101110     /* enable/disable display */

#define FLAG_DISPLAY_OFF      0b00000000     /* disable display (sleep mode, default) */
#define FLAG_DISPLAY_ON       0b00000001     /* enable display */



/* ************************************************************************
 *   scrolling commands
 * ************************************************************************ */


/*
 *  continious horizontal scroll setup
 *  - 7 byte command
 */

/* byte #1: command */
#define CMD_HOR_SCROLL        0b00100110     /* set horizontal scolling */

#define FLAG_SCROLL_RIGHT     0b00000000     /* scroll to right */
#define FLAG_SCROLL_LEFT      0b00000001     /* scroll to left */

/* byte #2: dummy byte */
#define FLAG_SCROLL_DUMMY1    0b00000000     /* dummy byte 0x00 */

/* byte #3: start page (valid range: 0-7) */
#define FLAG_PAGE_0           0b00000000     /* page #0 */
#define FLAG_PAGE_1           0b00000001     /* page #1 */
#define FLAG_PAGE_2           0b00000010     /* page #2 */
#define FLAG_PAGE_3           0b00000011     /* page #3 */
#define FLAG_PAGE_4           0b00000100     /* page #4 */
#define FLAG_PAGE_5           0b00000101     /* page #5 */
#define FLAG_PAGE_6           0b00000110     /* page #6 */
#define FLAG_PAGE_7           0b00000111     /* page #7 */

/* byte #4: time interval (based on frame fequency) */
#define FLAG_INTERVAL_5       0b00000000     /* 5 frames */
#define FLAG_INTERVAL_64      0b00000001     /* 64 frames */
#define FLAG_INTERVAL_128     0b00000010     /* 128 frames */
#define FLAG_INTERVAL_256     0b00000011     /* 256 frames */
#define FLAG_INTERVAL_3       0b00000100     /* 3 frames */
#define FLAG_INTERVAL_4       0b00000101     /* 4 frames */
#define FLAG_INTERVAL_25      0b00000110     /* 25 frames */
#define FLAG_INTERVAL_2       0b00000111     /* 2 frames */

/* byte #5: end page (valid range: 0-7) */
/* see FLAG_PAGE_x above */

/* byte #6: dummy byte */
#define FLAG_SCROLL_DUMMY2    0b00000000     /* dummy byte 0x00 */

/* byte #7: dummy byte */
#define FLAG_SCROLL_DUMMY3    0b11111111     /* dummy byte 0xff */


/*
 *  continious vertical and horizontal scroll setup
 *  - 6 byte command
 */

/* byte #1: command */
#define CMD_VH_SCROLL         0b00101000     /* set ver & hor scolling */

#define FLAG_SCROLL_VRIGHT    0b00000001     /* scroll to right */
#define FLAG_SCROLL_VLEFT     0b00000010     /* scroll to left */

/* byte #2: dummy byte */
#define FLAG_SCROLL_DUMMY1    0b00000000     /* dummy byte 0x00 */

/* byte #3: start page (valid range: 0-7) */
/* see FLAG_PAGE_x above */

/* byte #4: time interval (based on frame fequency) */
/* see FLAG_INTERVAL_xxx above */

/* byte #5: end page (valid range: 0-7) */
/* see FLAG_PAGE_x above */

/* byte #6: vertical scrolling offset (rows) */
/* valid range: 0 - 63 */


/*
 *  deactivate scrolling
 */

#define CMD_SCOLLING_OFF      0b00101110     /* disable scolling */


/*
 *  activate scrolling
 */

#define CMD_SCOLLING_ON       0b00101111     /* enable scolling */


/*
 *  set vertical scroll area
 *  - 3 byte command
 */

/* byte #1: command */
#define CMD_SCROLL_AREA_SET   0b10100011     /* set scroll area */

/* byte #2: number of rows in top fixed area */
/* valid range: 0 - 31 */
/* default: 0 */

/* byte #3: number of rows in scroll area */
/* valid range: 0 - 63 */
/* default: 64 */



/* ************************************************************************
 *   address setting commands
 * ************************************************************************ */


/*
 *  set lower nibble of column start address (page adressing mode)
 *  - valid range: 0 - 63 (both nibbles: 0-127)
 *    default: 0
 */

#define CMD_COLUMN_L          0b00000000     /* set lower nibble of start address */


/*
 *  set upper nibble column start address (page adressing mode)
 *  - valid range: 0 - 63 (both nibbles: 0-127)
 *    default: 0
 */

#define CMD_COLUMN_H          0b00010000     /* set upper nibble of start address */


/*
 *  set memory addressing mode
 *  - 2 byte command
 */

/* byte #1: command */
#define CMD_MEM_ADDR_MODE     0b00100000     /* set memory address mode */

/* byte #2: mode */
#define FLAG_ADDR_MODE_HOR    0b00000000     /* horizontal */
#define FLAG_ADDR_MODE_VER    0b00000001     /* vertical */
#define FLAG_ADDR_MODE_PAGE   0b00000010     /* page (default) */
#define FLAG_ADDR_MODE_NONE   0b00000011     /* invalid */


/*
 *  set column address (horizontal or vertical addressing mode)
 *  - 3 byte command
 */

/* byte #1: command */
#define CMD_COLUMN_ADDR       0b00100001     /* set column address */

/* byte #2: column start address */
/* valid range: 0 - 127 */
/* default: 0 */

/* byte #3: column end address */
/* valid range: 0 - 127 */
/* default: 127 */


/*
 *  set page address (horizontal or vertical addressing mode)
 *  - 3 byte command
 */

/* byte #1: command */
#define CMD_PAGE_ADDR         0b00100010     /* set page address */

/* byte #2: page start address */
/* valid range: 0 - 7 */
/* default: 0 */

/* byte #3: page end address */
/* valid range: 0 - 7 */
/* default: 7 */


/*
 *  set page start address (page adressing mode)
 *  - valid range: 0 - 7
 */

#define CMD_START_PAGE        0b10110000     /* set start page */
/* see FLAG_PAGE_x above */



/* ************************************************************************
 *   hardware configuration commands
 * ************************************************************************ */


/*
 *  set charge pump
 *  - 2 byte command
 */

/* byte #1: command */
#define CMD_CHARGE_PUMP       0b10001101     /* set charge pump*/

/* byte #2: mode */
#define FLAG_CHARGEPUMP_OFF   0b00010000     /* disable charge pump */
#define FLAG_CHARGEPUMP_ON    0b00010100     /* enable charge pump */


/*
 *  set display start line
 *  - valid range: 0 - 63
 *    default: 0
 */

#define CMD_START_LINE        0b01000000     /* set start line */


/*
 *  set segment mapping
 */

#define CMD_SEGMENT_MAP       0b10100000     /* set segment mapping */

#define FLAG_SEG_0            0b00000000     /* SEG0 = column #0 (default) */
#define FLAG_SEG_127          0b00000001     /* SEG0 = column #127 */


/*
 *  set multiplex ratio (N + 1)
 *  - 2 byte command
 */

/* byte #1: command */
#define CMD_MULTIPLEX_RATIO   0b10101000     /* set multiplex ratio */

/* byte #2: ratio */
/* valid range: 15 - 63 */
/* default: 63 */


/*
 *  set COM output scan direction
 */

#define CMD_COM_SCAN_DIR      0b11000000     /* set COM scan direction */

#define FLAG_COM_0            0b00000000     /* from COM0 to COM[N-1] */
#define FLAG_COM_63           0b00001000     /* from COM[N-1] to COM0 */


/*
 *  set display offset
 *  - vertical shift (COM)
 *  - 2 byte command
 */

/* byte #1: command */
#define CMD_DISP_OFFSET       0b11010011     /* set display offset */

/* byte #2: offset */
/* valid range: 0 - 63 */
/* default: 0*/


/*
 *  set COM pins hardware configuration
 * - 2 byte command
 */

/* byte #1: command */
#define CMD_COM_CONFIG_SET    0b11011010     /* set COM pins config */

/* byte #2: config */
#define FLAG_COM_SEQ          0b00000000     /* sequential */
#define FLAG_COM_ALT          0b00010000     /* alternative (default) */
#define FLAG_COM_NORM         0b00000000     /* normal mapping (default) */
#define FLAG_COM_REMAP        0b00100000     /* reversed mapping */



/* ************************************************************************
 *   timing & driving scheme setting commands
 * ************************************************************************ */


/*
 *  set display clock
 *  - divider ratio and oscillator frequency
 *  - 2 byte command
 */

/* byte #1: command */
#define CMD_DISP_CLOCK        0b11010101     /* set display clock */

/* byte #2: divider ratio and oscillator frequency */
/* divider ratio (n + 1) for DCLK */
#define FLAG_RATIO_1          0b00000000     /* ratio 1 (default) */
#define FLAG_RATIO_2          0b00000001     /* ratio 2 */
#define FLAG_RATIO_3          0b00000010     /* ratio 3 */
#define FLAG_RATIO_4          0b00000010     /* ratio 4 */
#define FLAG_RATIO_5          0b00000100     /* ratio 5 */
#define FLAG_RATIO_6          0b00000101     /* ratio 6 */
#define FLAG_RATIO_7          0b00000110     /* ratio 7 */
#define FLAG_RATIO_8          0b00000111     /* ratio 8 */
#define FLAG_RATIO_9          0b00001000     /* ratio 9 */
#define FLAG_RATIO_10         0b00001001     /* ratio 10 */
#define FLAG_RATIO_11         0b00001010     /* ratio 11 */
#define FLAG_RATIO_12         0b00001011     /* ratio 12 */
#define FLAG_RATIO_13         0b00001100     /* ratio 13 */
#define FLAG_RATIO_14         0b00001101     /* ratio 14 */
#define FLAG_RATIO_15         0b00001110     /* ratio 15 */
#define FLAG_RATIO_16         0b00001111     /* ratio 16 */
/* oscillator frequency */
#define FLAG_FREQ_1           0b00000000     /* frequency 1 */
#define FLAG_FREQ_2           0b00010000     /* frequency 2 */
#define FLAG_FREQ_3           0b00100000     /* frequency 3 */
#define FLAG_FREQ_4           0b00100000     /* frequency 4 */
#define FLAG_FREQ_5           0b01000000     /* frequency 5 */
#define FLAG_FREQ_6           0b01010000     /* frequency 6 */
#define FLAG_FREQ_7           0b01100000     /* frequency 7 */
#define FLAG_FREQ_8           0b01110000     /* frequency 8 */
#define FLAG_FREQ_9           0b10000000     /* frequency 9 (default) */
#define FLAG_FREQ_10          0b10010000     /* frequency 10 */
#define FLAG_FREQ_11          0b10100000     /* frequency 11 */
#define FLAG_FREQ_12          0b10110000     /* frequency 12 */
#define FLAG_FREQ_13          0b11000000     /* frequency 13 */
#define FLAG_FREQ_14          0b11010000     /* frequency 14 */
#define FLAG_FREQ_15          0b11100000     /* frequency 15 */
#define FLAG_FREQ_16          0b11110000     /* frequency 16 */


/*
 *  set pre-charge period
 *  - 2 byte command
 */

/* byte #1: command */
#define CMD_PRECHARGE         0b11011001     /* set pre-charge period */

/* byte #2: phase 1 and 2 periods */
/* phase 1 period */
#define FLAG_PHASE1_1         0b00000001     /* 1 DCLK */
#define FLAG_PHASE1_2         0b00000010     /* 2 DCLK (default) */
#define FLAG_PHASE1_3         0b00000010     /* 3 DCLK */
#define FLAG_PHASE1_4         0b00000100     /* 4 DCLK */
#define FLAG_PHASE1_5         0b00000101     /* 5 DCLK */
#define FLAG_PHASE1_6         0b00000110     /* 6 DCLK */
#define FLAG_PHASE1_7         0b00000111     /* 7 DCLK */
#define FLAG_PHASE1_8         0b00001000     /* 8 DCLK */
#define FLAG_PHASE1_9         0b00001001     /* 9 DCLK */
#define FLAG_PHASE1_10        0b00001010     /* 10 DCLK */
#define FLAG_PHASE1_11        0b00001011     /* 11 DCLK */
#define FLAG_PHASE1_12        0b00001100     /* 12 DCLK */
#define FLAG_PHASE1_13        0b00001101     /* 13 DCLK */
#define FLAG_PHASE1_14        0b00001110     /* 14 DCLK */
#define FLAG_PHASE1_15        0b00001111     /* 15 DCLK */
/* phase 2 period */
#define FLAG_PHASE2_1         0b00010000     /* 1 DCLK */
#define FLAG_PHASE2_2         0b00100000     /* 2 DCLK (default) */
#define FLAG_PHASE2_3         0b00100000     /* 3 DCLK */
#define FLAG_PHASE2_4         0b01000000     /* 4 DCLK */
#define FLAG_PHASE2_5         0b01010000     /* 5 DCLK */
#define FLAG_PHASE2_6         0b01100000     /* 6 DCLK */
#define FLAG_PHASE2_7         0b01110000     /* 7 DCLK */
#define FLAG_PHASE2_8         0b10000000     /* 8 DCLK */
#define FLAG_PHASE2_9         0b10010000     /* 9 DCLK */
#define FLAG_PHASE2_10        0b10100000     /* 10 DCLK */
#define FLAG_PHASE2_11        0b10110000     /* 11 DCLK */
#define FLAG_PHASE2_12        0b11000000     /* 12 DCLK */
#define FLAG_PHASE2_13        0b11010000     /* 13 DCLK */
#define FLAG_PHASE2_14        0b11100000     /* 14 DCLK */
#define FLAG_PHASE2_15        0b11110000     /* 15 DCLK */


/*
 *  set V_COM_H deselect level
 *  - 2 byte command
 */

/* byte #1: command */
#define CMD_DESELECT_LEVEL    0b11011011     /* set V_COM_H deselect level */

/* byte #2: level */
#define FLAG_LEVEL_65         0b00000000     /* 0.65 * Vcc */
#define FLAG_LEVEL_77         0b00100000     /* 0.77 * Vcc (default) */
#define FLAG_LEVEL_83         0b00110000     /* 0.83 * Vcc */



/* ************************************************************************
 *   advanced graphic commands
 * ************************************************************************ */


/*
 *  set fade-out and blinking
 *  - 2 byte command
 */

/* byte #1: command */
#define CMD_FADE_BLINK        0b00100011     /* set fade-out/blinking */

/* byte #2: modes */
#define FLAG_FADE_BLINK_OFF   0b00000000     /* disable fade-out/blinking */
#define FLAG_FADE_ON          0b00100000     /* enable fade-out */
#define FLAG_BLINK_ON         0b00110000     /* enable blinking */
/* time interval for fade-out steps */
#define FLAG_FADE_8           0b00000000     /* 8 frames */
#define FLAG_FADE_16          0b00000001     /* 16 frames */
#define FLAG_FADE_24          0b00000010     /* 24 frames */
#define FLAG_FADE_32          0b00000011     /* 32 frames */
#define FLAG_FADE_40          0b00000100     /* 40 frames */
#define FLAG_FADE_48          0b00000101     /* 48 frames */
#define FLAG_FADE_56          0b00000110     /* 56 frames */
#define FLAG_FADE_64          0b00000111     /* 64 frames */
#define FLAG_FADE_72          0b00001000     /* 72 frames */
#define FLAG_FADE_80          0b00001001     /* 80 frames */
#define FLAG_FADE_88          0b00001010     /* 88 frames */
#define FLAG_FADE_96          0b00001011     /* 96 frames */
#define FLAG_FADE_104         0b00001100     /* 104 frames */
#define FLAG_FADE_112         0b00001101     /* 112 frames */
#define FLAG_FADE_120         0b00001110     /* 120 frames */
#define FLAG_FADE_128         0b00001111     /* 128 frames */


/*
 *  set zoom-in
 *  - 2 byte command
 */

/* byte #1: command */
#define CMD_ZOOM              0b11010110     /* set zoom-in */

/* byte #2: mode */
#define FLAG_ZOOM_OFF         0b00000000     /* disable zoom-in */
#define FLAG_ZOOM_ON          0b00000001     /* enable zoom-in */


/*
 *  no operation
 */

#define CMD_NOP               0b11100011     /* no operation */



/* ************************************************************************
 *   EOF
 * ************************************************************************ */
