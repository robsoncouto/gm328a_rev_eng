/* ************************************************************************
 *
 *   ILI9341 color graphic display controller
 *
 *   (c) 2015-2016 by Markus Reschke
 *
 * ************************************************************************ */


/* ************************************************************************
 *   regulative command set
 * ************************************************************************ */


/*
 *  no operation
 *  - 1 byte cmd
 */

#define CMD_NOP               0b00000000     /* no operation */


/*
 *  software reset
 *  - 1 byte cmd
 */

#define CMD_RESET             0b00000001     /* software reset */


/*
 *  read display identification information
 *  - 1 byte cmd + 4 bytes data (read mode)
 */

#define CMD_READ_DISP_ID      0b00000100     /* read display ID */

/* data byte #1: dummy byte */
/* data byte #2: manufacturer ID */
/* data byte #3: module/driver version ID */
/* data byte #4: module/driver ID */


/*
 *  read display status
 *  - 1 byte cmd + 5 bytes data (read mode)
 */

#define CMD_READ_STATUS       0b00001001     /* read display status */

/* data byte #1: dummy data */

/* data byte #2: status */
  /* horizontal refresh: */
#define FLAG_STAT_HOR_NORM    0b00000000     /* left to right */
#define FLAG_STAT_HOR_REV     0b00000010     /* right to left */
  /* RGB/BGR order */
#define FLAG_STAT_RGB         0b00000000     /* RGB */
#define FLAG_STAT_BGR         0b00000100     /* BGR */
  /* vertical refresh: */
#define FLAG_STAT_VER_NORM    0b00000000     /* top to bottom */
#define FLAG_STAT_VER_REV     0b00001000     /* bottom to top */
  /* row/column exchange: */
#define FLAG_STAT_EXCH_NORM   0b00000000     /* normal */
#define FLAG_STAT_EXCH_REV    0b00010000     /* reversed */
  /* column address order: */
#define FLAG_STAT_COL_NORM    0b00000000     /* left to right */
#define FLAG_STAT_COL_REV     0b00100000     /* right to left */
  /* row address order: */
#define FLAG_STAT_ROW_NORM    0b00000000     /* top to bottom */
#define FLAG_STAT_ROW_REV     0b01000000     /* bottom to top */
  /* booster voltage: */
#define FLAG_STAT_BOOST_OFF   0b00000000     /* booster on */
#define FLAG_STAT_BOOST_ON    0b10000000     /* booster off */

/* data byte #3: status */
  /* display normal mode: */
#define FLAG_STAT_NORM_OFF    0b00000000     /* off */
#define FLAG_STAT_NORM_ON     0b00000001     /* on */
  /* sleep mode: */
#define FLAG_STAT_SLEEP_OFF   0b00000000     /* off */
#define FLAG_STAT_SLEEP_ON    0b00000010     /* on */
  /* partial mode: */
#define FLAG_STAT_PART_OFF    0b00000000     /* off */
#define FLAG_STAT_PART_ON     0b00000100     /* on */
  /* idle mode: */
#define FLAG_STAT_IDLE_OFF    0b00000000     /* off */
#define FLAG_STAT_IDLE_ON     0b00001000     /* on */
  /* interface color pixel format: */
#define FLAG_STAT_PIX_16      0b01010000     /* 16 bits per pixel */
#define FLAG_STAT_PIX_18      0b01100000     /* 18 bits per pixel */

/* data byte #4: status */
  /* gamma curve bit #2: always 0 */
  /* tearing effect line: */
#define FLAG_STAT_TEAR_OFF    0b00000000     /* off */
#define FLAG_STAT_TEAR_ON     0b00000010     /* on */
  /* display: */
#define FLAG_STAT_DISP_OFF    0b00000000     /* off */
#define FLAG_STAT_DISP_ON     0b00000100     /* on */

/* data byte #5: status */
  /* tearing effect line mode: */
#define FLAG_STAT_TEAR_MODE1  0b00000000     /* mode 1: V-blanking */
#define FLAG_STAT_TEAR_MODE2  0b00100000     /* mode 2: V-blanking and H-blanking */
  /* gamma curve bits #1 and #0 (bit #2 is always 0): */
#define FLAG_STAT_GAMMA1      0b00000000     /* gamma curve 1 */


/*
 *  read display power mode
 *  - 1 byte cmd + 2 bytes data (read mode)
 */

#define CMD_READ_PWR_MODE     0b00001010     /* read display power mode */

/* data byte #1: dummy data */

/* data byte #2: status */
  /* display: */
#define FLAG_DISPLAY_OFF      0b00000000     /* off */
#define FLAG_DISPLAY_ON       0b00000100     /* on */
  /* display normal mode: */
#define FLAG_NORMAL_OFF       0b00000000     /* off */
#define FLAG_NORMAL_ON        0b00001000     /* on */
  /* sleep mode: */
#define FLAG_SLEEP_OFF        0b00000000     /* off */
#define FLAG_SLEEP_ON         0b00010000     /* on */
  /* partial mode: */
#define FLAG_PARTIAL_OFF      0b00000000     /* off */
#define FLAG_PARTIAL_ON       0b00100000     /* on */
  /* idle mode: */
#define FLAG_IDLE_OFF         0b00000000     /* off */
#define FLAG_IDLE_ON          0b01000000     /* on */
  /* booster: */
#define FLAG_BOOSTER_OFF      0b00000000     /* off or faulty */
#define FLAG_BOOSTER_ON       0b10000000     /* on and working fine */


/*
 *  read display MADCTL (memory acccess control)
 *  - 1 byte cmd + 2 bytes data (read mode)
 */

#define CMD_READ_MADCTL       0b00001011     /* read display MADCTL */

/* data byte #1: dummy data */

/* data byte #2: status */
/* same as byte #1 of CMD_MEM_CTRL */
  /* horizontal refereshing direction: */
#define RFLAG_MH_NORM         0b00000000     /* left to right */
#define RFLAG_MH_REV          0b00000100     /* right to left */
  /* color selector switch: */
#define RFLAG_BGR_RGB         0b00000000     /* RGB color filter */
#define RFLAG_BGR_BGR         0b00001000     /* BGR color filter */
  /* vertical refereshing direction: */
#define RFLAG_ML_NORM         0b00000000     /* top to bottom */
#define RFLAG_ML_REV          0b00010000     /* bottom to top */
  /* MCU to memory direction for row column exchange: */
#define RFLAG_MV_NORM         0b00000000     /* normal */
#define RFLAG_MV_REV          0b00100000     /* reversed */
  /* MCU to memory direction for columns: */
#define RFLAG_MX_NORM         0b00000000     /* left to right */
#define RFLAG_MX_REV          0b01000000     /* right to left */
  /* MCU to memory direction for rows: */
#define RFLAG_MY_NORM         0b00000000     /* top to bottom */
#define RFLAG_MY_REV          0b10000000     /* bottom to top */


/*
 *  read display pixel format
 *  - 1 byte cmd + 2 bytes data (read mode)
 */

#define CMD_READ_PIX_FORMAT   0b00001100     /* read display pixel format */

/* data byte #1: dummy data */

/* data byte #2: status */
/* same as byte #1 of CMD_PIX_FORMAT_SET */
  /* pixel format of MCU interface: */
#define RFLAG_DBI_16          0b00000101     /* 16 bits per pixel */
#define RFLAG_DBI_18          0b00000110     /* 18 bits per pixel */
  /* pixel format of RGB interface: */
#define RFLAG_DPI_16          0b01010000     /* 16 bits per pixel */
#define RFLAG_DPI_18          0b01100000     /* 18 bits per pixel */
  /* data transfer: */
#define RFLAG_RIM_OFF         0b00000000     /* 18 bit */
#define RFLAG_RIM_ON          0b10000000     /* 3 times 6 bit */


/*
 *  read display image format
 *  - 1 byte cmd + 2 bytes data (read mode)
 */

#define CMD_READ_IMG_FORMAT   0b00001101     /* read display image format */

/* data byte #1: dummy data */

/* data byte #2: status */
  /* gamma curve */
#define FLAG_IMG_GAMMA1       0b00000000     /* gamma curve 1 */


/*
 *  read display signal mode
 *  - 1 byte cmd + 2 bytes data (read mode)
 */

#define CMD_READ_SIG_MODE     0b00001110     /* read display signal mode */

/* data byte #1: dummy data */

/* data byte #2: mode flags */
  /* data enable (DE, RGB interface): */
#define FLAG_SIG_DE_OFF       0b00000000     /* data enable off */
#define FLAG_SIG_DE_ON        0b00000100     /* data enable on */
  /* pixel clock (DOTCLK, RGB interface): */
#define FLAG_SIG_CLK_OFF      0b00000000     /* pixel clock off */
#define FLAG_SIG_CLK_ON       0b00001000     /* pixel clock on */
  /* vertical sync (RGB interface): */
#define FLAG_SIG_VSYNC_OFF    0b00000000     /* V-sync off */
#define FLAG_SIG_VSYNC_ON     0b00010000     /* V-sync on */
  /* horizontal sync (RGB interface): */
#define FLAG_SIG_HSYNC_OFF    0b00000000     /* H-sync off */
#define FLAG_SIG_HSYNC_ON     0b00100000     /* H-sync on */
  /* tearing effect line mode: */
#define FLAG_SIG_TEAR_1       0b00000000     /* effect mode 1 */
#define FLAG_SIG_TEAR_2       0b01000000     /* effect mode 2 */
  /* tearing effect line: */
#define FLAG_SIG_TEAR_OFF     0b00000000     /* effect off */
#define FLAG_SIG_TEAR_ON      0b10000000     /* effect on */


/*
 *  read display self-diagnostic result
 *  - 1 byte cmd + 2 bytes data (read mode)
 */

#define CMD_READ_DIAG         0b00001111     /* read diagnostics */

/* data byte #1: dummy data */

/* data byte #2: diag flags */
#define FLAG_DIAG_DISP_OK     0b01000000     /* display works */
#define FLAG_DIAG_REGS_OK     0b10000000     /* register values loaded */


/*
 *  enter sleep mode (sleep in)
 *  - 1 byte cmd
 */

#define CMD_SLEEP_IN          0b00010000     /* enter sleep mode */


/*
 *  leave sleep mode (sleep out)
 *  - 1 byte cmd
 */

#define CMD_SLEEP_OUT         0b00010001     /* leave sleep mode */


/*
 *  enter partial display mode
 *  - 1 byte cmd
 */

#define CMD_PARTIAL_MODE      0b00010010     /* enter partial mode */


/*
 *  enter normal display mode
 *  - 1 byte cmd
 */

#define CMD_NORMAL_MODE       0b00010011     /* normal mode on */


/*
 *  disable display inversion
 *  - 1 byte cmd
 */

#define CMD_INVERSION_OFF     0b00100000     /* display inversion off */


/*
 *  enable display inversion (invert pixels)
 *  - 1 byte cmd
 */

#define CMD_INVERSION_ON      0b00100001     /* display inversion on */


/*
 *  select gamma curve
 *  - 1 byte cmd + 1 byte data
 */

#define CMD_GAMMA_SET         0b00100110     /* gamma set */

/* data byte #1: gamma curve */
#define FLAG_GC_1             0b00000001     /* gamma curve 1 */


/*
 *  disable display output
 *  - 1 byte cmd
 */

#define CMD_DISPLAY_OFF       0b00101000     /* display off */


/*
 *  enable display output
 *  - 1 byte cmd
 */

#define CMD_DISPLAY_ON        0b00101001     /* display on */


/*
 *  set column address (accessable frame area)
 *  - 1 byte cmd + 4 bytes data
 */

#define CMD_COL_ADDR_SET      0b00101010     /* set column address */

/* data byte #1: start column, MSB (bits 15-8) */
/* data byte #2: start column, LSB (bits 7-0) */
/* data byte #3: end column, MSB (bits 15-8) */
/* data byte #4: end column, LSB (bits 7-0) */
/* valid range: 0x0000 - 0x00ef/0x013f */


/*
 *  set page address (row, accessable frame area)
 *  - 1 byte cmd + 4 bytes data
 */

#define CMD_PAGE_ADDR_SET     0b00101011     /* set page address */

/* data byte #1: start page, MSB (bits 15-8) */
/* data byte #2: start page, LSB (bits 7-0) */
/* data byte #3: end page, MSB (bits 15-8) */
/* data byte #4: end page, LSB (bits 7-0) */
/* valid range: 0x0000 - 0x013f/0x00ef */


/*
 *  write memory (starting at SC, SP)
 *  - 1 byte cmd + x bytes data
 */

#define CMD_MEM_WRITE         0b00101100     /* write memory */

/* data byte #1 - #x: image data */


/*
 *  define color conversion look-up-table
 *  - 1 byte cmd + 128 bytes data
 */

#define CMD_COLOR_SET         0b00101101     /* color set */

/* data byte #1 - #32: R00 - R31 (bits 5-0) */
/* data byte #33 - #96: G00 - G64 (bits 5-0) */
/* data byte #97 - #128: B00 - B31 (bits 5-0) */


/*
 *  read memory (starting at SC, SP)
 *  - 1 byte cmd + x bytes data (read mode)
 */

#define CMD_MEM_READ          0b00101110     /* read memory */

/* data byte #1: dummy data */
/* data byte #2 - #x: image data */


/*
 *  partial area
 *  - 1 byte cmd + 4 bytes data
 */

#define CMD_PARTIAL_AREA      0b00110000     /* set partial area */

/* data byte #1: start row, MSB (bits 15-8) */
/* data byte #2: start row, LSB (bits 7-0) */
/* data byte #3: end row, MSB (bits 15-8) */
/* data byte #4: end row, LSB (bits 7-0) */
/* valid range: 0x0000 - 0x013f */


/*
 *  vertical scrolling definition
 *  - 1 byte cmd + 6 bytes data
 */

#define CMD_V_SCROLL_DEF      0b00110011     /* set vertical scrolling area */

/* data byte #1: top fixed area line number, MSB (bits 15-8) */
/* data byte #2: top fixed area line number, LSB (bits 7-0) */
/* data byte #3: height of scrolling area, MSB (bits 15-8) */
/* data byte #4: height of scrolling area, LSB (bits 7-0) */
/* data byte #5: bottom fixed area line number, MSB (bits 15-8) */
/* data byte #6: bottom fixed area line number, LSB (bits 7-0) */


/*
 *  disable tearing effect line 
 *  - 1 byte cmd
 */

#define CMD_TEAR_OFF          0b00110100     /* disable tearing effect */


/*
 *  enable tearing effect line 
 *  - 1 byte cmd + 1 byte data
 */

#define CMD_TEAR_ON           0b00110101     /* enable tearing effect */

/* data byte #1: mode */
#define FLAG_M_0              0b00000000     /* V-blanking only */
#define FLAG_M_1              0b00000001     /* V-blanking and H-blanking */


/*
 *  memory access control 
 *  - 1 byte cmd + 1 byte data
 */

#define CMD_MEM_CTRL          0b00110110     /* memory access control */

/* data byte #1: read/write scanning direction of frame memory */
/* same as byte #2 of CMD_READ_MADCTL */
  /* horizontal refereshing direction: */
#define FLAG_MH_NORM          0b00000000     /* left to right */
#define FLAG_MH_REV           0b00000100     /* right to left */
  /* color selector switch: */
#define FLAG_BGR_RGB          0b00000000     /* RGB color filter */
#define FLAG_BGR_BGR          0b00001000     /* BGR color filter */
  /* vertical refereshing direction: */
#define FLAG_ML_NORM          0b00000000     /* top to bottom */
#define FLAG_ML_REV           0b00010000     /* bottom to top */
  /* MCU to memory direction for row column exchange: */
#define FLAG_MV_NORM          0b00000000     /* normal */
#define FLAG_MV_REV           0b00100000     /* reversed (swap x and y) */
  /* MCU to memory direction for columns: */
#define FLAG_MX_NORM          0b00000000     /* left to right */
#define FLAG_MX_REV           0b01000000     /* right to left */
  /* MCU to memory direction for rows: */
#define FLAG_MY_NORM          0b00000000     /* top to bottom */
#define FLAG_MY_REV           0b10000000     /* bottom to top */


/*
 *  set vertical scrolling start address 
 *  - 1 byte cmd + 2 bytes data
 */

#define CMD_V_SCROLL_ADDR     0b00110111     /* set vertical scrolling start address */

/* data byte #1: line number, MSB (bits 15-8) */
/* data byte #2: line number, MSB (bits 7-0) */


/*
 *  leave idle mode (full color depth)
 *  - 1 byte cmd
 */

#define CMD_IDLE_OFF          0b00111000     /* disable idle mode */


/*
 *  enter idle mode (8bit color depth)
 *  - 1 byte cmd
 */

#define CMD_IDLE_ON           0b00111001     /* enable idle mode */


/*
 *  set pixel format for RGB image data
 *  - 1 byte cmd + 1 byte data
 */

#define CMD_PIX_FORMAT_SET    0b00111010     /* set pixel format */

/* data byte #1: formats */
/* same as byte #2 of CMD_READ_PIX_FORMAT */
  /* pixel format of MCU interface: */
#define FLAG_DBI_16           0b00000101     /* 16 bits per pixel */
#define FLAG_DBI_18           0b00000110     /* 18 bits per pixel */
  /* pixel format of RGB interface: */
#define FLAG_DPI_16           0b01010000     /* 16 bits per pixel */
#define FLAG_DPI_18           0b01100000     /* 18 bits per pixel */


/*
 *  write memory continue (starting at last pixel position + 1)
 *  - 1 byte cmd + x bytes data
 */

#define CMD_WRITE_MEM_CONT    0b00111100     /* write memory continue */

/* data byte #1 - #x: image data */


/*
 *  read memory continue (starting at last pixel position + 1)
 *  - 1 byte cmd + x bytes data (read mode)
 */

#define CMD_READ_MEM_CONT     0b00111110     /* read memory continue */

/* data byte #1: dummy data */
/* data byte #2 - #x: image data */


/*
 *  set tearing effect scanline 
 *  - 1 byte cmd + 2 bytes data
 */

#define CMD_SET_SCANLINE      0b01000100     /* set scanline */

/* data byte #1: line number, MSB (bit 8) */
#define FLAG_STS_HIGH_MIN     0b00000000     /* minimum value */
#define FLAG_STS_HIGH_MAX     0b00000001     /* maximum value */

/* data byte #2: line number, LSB (bits 7-0)*/
#define FLAG_STS_LOW_MIN      0b00000000     /* minimum value */
#define FLAG_STS_LOW_MAX      0b11111111     /* maximum value */


/*
 *  get tearing effect scanline
 *  - 1 byte cmd + 3 bytes data (read mode)
 */

#define CMD_GET_SCANLINE      0b01000100     /* get scanline  */

/* data byte #1: dummy data */
/* data byte #2: line number, MSB (bits 9-8) */
/* data byte #3: line number, LSB (bits 7-0) */


/*
 *  write display brightness
 *  - 1 byte cmd + 1 byte data
 */

#define CMD_WRITE_BRIGHT      0b01010001     /* write display brightness */

/* data byte #1: brightness value */
#define FLAG_DBV_MIN          0b00000000     /* minimum value */
#define FLAG_DBV_MAX          0b11111111     /* maximum value */


/*
 *  read display brightness 
 *  - 1 byte cmd + 2 bytes data (read mode)
 */

#define CMD_READ_BRIGHT       0b01010010     /* read display brightness */

/* data byte #1: dummy data */
/* data byte #2: brightness value, please see above */


/*
 *  write CTRL
 *  - 1 byte cmd + 1 byte data
 */

#define CMD_WRITE_CTRL        0b01010011     /* write CTRL */

/* data byte #1: settings */
  /* backlight: */
#define FLAG_BL_OFF           0b00000000     /* off */
#define FLAG_BL_ON            0b00000100     /* on */
  /* display dimming for manual brightness setting: */
#define FLAG_DD_OFF           0b00000000     /* off */
#define FLAG_DD_ON            0b00001000     /* on */
  /* brightness control block: */
#define FLAG_BCTRL_OFF        0b00000000     /* off (registers are 0x00) */
#define FLAG_BCTRL_ON         0b00100000     /* on (registers are active) */


/*
 *  read CTRL 
 *  - 1 byte cmd + 2 bytes data (read mode)
 */

#define CMD_READ_CTRL         0b01010100     /* read CTRL */

/* data byte #1: dummy data */
/* data byte #2: settings, please see above */


/*
 *  write content adaptive brightness control (CABC)
 *  - 1 byte cmd + 1 byte data
 */

#define CMD_WRITE_CABC        0b01010101     /* write content adaptive brightness control */

/* data byte #1: mode */
#define FLAG_C_OFF            0b00000000     /* off */
#define FLAG_C_INTERFACE      0b00000001     /* user interface image */
#define FLAG_C_STILL          0b00000010     /* still picture */
#define FLAG_C_MOVING         0b00000011     /* moving image */


/*
 *  read content adaptive brightness control (CABC) 
 *  - 1 byte cmd + 2 bytes data (read mode)
 */

#define CMD_READ_CABC         0b01010110     /* read CABC */

/* data byte #1: dummy data */
/* data byte #2: mode, please see FLAG_C_* above */


/*
 *  write CABC minimum brightness
 *  - 1 byte cmd + 1 byte data
 */

#define CMD_WRITE_MIN_CABC    0b01011110     /* write CABC minimum brightness */

/* data byte #1: minimum brightness level of CABC function */
#define FLAG_CABC_MIN         0b00000000     /* minimum value */
#define FLAG_CABC_MAX         0b11111111     /* maximum value */


/*
 *  read CABC minimum brightness
 *  - 1 byte cmd + 2 bytes data (read mode)
 */

#define CMD_READ_MIN_CABC     0b01010111     /* read CABC minimum brightness */

/* data byte #1: dummy data */
/* data byte #2: minimum brightness level of CABC function */


/*
 *  read ID1
 *  - 1 byte cmd + 2 bytes data (read mode)
 */

#define CMD_READ_ID1          0b11011010     /* read ID1 (manufacturer) */

/* data byte #1: dummy data */
/* data byte #2: LCD module manufacturer ID */


/*
 *  read ID2
 *  - 1 byte cmd + 2 bytes data (read mode)
 */

#define CMD_READ_ID2          0b11011011     /* read ID2 (driver version) */

/* data byte #1: dummy data */
/* data byte #2: LCD module/driver version */


/*
 *  read ID3
 *  - 1 byte cmd + 2 bytes data (read mode)
 */

#define CMD_READ_ID3          0b11011100     /* read ID3 (driver) */

/* data byte #1: dummy data */
/* data byte #2: LCD module/driver */


/* ************************************************************************
 *   extended command set
 * ************************************************************************ */


/*
 *  RGB interface signal control
 *  - 1 byte cmd + 1 byte data
 */

#define CMD_RGB_SIG_CTRL      0b10110000     /* RGB interface signal control */

/* data byte #1: switches */
  /* DE polarity: */
#define FLAG_EPL_HIGH         0b00000000     /* high enable */
#define FLAG_EPL_LOW          0b00000001     /* lwo enable */
  /* DOTCLK polarity: */
#define FLAG_DPL_RISE         0b00000000     /* data fetched at rising edge */
#define FLAG_DPL_FALL         0b00000010     /* data fetched at falling edge */
  /* HSYNC polarity: */
#define FLAG_HSPL_LOW         0b00000000     /* low level sync */
#define FLAG_HSPL_HIGH        0b00000100     /* high level sync */
  /* VSYNC polarity: */
#define FLAG_VSPL_LOW         0b00000000     /* low level sync */
#define FLAG_VSPL_HIGH        0b00001000     /* high level sync */
  /* RGB interface, please see datasheet: */
#define FLAG_RCM_DE           0b01000000     /* DE mode */
#define FLAG_RCM_SYNC         0b01100000     /* SYNC mode */
  /* data path (RGB interface): */
#define FLAG_BYPASS_DIRECT    0b00000000     /* directly to shift register */
#define FLAG_BYPASS_MEMORY    0b10000000     /* to memory */


/*
 *  frame control in normal mode (full color)
 *  - 1 byte cmd + 2 bytes data
 */

#define CMD_FRAME_CTRL_NORM   0b10110001     /* frame control in normal mode */

/* data byte #1: division ratio for normal mode */
#define FLAG_DIVA_1           0b00000000     /* f_OSC */
#define FLAG_DIVA_2           0b00000001     /* f_OSC/2 */
#define FLAG_DIVA_4           0b00000010     /* f_OSC/4 */
#define FLAG_DIVA_8           0b00000011     /* f_OSC/8 */

/* data byte #2: line period for normal mode (clocks per line) */
#define FLAG_RTNA_16          0b00010000     /* 16 clocks */
#define FLAG_RTNA_17          0b00010001     /* 17 clocks */
#define FLAG_RTNA_18          0b00010010     /* 18 clocks */
#define FLAG_RTNA_19          0b00010011     /* 19 clocks */
#define FLAG_RTNA_20          0b00010100     /* 20 clocks */
#define FLAG_RTNA_21          0b00010101     /* 21 clocks */
#define FLAG_RTNA_22          0b00010110     /* 22 clocks */
#define FLAG_RTNA_23          0b00010111     /* 23 clocks */
#define FLAG_RTNA_24          0b00011000     /* 24 clocks */
#define FLAG_RTNA_25          0b00011001     /* 25 clocks */
#define FLAG_RTNA_26          0b00011010     /* 26 clocks */
#define FLAG_RTNA_27          0b00011011     /* 27 clocks */
#define FLAG_RTNA_28          0b00011100     /* 28 clocks */
#define FLAG_RTNA_29          0b00011101     /* 29 clocks */
#define FLAG_RTNA_30          0b00011110     /* 30 clocks */
#define FLAG_RTNA_31          0b00011111     /* 31 clocks */


/*
 *  frame control in idle mode (8bit color depth)
 *  - 1 byte cmd + 2 bytes data
 */

#define CMD_FRAME_CTRL_IDLE   0b10110010     /* frame control in idle mode */

/* data byte #1: division ratio for idle mode */
#define FLAG_DIVB_1           0b00000000     /* f_OSC */
#define FLAG_DIVB_2           0b00000001     /* f_OSC/2 */
#define FLAG_DIVB_4           0b00000010     /* f_OSC/4 */
#define FLAG_DIVB_8           0b00000011     /* f_OSC/8 */

/* data byte #2: line period for idle mode (clocks per line) */
#define FLAG_RTNB_16          0b00010000     /* 16 clocks */
#define FLAG_RTNB_17          0b00010001     /* 17 clocks */
#define FLAG_RTNB_18          0b00010010     /* 18 clocks */
#define FLAG_RTNB_19          0b00010011     /* 19 clocks */
#define FLAG_RTNB_20          0b00010100     /* 20 clocks */
#define FLAG_RTNB_21          0b00010101     /* 21 clocks */
#define FLAG_RTNB_22          0b00010110     /* 22 clocks */
#define FLAG_RTNB_23          0b00010111     /* 23 clocks */
#define FLAG_RTNB_24          0b00011000     /* 24 clocks */
#define FLAG_RTNB_25          0b00011001     /* 25 clocks */
#define FLAG_RTNB_26          0b00011010     /* 26 clocks */
#define FLAG_RTNB_27          0b00011011     /* 27 clocks */
#define FLAG_RTNB_28          0b00011100     /* 28 clocks */
#define FLAG_RTNB_29          0b00011101     /* 29 clocks */
#define FLAG_RTNB_30          0b00011110     /* 30 clocks */
#define FLAG_RTNB_31          0b00011111     /* 31 clocks */


/*
 *  frame control in partial mode
 *  - 1 byte cmd + 2 bytes data
 */

#define CMD_FRAME_CTRL_PART   0b10110011     /* frame control in partial mode */

/* data byte #1: division ratio for partial mode */
#define FLAG_DIVC_1           0b00000000     /* f_OSC */
#define FLAG_DIVC_2           0b00000001     /* f_OSC/2 */
#define FLAG_DIVC_4           0b00000010     /* f_OSC/4 */
#define FLAG_DIVC_8           0b00000011     /* f_OSC/8 */

/* data byte #2: line period for partial mode (clocks per line) */
#define FLAG_RTNC_16          0b00010000     /* 16 clocks */
#define FLAG_RTNC_17          0b00010001     /* 17 clocks */
#define FLAG_RTNC_18          0b00010010     /* 18 clocks */
#define FLAG_RTNC_19          0b00010011     /* 19 clocks */
#define FLAG_RTNC_20          0b00010100     /* 20 clocks */
#define FLAG_RTNC_21          0b00010101     /* 21 clocks */
#define FLAG_RTNC_22          0b00010110     /* 22 clocks */
#define FLAG_RTNC_23          0b00010111     /* 23 clocks */
#define FLAG_RTNC_24          0b00011000     /* 24 clocks */
#define FLAG_RTNC_25          0b00011001     /* 25 clocks */
#define FLAG_RTNC_26          0b00011010     /* 26 clocks */
#define FLAG_RTNC_27          0b00011011     /* 27 clocks */
#define FLAG_RTNC_28          0b00011100     /* 28 clocks */
#define FLAG_RTNC_29          0b00011101     /* 29 clocks */
#define FLAG_RTNC_30          0b00011110     /* 30 clocks */
#define FLAG_RTNC_31          0b00011111     /* 31 clocks */


/*
 *  display inversion control
 *  - 1 byte cmd + 1 byte data
 */

#define CMD_INVERS_CTRL       0b10110100     /* display inversion control */

/* data byte #1: mode switches */
  /* for full color partial mode: */
#define FLAG_NLC_LINE         0b00000000     /* line inversion */
#define FLAG_NLC_FRAME        0b00000001     /* frame inversion */
  /* for idle mode: */
#define FLAG_NLB_LINE         0b00000000     /* line inversion */
#define FLAG_NLB_FRAME        0b00000010     /* frame inversion */
  /* for full color normal mode: */
#define FLAG_NLA_LINE         0b00000000     /* line inversion */
#define FLAG_NLA_FRAME        0b00000100     /* frame inversion */


/*
 *  blanking porch control
 *  - 1 byte cmd + 4 byte data
 */

#define CMD_BLANK_CTRL        0b10110101     /* blanking porch control */

/* data byte #1: line number of vertical front porch period */
  /* number of HSYNCs, please see datasheet: */
#define FLAG_VFP_MIN          0b00000010     /* minimum value */
#define FLAG_VFP_MAX          0b01111111     /* maximum value */

/* data byte #2: line number of vertical back porch period */
  /* number of HSYNCs, please see datasheet: */
#define FLAG_VFP_MIN          0b00000010     /* minimum value */
#define FLAG_VFP_MAX          0b01111111     /* maximum value */

/* data byte #3: line number of horizontal front porch period */
  /* number of DOTCLKs, please see datasheet: */
#define FLAG_HFP_MIN          0b00000010     /* minimum value */
#define FLAG_HFP_MAX          0b00011111     /* maximum value */

/* data byte #4: line number of horizontal back porch period */
  /* number of DOTCLKs, please see datasheet: */
#define FLAG_HBP_MIN          0b00000010     /* minimum value */
#define FLAG_HBP_MAX          0b00011111     /* maximum value */


/*
 *  display function control
 *  - 1 byte cmd + 4 bytes data
 */

#define CMD_FUNC_CTRL         0b10110110     /* display function control */

/* data byte #1: */
  /* source/VCOM output in non-display area in partial display mode: */
                             /* source pos / source neg - VCOM pos / VCOM neg */
#define FLAG_PT_0            0b00000000     /* V63 / V0 - VCOML / VCOMH */
#define FLAG_PT_1            0b00000001     /* V0 / V63 - VCOML / VCOMH */
#define FLAG_PT_2            0b00000010     /* AGND / AGND - AGND / AGND */
#define FLAG_PT_3            0b00000011     /* Hi-Z / Hi-Z - AGND / AGND */
  /* scan mode in non-display area: */
#define FLAG_PTG_0           0b00000000     /* normal scan */
#define FLAG_PTG_2           0b00001000     /* interval scan */

/* data byte #2: */
  /* scan cycle interval of gate driver in non-display area: */
#define FLAG_ISC_01          0b00000000     /* 1 frame */
#define FLAG_ISC_03          0b00000001     /* 3 frames */
#define FLAG_ISC_05          0b00000010     /* 5 frames */
#define FLAG_ISC_07          0b00000011     /* 7 frames */
#define FLAG_ISC_09          0b00000100     /* 9 frames */
#define FLAG_ISC_11          0b00000101     /* 11 frames */
#define FLAG_ISC_13          0b00000110     /* 13 frames */
#define FLAG_ISC_15          0b00000111     /* 15 frames */
#define FLAG_ISC_17          0b00001000     /* 17 frames */
#define FLAG_ISC_19          0b00001001     /* 19 frames */
#define FLAG_ISC_21          0b00001010     /* 21 frames */
#define FLAG_ISC_23          0b00001011     /* 24 frames */
#define FLAG_ISC_25          0b00001100     /* 25 frames */
#define FLAG_ISC_27          0b00001101     /* 27 frames */
#define FLAG_ISC_29          0b00001110     /* 29 frames */
#define FLAG_ISC_31          0b00001111     /* 31 frames */
  /* gate driver pin arrangement (in combination with GS): */
#define FLAG_SM_0            0b00000000     /* in sequence */
#define FLAG_SM_1            0b00010000     /* even/odd separated */
  /* output shift direction / source driver scan direction: */
#define FLAG_SS_0            0b00000000     /* S1 -> S720 */
#define FLAG_SS_1            0b00100000     /* S720 -> S1 */
  /* gate driver scan direction: */
#define FLAG_GS_0            0b00000000     /* G1 -> G320 */
#define FLAG_GS_1            0b01000000     /* G320 -> G1 */
  /* liquid crytal type: */
#define FLAG_REV_0           0b00000000     /* normally black */
#define FLAG_REV_1           0b10000000     /* normally white */

/* data byte #3: number of lines to drive LCD at an interval of 8 lines: */
#define FLAG_NL_016          0b00000001     /* 16 lines */
#define FLAG_NL_024          0b00000010     /* 24 lines */
#define FLAG_NL_036          0b00000011     /* 36 lines */
#define FLAG_NL_040          0b00000100     /* 40 lines */
#define FLAG_NL_048          0b00000101     /* 48 lines */
#define FLAG_NL_056          0b00000110     /* 56 lines */
#define FLAG_NL_064          0b00000111     /* 64 lines */
#define FLAG_NL_072          0b00001000     /* 72 lines */
#define FLAG_NL_080          0b00001001     /* 80 lines */
#define FLAG_NL_088          0b00001010     /* 88 lines */
#define FLAG_NL_096          0b00001011     /* 96 lines */
#define FLAG_NL_104          0b00001100     /* 104 lines */
#define FLAG_NL_112          0b00001101     /* 112 lines */
#define FLAG_NL_120          0b00001110     /* 120 lines */
#define FLAG_NL_128          0b00001111     /* 128 lines */
#define FLAG_NL_136          0b00010000     /* 136 lines */
#define FLAG_NL_144          0b00010001     /* 144 lines */
#define FLAG_NL_152          0b00010010     /* 152 lines */
#define FLAG_NL_160          0b00010011     /* 160 lines */
#define FLAG_NL_168          0b00010100     /* 168 lines */
#define FLAG_NL_176          0b00010101     /* 176 lines */
#define FLAG_NL_184          0b00010110     /* 184 lines */
#define FLAG_NL_192          0b00010111     /* 192 lines */
#define FLAG_NL_200          0b00011000     /* 200 lines */
#define FLAG_NL_208          0b00011001     /* 208 lines */
#define FLAG_NL_216          0b00011010     /* 216 lines */
#define FLAG_NL_224          0b00011011     /* 224 lines */
#define FLAG_NL_232          0b00011100     /* 232 lines */
#define FLAG_NL_240          0b00011101     /* 240 lines */
#define FLAG_NL_248          0b00011110     /* 248 lines */
#define FLAG_NL_256          0b00011111     /* 256 lines */
#define FLAG_NL_264          0b00100000     /* 264 lines */
#define FLAG_NL_272          0b00100001     /* 272 lines */
#define FLAG_NL_280          0b00100010     /* 280 lines */
#define FLAG_NL_288          0b00100011     /* 288 lines */
#define FLAG_NL_296          0b00100100     /* 296 lines */
#define FLAG_NL_304          0b00100101     /* 304 lines */
#define FLAG_NL_312          0b00100110     /* 312 lines */
#define FLAG_NL_320          0b00100111     /* 320 lines */

/* data byte #4: PCDIV (please see datasheet) */



/*
 *  entry mode
 *  - 1 byte cmd + 1 byte data
 */

#define CMD_ENTRY_MODE        0b10110111     /* entry mode set */

/* data byte #1: */
  /* low voltage detection control: */
#define FLAG_GAS_ON           0b00000000     /* enable */
#define FLAG_GAS_OFF          0b00000001     /* disable */
  /* output level of gate drivers G1-G320 */
#define FLAG_DTE_GON_0        0b00000000     /* VGH */
#define FLAG_DTE_GON_1        0b00000010     /* VGH */
#define FLAG_DTE_GON_2        0b00000100     /* VGL */
#define FLAG_DTE_GON_3        0b00000110     /* normal display */


/*
 *  backlight control 1
 *  - 1 byte cmd + 1 byte data
 */

#define CMD_BACKLIGHT_CTRL_1  0b10111000     /* backlight control 1 */

/* data byte #1: percentage of accumulated greyscale histogram */
  /* user interface mode: */
#define FLAG_TH_UI_99         0b00000000     /* 99% */
#define FLAG_TH_UI_98         0b00000001     /* 98% */
#define FLAG_TH_UI_96         0b00000010     /* 96% */
#define FLAG_TH_UI_94         0b00000011     /* 94% */
#define FLAG_TH_UI_92         0b00000100     /* 92% */
#define FLAG_TH_UI_90         0b00000101     /* 90% */
#define FLAG_TH_UI_88         0b00000110     /* 88% */
#define FLAG_TH_UI_86         0b00000111     /* 86% */
#define FLAG_TH_UI_84         0b00001000     /* 84% */
#define FLAG_TH_UI_82         0b00001001     /* 82% */
#define FLAG_TH_UI_80         0b00001010     /* 80% */
#define FLAG_TH_UI_78         0b00001011     /* 78% */
#define FLAG_TH_UI_76         0b00001100     /* 76% */
#define FLAG_TH_UI_74         0b00001101     /* 74% */
#define FLAG_TH_UI_72         0b00001110     /* 72% */
#define FLAG_TH_UI_70         0b00001111     /* 70% */


/*
 *  backlight control 2
 *  - 1 byte cmd + 1 byte data
 */

#define CMD_BACKLIGHT_CTRL_2  0b10111001     /* backlight control 2 */

/* data byte #1: percentage of accumulated greyscale histogram */
  /* still picture mode: */
#define FLAG_TH_ST_99         0b00000000     /* 99% */
#define FLAG_TH_ST_98         0b00000001     /* 98% */
#define FLAG_TH_ST_96         0b00000010     /* 96% */
#define FLAG_TH_ST_94         0b00000011     /* 94% */
#define FLAG_TH_ST_92         0b00000100     /* 92% */
#define FLAG_TH_ST_90         0b00000101     /* 90% */
#define FLAG_TH_ST_88         0b00000110     /* 88% */
#define FLAG_TH_ST_86         0b00000111     /* 86% */
#define FLAG_TH_ST_84         0b00001000     /* 84% */
#define FLAG_TH_ST_82         0b00001001     /* 82% */
#define FLAG_TH_ST_80         0b00001010     /* 80% */
#define FLAG_TH_ST_78         0b00001011     /* 78% */
#define FLAG_TH_ST_76         0b00001100     /* 76% */
#define FLAG_TH_ST_74         0b00001101     /* 74% */
#define FLAG_TH_ST_72         0b00001110     /* 72% */
#define FLAG_TH_ST_70         0b00001111     /* 70% */
  /* moving image mode: */
#define FLAG_TH_MV_99         0b00000000     /* 99% */
#define FLAG_TH_MV_98         0b00010000     /* 98% */
#define FLAG_TH_MV_96         0b00100000     /* 96% */
#define FLAG_TH_MV_94         0b00110000     /* 94% */
#define FLAG_TH_MV_92         0b01000000     /* 92% */
#define FLAG_TH_MV_90         0b01010000     /* 90% */
#define FLAG_TH_MV_88         0b01100000     /* 88% */
#define FLAG_TH_MV_86         0b01110000     /* 86% */
#define FLAG_TH_MV_84         0b10000000     /* 84% */
#define FLAG_TH_MV_82         0b10010000     /* 82% */
#define FLAG_TH_MV_80         0b10100000     /* 80% */
#define FLAG_TH_MV_78         0b10110000     /* 78% */
#define FLAG_TH_MV_76         0b11000000     /* 76% */
#define FLAG_TH_MV_74         0b11010000     /* 74% */
#define FLAG_TH_MV_72         0b11100000     /* 72% */
#define FLAG_TH_MV_70         0b11110000     /* 70% */


/*
 *  backlight control 3
 *  - 1 byte cmd + 1 byte data
 */

#define CMD_BACKLIGHT_CTRL_3  0b10111010     /* backlight control 3 */

/* data byte #1: lower limit of greyscale threshold in user icon image mode */
#define FLAG_DTH_UI_252       0b00000000     /* 252 */
#define FLAG_DTH_UI_248       0b00000001     /* 248 */
#define FLAG_DTH_UI_244       0b00000010     /* 244 */
#define FLAG_DTH_UI_240       0b00000011     /* 240 */
#define FLAG_DTH_UI_236       0b00000100     /* 236 */
#define FLAG_DTH_UI_232       0b00000101     /* 232 */
#define FLAG_DTH_UI_228       0b00000110     /* 228 */
#define FLAG_DTH_UI_224       0b00000111     /* 224 */
#define FLAG_DTH_UI_220       0b00001000     /* 220 */
#define FLAG_DTH_UI_216       0b00001001     /* 216 */
#define FLAG_DTH_UI_212       0b00001010     /* 212 */
#define FLAG_DTH_UI_208       0b00001011     /* 208 */
#define FLAG_DTH_UI_204       0b00001100     /* 204 */
#define FLAG_DTH_UI_200       0b00001101     /* 200 */
#define FLAG_DTH_UI_196       0b00001110     /* 196 */
#define FLAG_DTH_UI_192       0b00001111     /* 192 */


/*
 *  backlight control 4
 *  - 1 byte cmd + 1 byte data
 */

#define CMD_BACKLIGHT_CTRL_4  0b10111011     /* backlight control 4 */

/* data byte #1: lower limit of greyscale threshold */
  /* still picture mode: */
#define FLAG_DTH_ST_224       0b00000000     /* 224 */
#define FLAG_DTH_ST_220       0b00000001     /* 220 */
#define FLAG_DTH_ST_216       0b00000010     /* 216 */
#define FLAG_DTH_ST_212       0b00000011     /* 212 */
#define FLAG_DTH_ST_208       0b00000100     /* 208 */
#define FLAG_DTH_ST_204       0b00000101     /* 204 */
#define FLAG_DTH_ST_200       0b00000110     /* 200 */
#define FLAG_DTH_ST_196       0b00000111     /* 196 */
#define FLAG_DTH_ST_192       0b00001000     /* 192 */
#define FLAG_DTH_ST_188       0b00001001     /* 188 */
#define FLAG_DTH_ST_184       0b00001010     /* 184 */
#define FLAG_DTH_ST_180       0b00001011     /* 180 */
#define FLAG_DTH_ST_176       0b00001100     /* 176 */
#define FLAG_DTH_ST_172       0b00001101     /* 172 */
#define FLAG_DTH_ST_168       0b00001110     /* 168 */
#define FLAG_DTH_ST_164       0b00001111     /* 164 */
  /* moving image mode: */
#define FLAG_DTH_MV_224       0b00000000     /* 224 */
#define FLAG_DTH_MV_220       0b00010000     /* 220 */
#define FLAG_DTH_MV_216       0b00100000     /* 216 */
#define FLAG_DTH_MV_212       0b00110000     /* 212 */
#define FLAG_DTH_MV_208       0b01000000     /* 208 */
#define FLAG_DTH_MV_204       0b01010000     /* 204 */
#define FLAG_DTH_MV_200       0b01100000     /* 200 */
#define FLAG_DTH_MV_196       0b01110000     /* 196 */
#define FLAG_DTH_MV_192       0b10000000     /* 192 */
#define FLAG_DTH_MV_188       0b10010000     /* 188 */
#define FLAG_DTH_MV_184       0b10100000     /* 184 */
#define FLAG_DTH_MV_180       0b10110000     /* 180 */
#define FLAG_DTH_MV_176       0b11000000     /* 176 */
#define FLAG_DTH_MV_172       0b11010000     /* 172 */
#define FLAG_DTH_MV_168       0b11100000     /* 168 */
#define FLAG_DTH_MV_164       0b11110000     /* 164 */


/*
 *  backlight control 5
 *  - 1 byte cmd + 1 byte data
 */

#define CMD_BACKLIGHT_CTRL_5  0b10111100     /* backlight control 5 */

/* data byte #1: brightness control */
  /* transition time of brightness level: */
#define FLAG_DIM1_0           0b00000000     /* 1 frame */
#define FLAG_DIM1_1           0b00000001     /* 1 frame */
#define FLAG_DIM1_2           0b00000010     /* 2 frames */
#define FLAG_DIM1_3           0b00000011     /* 4 frames */
#define FLAG_DIM1_4           0b00000100     /* 8 frames */
#define FLAG_DIM1_5           0b00000101     /* 16 frames */
#define FLAG_DIM1_6           0b00000110     /* 32 frames */
#define FLAG_DIM1_7           0b00000111     /* 64 frames */
  /* threshold of brightness change: */
#define FLAG_DIM2_MIN         0b00000000     /* min value */
#define FLAG_DIM2_MAX         0b11110000     /* max value */


/*
 *  backlight control 7
 *  - 1 byte cmd + 1 byte data
 */

#define CMD_BACKLIGHT_CTRL_7  0b10111110     /* backlight control 7 */

/* data byte #1: PWM_OUT frequency control, please see datasheet */
#define FLAG_PWM_DIV_MIN      0b00000000     /* miminum value */
#define FLAG_PWM_62740        0b00000000     /* 62.740kHz */
#define FLAG_PWM_31380        0b00000001     /* 31.380kHz */

#define FLAG_PWM_00246        0b11111110     /* 246Hz */
#define FLAG_PWM_00245        0b11111111     /* 245Hz */
#define FLAG_PWM_DIV_MAX      0b11111111     /* maximum value */


/*
 *  backlight control 8
 *  - 1 byte cmd + 1 byte data
 */

#define CMD_BACKLIGHT_CTRL_8  0b10111111     /* backlight control 8 */

/* data byte #1: LED switches */
  /* polarity of LEDPWM: */
#define FLAG_LEDPWMPOL_0      0b00000000     /* if BL=0: 0 */
                                             /* if BL=1: original polarity */
#define FLAG_LEDPWMPOL_1      0b00000001     /* if BL=0: 1 */
                                             /* if BL=1: inversed original polarity */
  /* LEDON pin: */
#define FLAG_LEDONPOL_0       0b00000000     /* if BL=0: 0 */
                                             /* if BL=1: LEDONR */
#define FLAG_LEDONPOL_1       0b00000010     /* if BL=0: 1 */
                                             /* if BL=1: inversed LEDONR */
#define FLAG_LEDONR_0         0b00000000     /* low ? */
#define FLAG_LEDONR_1         0b00000100     /* high ? */


/*
 *  power control 1
 *  - 1 byte cmd + 1 byte data
 */

#define CMD_POWER_CTRL_1      0b11000000     /* power control 1 */

/* data byte #1: GVDD level (reference for VCOM) */
#define FLAG_VRH_300          0b00000011     /* 3.00V */
#define FLAG_VRH_305          0b00000100     /* 3.05V */
#define FLAG_VRH_310          0b00000101     /* 3.10V */
#define FLAG_VRH_315          0b00000110     /* 3.15V */
#define FLAG_VRH_320          0b00000111     /* 3.20V */
#define FLAG_VRH_325          0b00001000     /* 3.25V */
#define FLAG_VRH_330          0b00001001     /* 3.30V */
#define FLAG_VRH_335          0b00001010     /* 3.35V */
#define FLAG_VRH_340          0b00001011     /* 3.40V */
#define FLAG_VRH_345          0b00001100     /* 3.45V */
#define FLAG_VRH_350          0b00001101     /* 3.50V */
#define FLAG_VRH_355          0b00001110     /* 3.55V */
#define FLAG_VRH_360          0b00001111     /* 3.60V */
#define FLAG_VRH_365          0b00010000     /* 3.65V */
#define FLAG_VRH_370          0b00010001     /* 3.70V */
#define FLAG_VRH_375          0b00010010     /* 3.75V */
#define FLAG_VRH_380          0b00010011     /* 3.80V */
#define FLAG_VRH_385          0b00010100     /* 3.85V */
#define FLAG_VRH_390          0b00010101     /* 3.90V */
#define FLAG_VRH_395          0b00010110     /* 3.95V */
#define FLAG_VRH_400          0b00010111     /* 4.00V */
#define FLAG_VRH_405          0b00011000     /* 4.05V */
#define FLAG_VRH_410          0b00011001     /* 4.10V */
#define FLAG_VRH_415          0b00011010     /* 4.15V */
#define FLAG_VRH_420          0b00011011     /* 4.20V */
#define FLAG_VRH_425          0b00011100     /* 4.25V */
#define FLAG_VRH_430          0b00011101     /* 4.30V */
#define FLAG_VRH_435          0b00011110     /* 4.35V */
#define FLAG_VRH_440          0b00011111     /* 4.40V */
#define FLAG_VRH_445          0b00100000     /* 4.45V */
#define FLAG_VRH_450          0b00100001     /* 4.50V */
#define FLAG_VRH_455          0b00100010     /* 4.55V */
#define FLAG_VRH_460          0b00100011     /* 4.60V */
#define FLAG_VRH_465          0b00100100     /* 4.65V */
#define FLAG_VRH_470          0b00100101     /* 4.70V */
#define FLAG_VRH_475          0b00100110     /* 4.75V */
#define FLAG_VRH_480          0b00100111     /* 4.80V */
#define FLAG_VRH_485          0b00101000     /* 4.85V */
#define FLAG_VRH_490          0b00101001     /* 4.90V */
#define FLAG_VRH_495          0b00101010     /* 4.95V */
#define FLAG_VRH_500          0b00101011     /* 5.00V */
#define FLAG_VRH_505          0b00101100     /* 5.05V */
#define FLAG_VRH_510          0b00101101     /* 5.10V */
#define FLAG_VRH_515          0b00101110     /* 5.15V */
#define FLAG_VRH_520          0b00101111     /* 5.20V */
#define FLAG_VRH_525          0b00110000     /* 5.25V */
#define FLAG_VRH_530          0b00110001     /* 5.30V */
#define FLAG_VRH_535          0b00110010     /* 5.35V */
#define FLAG_VRH_540          0b00110011     /* 5.40V */
#define FLAG_VRH_545          0b00110100     /* 5.45V */
#define FLAG_VRH_550          0b00110101     /* 5.50V */
#define FLAG_VRH_555          0b00110110     /* 5.55V */
#define FLAG_VRH_560          0b00110111     /* 5.60V */
#define FLAG_VRH_565          0b00111000     /* 5.65V */
#define FLAG_VRH_570          0b00111001     /* 5.70V */
#define FLAG_VRH_575          0b00111010     /* 5.75V */
#define FLAG_VRH_580          0b00111011     /* 5.80V */
#define FLAG_VRH_585          0b00111100     /* 5.85V */
#define FLAG_VRH_590          0b00111101     /* 5.90V */
#define FLAG_VRH_595          0b00111110     /* 5.95V */
#define FLAG_VRH_600          0b00111111     /* 6.00V */


/*
 *  power control 2
 *  - 1 byte cmd + 1 byte data
 */

#define CMD_POWER_CTRL_2      0b11000001     /* power control 2 */

/* data byte #1: factor used in step-up converter */
#define FLAG_BT_0             0b00000000     /* DDVDH=2*VCI, VGH=7*VCI, VGL=-4*VCI */
#define FLAG_BT_1             0b00000001     /* DDVDH=2*VCI, VGH=7*VCI, VGL=-3*VCI */
#define FLAG_BT_2             0b00000010     /* DDVDH=2*VCI, VGH=6*VCI, VGL=-4*VCI */
#define FLAG_BT_3             0b00000011     /* DDVDH=2*VCI, VGH=6*VCI, VGL=-3*VCI */


/*
 *  VCOM control 1
 *  - 1 byte cmd + 2 bytes data
 */

#define CMD_VCOM_CTRL_1       0b11000101     /* VCOM control 1 */

/* data byte #1: VCOMH voltage */
#define FLAG_VMH_MIN          0b00000000     /* minumum value */
#define FLAG_VMH_2700         0b00000000     /* 2.700V */
#define FLAG_VMH_2725         0b00000001     /* 2.725V */
#define FLAG_VMH_2750         0b00000010     /* 2.750V */
#define FLAG_VMH_2775         0b00000011     /* 2.775V */
#define FLAG_VMH_2800         0b00000100     /* 2.800V */
#define FLAG_VMH_2825         0b00000101     /* 2.825V */
#define FLAG_VMH_2850         0b00000110     /* 2.850V */
#define FLAG_VMH_2875         0b00000111     /* 2.875V */
#define FLAG_VMH_2900         0b00001000     /* 2.900V */
#define FLAG_VMH_2925         0b00001001     /* 2.925V */
#define FLAG_VMH_2950         0b00001010     /* 2.950V */
#define FLAG_VMH_2975         0b00001011     /* 2.975V */
#define FLAG_VMH_3000         0b00001100     /* 3.000V */
#define FLAG_VMH_3025         0b00001101     /* 3.025V */
#define FLAG_VMH_3050         0b00001110     /* 3.050V */
#define FLAG_VMH_3075         0b00001111     /* 3.075V */
#define FLAG_VMH_3100         0b00010000     /* 3.100V */
#define FLAG_VMH_3125         0b00010001     /* 3.125V */
#define FLAG_VMH_3150         0b00010010     /* 3.150V */
#define FLAG_VMH_3175         0b00010011     /* 3.175V */
#define FLAG_VMH_3200         0b00010100     /* 3.200V */
#define FLAG_VMH_3225         0b00010101     /* 3.225V */
#define FLAG_VMH_3250         0b00010110     /* 3.250V */
#define FLAG_VMH_3275         0b00010111     /* 3.275V */
#define FLAG_VMH_3300         0b00011000     /* 3.300V */
#define FLAG_VMH_3325         0b00011001     /* 3.325V */
#define FLAG_VMH_3350         0b00011010     /* 3.350V */
#define FLAG_VMH_3375         0b00011011     /* 3.375V */
#define FLAG_VMH_3400         0b00011100     /* 3.400V */
#define FLAG_VMH_3425         0b00011101     /* 3.425V */
#define FLAG_VMH_3450         0b00011110     /* 3.450V */
#define FLAG_VMH_3475         0b00011111     /* 3.475V */
#define FLAG_VMH_3500         0b00100000     /* 3.500V */
#define FLAG_VMH_3525         0b00100001     /* 3.525V */
#define FLAG_VMH_3550         0b00100010     /* 3.550V */
#define FLAG_VMH_3575         0b00100011     /* 3.575V */
#define FLAG_VMH_3600         0b00100100     /* 3.600V */
#define FLAG_VMH_3625         0b00100101     /* 3.625V */
#define FLAG_VMH_3650         0b00100110     /* 3.650V */
#define FLAG_VMH_3675         0b00100111     /* 3.675V */
#define FLAG_VMH_3700         0b00101000     /* 3.700V */
#define FLAG_VMH_3725         0b00101001     /* 3.725V */
#define FLAG_VMH_3750         0b00101010     /* 3.750V */
#define FLAG_VMH_3775         0b00101011     /* 3.775V */
#define FLAG_VMH_3800         0b00101100     /* 3.800V */
#define FLAG_VMH_3825         0b00101101     /* 3.825V */
#define FLAG_VMH_3850         0b00101110     /* 3.850V */
#define FLAG_VMH_3875         0b00101111     /* 3.875V */
#define FLAG_VMH_3900         0b00110000     /* 3.900V */
#define FLAG_VMH_3925         0b00110001     /* 3.925V */
#define FLAG_VMH_3950         0b00110010     /* 3.950V */
#define FLAG_VMH_3975         0b00110011     /* 3.975V */
#define FLAG_VMH_4000         0b00110100     /* 4.000V */
#define FLAG_VMH_4025         0b00110101     /* 4.025V */
#define FLAG_VMH_4050         0b00110110     /* 4.050V */
#define FLAG_VMH_4075         0b00110111     /* 4.075V */
#define FLAG_VMH_4100         0b00111000     /* 4.100V */
#define FLAG_VMH_4125         0b00111001     /* 4.125V */
#define FLAG_VMH_4150         0b00111010     /* 4.150V */
#define FLAG_VMH_4175         0b00111011     /* 4.175V */
#define FLAG_VMH_4200         0b00111100     /* 4.200V */
#define FLAG_VMH_4225         0b00111101     /* 4.225V */
#define FLAG_VMH_4250         0b00111110     /* 4.250V */
#define FLAG_VMH_4275         0b00111111     /* 4.275V */
#define FLAG_VMH_4300         0b01000000     /* 4.300V */
#define FLAG_VMH_4325         0b01000001     /* 4.325V */
#define FLAG_VMH_4350         0b01000010     /* 4.350V */
#define FLAG_VMH_4375         0b01000011     /* 4.375V */
#define FLAG_VMH_4400         0b01000100     /* 4.400V */
#define FLAG_VMH_4425         0b01000101     /* 4.425V */
#define FLAG_VMH_4450         0b01000110     /* 4.450V */
#define FLAG_VMH_4475         0b01000111     /* 4.475V */
#define FLAG_VMH_4500         0b01001000     /* 4.500V */
#define FLAG_VMH_4525         0b01001001     /* 4.525V */
#define FLAG_VMH_4550         0b01001010     /* 4.550V */
#define FLAG_VMH_4575         0b01001011     /* 4.575V */
#define FLAG_VMH_4600         0b01001100     /* 4.600V */
#define FLAG_VMH_4625         0b01001101     /* 4.625V */
#define FLAG_VMH_4650         0b01001110     /* 4.650V */
#define FLAG_VMH_4675         0b01001111     /* 4.675V */
#define FLAG_VMH_4700         0b01010000     /* 4.700V */
#define FLAG_VMH_4725         0b01010001     /* 4.725V */
#define FLAG_VMH_4750         0b01010010     /* 4.750V */
#define FLAG_VMH_4775         0b01010011     /* 4.775V */
#define FLAG_VMH_4800         0b01010100     /* 4.800V */
#define FLAG_VMH_4825         0b01010101     /* 4.825V */
#define FLAG_VMH_4850         0b01010110     /* 4.850V */
#define FLAG_VMH_4875         0b01010111     /* 4.875V */
#define FLAG_VMH_4900         0b01011000     /* 4.900V */
#define FLAG_VMH_4925         0b01011001     /* 4.925V */
#define FLAG_VMH_4950         0b01011010     /* 4.950V */
#define FLAG_VMH_4975         0b01011011     /* 4.975V */
#define FLAG_VMH_5000         0b01011100     /* 5.000V */
#define FLAG_VMH_5025         0b01011101     /* 5.025V */
#define FLAG_VMH_5050         0b01011110     /* 5.050V */
#define FLAG_VMH_5075         0b01011111     /* 5.075V */
#define FLAG_VMH_5100         0b01100000     /* 5.100V */
#define FLAG_VMH_5125         0b01100001     /* 5.125V */
#define FLAG_VMH_5150         0b01100010     /* 5.150V */
#define FLAG_VMH_5175         0b01100011     /* 5.175V */
#define FLAG_VMH_5200         0b01100100     /* 5.200V */
#define FLAG_VMH_5225         0b01100101     /* 5.225V */
#define FLAG_VMH_5250         0b01100110     /* 5.250V */
#define FLAG_VMH_5275         0b01100111     /* 5.275V */
#define FLAG_VMH_5300         0b01101000     /* 5.300V */
#define FLAG_VMH_5325         0b01101001     /* 5.325V */
#define FLAG_VMH_5350         0b01101010     /* 5.350V */
#define FLAG_VMH_5375         0b01101011     /* 5.375V */
#define FLAG_VMH_5400         0b01101100     /* 5.400V */
#define FLAG_VMH_5425         0b01101101     /* 5.425V */
#define FLAG_VMH_5450         0b01101110     /* 5.450V */
#define FLAG_VMH_5475         0b01101111     /* 5.475V */
#define FLAG_VMH_5500         0b01110000     /* 5.500V */
#define FLAG_VMH_5525         0b01110001     /* 5.525V */
#define FLAG_VMH_5550         0b01110010     /* 5.550V */
#define FLAG_VMH_5575         0b01110011     /* 5.575V */
#define FLAG_VMH_5600         0b01110100     /* 5.600V */
#define FLAG_VMH_5625         0b01110101     /* 5.625V */
#define FLAG_VMH_5650         0b01110110     /* 5.650V */
#define FLAG_VMH_5675         0b01110111     /* 5.675V */
#define FLAG_VMH_5700         0b01111000     /* 5.700V */
#define FLAG_VMH_5725         0b01111001     /* 5.725V */
#define FLAG_VMH_5750         0b01111010     /* 5.750V */
#define FLAG_VMH_5775         0b01111011     /* 5.775V */
#define FLAG_VMH_5800         0b01111100     /* 5.800V */
#define FLAG_VMH_5825         0b01111101     /* 5.825V */
#define FLAG_VMH_5850         0b01111110     /* 5.850V */
#define FLAG_VMH_5875         0b01111111     /* 5.875V */
#define FLAG_VMH_MAX          0b01111111     /* maximum value */

/* data byte #2: VCOML voltage, please see datasheet */
#define FLAG_VML_MIN          0b00000000     /* minumum value */
#define FLAG_VML_2500         0b00000000     /* -2.500V */
#define FLAG_VML_2475         0b00000001     /* -2.475V */
#define FLAG_VML_2450         0b00000010     /* -2.450V */
#define FLAG_VML_2425         0b00000011     /* -2.425V */
#define FLAG_VML_2400         0b00000100     /* -2.400V */
#define FLAG_VML_2375         0b00000101     /* -2.375V */
#define FLAG_VML_2350         0b00000110     /* -2.350V */
#define FLAG_VML_2325         0b00000111     /* -2.325V */
#define FLAG_VML_2300         0b00001000     /* -2.300V */
#define FLAG_VML_2275         0b00001001     /* -2.275V */
#define FLAG_VML_2250         0b00001010     /* -2.250V */
#define FLAG_VML_2225         0b00001011     /* -2.225V */
#define FLAG_VML_2200         0b00001100     /* -2.200V */
#define FLAG_VML_2175         0b00001101     /* -2.175V */
#define FLAG_VML_2150         0b00001110     /* -2.150V */
#define FLAG_VML_2125         0b00001111     /* -2.125V */
#define FLAG_VML_2100         0b00010000     /* -2.100V */
#define FLAG_VML_2075         0b00010001     /* -2.075V */
#define FLAG_VML_2050         0b00010010     /* -2.050V */
#define FLAG_VML_2025         0b00010011     /* -2.025V */
#define FLAG_VML_2000         0b00010100     /* -2.000V */
#define FLAG_VML_1975         0b00010101     /* -1.975V */
#define FLAG_VML_1950         0b00010110     /* -1.950V */
#define FLAG_VML_1925         0b00010111     /* -1.925V */
#define FLAG_VML_1900         0b00011000     /* -1.900V */
#define FLAG_VML_1875         0b00011001     /* -1.875V */
#define FLAG_VML_1850         0b00011010     /* -1.850V */
#define FLAG_VML_1825         0b00011011     /* -1.825V */
#define FLAG_VML_1800         0b00011100     /* -1.800V */
#define FLAG_VML_1775         0b00011101     /* -1.775V */
#define FLAG_VML_1750         0b00011110     /* -1.750V */
#define FLAG_VML_1725         0b00011111     /* -1.725V */
#define FLAG_VML_1700         0b00100000     /* -1.700V */
#define FLAG_VML_1675         0b00100001     /* -1.675V */
#define FLAG_VML_1650         0b00100010     /* -1.650V */
#define FLAG_VML_1625         0b00100011     /* -1.625V */
#define FLAG_VML_1600         0b00100100     /* -1.600V */
#define FLAG_VML_1575         0b00100101     /* -1.575V */
#define FLAG_VML_1550         0b00100110     /* -1.550V */
#define FLAG_VML_1525         0b00100111     /* -1.525V */
#define FLAG_VML_1500         0b00101000     /* -1.500V */
#define FLAG_VML_1475         0b00101001     /* -1.475V */
#define FLAG_VML_1450         0b00101010     /* -1.450V */
#define FLAG_VML_1425         0b00101011     /* -1.425V */
#define FLAG_VML_1400         0b00101100     /* -1.400V */
#define FLAG_VML_1375         0b00101101     /* -1.375V */
#define FLAG_VML_1350         0b00101110     /* -1.350V */
#define FLAG_VML_1325         0b00101111     /* -1.325V */
#define FLAG_VML_1300         0b00110000     /* -1.300V */
#define FLAG_VML_1275         0b00110001     /* -1.275V */
#define FLAG_VML_1250         0b00110010     /* -1.250V */
#define FLAG_VML_1225         0b00110011     /* -1.225V */
#define FLAG_VML_1200         0b00110100     /* -1.200V */
#define FLAG_VML_1175         0b00110101     /* -1.175V */
#define FLAG_VML_1150         0b00110110     /* -1.150V */
#define FLAG_VML_1125         0b00110111     /* -1.125V */
#define FLAG_VML_1100         0b00111000     /* -1.100V */
#define FLAG_VML_1075         0b00111001     /* -1.075V */
#define FLAG_VML_1050         0b00111010     /* -1.050V */
#define FLAG_VML_1025         0b00111011     /* -1.025V */
#define FLAG_VML_1000         0b00111100     /* -1.000V */
#define FLAG_VML_0975         0b00111101     /* -0.975V */
#define FLAG_VML_0950         0b00111110     /* -0.950V */
#define FLAG_VML_0925         0b00111111     /* -0.925V */
#define FLAG_VML_0900         0b01000000     /* -0.900V */
#define FLAG_VML_0875         0b01000001     /* -0.875V */
#define FLAG_VML_0850         0b01000010     /* -0.850V */
#define FLAG_VML_0825         0b01000011     /* -0.825V */
#define FLAG_VML_0800         0b01000100     /* -0.800V */
#define FLAG_VML_0775         0b01000101     /* -0.775V */
#define FLAG_VML_0750         0b01000110     /* -0.750V */
#define FLAG_VML_0725         0b01000111     /* -0.725V */
#define FLAG_VML_0700         0b01001000     /* -0.700V */
#define FLAG_VML_0675         0b01001001     /* -0.675V */
#define FLAG_VML_0650         0b01001010     /* -0.650V */
#define FLAG_VML_0625         0b01001111     /* -0.625V */
#define FLAG_VML_0600         0b01001100     /* -0.600V */
#define FLAG_VML_0575         0b01001101     /* -0.575V */
#define FLAG_VML_0550         0b01001110     /* -0.550V */
#define FLAG_VML_0525         0b01001111     /* -0.525V */
#define FLAG_VML_0500         0b01010000     /* -0.500V */
#define FLAG_VML_0475         0b01010001     /* -0.475V */
#define FLAG_VML_0450         0b01010010     /* -0.450V */
#define FLAG_VML_0425         0b01010011     /* -0.425V */
#define FLAG_VML_0400         0b01010100     /* -0.400V */
#define FLAG_VML_0375         0b01010101     /* -0.375V */
#define FLAG_VML_0350         0b01010110     /* -0.350V */
#define FLAG_VML_0325         0b01010111     /* -0.325V */
#define FLAG_VML_0300         0b01011000     /* -0.300V */
#define FLAG_VML_0275         0b01011001     /* -0.275V */
#define FLAG_VML_0250         0b01011010     /* -0.250V */
#define FLAG_VML_0225         0b01011011     /* -0.225V */
#define FLAG_VML_0200         0b01011100     /* -0.200V */
#define FLAG_VML_0175         0b01011101     /* -0.175V */
#define FLAG_VML_0150         0b01011110     /* -0.150V */
#define FLAG_VML_0125         0b01011111     /* -0.125V */
#define FLAG_VML_0100         0b01100000     /* -0.100V */
#define FLAG_VML_0075         0b01100001     /* -0.075V */
#define FLAG_VML_0050         0b01100010     /* -0.050V */
#define FLAG_VML_0025         0b01100011     /* -0.025V */
#define FLAG_VML_0000         0b01100100     /*  0.000V */
#define FLAG_VML_MAX          0b01100100     /* maximum value */


/*
 *  VCOM control 2
 *  - 1 byte cmd + 1 byte data
 */

#define CMD_VCOM_CTRL_2       0b11000111     /* VCOM control 2 */

/* data byte #1: */
  /* VCOM offset voltage: VCOMH=VMH+x, VCOML=VML+x */
#define FLAG_VMF_MIN          0b00000000     /* minumum value */
#define FLAG_VMF_M00          0b00000000     /* -0 */
#define FLAG_VMF_M63          0b00000001     /* -63 */
#define FLAG_VMF_M62          0b00000010     /* -62 */
#define FLAG_VMF_M61          0b00000011     /* -61 */
#define FLAG_VMF_M60          0b00000100     /* -60 */
#define FLAG_VMF_M59          0b00000101     /* -59 */
#define FLAG_VMF_M58          0b00000110     /* -58 */
#define FLAG_VMF_M57          0b00000111     /* -57 */
#define FLAG_VMF_M56          0b00001000     /* -56 */
#define FLAG_VMF_M55          0b00001001     /* -55 */
#define FLAG_VMF_M54          0b00001010     /* -54 */
#define FLAG_VMF_M53          0b00001011     /* -53 */
#define FLAG_VMF_M52          0b00001100     /* -52 */
#define FLAG_VMF_M51          0b00001101     /* -51 */
#define FLAG_VMF_M50          0b00001110     /* -50 */
#define FLAG_VMF_M49          0b00001111     /* -49 */
#define FLAG_VMF_M48          0b00010000     /* -48 */
#define FLAG_VMF_M47          0b00010001     /* -47 */
#define FLAG_VMF_M46          0b00010010     /* -46 */
#define FLAG_VMF_M45          0b00010011     /* -45 */
#define FLAG_VMF_M44          0b00010100     /* -44 */
#define FLAG_VMF_M43          0b00010101     /* -43 */
#define FLAG_VMF_M42          0b00010110     /* -42 */
#define FLAG_VMF_M41          0b00010111     /* -41 */
#define FLAG_VMF_M40          0b00011000     /* -40 */
#define FLAG_VMF_M39          0b00011001     /* -39 */
#define FLAG_VMF_M38          0b00011010     /* -38 */
#define FLAG_VMF_M37          0b00011011     /* -37 */
#define FLAG_VMF_M36          0b00011100     /* -36 */
#define FLAG_VMF_M35          0b00011101     /* -35 */
#define FLAG_VMF_M34          0b00011110     /* -34 */
#define FLAG_VMF_M33          0b00011111     /* -33 */
#define FLAG_VMF_M32          0b00100000     /* -32 */
#define FLAG_VMF_M31          0b00100001     /* -31 */
#define FLAG_VMF_M30          0b00100010     /* -30 */
#define FLAG_VMF_M29          0b00100011     /* -29 */
#define FLAG_VMF_M28          0b00100100     /* -28 */
#define FLAG_VMF_M27          0b00100101     /* -27 */
#define FLAG_VMF_M26          0b00100110     /* -26 */
#define FLAG_VMF_M25          0b00100111     /* -25 */
#define FLAG_VMF_M24          0b00101000     /* -24 */
#define FLAG_VMF_M23          0b00101001     /* -23 */
#define FLAG_VMF_M22          0b00101010     /* -22 */
#define FLAG_VMF_M21          0b00101011     /* -21 */
#define FLAG_VMF_M20          0b00101100     /* -20 */
#define FLAG_VMF_M19          0b00101101     /* -19 */
#define FLAG_VMF_M18          0b00101110     /* -18 */
#define FLAG_VMF_M17          0b00101111     /* -17 */
#define FLAG_VMF_M16          0b00110000     /* -16 */
#define FLAG_VMF_M15          0b00110001     /* -15 */
#define FLAG_VMF_M14          0b00110010     /* -14 */
#define FLAG_VMF_M13          0b00110011     /* -13 */
#define FLAG_VMF_M12          0b00110100     /* -12 */
#define FLAG_VMF_M11          0b00110101     /* -11 */
#define FLAG_VMF_M10          0b00110110     /* -10 */
#define FLAG_VMF_M09          0b00110111     /* -09 */
#define FLAG_VMF_M08          0b00111000     /* -08 */
#define FLAG_VMF_M07          0b00111001     /* -07 */
#define FLAG_VMF_M06          0b00111010     /* -06 */
#define FLAG_VMF_M05          0b00111011     /* -05 */
#define FLAG_VMF_M04          0b00111100     /* -04 */
#define FLAG_VMF_M03          0b00111101     /* -03 */
#define FLAG_VMF_M02          0b00111110     /* -02 */
#define FLAG_VMF_M01          0b00111111     /* -01 */
#define FLAG_VMF_P00          0b01000000     /* +0 */
#define FLAG_VMF_P01          0b01000001     /* +1 */
#define FLAG_VMF_P02          0b01000010     /* +2 */
#define FLAG_VMF_P03          0b01000011     /* +3 */
#define FLAG_VMF_P04          0b01000100     /* +4 */
#define FLAG_VMF_P05          0b01000101     /* +5 */
#define FLAG_VMF_P06          0b01000110     /* +6 */
#define FLAG_VMF_P07          0b01000111     /* +7 */
#define FLAG_VMF_P08          0b01001000     /* +8 */
#define FLAG_VMF_P09          0b01001001     /* +9 */
#define FLAG_VMF_P10          0b01001010     /* +10 */
#define FLAG_VMF_P11          0b01001011     /* +11 */
#define FLAG_VMF_P12          0b01001100     /* +12 */
#define FLAG_VMF_P13          0b01001101     /* +13 */
#define FLAG_VMF_P14          0b01001110     /* +14 */
#define FLAG_VMF_P15          0b01001111     /* +1 */
#define FLAG_VMF_P16          0b01010000     /* +1 */
#define FLAG_VMF_P17          0b01010001     /* +1 */
#define FLAG_VMF_P18          0b01010010     /* +1 */
#define FLAG_VMF_P19          0b01010011     /* +1 */
#define FLAG_VMF_P20          0b01010100     /* +20 */
#define FLAG_VMF_P21          0b01010101     /* +21 */
#define FLAG_VMF_P22          0b01010110     /* +22 */
#define FLAG_VMF_P23          0b01010111     /* +23 */
#define FLAG_VMF_P24          0b01011000     /* +24 */
#define FLAG_VMF_P25          0b01011001     /* +25 */
#define FLAG_VMF_P26          0b01011010     /* +26 */
#define FLAG_VMF_P27          0b01011011     /* +27 */
#define FLAG_VMF_P28          0b01011100     /* +28 */
#define FLAG_VMF_P29          0b01011101     /* +29 */
#define FLAG_VMF_P30          0b01011110     /* +30 */
#define FLAG_VMF_P31          0b01011111     /* +31 */
#define FLAG_VMF_P32          0b01100000     /* +32 */
#define FLAG_VMF_P33          0b01100001     /* +33 */
#define FLAG_VMF_P34          0b01100010     /* +34 */
#define FLAG_VMF_P35          0b01100011     /* +35 */
#define FLAG_VMF_P36          0b01100100     /* +36 */
#define FLAG_VMF_P37          0b01100101     /* +37 */
#define FLAG_VMF_P38          0b01100110     /* +38 */
#define FLAG_VMF_P39          0b01100111     /* +39 */
#define FLAG_VMF_P40          0b01101000     /* +40 */
#define FLAG_VMF_P41          0b01101001     /* +41 */
#define FLAG_VMF_P42          0b01101010     /* +42 */
#define FLAG_VMF_P43          0b01101011     /* +43 */
#define FLAG_VMF_P44          0b01101100     /* +44 */
#define FLAG_VMF_P45          0b01101101     /* +45 */
#define FLAG_VMF_P46          0b01101110     /* +46 */
#define FLAG_VMF_P47          0b01101111     /* +47 */
#define FLAG_VMF_P48          0b01110000     /* +48 */
#define FLAG_VMF_P49          0b01110001     /* +49 */
#define FLAG_VMF_P50          0b01110010     /* +50 */
#define FLAG_VMF_P51          0b01110011     /* +51 */
#define FLAG_VMF_P52          0b01110100     /* +52 */
#define FLAG_VMF_P53          0b01110101     /* +53 */
#define FLAG_VMF_P54          0b01110110     /* +54 */
#define FLAG_VMF_P55          0b01110111     /* +55 */
#define FLAG_VMF_P56          0b01111000     /* +56 */
#define FLAG_VMF_P57          0b01111001     /* +57 */
#define FLAG_VMF_P58          0b01111010     /* +58 */
#define FLAG_VMF_P59          0b01111011     /* +59 */
#define FLAG_VMF_P60          0b01111100     /* +60 */
#define FLAG_VMF_P61          0b01111101     /* +61 */
#define FLAG_VMF_P62          0b01111110     /* +62 */
#define FLAG_VMF_P63          0b01111111     /* +63 */
#define FLAG_VMF_MAX          0b01111111     /* maximum value */
  /* enable setting of VMF */
#define FLAG_NVM_0            0b00000000     /* disable setting of VMF */
#define FLAG_NVM_1            0b10000000     /* enable setting of VMF */


/*
 *  write NV memory
 *  - 1 byte cmd + 2 bytes data
 */

#define CMD_NV_WRITE          0b11010000     /* write NV memory */

/* data byte #1: address */
#define FLAG_NV_ADDR_ID1      0b00000000     /* ID1 */
#define FLAG_NV_ADDR_ID2      0b00000001     /* ID2 */
#define FLAG_NV_ADDR_ID3      0b00000010     /* ID3 */
#define FLAG_NV_ADDR_VMF      0b00000100     /* VMF */

/* data byte #2: data */


/*
 *  NV memory protection key
 *  - 1 byte cmd + 3 bytes data
 */

#define CMD_NV_KEY            0b11010001     /* NV memory protection key */

/* data byte #1: key bits 23-16 */
#define FLAG_KEY_2            0x55           /* fixed key */

/* data byte #2: key bits 15-8 */
#define FLAG_KEY_1            0xAA           /* fixed key */

/* data byte #3: key bits 7-0 */
#define FLAG_KEY_0            0x66           /* fixed key */


/*
 *  read NV memory status
 *  - 1 byte cmd + 3 bytes data (read mode)
 */

#define CMD_NV_STAT_READ      0b11010010     /* read NV memory status  */

/* data byte #1: dummy byte */
/* data byte #2: write counter, please see dataheet */
/* data byte #3: write counter, please see dataheet */
  /* status of NV mem programming: */
#define FLAG_BUSY_0           0b00000000     /* idle */
#define FLAG_BUSY_1           0b10000000     /* busy */


/*
 *  read ID4
 *  - 1 byte cmd + 4 bytes data (read mode)
 */

#define CMD_READ_ID4          0b11010011     /* read ID4 (device code) */

/* data byte #1: dummy byte */
/* data byte #2: IC version */
/* data byte #3: IC model name */
/* data byte #4: IC model name */


/*
 *  positive gamma correction, gray scale voltage
 *  - 1 byte cmd + 16 bytes data
 */

#define CMD_GAMMA_CORR_POS    0b11100000     /* positive gamma correction */

/* data bytes: please see dataheet */


/*
 *  negative gamma correction, gray scale voltage
 *  - 1 byte cmd + 16 bytes data
 */

#define CMD_GAMMA_CORR_NEG    0b11100001     /* negative gamma correction */

/* data bytes: please see dataheet */



/*
 *  digital gamma control 1
 *  - 1 byte cmd + 16 bytes data
 */

#define CMD_GAMMA_CTRL_1      0b11100010     /* digital gamma control 1 */

/* data byte x (0-15): macro-adjustment for red and blue gamma curve */
/*   RCAx: 4 bits, BCAx: 4 bits */


/*
 *  digital gamma control 2
 *  - 1 byte cmd + 64 bytes data
 */

#define CMD_GAMMA_CTRL_2      0b11100011     /* digital gamma control 2 */

/* data byte x (0-63): micro-adjustment for red and blue gamma curve */
/*   RFAx: 4 bits, BFAx: 4 bits */


/*
 *  interface control
 *  - 1 byte cmd + 3 bytes data
 */

#define CMD_INTERFACE_CTRL    0b11110110     /* interface control */

/* data byte #1: misc switches */
#define MASK_INTERFACE_CTRL_1 0b00000000     /* mask: fff0f00f */
#define FLAG_WE_MODE_0        0b00000000     /* memory write control: ignore */
#define FLAG_WE_MODE_1        0b00000001     /* memory write control: next */
/* The set value of MADCTL is used in the IC is derived as exclusive
   OR between 1st Parameter of IFCTL and MADCTL Parameter. */
#define FLAG_BGR_EOR          0b00001000     /* ? */
#define FLAG_MV_EOR           0b00100000     /* ? */
#define FLAG_MX_EOR           0b01000000     /* ? */
#define FLAG_MY_EOR           0b10000000     /* ? */

/* data byte #2: misc switches */
#define MASK_INTERFACE_CTRL_2 0b00000000     /* mask: 00ff00ff */
  /* display data transfer method: */
#define FLAG_MDT_1            0b00000001     /* ? */
  /* 65k color mode data format: */
#define FLAG_EPF_0            0b00000000     /* R&B LSB = MSB */
#define FLAG_EPF_1            0b00010000     /* R&B LSB = 0 */
#define FLAG_EPF_2            0b00100000     /* R&B LSB = 1 */
#define FLAG_EPF_3            0b00110000     /* R&B LSB = conditional copy */

/* data byte #3: misc switches */
#define MASK_INTERFACE_CTRL_3 0b00000000     /* mask: 00f0ffff */
  /* RGB interface mode: */
#define FLAG_RIM_0            0b00000000     /* 18/16 bit (1 xfer/pixel) */
#define FLAG_RIM_1            0b00000001     /* 6 bit (3 xfers/pixel) */
  /* interface to access GRAM: */
#define FLAG_RM_NO_RGB        0b00000000     /* system/VSYNC interface */
#define FLAG_RM_RGB           0b00000010     /* RGB interface */
  /* display operation mode: */
#define FLAG_DM_INT           0b00000000     /* internal clock */
#define FLAG_DM_RGB           0b00000100     /* RGB interface */
#define FLAG_DM_VSYNC         0b00001000     /* VSYNC interface */
#define FLAG_DM_OFF           0b00001100     /* setting disabled */
  /* endiness: */
#define FLAG_ENDIAN_BIG       0b00000000     /* MSB first, default */
#define FLAG_ENDIAN_LITTLE    0b00100000     /* LSB first */



/* ************************************************************************
 *   extended register commands
 * ************************************************************************ */


/* 
 *  power control A
 *  - 1 byte cmd + 5 bytes data
 */

#define CMD_POWER_CTRL_A      0b11110110     /* power control A */

/* data byte #1: fixed byte */
#define MASK_POWER_CTRL_A_1   0b00111001     /* mask: 00111001 */

/* data byte #2: fixed byte */
#define MASK_POWER_CTRL_A_2   0b00101100     /* mask: 00101100 */

/* data byte #3: fixed byte */
#define MASK_POWER_CTRL_A_3   0b00000000     /* mask: 00000000 */

/* data byte #4: vcore control */
#define MASK_POWER_CTRL_A_4   0b00110000     /* mask: 00110fff */
#define FLAG_REG_VD_155       0b00000000     /* Vcore 1.55V */
#define FLAG_REG_VD_140       0b00000001     /* Vcore 1.4V */
#define FLAG_REG_VD_150       0b00000010     /* Vcore 1.5V */
#define FLAG_REG_VD_165       0b00000011     /* Vcore 1.65V */
#define FLAG_REG_VD_160       0b00000100     /* Vcore 1.6V */
#define FLAG_REG_VD_170       0b00000101     /* Vcore 1.7V */

/* data byte #5: DDVDH control */
#define MASK_POWER_CTRL_A_5   0b00000000     /* mask: 00000fff */
#define FLAG_VBC_58           0b00000000     /* DDVDH 5.8V */
#define FLAG_VBC_57           0b00000001     /* DDVDH 5.7V */
#define FLAG_VBC_56           0b00000010     /* DDVDH 5.6V */
#define FLAG_VBC_55           0b00000011     /* DDVDH 5.5V */
#define FLAG_VBC_54           0b00000100     /* DDVDH 5.4V */
#define FLAG_VBC_53           0b00000101     /* DDVDH 5.3V */
#define FLAG_VBC_52           0b00000110     /* DDVDH 5.2V */


/*
 *  power control B
 *  - 1 byte cmd + 3 bytes data
 */

#define CMD_POWER_CTRL_B      0b11001111     /* power control B */

/* data byte #1: fixed byte */
#define MASK_POWER_CTRL_B_1   0b00000000     /* mask: 00000000 */

/* data byte #2: power control */
#define MASK_POWER_CTRL_B_2   0b10000001     /* mask: 100ff001*/
#define FLAG_POWER_CTRL_B     0b00011000     /* only setting allowed */

/* data byte #3: discharge path for ESD protection */
#define MASK_POWER_CTRL_B_3   0b00100000     /* mask: 001f0000 */
#define FLAG_DC_OFF           0b00000000     /* disable discharge path */
#define FLAG_DC_ON            0b00010000     /* enable discharge path */


/*
 *  driver timing control A
 *  - 1 byte cmd + 3 bytes data
 */

#define CMD_TIME_CTRL_A       0b11101000     /* driver timing control A */

/* data byte #1: gate driver non-overlap timing */
#define MASK_TIME_CTRL_A_1    0b10000100     /* mask: 1000010f */
#define FLAG_NOW_0            0b00000000     /* default non-overlap time */
#define FLAG_NOW_1            0b00000001     /* default + 1 unit */

/* data byte #2: EQ & CR timing */
#define MASK_TIME_CTRL_A_2    0b00000000     /* mask: 000f000f */
#define FLAG_CR_0             0b00000000     /* CR: default - 1 unit */
#define FLAG_CR_1             0b00000001     /* CR: default CR timing */
#define FLAG_EQ_0             0b00000000     /* EQ: default - 1 unit */
#define FLAG_EQ_1             0b00010000     /* EQ: default EQ timing */

/* data byte #3: pre-charge timing */
#define MASK_TIME_CTRL_A_3    0b01111000     /* mask: 011110ff */
#define FLAG_PC_0             0b00000000     /* default - 2 units */
#define FLAG_PC_1             0b00000001     /* default - 1 unit */
#define FLAG_PC_3             0b00000010     /* default pre-charge time */


/*
 *  driver timing control B
 *  - 1 byte cmd + 2 bytes data
 */

#define CMD_TIME_CTRL_B       0b11101010     /* driver timing control B */

/* data byte #1: gate driver timing */
#define MASK_TIME_CTRL_B_1     0b00000000     /* mask: ffffffff */
#define FLAG_VG_SW_T1_0        0b00000000     /* VG_SW_T1: 0 units */
#define FLAG_VG_SW_T1_1        0b00000001     /* VG_SW_T1: 1 unit */
#define FLAG_VG_SW_T1_2        0b00000010     /* VG_SW_T1: 2 units */
#define FLAG_VG_SW_T1_3        0b00000011     /* VG_SW_T1: 3 units */
#define FLAG_VG_SW_T2_0        0b00000000     /* VG_SW_T2: 0 units */
#define FLAG_VG_SW_T2_1        0b00000100     /* VG_SW_T2: 1 unit */
#define FLAG_VG_SW_T2_2        0b00001000     /* VG_SW_T2: 2 units */
#define FLAG_VG_SW_T2_3        0b00001100     /* VG_SW_T2: 3 units */
#define FLAG_VG_SW_T3_0        0b00000000     /* VG_SW_T3: 0 units */
#define FLAG_VG_SW_T3_1        0b00010000     /* VG_SW_T3: 1 unit */
#define FLAG_VG_SW_T3_2        0b00100000     /* VG_SW_T3: 2 units */
#define FLAG_VG_SW_T3_3        0b00110000     /* VG_SW_T3: 3 units */
#define FLAG_VG_SW_T4_0        0b00000000     /* VG_SW_T4: 0 units */
#define FLAG_VG_SW_T4_1        0b01000000     /* VG_SW_T4: 1 unit */
#define FLAG_VG_SW_T4_2        0b10000000     /* VG_SW_T4: 2 units */
#define FLAG_VG_SW_T4_3        0b11000000     /* VG_SW_T4: 3 units */

/* data byte #2 (mask: xxxxxx00) */
#define MASK_TIME_CTRL_B_2     0b00000000     /* mask: xxxxxx00 */


/*
 *  power-on sequence control
 *  - 1 byte cmd + 4 bytes data
 */

#define CMD_SEQ_CTRL          0b11101101     /* power-on sequence control */

/* data byte #1: soft start */
#define MASK_SEQ_CTRL_1       0b01000100     /* mask: x1ffx1ff */
#define FLAG_CP23_3           0b00000000     /* CP23 soft start: keep 3 frames */
#define FLAG_CP23_2           0b00000001     /* CP23 soft start: keep 2 frames */
#define FLAG_CP23_1           0b00000010     /* CP23 soft start: keep 1 frame */
#define FLAG_CP23_0           0b00000011     /* CP23 soft start: disable */
#define FLAG_CP1_3            0b00000000     /* CP1 soft start: keep 3 frames */
#define FLAG_CP1_2            0b00010000     /* CP1 soft start: keep 2 frames */
#define FLAG_CP1_1            0b00100000     /* CP1 soft start: keep 1 frame */
#define FLAG_CP1_0            0b00110000     /* CP1 soft start: disable */

/* data byte #2: power on sequence */
#define MASK_SEQ_CTRL_2       0b00000000     /* mask: x0ffx0ff */
#define FLAG_DDVDH_1          0b00000000     /* En_ddvdh: enable 1st frame */
#define FLAG_DDVDH_2          0b00000001     /* En_ddvdh: enable 2st frame */
#define FLAG_DDVDH_3          0b00000010     /* En_ddvdh: enable 3st frame */
#define FLAG_DDVDH_4          0b00000011     /* En_ddvdh: enable 4st frame */
#define FLAG_VCL_1            0b00000000     /* En_vcl: enable 1st frame */
#define FLAG_VCL_2            0b00010000     /* En_vcl: enable 2st frame */
#define FLAG_VCL_3            0b00100000     /* En_vcl: enable 3st frame */
#define FLAG_VCL_4            0b00110000     /* En_vcl: enable 4st frame */

/* data byte #3: power on sequence */
#define MASK_SEQ_CTRL_3       0b00000000     /* mask: x0ffx0ff */
#define FLAG_VGL_1            0b00000000     /* En_vgl: enable 1st frame */
#define FLAG_VGL_2            0b00000001     /* En_vgl: enable 2st frame */
#define FLAG_VGL_3            0b00000010     /* En_vgl: enable 3st frame */
#define FLAG_VGL_4            0b00000011     /* En_vgl: enable 4st frame */
#define FLAG_VGH_1            0b00000000     /* En_vgh: enable 1st frame */
#define FLAG_VGH_2            0b00010000     /* En_vgh: enable 2st frame */
#define FLAG_VGH_3            0b00100000     /* En_vgh: enable 3st frame */
#define FLAG_VGH_4            0b00110000     /* En_vgh: enable 4st frame */

/* data byte #4: DDVDH enhance mode */
#define MASK_SEQ_CTRL_4       0b00000001     /* mask: f0000001 */
#define FLAG_DDVDH_ENH_OFF    0b00000000     /* disable DDVDH */
#define FLAG_DDVDH_ENH_ON     0b10000000     /* enable DDVDH */


/*
 *  enable 3G
 *  - 1 byte cmd + 1 byte data
 */

#define CMD_ENABLE_3G         0b11110010     /* enable 3 gamma control */

/* data byte #1 */
#define MASK_ENABLE_3G_1      0b00000010     /* mask: 0000001f */
#define FLAG_3G_OFF           0b00000000     /* disable 3 gamma control */
#define FLAG_3G_ON            0b00000001     /* enable 3 gamma control */


/*
 *  pump ratio control
 *  - 1 byte cmd + 1 byte data
 */

#define CMD_PUMP_RATIO_CTRL   0b11110111     /* pump ratio control */

/* data byte #1: pump ratio */
#define MASK_PUMP_RATIO_1     0b00000000     /* mask: xxff0000 */
#define FLAG_PUMP_RATIO_2     0b00100000     /* DDVDH = 2xVCI */
#define FLAG_PUMP_RATIO_3     0b00110000     /* DDVDH = 3xVCI */



/* ************************************************************************
 *   EOF
 * ************************************************************************ */
