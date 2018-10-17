/* ************************************************************************
 *
 *   ST7735 color graphic display controller
 *
 *   (c) 2016-2018 by Markus Reschke
 *
 * ************************************************************************ */


/* ************************************************************************
 *   system function commands
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
  /* horizontal refresh (horizontal order): */
#define FLAG_STAT_HOR_NORM    0b00000000     /* decrement, left to right */
#define FLAG_STAT_HOR_REV     0b00000010     /* increment, right to left */
  /* RGB/BGR order */
#define FLAG_STAT_RGB         0b00000000     /* RGB */
#define FLAG_STAT_BGR         0b00000100     /* BGR */
  /* vertical refresh (scan address order): */
#define FLAG_STAT_VER_NORM    0b00000000     /* decrement, top to bottom */
#define FLAG_STAT_VER_REV     0b00001000     /* increment, bottom to top */
  /* row/column exchange: */
#define FLAG_STAT_EXCH_NORM   0b00000000     /* normal */
#define FLAG_STAT_EXCH_REV    0b00010000     /* row/column swapped */
  /* column address order: */
#define FLAG_STAT_COL_NORM    0b00000000     /* increment, left to right */
#define FLAG_STAT_COL_REV     0b00100000     /* decrement, right to left */
  /* row address order: */
#define FLAG_STAT_ROW_NORM    0b00000000     /* increment, top to bottom */
#define FLAG_STAT_ROW_REV     0b01000000     /* decrement, bottom to top */
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
#define FLAG_STAT_PIX_12      0b00110000     /* 12 bits per pixel */
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
  /* inversion: */
#define FLAG_STAT_INV_OFF     0b00000000     /* off */
#define FLAG_STAT_INV_ON      0b00100000     /* on */

/* data byte #5: status */
  /* tearing effect line mode: */
#define FLAG_STAT_TEAR_MODE1  0b00000000     /* mode 1: V-blanking */
#define FLAG_STAT_TEAR_MODE2  0b00100000     /* mode 2: V-blanking and H-blanking */
  /* gamma curve bits #1 and #0 (bit #2 is always 0): */
#define FLAG_STAT_GAMMA1      0b00000000     /* gamma curve 1 */
#define FLAG_STAT_GAMMA2      0b01000000     /* gamma curve 2 */
#define FLAG_STAT_GAMMA3      0b10000000     /* gamma curve 3 */
#define FLAG_STAT_GAMMA4      0b11000000     /* gamma curve 4 */


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
#define FLAG_NORMAL_OFF       0b00000000     /* off (partial display) */
#define FLAG_NORMAL_ON        0b00001000     /* on (normal display) */
  /* sleep mode: */
#define FLAG_SLEEP_OFF        0b00000000     /* off (sleep in) */
#define FLAG_SLEEP_ON         0b00010000     /* on (sleep out) */
  /* partial mode: */
#define FLAG_PARTIAL_OFF      0b00000000     /* off */
#define FLAG_PARTIAL_ON       0b00100000     /* on */
  /* idle mode: */
#define FLAG_IDLE_OFF         0b00000000     /* off */
#define FLAG_IDLE_ON          0b01000000     /* on */
  /* booster voltage: */
#define FLAG_BOOSTER_OFF      0b00000000     /* off */
#define FLAG_BOOSTER_ON       0b10000000     /* on */


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
  /* color order: */
#define RFLAG_RGB_RGB         0b00000000     /* RGB color filter */
#define RFLAG_RGB_BGR         0b00001000     /* BGR color filter */
  /* vertical refereshing direction: */
#define RFLAG_ML_NORM         0b00000000     /* top to bottom */
#define RFLAG_ML_REV          0b00010000     /* bottom to top */
  /* row/column exchange (MCU to memory): */
#define RFLAG_MV_NORM         0b00000000     /* normal */
#define RFLAG_MV_REV          0b00100000     /* row/column swapped */
  /* column address direction (MCU to memory): */
#define RFLAG_MX_NORM         0b00000000     /* left to right */
#define RFLAG_MX_REV          0b01000000     /* right to left */
  /* row address direction (MCU to memory): */
#define RFLAG_MY_NORM         0b00000000     /* top to bottom */
#define RFLAG_MY_REV          0b10000000     /* bottom to top */


/*
 *  read display pixel format
 *  - 1 byte cmd + 2 bytes data (read mode)
 */

#define CMD_READ_PIX_FORMAT   0b00001100     /* read display pixel format */

/* data byte #1: dummy data */

/* data byte #2: status */
/* same as byte #1 of CMD_PIX_FORMAT */
  /* MCU interface color format: */
#define RFLAG_IFPF_12         0b00000011     /* 12 bits per pixel */
#define RFLAG_IFPF_16         0b00000101     /* 16 bits per pixel */
#define RFLAG_IFPF_18         0b00000110     /* 18 bits per pixel */


/*
 *  read display image mode
 *  - 1 byte cmd + 2 bytes data (read mode)
 */

#define CMD_READ_IMG_MODE     0b00001101     /* read display image mode */

/* data byte #1: dummy data */

/* data byte #2: status */
  /* gamma curve */
#define FLAG_IMG_GAMMA1       0b00000000     /* gamma curve 1 */
#define FLAG_IMG_GAMMA2       0b00000001     /* gamma curve 2 */
#define FLAG_IMG_GAMMA3       0b00000010     /* gamma curve 3 */
#define FLAG_IMG_GAMMA4       0b00000011     /* gamma curve 4 */
  /* inversion */
#define FLAG_IMG_INV_OFF      0b00000000     /* off */
#define FLAG_IMG_INV_ON       0b00100000     /* on */


/*
 *  read display signal mode
 *  - 1 byte cmd + 2 bytes data (read mode)
 */

#define CMD_READ_SIG_MODE     0b00001110     /* read display signal mode */

/* data byte #1: dummy data */

/* data byte #2: mode flags */
  /* tearing effect line mode: */
#define FLAG_SIG_TEAR_1       0b00000000     /* mode 1 */
#define FLAG_SIG_TEAR_2       0b01000000     /* mode 2 */
  /* tearing effect line: */
#define FLAG_SIG_TEAR_OFF     0b00000000     /* off */
#define FLAG_SIG_TEAR_ON      0b10000000     /* on */


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
#define FLAG_GC_2             0b00000010     /* gamma curve 2 */
#define FLAG_GC_3             0b00000100     /* gamma curve 3 */
#define FLAG_GC_4             0b00001000     /* gamma curve 4 */


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
/* valid range: 0x0000 - 0x007f/0x009f (128x160) */
/*              0x0000 - 0x0083/0x00a1 (132x162) */


/*
 *  set row address (accessable frame area)
 *  - 1 byte cmd + 4 bytes data
 */

#define CMD_ROW_ADDR_SET      0b00101011     /* set row address */

/* data byte #1: start row, MSB (bits 15-8) */
/* data byte #2: start row, LSB (bits 7-0) */
/* data byte #3: end row, MSB (bits 15-8) */
/* data byte #4: end row, LSB (bits 7-0) */
/* valid range: 0x0000 - 0x009f/0x007f (128x160) */
/*              0x0000 - 0x00a1/0x0083 (132x162) */


/*
 *  write memory (starting at start column and start row)
 *  - 1 byte cmd + x bytes data
 */

#define CMD_MEM_WRITE         0b00101100     /* write memory */

/* data byte #1 - #x: image data */


/*
 *  read memory (starting at start column and start row)
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
/* valid range: 0x0000 - 0x09f/0x00a1 */


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
  /* tearing effect mode: */
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
  /* color order: */
#define FLAG_RGB_RGB          0b00000000     /* RGB color filter */
#define FLAG_RGB_BGR          0b00001000     /* BGR color filter */
  /* vertical refereshing direction: */
#define FLAG_ML_NORM          0b00000000     /* top to bottom */
#define FLAG_ML_REV           0b00010000     /* bottom to top */
  /* row/column exchange (MCU to memory): */
#define FLAG_MV_NORM          0b00000000     /* normal */
#define FLAG_MV_REV           0b00100000     /* row/column swapped */
  /* column address direction (MCU to memory): */
#define FLAG_MX_NORM          0b00000000     /* left to right */
#define FLAG_MX_REV           0b01000000     /* right to left */
  /* row address direction (MCU to memory): */
#define FLAG_MY_NORM          0b00000000     /* top to bottom */
#define FLAG_MY_REV           0b10000000     /* bottom to top */


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

#define CMD_PIX_FORMAT        0b00111010     /* set pixel format */

/* data byte #1: formats */
/* same as byte #2 of CMD_READ_PIX_FORMAT */
  /* MCU interface color format: */
#define FLAG_IFPF_12          0b00000011     /* 12 bits per pixel */
#define FLAG_IFPF_16          0b00000101     /* 16 bits per pixel */
#define FLAG_IFPF_18          0b00000110     /* 18 bits per pixel */


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
/* data byte #2: LCD module/driver ID */



/* ************************************************************************
 *   panel function commands
 * ************************************************************************ */


/*
 *  frame rate control in normal mode (full color)
 *  - 1 byte cmd + 3 bytes data
 */

#define CMD_FRAME_CTRL_NORM   0b10110001     /* frame control in normal mode */

/* data byte #1: line period */
  /* valid range: 0 - 15 */

/* data byte #2: front porch */
  /* valid range: 0 - 63 */

/* data byte #3: back porch */
  /* valid range: 0 - 63 */


/*
 *  frame rate control in idle mode (8bit color depth)
 *  - 1 byte cmd + 3 bytes data
 */

#define CMD_FRAME_CTRL_IDLE   0b10110010     /* frame control in idle mode */

/* data byte #1: line period */
  /* valid range: 0 - 15 */

/* data byte #2: front porch */
  /* valid range: 0 - 63 */

/* data byte #3: back porch */
  /* valid range: 0 - 63 */


/*
 *  frame control in partial mode (full color)
 *  - 1 byte cmd + 6 bytes data
 */

#define CMD_FRAME_CTRL_PART   0b10110011     /* frame control in partial mode */

/* data byte #1: line period (line inversion mode) */
  /* valid range: 0 - 15 */

/* data byte #2: front porch (line inversion mode) */
  /* valid range: 0 - 63 */

/* data byte #3: back porch (line inversion mode) */
  /* valid range: 0 - 63 */

/* data byte #4: line period (frame inversion mode) */
  /* valid range: 0 - 15 */

/* data byte #5: front porch (frame inversion mode) */
  /* valid range: 0 - 63 */

/* data byte #6: back porch (frame inversion mode) */
  /* valid range: 0 - 63 */


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
 *  set display function
 *  - 1 byte cmd + 2 bytes data
 */

#define CMD_FUNC_CTRL         0b10110110     /* set display function */

/* data byte #1: */
  /* equalizing period */
#define FLAG_EQ_0             0b00000000     /* disabled */
#define FLAG_EQ_3             0b00000001     /* 3 clock cycles */
#define FLAG_EQ_5             0b00000010     /* 5 clock cycles */
#define FLAG_EQ_7             0b00000011     /* 7 clock cycles */
  /* delay after rising edge of gate signal of source output */
#define FLAG_SDT_0            0b00000000     /* 0 clock cycles */
#define FLAG_SDT_1            0b00000100     /* 1 clock cycle */
#define FLAG_SDT_2            0b00001000     /* 2 clock cycles */
#define FLAG_SDT_3            0b00001100     /* 3 clock cycles */
  /* non-overlap of gate output */
#define FLAG_NO_0             0b00000000     /* 1 clock cycle */
#define FLAG_NO_2             0b00010000     /* 2 clock cycles */
#define FLAG_NO_4             0b00100000     /* 4 clock cycles */
#define FLAG_NO_6             0b00110000     /* 6 clock cycles */

/* data byte #2: */
  /* source/VCOM output in non-display area in partial display mode: */
                              /* source pos / source neg - VCOM pos / VCOM neg */
#define FLAG_PT_0             0b00000000     /* V63 / V0 - VCOML / VCOMH */
#define FLAG_PT_1             0b00000001     /* V0 / V63 - VCOML / VCOMH */
#define FLAG_PT_2             0b00000010     /* AGND / AGND - AGND / AGND */
#define FLAG_PT_3             0b00000011     /* Hi-Z / Hi-Z - AGND / AGND */
  /* gate output in non-display area in partial display mode: */
#define FLAG_PTG_0            0b00000000     /* normal scan */
#define FLAG_PTG_1            0b00000001     /* fix on VGL */
#define FLAG_PTG_2            0b00000010     /* fix on VGL */
#define FLAG_PTG_3            0b00000011     /* fix on VGL */


/*
 *  power control 1
 *  - 1 byte cmd + 2 bytes data
 */

#define CMD_POWER_CTRL_1      0b11000000     /* power control 1 */

/* data byte #1: GVDD level (reference for VCOM) */
#define FLAG_VRH_500          0b00000000     /* 5.00V */
#define FLAG_VRH_475          0b00000001     /* 4.75V */
#define FLAG_VRH_470          0b00000010     /* 4.70V */
#define FLAG_VRH_465          0b00000011     /* 4.65V */
#define FLAG_VRH_460          0b00000100     /* 4.60V */
#define FLAG_VRH_455          0b00000101     /* 4.55V */
#define FLAG_VRH_450          0b00000110     /* 4.50V */
#define FLAG_VRH_445          0b00000111     /* 4.45V */
#define FLAG_VRH_440          0b00001000     /* 4.40V */
#define FLAG_VRH_435          0b00001001     /* 4.35V */
#define FLAG_VRH_430          0b00001010     /* 4.30V */
#define FLAG_VRH_425          0b00001011     /* 4.25V */
#define FLAG_VRH_420          0b00001100     /* 4.20V */
#define FLAG_VRH_415          0b00001101     /* 4.15V */
#define FLAG_VRH_410          0b00001110     /* 4.10V */
#define FLAG_VRH_405          0b00001111     /* 4.05V */
#define FLAG_VRH_400          0b00010000     /* 4.00V */
#define FLAG_VRH_395          0b00010001     /* 3.95V */
#define FLAG_VRH_390          0b00010010     /* 3.90V */
#define FLAG_VRH_385          0b00010011     /* 3.85V */
#define FLAG_VRH_380          0b00010100     /* 3.80V */
#define FLAG_VRH_375          0b00010101     /* 3.75V */
#define FLAG_VRH_370          0b00010110     /* 3.70V */
#define FLAG_VRH_365          0b00010111     /* 3.65V */
#define FLAG_VRH_360          0b00011000     /* 3.60V */
#define FLAG_VRH_355          0b00011001     /* 3.55V */
#define FLAG_VRH_350          0b00011010     /* 3.50V */
#define FLAG_VRH_345          0b00011011     /* 3.45V */
#define FLAG_VRH_340          0b00011100     /* 3.40V */
#define FLAG_VRH_335          0b00011101     /* 3.35V */
#define FLAG_VRH_325          0b00011110     /* 3.25V */
#define FLAG_VRH_300          0b00011111     /* 3.00V */

/* data byte #2: AVDD current */
#define FLAG_IB_25            0b01000000     /* 2.5 µA */
#define FLAG_IB_20            0b01010000     /* 2.0 µA */
#define FLAG_IB_15            0b01100000     /* 1.5 µA */
#define FLAG_IB_10            0b01110000     /* 1.0 µA */


/*
 *  power control 2
 *  - 1 byte cmd + 1 byte data
 */

#define CMD_POWER_CTRL_2      0b11000001     /* power control 2 */

/* data byte #1: VGH and VGL factors for step-up converter */
                                             /* VGH / VGL */
#define FLAG_BT_0             0b00000000     /* 4* (9.8)   / -3* (-7.35) */
#define FLAG_BT_1             0b00000001     /* 4* (9.8)   / -4* (-9.8) */
#define FLAG_BT_2             0b00000010     /* 5* (12.25) / -3* (-7.35) */
#define FLAG_BT_3             0b00000011     /* 5* (12.25) / -4* (-9.8) */
#define FLAG_BT_4             0b00000100     /* 5* (12.25) / -5* (-12.25) */
#define FLAG_BT_5             0b00000101     /* 6* (14.7)  / -3* (-7.35) */
#define FLAG_BT_6             0b00000101     /* 6* (14.7)  / -4* (-9.8) */
#define FLAG_BT_7             0b00000111     /* 6* (14.7)  / -5* (-12.25) */


/*
 *  power control 3 (normal mode / full color)
 *  - 1 byte cmd + 2 bytes data
 */

#define CMD_POWER_CTRL_3      0b11000010     /* power control 3 */

/* data byte #1: current for source driver OPamp */
#define FLAG_APA_0            0b00000000     /* disabled */
#define FLAG_APA_1            0b00000001     /* small */
#define FLAG_APA_2            0b00000010     /* medium low */
#define FLAG_APA_3            0b00000011     /* medium */
#define FLAG_APA_4            0b00000100     /* medium high */
#define FLAG_APA_5            0b00000101     /* large */
#define FLAG_APA_6            0b00000110     /* reserved */
#define FLAG_APA_7            0b00000111     /* reserved */

/* data byte #2: step-up cycles of booster circuit */
                                             /* circuit 1 / circuits 2 & 4 */ 
#define FLAG_DCA_0            0b00000000     /* BCLK/1 / BCLK/1 */
#define FLAG_DCA_1            0b00000001     /* BCLK/1 / BCLK/2 */
#define FLAG_DCA_2            0b00000010     /* BCLK/1 / BCLK/4 */
#define FLAG_DCA_3            0b00000011     /* BCLK/2 / BCLK/2 */
#define FLAG_DCA_4            0b00000100     /* BCLK/2 / BCLK/4 */
#define FLAG_DCA_5            0b00000101     /* BCLK/4 / BCLK/4 */
#define FLAG_DCA_6            0b00000110     /* BCLK/4 / BCLK/8 */
#define FLAG_DCA_7            0b00000111     /* BCLK/4 / BCLK/16 */


/*
 *  power control 4 (idle mode / 8bit color)
 *  - 1 byte cmd + 2 bytes data
 */

#define CMD_POWER_CTRL_4      0b11000011     /* power control 4 */

/* data byte #1: current for source driver OPamp */
#define FLAG_APB_0            0b00000000     /* disabled */
#define FLAG_APB_1            0b00000001     /* small */
#define FLAG_APB_2            0b00000010     /* medium low */
#define FLAG_APB_3            0b00000011     /* medium */
#define FLAG_APB_4            0b00000100     /* medium high */
#define FLAG_APB_5            0b00000101     /* large */
#define FLAG_APB_6            0b00000110     /* reserved */
#define FLAG_APB_7            0b00000111     /* reserved */

/* data byte #2: step-up cycles of booster circuit */
                                             /* circuit 1 / circuits 2 & 4 */ 
#define FLAG_DCB_0            0b00000000     /* BCLK/1 / BCLK/1 */
#define FLAG_DCB_1            0b00000001     /* BCLK/1 / BCLK/2 */
#define FLAG_DCB_2            0b00000010     /* BCLK/1 / BCLK/4 */
#define FLAG_DCB_3            0b00000011     /* BCLK/2 / BCLK/2 */
#define FLAG_DCB_4            0b00000100     /* BCLK/2 / BCLK/4 */
#define FLAG_DCB_5            0b00000101     /* BCLK/4 / BCLK/4 */
#define FLAG_DCB_6            0b00000110     /* BCLK/4 / BCLK/8 */
#define FLAG_DCB_7            0b00000111     /* BCLK/4 / BCLK/16 */


/*
 *  power control 5 (partial mode / full color)
 *  - 1 byte cmd + 2 bytes data
 */

#define CMD_POWER_CTRL_5      0b11000100     /* power control 5 */

/* data byte #1: current for source driver OPamp */
#define FLAG_APC_0            0b00000000     /* disabled */
#define FLAG_APC_1            0b00000001     /* small */
#define FLAG_APC_2            0b00000010     /* medium low */
#define FLAG_APC_3            0b00000011     /* medium */
#define FLAG_APC_4            0b00000100     /* medium high */
#define FLAG_APC_5            0b00000101     /* large */
#define FLAG_APC_6            0b00000110     /* reserved */
#define FLAG_APC_7            0b00000111     /* reserved */

/* data byte #2: step-up cycles of booster circuit */
                                             /* circuit 1 / circuits 2 & 4 */ 
#define FLAG_DCC_0            0b00000000     /* BCLK/1 / BCLK/1 */
#define FLAG_DCC_1            0b00000001     /* BCLK/1 / BCLK/2 */
#define FLAG_DCC_2            0b00000010     /* BCLK/1 / BCLK/4 */
#define FLAG_DCC_3            0b00000011     /* BCLK/2 / BCLK/2 */
#define FLAG_DCC_4            0b00000100     /* BCLK/2 / BCLK/4 */
#define FLAG_DCC_5            0b00000101     /* BCLK/4 / BCLK/4 */
#define FLAG_DCC_6            0b00000110     /* BCLK/4 / BCLK/8 */
#define FLAG_DCC_7            0b00000111     /* BCLK/4 / BCLK/16 */


/*
 *  VCOM control 1
 *  - 1 byte cmd + 2 bytes data
 */

#define CMD_VCOM_CTRL_1       0b11000101     /* VCOM control 1 */

/* data byte #1: VCOMH voltage */
#define FLAG_VMH_MIN          0b00000000     /* minumum value */
#define FLAG_VMH_2500         0b00000000     /* 2.500V */
#define FLAG_VMH_2525         0b00000001     /* 2.525V */
#define FLAG_VMH_2550         0b00000010     /* 2.550V */
#define FLAG_VMH_2575         0b00000011     /* 2.575V */
#define FLAG_VMH_2600         0b00000100     /* 2.600V */
#define FLAG_VMH_2625         0b00000101     /* 2.625V */
#define FLAG_VMH_2650         0b00000110     /* 2.650V */
#define FLAG_VMH_2675         0b00000111     /* 2.675V */
#define FLAG_VMH_2700         0b00001000     /* 2.700V */
#define FLAG_VMH_2725         0b00001001     /* 2.725V */
#define FLAG_VMH_2750         0b00001010     /* 2.750V */
#define FLAG_VMH_2775         0b00001011     /* 2.775V */
#define FLAG_VMH_2800         0b00001100     /* 2.800V */
#define FLAG_VMH_2825         0b00001101     /* 2.825V */
#define FLAG_VMH_2850         0b00001110     /* 2.850V */
#define FLAG_VMH_2875         0b00001111     /* 2.875V */
#define FLAG_VMH_2900         0b00010000     /* 2.900V */
#define FLAG_VMH_2925         0b00010001     /* 2.925V */
#define FLAG_VMH_2950         0b00010010     /* 2.950V */
#define FLAG_VMH_2975         0b00010011     /* 2.975V */
#define FLAG_VMH_3000         0b00010100     /* 3.000V */
#define FLAG_VMH_3025         0b00010101     /* 3.025V */
#define FLAG_VMH_3050         0b00010110     /* 3.050V */
#define FLAG_VMH_3075         0b00010111     /* 3.075V */
#define FLAG_VMH_3100         0b00011000     /* 3.100V */
#define FLAG_VMH_3125         0b00011001     /* 3.125V */
#define FLAG_VMH_3150         0b00011010     /* 3.150V */
#define FLAG_VMH_3175         0b00011011     /* 3.175V */
#define FLAG_VMH_3200         0b00011100     /* 3.200V */
#define FLAG_VMH_3225         0b00011101     /* 3.225V */
#define FLAG_VMH_3250         0b00011110     /* 3.250V */
#define FLAG_VMH_3275         0b00011111     /* 3.275V */
#define FLAG_VMH_3300         0b00100000     /* 3.300V */
#define FLAG_VMH_3325         0b00100001     /* 3.325V */
#define FLAG_VMH_3350         0b00100010     /* 3.350V */
#define FLAG_VMH_3375         0b00100011     /* 3.375V */
#define FLAG_VMH_3400         0b00100100     /* 3.400V */
#define FLAG_VMH_3425         0b00100101     /* 3.425V */
#define FLAG_VMH_3450         0b00100110     /* 3.450V */
#define FLAG_VMH_3475         0b00100111     /* 3.475V */
#define FLAG_VMH_3500         0b00101000     /* 3.500V */
#define FLAG_VMH_3525         0b00101001     /* 3.525V */
#define FLAG_VMH_3550         0b00101010     /* 3.550V */
#define FLAG_VMH_3575         0b00101011     /* 3.575V */
#define FLAG_VMH_3600         0b00101100     /* 3.600V */
#define FLAG_VMH_3625         0b00101101     /* 3.625V */
#define FLAG_VMH_3650         0b00101110     /* 3.650V */
#define FLAG_VMH_3675         0b00101111     /* 3.675V */
#define FLAG_VMH_3700         0b00110000     /* 3.700V */
#define FLAG_VMH_3725         0b00110001     /* 3.725V */
#define FLAG_VMH_3750         0b00110010     /* 3.750V */
#define FLAG_VMH_3775         0b00110011     /* 3.775V */
#define FLAG_VMH_3800         0b00110100     /* 3.800V */
#define FLAG_VMH_3825         0b00110101     /* 3.825V */
#define FLAG_VMH_3850         0b00110110     /* 3.850V */
#define FLAG_VMH_3875         0b00110111     /* 3.875V */
#define FLAG_VMH_3900         0b00111000     /* 3.900V */
#define FLAG_VMH_3925         0b00111001     /* 3.925V */
#define FLAG_VMH_3950         0b00111010     /* 3.950V */
#define FLAG_VMH_3975         0b00111011     /* 3.975V */
#define FLAG_VMH_4000         0b00111100     /* 4.000V */
#define FLAG_VMH_4025         0b00111101     /* 4.025V */
#define FLAG_VMH_4050         0b00111110     /* 4.050V */
#define FLAG_VMH_4075         0b00111111     /* 4.075V */
#define FLAG_VMH_4100         0b01000000     /* 4.100V */
#define FLAG_VMH_4125         0b01000001     /* 4.125V */
#define FLAG_VMH_4150         0b01000010     /* 4.150V */
#define FLAG_VMH_4175         0b01000011     /* 4.175V */
#define FLAG_VMH_4200         0b01000100     /* 4.200V */
#define FLAG_VMH_4225         0b01000101     /* 4.225V */
#define FLAG_VMH_4250         0b01000110     /* 4.250V */
#define FLAG_VMH_4275         0b01000111     /* 4.275V */
#define FLAG_VMH_4300         0b01001000     /* 4.300V */
#define FLAG_VMH_4325         0b01001001     /* 4.325V */
#define FLAG_VMH_4350         0b01001010     /* 4.350V */
#define FLAG_VMH_4375         0b01001111     /* 4.375V */
#define FLAG_VMH_4400         0b01001100     /* 4.400V */
#define FLAG_VMH_4425         0b01001101     /* 4.425V */
#define FLAG_VMH_4450         0b01001110     /* 4.450V */
#define FLAG_VMH_4475         0b01001111     /* 4.475V */
#define FLAG_VMH_4500         0b01010000     /* 4.500V */
#define FLAG_VMH_4525         0b01010001     /* 4.525V */
#define FLAG_VMH_4550         0b01010010     /* 4.550V */
#define FLAG_VMH_4575         0b01010011     /* 4.575V */
#define FLAG_VMH_4600         0b01010100     /* 4.600V */
#define FLAG_VMH_4625         0b01010101     /* 4.625V */
#define FLAG_VMH_4650         0b01010110     /* 4.650V */
#define FLAG_VMH_4675         0b01010111     /* 4.675V */
#define FLAG_VMH_4700         0b01011000     /* 4.700V */
#define FLAG_VMH_4725         0b01011001     /* 4.725V */
#define FLAG_VMH_4750         0b01011010     /* 4.750V */
#define FLAG_VMH_4775         0b01011011     /* 4.775V */
#define FLAG_VMH_4800         0b01011100     /* 4.800V */
#define FLAG_VMH_4825         0b01011101     /* 4.825V */
#define FLAG_VMH_4850         0b01011110     /* 4.850V */
#define FLAG_VMH_4875         0b01011111     /* 4.875V */
#define FLAG_VMH_4900         0b01100000     /* 4.900V */
#define FLAG_VMH_4925         0b01100001     /* 4.925V */
#define FLAG_VMH_4950         0b01100010     /* 4.950V */
#define FLAG_VMH_4975         0b01100011     /* 4.975V */
#define FLAG_VMH_5000         0b01100100     /* 5.000V */
#define FLAG_VMH_MAX          0b01100100     /* maximum value */

/* data byte #2: VCOML voltage */
#define FLAG_VML_MIN          0b00000100     /* minumum value */
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
#define FLAG_VML_0625         0b01001111     /* -0.6V25 */
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
#define FLAG_VML_0000         0b01100100     /* 0.000V */
#define FLAG_VML_MAX          0b01100100     /* maximum value */


/*
 *  VCOM offset control
 *  - 1 byte cmd + 1 byte data
 */

#define CMD_VCOM_OFFSET       0b11000111     /* VCOM offset control */

/* data byte #1: */
  /* VCOM offset voltage for reducing flicker (d=25mV): */
                                             /* VMH + x / VML + x */
#define FLAG_VMF_MIN          0b00000000     /* minumum value */
#define FLAG_VMF_M16          0b00000000     /* -16d -4.00V */
#define FLAG_VMF_M15          0b00000001     /* -15d -3.75V */
#define FLAG_VMF_M14          0b00000010     /* -14d -3.50V */
#define FLAG_VMF_M13          0b00000011     /* -13d -3.25V */
#define FLAG_VMF_M12          0b00000100     /* -12d -3.00V */
#define FLAG_VMF_M11          0b00000101     /* -11d -2.75V */
#define FLAG_VMF_M10          0b00000110     /* -10d -2.50V */
#define FLAG_VMF_M09          0b00000111     /* -9d  -2.25V */
#define FLAG_VMF_M08          0b00001000     /* -8d  -2.00V */
#define FLAG_VMF_M07          0b00001001     /* -7d  -1.75V */
#define FLAG_VMF_M06          0b00001010     /* -6d  -1.50V */
#define FLAG_VMF_M05          0b00001011     /* -5d  -1.25V */
#define FLAG_VMF_M04          0b00001100     /* -4d  -1.00V */
#define FLAG_VMF_M03          0b00001101     /* -3d  -0.75V */
#define FLAG_VMF_M02          0b00001110     /* -2d  -0.50V */
#define FLAG_VMF_M01          0b00001111     /* -1d  -0.25V */
#define FLAG_VMF_M00          0b00010000     /* -0d  -0.00V */
#define FLAG_VMF_P00          0b00010000     /* +0d  +0.00V */
#define FLAG_VMF_P01          0b00010001     /* +1d  +0.25V */
#define FLAG_VMF_P02          0b00010010     /* +2d  +0.50V */
#define FLAG_VMF_P03          0b00010011     /* +3d  +0.75V */
#define FLAG_VMF_P04          0b00010100     /* +4d  +1.00V */
#define FLAG_VMF_P05          0b00010101     /* +5d  +1.25V */
#define FLAG_VMF_P06          0b00010110     /* +6d  +1.50V */
#define FLAG_VMF_P07          0b00010111     /* +7d  +1.75V */
#define FLAG_VMF_P08          0b00011000     /* +8d  +2.00V */
#define FLAG_VMF_P09          0b00011001     /* +9d  +2.25V */
#define FLAG_VMF_P10          0b00011010     /* +10d +2.50V */
#define FLAG_VMF_P11          0b00011011     /* +11d +2.75V */
#define FLAG_VMF_P12          0b00011100     /* +12d +3.00V */
#define FLAG_VMF_P13          0b00011101     /* +13d +3.25V */
#define FLAG_VMF_P14          0b00011110     /* +14d +3.75V */
#define FLAG_VMF_P15          0b00011111     /* +15d +4.00V */
#define FLAG_VMF_MAX          0b00011111     /* maximum value */


/*
 *  write ID2 value to EEPROM (module version)
 *  - 1 byte cmd + 1 byte data
 */

#define CMD_WRITE_ID2         0b11010001     /* write ID2 */

/* data byte #1: module version (7 bits) */


/*
 *  write ID3 value to EEPROM (product project)
 *  - 1 byte cmd + 1 byte data
 */

#define CMD_WRITE_ID3         0b11010010     /* write ID3 */

/* data byte #1: product project (8 bits) */


/*
 *  power control 6 (partial mode & idle mode)
 *  - 1 byte cmd + 2 bytes data
 */

#define CMD_POWER_CTRL_6      0b11111100     /* power control 6 */

/* data byte #1: mode */
  /* current for source driver OPamp (Sapb): */
#define FLAG_SAPB_0           0b00000000     /* disabled */
#define FLAG_SAPB_1           0b00000001     /* small */
#define FLAG_SAPB_2           0b00000010     /* medium low */
#define FLAG_SAPB_3           0b00000011     /* medium */
#define FLAG_SAPB_4           0b00000100     /* medium high */
#define FLAG_SAPB_5           0b00000101     /* large */
#define FLAG_SAPB_6           0b00000110     /* reserved */
#define FLAG_SAPB_7           0b00000111     /* reserved */
  /* current for source driver OPamp (Sapa): */
#define FLAG_SAPA_0           0b00000000     /* disabled */
#define FLAG_SAPA_1           0b00010000     /* small */
#define FLAG_SAPA_2           0b00100000     /* medium low */
#define FLAG_SAPA_3           0b00110000     /* medium */
#define FLAG_SAPA_4           0b01000000     /* medium high */
#define FLAG_SAPA_5           0b01010000     /* large */
#define FLAG_SAPA_6           0b01100000     /* reserved */
#define FLAG_SAPA_7           0b01110000     /* reserved */

/* data byte #2: mode */
  /* step-up cycles of booster circuit */
                                             /* circuit 1 / circuits 2 & 4 */ 
#define FLAG_DCD_0            0b00000000     /* BCLK/1 / BCLK/1 */
#define FLAG_DCD_1            0b00000001     /* BCLK/1 / BCLK/2 */
#define FLAG_DCD_2            0b00000010     /* BCLK/1 / BCLK/4 */
#define FLAG_DCD_3            0b00000011     /* BCLK/2 / BCLK/2 */
#define FLAG_DCD_4            0b00000100     /* BCLK/2 / BCLK/4 */
#define FLAG_DCD_5            0b00000101     /* BCLK/4 / BCLK/4 */
#define FLAG_DCD_6            0b00000110     /* BCLK/4 / BCLK/8 */
#define FLAG_DCD_7            0b00000111     /* BCLK/4 / BCLK/16 */
  /* current for source driver OPamp (Sapc): */  
#define FLAG_SAPC_0           0b00000000     /* disabled */
#define FLAG_SAPC_1           0b00010000     /* small */
#define FLAG_SAPC_2           0b00100000     /* medium low */
#define FLAG_SAPC_3           0b00110000     /* medium */
#define FLAG_SAPC_4           0b01000000     /* medium high */
#define FLAG_SAPC_5           0b01010000     /* large */
#define FLAG_SAPC_6           0b01100000     /* reserved */
#define FLAG_SAPC_7           0b01110000     /* reserved */


/*
 *  EEPROM control 1: EEPROM control status
 *  - 1 byte cmd + 1 byte data
 */

#define CMD_NV_CTRL1          0b11001001     /* NV control 1 */

/* data byte #1: mode */
  /* command "write ID2": */
#define FLAG_ID2_DISABLE      0b00000000     /* disable command */
#define FLAG_ID2_ENABLE       0b00010000     /* enable command */
  /* command "VCOM Offset Control": */
#define FLAG_VMF_DISABLE      0b00000000     /* disable command */
#define FLAG_VMF_ENABLE       0b00100000     /* enable command */


/*
 *  EEPROM control 2: EEPROM read command
 *  - 1 byte cmd + 1 byte data
 */

#define CMD_NV_CTRL2          0b11011110     /* NV control 2 */

/* data byte #1: fixed value */
#define FLAG_NV_CTRL2         0b10100101     /* fixed value */


/*
 *  EEPROM control 3: EEPROM write command
 *  - 1 byte cmd + 3 bytes data
 */

#define CMD_NV_CTRL3          0b11011111     /* NV control 3 */

/* data byte #1: command (register) selection */
#define FLAG_NV_VCOM          0b11000111     /* VCOM Offset Control */
#define FLAG_NV_ID2           0b11010001     /* write ID2 */
#define FLAG_NV_ID3           0b11010010     /* write ID3 */

/* data byte #2: EEPROM command */
#define FLAG_NV_PROGRAM       0b00111010     /* program */ 
#define FLAG_NV_ERASE         0b11000101     /* erase */

/* data byte #3: fixed value */
#define FLAG_NV_CTRL3         0b10100101     /* fixed value */


/*
 *  positive gamma correction, gray scale voltage
 *  - 1 byte cmd + 16 bytes data
 */

#define CMD_GAMMA_CORR_POS    0b11100000     /* positive gamma correction */

/* data bytes: please see datasheet */


/*
 *  negative gamma correction, gray scale voltage
 *  - 1 byte cmd + 16 bytes data
 */

#define CMD_GAMMA_CORR_NEG    0b11100001     /* negative gamma correction */

/* data bytes: please see datasheet */


/*
 *  extension command control (if EXTC pin is low)
 *  - 1 byte cmd + 1 byte data
 */

#define CMD_EXT_CTRL          0b11110000     /* extension command control */

/* data byte #1: fixed value */
#define FLAG_EXT_CTRL         0b00000001     /* fixed value */


/*
 *  VCOM control 4
 *  - 1 byte cmd + 3 bytes data
 */

#define CMD_VCOM_CTRL_4       0b11111111     /* VCOM control 4 */

/* data byte #1:  */
  /* delay TC1: */
#define FLAG_TC1_00           0b00000000     /* 0 clock cycles */
#define FLAG_TC1_01           0b00000001     /* 1 clock cycle */
#define FLAG_TC1_02           0b00000010     /* 2 clock cycles */
#define FLAG_TC1_03           0b00000011     /* 3 clock cycles */
#define FLAG_TC1_04           0b00000100     /* 4 clock cycles */
#define FLAG_TC1_05           0b00000101     /* 5 clock cycles */
#define FLAG_TC1_06           0b00000110     /* 6 clock cycles */
#define FLAG_TC1_07           0b00000111     /* 7 clock cycles */
#define FLAG_TC1_08           0b00001000     /* 8 clock cycles */
#define FLAG_TC1_09           0b00001001     /* 9 clock cycles */
#define FLAG_TC1_10           0b00001010     /* 10 clock cycles */
#define FLAG_TC1_11           0b00001011     /* 11 clock cycles */
#define FLAG_TC1_12           0b00001100     /* 12 clock cycles */
#define FLAG_TC1_13           0b00001101     /* 13 clock cycles */
#define FLAG_TC1_14           0b00001110     /* 14 clock cycles */
#define FLAG_TC1_15           0b00001111     /* 15 clock cycles */
  /* delay TC2: */
#define FLAG_TC2_00           0b00000000     /* 0 clock cycles */
#define FLAG_TC2_01           0b00010000     /* 1 clock cycle */
#define FLAG_TC2_02           0b00100000     /* 2 clock cycles */
#define FLAG_TC2_03           0b00110000     /* 3 clock cycles */
#define FLAG_TC2_04           0b01000000     /* 4 clock cycles */
#define FLAG_TC2_05           0b01010000     /* 5 clock cycles */
#define FLAG_TC2_06           0b01100000     /* 6 clock cycles */
#define FLAG_TC2_07           0b01110000     /* 7 clock cycles */
#define FLAG_TC2_08           0b10000000     /* 8 clock cycles */
#define FLAG_TC2_09           0b10010000     /* 9 clock cycles */
#define FLAG_TC2_10           0b10100000     /* 10 clock cycles */
#define FLAG_TC2_11           0b10110000     /* 11 clock cycles */
#define FLAG_TC2_12           0b11000000     /* 12 clock cycles */
#define FLAG_TC2_13           0b11010000     /* 13 clock cycles */
#define FLAG_TC2_14           0b11100000     /* 14 clock cycles */
#define FLAG_TC2_15           0b11110000     /* 15 clock cycles */

/* data byte #2:  */
  /* delay TC3: */
#define FLAG_TC3_00           0b00000000     /* 0 clock cycles */
#define FLAG_TC3_01           0b00000001     /* 1 clock cycle */
#define FLAG_TC3_02           0b00000010     /* 2 clock cycles */
#define FLAG_TC3_03           0b00000011     /* 3 clock cycles */
#define FLAG_TC3_04           0b00000100     /* 4 clock cycles */
#define FLAG_TC3_05           0b00000101     /* 5 clock cycles */
#define FLAG_TC3_06           0b00000110     /* 6 clock cycles */
#define FLAG_TC3_07           0b00000111     /* 7 clock cycles */
#define FLAG_TC3_08           0b00001000     /* 8 clock cycles */
#define FLAG_TC3_09           0b00001001     /* 9 clock cycles */
#define FLAG_TC3_10           0b00001010     /* 10 clock cycles */
#define FLAG_TC3_11           0b00001011     /* 11 clock cycles */
#define FLAG_TC3_12           0b00001100     /* 12 clock cycles */
#define FLAG_TC3_13           0b00001101     /* 13 clock cycles */
#define FLAG_TC3_14           0b00001110     /* 14 clock cycles */
#define FLAG_TC3_15           0b00001111     /* 15 clock cycles */

/* data byte #3: fixed value */
#define FLAG_VCOM_CTRL_4      0b00011010     /* fixed value */


/* ************************************************************************
 *   EOF
 * ************************************************************************ */
