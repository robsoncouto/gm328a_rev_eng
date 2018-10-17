/* ************************************************************************
 *
 *   ADS7843 compatible touchscreen controller
 *
 *   (c) 2015-2017 by Markus Reschke
 *
 * ************************************************************************ */


/* ************************************************************************
 *   constants
 * ************************************************************************ */


/*
 *  control byte
 */

#define FLAG_STARTBIT    0b10000000     /* signal control byte */

/* channel selection */
#define FLAG_CHAN_X      0b00010000     /* input: X+ */
#define FLAG_CHAN_Y      0b01010000     /* input: Y+ */
#define FLAG_CHAN_IN3    0b00100000     /* input: IN3 */
#define FLAG_CHAN_IN4    0b01100000     /* input: IN4 */

/* conversion mode */
#define FLAG_12BITS      0b00000000     /* 12 bits */
#define FLAG_8BITS       0b00001000     /* 8 bits */
#define FLAG_DFR         0b00000000     /* differential reference */
#define FLAG_SER         0b00000100     /* single-ended reference */

/* power down mode */
#define FLAG_PWR_DOWN_1  0b00000000     /* power-down between conversions */
                                        /* and enable PENIRQ */
#define FLAG_PWR_DOWN_2  0b00000001     /* power-down between conversions */
                                        /* and disable PENIRQ */
#define FLAG_PWR_RSRVD   0b00000010     /* reserved for future use */
#define FLAG_PWR_ALWAYS  0b00000011     /* no power-down between conversions */
                                        /* and disable PENIRQ */



/* ************************************************************************
 *   EOF
 * ************************************************************************ */
