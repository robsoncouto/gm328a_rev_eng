/* ************************************************************************
 *
 *   PCD8544 graphic display controller
 *
 *   (c) 2016 by Markus Reschke
 *
 * ************************************************************************ */


/* ************************************************************************
 *  basic LCD commands
 * ************************************************************************ */


/*
 *  no operation
 */

#define CMD_NOP               0b00000000     /* no operation */


/*
 *  function set
 */

#define CMD_FUNCTION_SET      0b00100000     /* function set */

  /* power down control */
#define FLAG_DISPLAY_ON       0b00000000     /* display on */
#define FLAG_DISPLAY_OFF      0b00000100     /* display off */
  /* entry mode */
#define FLAG_ENTRY_HOR        0b00000000     /* horizontal addressing */
#define FLAG_ENTRY_VER        0b00000010     /* vertical addressing */
  /* extended instruction set control */
#define FLAG_CMD_NORMAL       0b00000000     /* use normal instruction set */
#define FLAG_CMD_EXTENDED     0b00000001     /* use extended instruction set */


/* ************************************************************************
 *   normal instruction set
 * ************************************************************************ */


/*
 *  display control
 */

#define CMD_DISP_CONTROL      0b00001000     /* display control */

#define FLAG_DISPLAY_BLANK    0b00000000     /* display blank */
#define FLAG_NORMAL_MODE      0b00000100     /* normal mode */
#define FLAG_ALL_SEGMENTS     0b00000001     /* all display segments on */
#define FLAG_INVERSE_MODE     0b00000101     /* inverse video mode */


/*
 *  set Y address of RAM
 *  - bank number
 *  - valid range: 0 - 5
 */

#define CMD_ADDR_Y            0b01000000     /* set Y address */


/*
 *  set X address of RAM
 *  - column number
 *  - valid range: 0 - 83
 */

#define CMD_ADDR_X            0b10000000     /* set X address */



/* ************************************************************************
 *   extended instruction set
 * ************************************************************************ */


/*
 *  temperature control
 */

#define CMD_TEMP_CONTROL      0b00000100     /* set temperature coefficient */

/* V_LCD temperature coefficient */
#define FLAG_TEMP_0           0b00000000     /* coefficient 0 */
#define FLAG_TEMP_1           0b00000001     /* coefficient 1 */
#define FLAG_TEMP_2           0b00000010     /* coefficient 2 */
#define FLAG_TEMP_3           0b00000011     /* coefficient 3 */


/* 
 *  bias system
 *  - 1/(n + 4) 
 */

#define CMD_BIAS_SYSTEM       0b00010000     /* set bias system */

/* n */                                      /* recommendend MUX rate */
#define FLAG_BIAS_7           0b00000000     /* 1:100 */
#define FLAG_BIAS_6           0b00000001     /* 1:80 */
#define FLAG_BIAS_5           0b00000010     /* 1:65 */
#define FLAG_BIAS_4           0b00000011     /* 1:48 */
#define FLAG_BIAS_3           0b00000100     /* 1:40 1:34 */
#define FLAG_BIAS_2           0b00000101     /* 1:24 */
#define FLAG_BIAS_1           0b00000110     /* 1:18 1:16 */
#define FLAG_BIAS_0           0b00000111     /* 1:10 1:9 1:8 */


/*
 *  set V_OP (V_LCD)
 *  - a = 3.06  b = 0.06
 *  - V_LCD = a + (VOP6 to VOP0) × b
 *    -> 3.00 - 10.68 at room temperature
 *  - valid range: 0-127
 */

#define CMD_SET_VOP           0b10000000     /* set V_OP */

#define FLAG_VOP_OFF          0b00000000     /* charge pump disabled */
#define FLAG_VOP_0312         0b00000001     /* 3.12 V */
#define FLAG_VOP_0318         0b00000010     /* 3.18 V */
#define FLAG_VOP_0324         0b00000011     /* 3.24 V */
#define FLAG_VOP_0330         0b00000100     /* 3.30 V */
#define FLAG_VOP_0336         0b00000101     /* 3.36 V */
#define FLAG_VOP_0342         0b00000110     /* 3.42 V */
#define FLAG_VOP_0348         0b00000111     /* 3.48 V */
#define FLAG_VOP_0354         0b00001000     /* 3.54 V */
#define FLAG_VOP_0360         0b00001001     /* 3.60 V */
#define FLAG_VOP_0366         0b00001010     /* 3.66 V */
#define FLAG_VOP_0372         0b00001011     /* 3.72 V */
#define FLAG_VOP_0378         0b00001100     /* 3.78 V */
#define FLAG_VOP_0384         0b00001101     /* 3.84 V */
#define FLAG_VOP_0390         0b00001110     /* 3.90 V */
#define FLAG_VOP_0396         0b00001111     /* 3.96 V */
#define FLAG_VOP_0402         0b00010000     /* 4.02 V */
#define FLAG_VOP_0408         0b00010001     /* 4.08 V */
#define FLAG_VOP_0414         0b00010010     /* 4.14 V */
#define FLAG_VOP_0420         0b00010011     /* 4.20 V */
#define FLAG_VOP_0426         0b00010100     /* 4.26 V */
#define FLAG_VOP_0432         0b00010101     /* 4.32 V */
#define FLAG_VOP_0438         0b00010110     /* 4.38 V */
#define FLAG_VOP_0444         0b00010111     /* 4.44 V */
#define FLAG_VOP_0450         0b00011000     /* 4.50 V */
#define FLAG_VOP_0456         0b00011001     /* 4.56 V */
#define FLAG_VOP_0462         0b00011010     /* 4.62 V */
#define FLAG_VOP_0468         0b00011011     /* 4.68 V */
#define FLAG_VOP_0474         0b00011100     /* 4.74 V */
#define FLAG_VOP_0480         0b00011101     /* 4.80 V */
#define FLAG_VOP_0486         0b00011110     /* 4.86 V */
#define FLAG_VOP_0492         0b00011111     /* 4.92 V */
#define FLAG_VOP_0498         0b00100000     /* 4.98 V */
#define FLAG_VOP_0504         0b00100001     /* 5.04 V */
#define FLAG_VOP_0510         0b00100010     /* 5.10 V */
#define FLAG_VOP_0516         0b00100011     /* 5.16 V */
#define FLAG_VOP_0522         0b00100100     /* 5.22 V */
#define FLAG_VOP_0528         0b00100101     /* 5.28 V */
#define FLAG_VOP_0534         0b00100110     /* 5.34 V */
#define FLAG_VOP_0540         0b00100111     /* 5.40 V */
#define FLAG_VOP_0546         0b00101000     /* 5.46 V */
#define FLAG_VOP_0552         0b00101001     /* 5.52 V */
#define FLAG_VOP_0558         0b00101010     /* 5.58 V */
#define FLAG_VOP_0564         0b00101011     /* 5.64 V */
#define FLAG_VOP_0570         0b00101100     /* 5.70 V */
#define FLAG_VOP_0576         0b00101101     /* 5.76 V */
#define FLAG_VOP_0582         0b00101110     /* 5.82 V */
#define FLAG_VOP_0588         0b00101111     /* 5.88 V */
#define FLAG_VOP_0594         0b00110000     /* 5.94 V */
#define FLAG_VOP_0600         0b00110001     /* 6.00 V */
#define FLAG_VOP_0606         0b00110010     /* 6.06 V */
#define FLAG_VOP_0612         0b00110011     /* 6.12 V */
#define FLAG_VOP_0618         0b00110100     /* 6.18 V */
#define FLAG_VOP_0624         0b00110101     /* 6.24 V */
#define FLAG_VOP_0630         0b00110110     /* 6.30 V */
#define FLAG_VOP_0636         0b00110111     /* 6.36 V */
#define FLAG_VOP_0642         0b00111000     /* 6.42 V */
#define FLAG_VOP_0648         0b00111001     /* 6.48 V */
#define FLAG_VOP_0654         0b00111010     /* 6.54 V */
#define FLAG_VOP_0660         0b00111011     /* 6.60 V */
#define FLAG_VOP_0666         0b00111100     /* 6.66 V */
#define FLAG_VOP_0672         0b00111101     /* 6.72 V */
#define FLAG_VOP_0678         0b00111110     /* 6.78 V */
#define FLAG_VOP_0684         0b00111111     /* 6.84 V */
#define FLAG_VOP_0690         0b01000000     /* 6.90 V */
#define FLAG_VOP_0696         0b01000001     /* 6.96 V */
#define FLAG_VOP_0702         0b01000010     /* 7.02 V */
#define FLAG_VOP_0708         0b01000011     /* 7.08 V */
#define FLAG_VOP_0714         0b01000100     /* 7.14 V */
#define FLAG_VOP_0720         0b01000101     /* 7.20 V */
#define FLAG_VOP_0726         0b01000110     /* 7.26 V */
#define FLAG_VOP_0732         0b01000111     /* 7.30 V */
#define FLAG_VOP_0738         0b01001000     /* 7.38 V */
#define FLAG_VOP_0744         0b01001001     /* 7.44 V */
#define FLAG_VOP_0750         0b01001010     /* 7.50 V */
#define FLAG_VOP_0756         0b01001011     /* 7.56 V */
#define FLAG_VOP_0762         0b01001100     /* 7.62 V */
#define FLAG_VOP_0768         0b01001101     /* 7.68 V */
#define FLAG_VOP_0774         0b01001110     /* 7.74 V */
#define FLAG_VOP_0780         0b01001111     /* 7.80 V */
#define FLAG_VOP_0786         0b01010000     /* 7.86 V */
#define FLAG_VOP_0792         0b01010001     /* 7.92 V */
#define FLAG_VOP_0798         0b01010010     /* 7.98 V */
#define FLAG_VOP_0804         0b01010011     /* 8.04 V */
#define FLAG_VOP_0810         0b01010100     /* 8.10 V */
#define FLAG_VOP_0816         0b01010101     /* 8.16 V */
#define FLAG_VOP_0822         0b01010110     /* 8.22 V */
#define FLAG_VOP_0828         0b01010111     /* 8.28 V */
#define FLAG_VOP_0834         0b01011000     /* 8.34 V */
#define FLAG_VOP_0840         0b01011001     /* 8.40 V */
#define FLAG_VOP_0846         0b01011010     /* 8.46 V */
#define FLAG_VOP_0852         0b01011011     /* 8.52 V */
#define FLAG_VOP_0858         0b01011100     /* 8.58 V */
#define FLAG_VOP_0864         0b01011101     /* 8.64 V */
#define FLAG_VOP_0870         0b01011110     /* 8.70 V */
#define FLAG_VOP_0876         0b01011111     /* 8.76 V */
#define FLAG_VOP_0882         0b01100000     /* 8.82 V */
#define FLAG_VOP_0888         0b01100001     /* 8.88 V */
#define FLAG_VOP_0894         0b01100010     /* 8.94 V */
#define FLAG_VOP_0900         0b01100011     /* 9.00 V */
#define FLAG_VOP_0906         0b01100100     /* 9.06 V */
#define FLAG_VOP_0912         0b01100101     /* 9.12 V */
#define FLAG_VOP_0918         0b01100110     /* 9.18 V */
#define FLAG_VOP_0924         0b01100111     /* 9.24 V */
#define FLAG_VOP_0930         0b01101000     /* 9.30 V */
#define FLAG_VOP_0936         0b01101001     /* 9.36 V */
#define FLAG_VOP_0942         0b01101010     /* 9.42 V */
#define FLAG_VOP_0948         0b01101011     /* 9.48 V */
#define FLAG_VOP_0954         0b01101100     /* 9.54 V */
#define FLAG_VOP_0960         0b01101101     /* 9.60 V */
#define FLAG_VOP_0966         0b01101110     /* 9.66 V */
#define FLAG_VOP_0972         0b01101111     /* 9.72 V */
#define FLAG_VOP_0978         0b01110000     /* 9.78 V */
#define FLAG_VOP_0984         0b01110001     /* 9.84 V */
#define FLAG_VOP_0990         0b01110010     /* 9.90 V */
#define FLAG_VOP_0996         0b01110011     /* 9.96 V */
#define FLAG_VOP_1002         0b01110100     /* 10.02 V */
#define FLAG_VOP_1008         0b01110101     /* 10.08 V */
#define FLAG_VOP_1014         0b01110110     /* 10.14 V */
#define FLAG_VOP_1020         0b01110111     /* 10.20 V */
#define FLAG_VOP_1026         0b01111000     /* 10.26 V */
#define FLAG_VOP_1032         0b01111001     /* 10.32 V */
#define FLAG_VOP_1038         0b01111010     /* 10.38 V */
#define FLAG_VOP_1044         0b01111011     /* 10.44 V */
#define FLAG_VOP_1050         0b01111100     /* 10.50 V */
#define FLAG_VOP_1056         0b01111101     /* 10.56 V */
#define FLAG_VOP_1062         0b01111110     /* 10.62 V */
#define FLAG_VOP_1068         0b01111111     /* 10.68V */


/* ************************************************************************
 *   EOF
 * ************************************************************************ */
