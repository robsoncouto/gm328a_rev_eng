/* ************************************************************************
 *
 *   global variables
 *
 *   (c) 2012-2018 by Markus Reschke
 *   based on code from Markus Frejek and Karl-Heinz Kübbeler
 *
 * ************************************************************************ */


/*
 *  include header files
 */

/* local includes */
#ifndef COMMON_H
  #include "common.h"
#endif

#ifndef CONFIG_H
  #include "config.h"
#endif

#ifndef COLORS_H
  #include "colors.h"
#endif



/* ************************************************************************
 *   internal variables
 * ************************************************************************ */


/*
 *  only included by
 *  - main.c
 */

#if defined (MAIN_C)


  /*
   *  global variables
   */

  /* buffers */
  char              OutBuffer[OUT_BUFFER_SIZE];   /* output buffer */
  #ifdef SERIAL_RW
  char              RX_Buffer[RX_BUFFER_SIZE];    /* serial RX buffer */
  uint8_t           RX_Pos = 0;                   /* position in buffer */
    #ifdef SERIAL_BITBANG
    uint8_t         RX_Char;                 /* RX char (bit buffer) */
    uint8_t         RX_Bits;                 /* bit counter for RX char */
    #endif
  #endif

  /* configuration */
  UI_Type           UI;                      /* user interface */
  Config_Type       Cfg;                     /* tester modes, offsets and values */
  Adjust_Type       NV;                      /* basic adjustment offsets and values */

  #ifdef HW_TOUCH
  Touch_Type        Touch;                   /* touch screen adjustment offsets */
  #endif

  /* probing */
  Probe_Type        Probes;                  /* test probes */
  Check_Type        Check;                   /* checking/testing */

  /* components */
  Resistor_Type     Resistors[3];            /* resistors */
  Capacitor_Type    Caps[3];                 /* capacitors */
  Diode_Type        Diodes[6];               /* diodes */
  Semi_Type         Semi;                    /* common semiconductor */
  AltSemi_Type      AltSemi;                 /* special semiconductor */

  #ifdef SW_INDUCTOR
  Inductor_Type     Inductor;                /* inductor */
  #endif

  #ifdef UI_SERIAL_COMMANDS
  Info_Type         Info;                    /* additional component data */
  #endif

  #ifdef SW_PROBE_COLORS
  /* probe color coding */
  uint16_t          ProbeColors[3] = {COLOR_PROBE_1, COLOR_PROBE_2, COLOR_PROBE_3};
  #endif

  #ifdef HW_SPI
  SPI_Type          SPI;                     /* SPI */
  #endif

  #ifdef HW_I2C
  I2C_Type          I2C;                     /* I2C */
  #endif


  /*
   *  NVRAM values (stored in EEPROM) with their defaults
   */

  /* basic adjustment values: profile #1 */
  const Adjust_Type     NV_Adjust_1 EEMEM = {R_MCU_LOW, R_MCU_HIGH, R_ZERO, C_ZERO, UREF_OFFSET, COMPARATOR_OFFSET, LCD_CONTRAST, 0};

  /* basic adjustment values: profile #2 */
  const Adjust_Type     NV_Adjust_2 EEMEM = {R_MCU_LOW, R_MCU_HIGH, R_ZERO, C_ZERO, UREF_OFFSET, COMPARATOR_OFFSET, LCD_CONTRAST, 0};

  #ifdef HW_TOUCH
  /* touch screen adjustment offsets */
  const Touch_Type      NV_Touch EEMEM = {0, 0, 0, 0, 0};
  #endif


  /*
   *  constant strings (stored in EEPROM)
   */

  /* language specific */
  #include "var_czech.h"
  #include "var_danish.h"
  #include "var_english.h"
  #include "var_german.h"
  #include "var_italian.h"
  #include "var_polish.h"
  #include "var_spanish.h"
  #include "var_russian.h"

  /* language independent */
  const unsigned char Tester_str[] EEMEM = "Component Tester";
  const unsigned char Version_str[] EEMEM = "v1.34m";
  const unsigned char MOS_str[] EEMEM = "MOS";
  const unsigned char FET_str[] EEMEM = "FET";
  const unsigned char Channel_str[] EEMEM = "-ch";
  const unsigned char Enhancement_str[] EEMEM = "enh.";
  const unsigned char Depletion_str[] EEMEM = "dep.";
  const unsigned char IGBT_str[] EEMEM = "IGBT";
  const unsigned char Cgs_str[] EEMEM = "Cgs";
  const unsigned char NPN_str[] EEMEM = "NPN";
  const unsigned char PNP_str[] EEMEM = "PNP";
  const unsigned char h_FE_str[] EEMEM ="hFE";
  const unsigned char V_BE_str[] EEMEM ="Vbe";
  const unsigned char V_GT_str[] EEMEM ="V_GT";
  const unsigned char I_CEO_str[] EEMEM = "Iceo";
  const unsigned char Vf_str[] EEMEM = "Vf";
  const unsigned char DiodeCap_str[] EEMEM = "C";
  const unsigned char Vth_str[] EEMEM = "Vth";
  const unsigned char I_R_str[] EEMEM = "I_R";
  const unsigned char V_T_str[] EEMEM = "VT";
  const unsigned char URef_str[] EEMEM = "Vref";
  const unsigned char RhLow_str[] EEMEM = "Rh-";
  const unsigned char RhHigh_str[] EEMEM = "Rh+";
  const unsigned char RiLow_str[] EEMEM = "Ri-";
  const unsigned char RiHigh_str[] EEMEM = "Ri+";
  const unsigned char Rl_str[] EEMEM = "+Rl-";
  const unsigned char Rh_str[] EEMEM = "+Rh-";
  const unsigned char ProbeComb_str[] EEMEM = "12 13 23";
  const unsigned char CapOffset_str[] EEMEM = "C0";
  const unsigned char ROffset_str[] EEMEM = "R0";
  const unsigned char Vcc_str[] EEMEM = "Vcc";
  const unsigned char CompOffset_str[] EEMEM = "AComp";
  const unsigned char Profile1_str[] EEMEM = "#1";
  const unsigned char Profile2_str[] EEMEM = "#2";
  const unsigned char I_DSS_str[] EEMEM = "Idss";
  const unsigned char I_leak_str[] EEMEM = "I_l";
  const unsigned char R_DS_str[] EEMEM = "Rds";

  #ifndef BAT_NONE
    const unsigned char Battery_str[] EEMEM = "Bat";
    const unsigned char OK_str[] EEMEM = "ok";
  #endif
  #ifdef BAT_EXT_UNMONITORED
    const unsigned char External_str[] EEMEM = "ext";
  #endif

  /* options */
  #if defined (SW_ESR) || defined (SW_OLD_ESR)
    const unsigned char ESR_str[] EEMEM = "ESR";
  #endif
  #if defined (SW_PWM_SIMPLE) || defined (SW_PWM_PLUS)
    const unsigned char PWM_str[] EEMEM = "PWM";
  #endif
  #if defined (SW_PWM_SIMPLE) || defined (SW_PWM_PLUS) || defined (HW_FREQ_COUNTER_EXT)
    const unsigned char Hertz_str[] EEMEM = "Hz";
  #endif
  #if defined (SW_IR_RECEIVER) || defined (HW_IR_RECEIVER)
    const unsigned char IR_NEC_str[] EEMEM = "NEC";
    const unsigned char IR_SIRC_str[] EEMEM = "SIRC";
    #ifdef SW_IR_EXTRA
    const unsigned char IR_IR60_str[] EEMEM = "IR60";
    const unsigned char IR_RCA_str[] EEMEM = "RCA";
    const unsigned char IR_RECS80_str[] EEMEM = "RECS80";
    const unsigned char IR_Sanyo_str[] EEMEM = "Sanyo";
    const unsigned char IR_uPD1986C_str[] EEMEM = "µPD1986C";
    #endif
  #endif
  #if defined (SW_IR_RECEIVER) || defined (HW_IR_RECEIVER) || defined (SW_IR_TRANSMITTER)
    const unsigned char IR_JVC_str[] EEMEM = "JVC";
    const unsigned char IR_Kaseikyo_str[] EEMEM = "Kas";
    const unsigned char IR_Matsushita_str[] EEMEM = "Mat";
    const unsigned char IR_Motorola_str[] EEMEM = "Mot";
    const unsigned char IR_Proton_str[] EEMEM = "Prot";
    const unsigned char IR_RC5_str[] EEMEM = "RC-5";
    const unsigned char IR_RC6_str[] EEMEM = "RC-6";
    const unsigned char IR_Samsung_str[] EEMEM = "Sams";
    const unsigned char IR_Sharp_str[] EEMEM = "Sharp";
    #ifdef SW_IR_EXTRA
    const unsigned char IR_Thomson_str[] EEMEM = "Thom";
    #endif
  #endif
  #ifdef SW_IR_TRANSMITTER
    const unsigned char IR_NEC_Std_str[] EEMEM = "NEC Std";
    const unsigned char IR_NEC_Ext_str[] EEMEM = "NEC Ext";
    const unsigned char IR_SIRC_12_str[] EEMEM = "SIRC-12";
    const unsigned char IR_SIRC_15_str[] EEMEM = "SIRC-15";
    const unsigned char IR_SIRC_20_str[] EEMEM = "SIRC-20";
  #endif
  #ifdef SW_OPTO_COUPLER
    const unsigned char CTR_str[] EEMEM = "CTR";
    const unsigned char If_str[] EEMEM = "If";
    const unsigned char t_on_str[] EEMEM = "t_on";
    const unsigned char t_off_str[] EEMEM = "t_off";
  #endif
  #ifdef SW_UJT
    const unsigned char R_BB_str[] EEMEM = "R_BB";
  #endif
  #ifdef SW_DS18B20
    const unsigned char DS18B20_str[] EEMEM = "DS18B20";
  #endif

  /* component symbols */
  const unsigned char Cap_str[] EEMEM = {'-', LCD_CHAR_CAP, '-',0};
  const unsigned char Diode_AC_str[] EEMEM = {'-', LCD_CHAR_DIODE_AC, '-', 0};
  const unsigned char Diode_CA_str[] EEMEM = {'-', LCD_CHAR_DIODE_CA, '-', 0};
  const unsigned char Resistor_str[] EEMEM = {'-', LCD_CHAR_RESISTOR_L, LCD_CHAR_RESISTOR_R, '-', 0};

  /* remote commands */
  #ifdef UI_SERIAL_COMMANDS
    /* feedback */ 
    const unsigned char Cmd_ERR_str[] EEMEM = "ERR";
    const unsigned char Cmd_OK_str[] EEMEM = "OK";
    const unsigned char Cmd_NA_str[] EEMEM = "N/A";
    const unsigned char Cmd_R_be_str[] EEMEM = "R_BE";
    const unsigned char Cmd_D_fb_str[] EEMEM = "D_FB";
    const unsigned char Cmd_BJT_str[] EEMEM = "BJT";
    const unsigned char Cmd_SYM_str[] EEMEM = "SYM";

    /* commands */
    const unsigned char Cmd_VER_str[] EEMEM = "VER";
    const unsigned char Cmd_PROBE_str[] EEMEM = "PROBE";
    const unsigned char Cmd_OFF_str[] EEMEM = "OFF";
    const unsigned char Cmd_COMP_str[] EEMEM = "COMP";
    const unsigned char Cmd_MSG_str[] EEMEM = "MSG";
    const unsigned char Cmd_QTY_str[] EEMEM = "QTY";
    const unsigned char Cmd_NEXT_str[] EEMEM = "NEXT";
    const unsigned char Cmd_TYPE_str[] EEMEM = "TYPE";
    const unsigned char Cmd_HINT_str[] EEMEM = "HINT";
    const unsigned char Cmd_PIN_str[] EEMEM = "PIN";
    const unsigned char Cmd_R_str[] EEMEM = "R";
    const unsigned char Cmd_C_str[] EEMEM = "C";
    #ifdef SW_INDUCTOR
    const unsigned char Cmd_L_str[] EEMEM = "L";
    #endif
    #if defined (SW_ESR) || defined (SW_OLD_ESR)
    const unsigned char Cmd_ESR_str[] EEMEM = "ESR";
    #endif
    const unsigned char Cmd_V_F_str[] EEMEM = "V_F";
    const unsigned char Cmd_V_F2_str[] EEMEM = "V_F2";
    const unsigned char Cmd_C_D_str[] EEMEM = "C_D";
    const unsigned char Cmd_R_BE_str[] EEMEM = "R_BE";
    const unsigned char Cmd_h_FE_str[] EEMEM = "h_FE";
    const unsigned char Cmd_V_BE_str[] EEMEM = "V_BE";
    const unsigned char Cmd_I_CEO_str[] EEMEM = "I_CEO";
    const unsigned char Cmd_V_TH_str[] EEMEM = "V_th";
    const unsigned char Cmd_C_GS_str[] EEMEM = "C_GS";
    const unsigned char Cmd_R_DS_str[] EEMEM = "R_DS";
    const unsigned char Cmd_I_DSS_str[] EEMEM = "I_DSS";
    const unsigned char Cmd_C_GE_str[] EEMEM = "C_GE";
    const unsigned char Cmd_V_T_str[] EEMEM = "V_T";

    /* command reference table */
    const Cmd_Type Cmd_Table[] EEMEM = {
      {CMD_VER, Cmd_VER_str},
      {CMD_PROBE, Cmd_PROBE_str},
      {CMD_OFF, Cmd_OFF_str},
      {CMD_COMP, Cmd_COMP_str},
      {CMD_MSG, Cmd_MSG_str},
      {CMD_QTY, Cmd_QTY_str},
      {CMD_NEXT, Cmd_NEXT_str},
      {CMD_TYPE, Cmd_TYPE_str},
      {CMD_HINT, Cmd_HINT_str},
      {CMD_PIN, Cmd_PIN_str},
      {CMD_R, Cmd_R_str},
      {CMD_C, Cmd_C_str},
      #ifdef SW_INDUCTOR
      {CMD_L, Cmd_L_str},
      #endif
      #if defined (SW_ESR) || defined (SW_OLD_ESR)
      {CMD_ESR, Cmd_ESR_str},
      #endif
      {CMD_I_L, I_leak_str},
      {CMD_V_F, Cmd_V_F_str},
      {CMD_V_F2, Cmd_V_F2_str},
      {CMD_C_D, Cmd_C_D_str},
      {CMD_I_R, I_R_str},
      {CMD_R_BE, Cmd_R_BE_str},
      {CMD_H_FE, Cmd_h_FE_str},
      {CMD_V_BE, Cmd_V_BE_str},
      {CMD_I_CEO, Cmd_I_CEO_str},
      {CMD_V_TH, Cmd_V_TH_str},
      {CMD_C_GS, Cmd_C_GS_str},
      {CMD_R_DS, Cmd_R_DS_str},
      {CMD_I_DSS, Cmd_I_DSS_str},
      {CMD_C_GE, Cmd_C_GE_str},
      {CMD_V_GT, V_GT_str},
      {CMD_V_T, Cmd_V_T_str},
      #ifdef SW_UJT
      {CMD_R_BB, R_BB_str},
      #endif
      {0, 0}
    };
  #endif


  /*
   *  constant tables (stored in EEPROM)
   */

  /* unit prefixes: p, n, µ, m, 0, k, M (used by value display) */
  const unsigned char Prefix_table[NUM_PREFIXES] EEMEM = {'p', 'n', LCD_CHAR_MICRO, 'm', 0, 'k', 'M'};

  /* voltage based factors for large caps (using Rl) */
  /* voltage in mV:                                       300    325    350    375    400    425    450    475    500    525    550    575    600    625    650   675   700   725   750   775   800   825   850   875   900   925   950   975  1000  1025  1050  1075  1100  1125  1150  1175  1200  1225  1250  1275  1300  1325  1350  1375  1400 */
  const uint16_t LargeCap_table[NUM_LARGE_CAP] EEMEM = {23022, 21195, 19629, 18272, 17084, 16036, 15104, 14271, 13520, 12841, 12224, 11660, 11143, 10668, 10229, 9822, 9445, 9093, 8765, 8458, 8170, 7900, 7645, 7405, 7178, 6963, 6760, 6567, 6384, 6209, 6043, 5885, 5733, 5589, 5450, 5318, 5191, 5069, 4952, 4839, 4731, 4627, 4526, 4430, 4336};

  /* voltage based factors for small caps (using Rh) */
  /* voltages in mV:                                   1000 1050 1100 1150 1200 1250 1300 1350 1400 */
  const uint16_t SmallCap_table[NUM_SMALL_CAP] EEMEM = {954, 903, 856, 814, 775, 740, 707, 676, 648};
//const uint16_t SmallCap_table[NUM_SMALL_CAP] EEMEM = {9535, 9026, 8563, 8141, 7753, 7396, 7066, 6761, 6477}; 

  #ifdef SW_PWM_SIMPLE
  /* PWM menu: frequencies */    
  const uint16_t PWM_Freq_table[NUM_PWM_FREQ] EEMEM = {100, 250, 500, 1000, 2500, 5000, 10000, 25000};
  #endif

  #ifdef SW_INDUCTOR
  /* ratio based factors for inductors */
  /* ratio:                                             200   225   250   275   300   325   350   375   400   425   450   475   500   525   550   575   600   625  650  675  700  725  750  775  800  825  850  875  900  925  950  975 */
  const uint16_t Inductor_table[NUM_INDUCTOR] EEMEM = {4481, 3923, 3476, 3110, 2804, 2544, 2321, 2128, 1958, 1807, 1673, 1552, 1443, 1343, 1252, 1169, 1091, 1020, 953, 890, 831, 775, 721, 670, 621, 574, 527, 481, 434, 386, 334, 271};
  #endif

  #if defined (HW_FREQ_COUNTER) || defined (SW_SQUAREWAVE)
  /* Timer1 prescalers and corresponding bitmasks */
  const uint16_t T1_Prescaler_table[NUM_TIMER1] EEMEM = {1, 8, 64, 256, 1024};
  const uint8_t T1_Bitmask_table[NUM_TIMER1] EEMEM = {(1 << CS10), (1 << CS11), (1 << CS11) | (1 << CS10), (1 << CS12), (1 << CS12) | (1 << CS10)};
  #endif


  /*
   *  bitmask tables for probe settings (stored in EEPROM)
   *  - this saves some bytes in the firmware
   */

  /* bitmasks for Rl probe resistors based on probe ID */
  const unsigned char Rl_table[] EEMEM = {(1 << R_RL_1), (1 << R_RL_2), (1 << R_RL_3)};

  /* bitmasks for Rh probe resistors based on probe ID */
  const unsigned char Rh_table[] EEMEM = {(1 << R_RH_1), (1 << R_RH_2), (1 << R_RH_3)};

  /* bitmasks for pins (ADC port) based on probe ID */
  const unsigned char Pin_table[] EEMEM = {(1 << TP1), (1 << TP2), (1 << TP3)};

  /* bitmasks for ADC MUX input addresses based on probe ID */
  const unsigned char ADC_table[] EEMEM = {TP1, TP2, TP3};



/* ************************************************************************
 *   external variables
 * ************************************************************************ */


/*
 *  included by all other source files
 */

#else

  /*
   *  global variables
   */

  /* buffers */
  extern char            OutBuffer[];        /* output buffer */
  #ifdef SERIAL_RW
  extern char            RX_Buffer[];        /* serial RX buffer */
  extern uint8_t         RX_Pos;             /* position in buffer */
    #ifdef SERIAL_BITBANG
    extern uint8_t       RX_Char;            /* RX char (bit buffer) */
    extern uint8_t       RX_Bits;            /* bit counter for RX char */
    #endif
  #endif

  /* configuration */
  extern UI_Type         UI;                 /* user interface */
  extern Config_Type     Cfg;                /* offsets and values */
  extern Adjust_Type     NV;                 /* basic adjustment offsets and values */

  #ifdef HW_TOUCH
  extern Touch_Type      Touch;              /* touch screen adjustment offsets */
  #endif

  /* probing */
  extern Probe_Type      Probes;             /* test probes */
  extern Check_Type      Check;              /* checking/testing */

  /* components */
  extern Resistor_Type   Resistors[];        /* resistors */
  extern Capacitor_Type  Caps[];             /* capacitors */
  extern Diode_Type      Diodes[];           /* diodes */
  extern Semi_Type       Semi;               /* common semiconductor */
  extern AltSemi_Type    AltSemi;            /* special semiconductor */

  #ifdef SW_INDUCTOR
  extern Inductor_Type   Inductor;           /* inductor */
  #endif

  #ifdef UI_SERIAL_COMMANDS
  extern Info_Type       Info;               /* additional component data */
  #endif

  #ifdef SW_PROBE_COLORS
  extern uint16_t        ProbeColors[];      /* probe color coding */
  #endif

  #ifdef HW_SPI
  extern SPI_Type        SPI;                /* SPI */
  #endif

  #ifdef HW_I2C
  extern I2C_Type        I2C;                /* I2C */
  #endif


  /*
   *  NVRAM values (stored in EEPROM) with their defaults
   */

  /* basic adjustment values: profile #1 */
  extern const Adjust_Type    NV_Adjust_1;

  /* basic adjustment values: profile #2 */
  extern const Adjust_Type    NV_Adjust_2;

  #ifdef HW_TOUCH
  /* touch screen adjustment offsets */
  extern const Touch_Type     NV_Touch;
  #endif


  /*
   *  constant strings (stored in EEPROM)
   */

  extern const unsigned char Version_str[];
  extern const unsigned char Probing_str[];
  extern const unsigned char Done_str[];
  extern const unsigned char Select_str[];
  extern const unsigned char Selftest_str[];
  extern const unsigned char Adjustment_str[];
  extern const unsigned char Save_str[];
  extern const unsigned char Load_str[];
  extern const unsigned char Show_str[];
  extern const unsigned char Remove_str[];
  extern const unsigned char Create_str[];
  extern const unsigned char ShortCircuit_str[];
  extern const unsigned char DischargeFailed_str[];
  extern const unsigned char Error_str[];
  extern const unsigned char Exit_str[];
  extern const unsigned char BJT_str[];
  extern const unsigned char Triac_str[];
  extern const unsigned char PUT_str[];

  extern const unsigned char MOS_str[];
  extern const unsigned char FET_str[];
  extern const unsigned char Channel_str[];
  extern const unsigned char Enhancement_str[];
  extern const unsigned char Depletion_str[];
  extern const unsigned char Cgs_str[];
  extern const unsigned char NPN_str[];
  extern const unsigned char PNP_str[];
  extern const unsigned char h_FE_str[];
  extern const unsigned char V_BE_str[];
  extern const unsigned char I_CEO_str[];
  extern const unsigned char Vf_str[];
  extern const unsigned char Vth_str[];
  extern const unsigned char URef_str[];
  extern const unsigned char RhLow_str[];
  extern const unsigned char RhHigh_str[];
  extern const unsigned char RiLow_str[];
  extern const unsigned char RiHigh_str[];
  extern const unsigned char Rl_str[];
  extern const unsigned char Rh_str[];
  extern const unsigned char ProbeComb_str[];
  extern const unsigned char CapOffset_str[];
  extern const unsigned char ROffset_str[];
  extern const unsigned char Vcc_str[];
  extern const unsigned char CompOffset_str[];
  extern const unsigned char Checksum_str[];
  extern const unsigned char Profile1_str[];
  extern const unsigned char Profile2_str[];


  #ifdef BAT_EXT_UNMONITORED
    extern const unsigned char External_str[];
  #endif

  /* options */
  #if defined (SW_ESR) || defined (SW_OLD_ESR)
    extern const unsigned char ESR_str[];
  #endif
  #if defined (SW_PWM_SIMPLE) || defined (SW_PWM_PLUS)
    extern const unsigned char PWM_str[];    
  #endif
  #if defined (SW_PWM_SIMPLE) || defined (SW_PWM_PLUS) || defined (HW_FREQ_COUNTER_EXT)
    extern const unsigned char Hertz_str[];
  #endif
  #ifdef SW_SQUAREWAVE
    extern const unsigned char SquareWave_str[];
  #endif
  #ifdef HW_ZENER
    extern const unsigned char Zener_str[];
    extern const unsigned char Min_str[];
  #endif
  #ifdef HW_FREQ_COUNTER
    extern const unsigned char FreqCounter_str[];
  #endif
  #ifdef HW_FREQ_COUNTER_EXT
    extern const unsigned char CounterChannel_str[];
    extern const unsigned char FreqInput_str[];
    extern const unsigned char LF_Crystal_str[];
    extern const unsigned char HF_Crystal_str[];
  #endif
  #ifdef SW_ENCODER
    extern const unsigned char Encoder_str[];
    extern const unsigned char TurnRight_str[];
  #endif
  #ifdef SW_CONTRAST
    extern const unsigned char Contrast_str[];
  #endif
  #if defined (SW_IR_RECEIVER) || defined (HW_IR_RECEIVER)
    extern const unsigned char IR_Detector_str[];
    extern const unsigned char IR_NEC_str[];
    extern const unsigned char IR_SIRC_str[];
    #ifdef SW_IR_EXTRA
    extern const unsigned char IR_IR60_str[];
    extern const unsigned char IR_RCA_str[];
    extern const unsigned char IR_RECS80_str[];
    extern const unsigned char IR_Sanyo_str[];
    extern const unsigned char IR_uPD1986C_str[];
    #endif
  #endif
  #if defined (SW_IR_RECEIVER) || defined (HW_IR_RECEIVER) || defined (SW_IR_TRANSMITTER)
    extern const unsigned char IR_JVC_str[];
    extern const unsigned char IR_Kaseikyo_str[];
    extern const unsigned char IR_Matsushita_str[];
    extern const unsigned char IR_Motorola_str[];
    extern const unsigned char IR_Proton_str[];
    extern const unsigned char IR_RC5_str[];
    extern const unsigned char IR_RC6_str[];
    extern const unsigned char IR_Samsung_str[];
    extern const unsigned char IR_Sharp_str[];
    #ifdef SW_IR_EXTRA
    extern const unsigned char IR_Thomson_str[];
    #endif
  #endif
  #ifdef SW_IR_TRANSMITTER
    extern const unsigned char IR_Transmitter_str[];
    extern const unsigned char IR_Send_str[];
    extern const unsigned char IR_NEC_Std_str[];
    extern const unsigned char IR_NEC_Ext_str[];
    extern const unsigned char IR_SIRC_12_str[];
    extern const unsigned char IR_SIRC_15_str[];
    extern const unsigned char IR_SIRC_20_str[];
  #endif
  #ifdef SW_OPTO_COUPLER
    extern const unsigned char OptoCoupler_str[];
    extern const unsigned char None_str[];
    extern const unsigned char CTR_str[];
    extern const unsigned char If_str[];
    extern const unsigned char t_on_str[];
    extern const unsigned char t_off_str[];
  #endif
  #if defined (SW_OPTO_COUPLER) || defined (SW_DS18B20)
    extern const unsigned char Start_str[];
  #endif
  #ifdef SW_UJT
    extern const unsigned char UJT_str[];
    extern const unsigned char R_BB_str[];
  #endif
  #ifdef SW_SERVO
    extern const unsigned char Servo_str[];
    extern const unsigned char Sweep_str[];
  #endif
  #ifdef HW_TOUCH
    extern const unsigned char TouchSetup_str[];
  #endif
  #ifdef SW_DS18B20
    extern const unsigned char DS18B20_str[];
  #endif
  #ifdef SW_CAP_LEAKAGE
    extern const unsigned char CapLeak_str[];
    extern const unsigned char CapCharge_str[];
    extern const unsigned char CapHigh_str[];
    extern const unsigned char CapLow_str[];
    extern const unsigned char CapDischarge_str[];
  #endif


  /* remote commands */
  #ifdef UI_SERIAL_COMMANDS
    /* feedback */
    extern const unsigned char Cmd_ERR_str[];
    extern const unsigned char Cmd_OK_str[];
    extern const unsigned char Cmd_NA_str[];
    extern const unsigned char Cmd_R_be_str[];
    extern const unsigned char Cmd_D_fb_str[];
    extern const unsigned char Cmd_BJT_str[];
    extern const unsigned char Cmd_SYM_str[];

    /* commands */
    extern const unsigned char Cmd_VER_str[];
    extern const unsigned char Cmd_PROBE_str[];
    extern const unsigned char Cmd_OFF_str[];
    extern const unsigned char Cmd_COMP_str[];
    extern const unsigned char Cmd_MSG_str[];
    extern const unsigned char Cmd_QTY_str[];
    extern const unsigned char Cmd_NEXT_str[];
    extern const unsigned char Cmd_TYPE_str[];
    extern const unsigned char Cmd_HINT_str[];
    extern const unsigned char Cmd_PIN_str[];
    extern const unsigned char Cmd_R_str[];
    extern const unsigned char Cmd_C_str[];
    #ifdef SW_INDUCTOR
    extern const unsigned char Cmd_L_str[];
    #endif
    #if defined (SW_ESR) || defined (SW_OLD_ESR)
    extern const unsigned char Cmd_ESR_str[];
    #endif
    extern const unsigned char Cmd_V_F_str[];
    extern const unsigned char Cmd_V_F2_str[];
    extern const unsigned char Cmd_C_D_str[];
    extern const unsigned char Cmd_R_BE_str[];
    extern const unsigned char Cmd_h_FE_str[];
    extern const unsigned char Cmd_V_BE_str[];
    extern const unsigned char Cmd_I_CEO_str[];
    extern const unsigned char Cmd_V_TH_str[];
    extern const unsigned char Cmd_C_GS_str[];
    extern const unsigned char Cmd_R_DS_str[];
    extern const unsigned char Cmd_I_DSS_str[];
    extern const unsigned char Cmd_C_GE_str[];
    extern const unsigned char Cmd_V_T_str[];

    /* command reference table */
    extern const Cmd_Type Cmd_Table[];
  #endif



  /*
   *  constant tables (stored in EEPROM)
   */

  /* unit prefixes: p, n, µ, m, 0, k, M (used by value display) */
  extern const unsigned char Prefix_table[];

  /* voltage based factors for large caps (using Rl) */
  extern const uint16_t LargeCap_table[];

  /* voltage based factors for small caps (using Rh) */
  extern const uint16_t SmallCap_table[];

  #ifdef SW_PWM_SIMPLE
  /* PWM menu: frequencies */
  extern const uint16_t PWM_Freq_table[];
  #endif

  #ifdef SW_INDUCTOR
  /* voltage based factors for inductors */
  extern const uint16_t Inductor_table[];
  #endif

  #if defined (HW_FREQ_COUNTER) || defined (SW_SQUAREWAVE)
  /* Timer1 prescalers and corresponding bitmasks */
  extern const uint16_t T1_Prescaler_table[];
  extern const uint8_t T1_Bitmask_table[];
  #endif


  /*
   *  bitmask tables for probe settings
   *  - they save some bytes in the firmware
   */

  /* bitmasks for Rl probe resistors based on probe ID */
  extern const unsigned char Rl_table[];

  /* bitmasks for Rh probe resistors based on probe ID */
  extern const unsigned char Rh_table[];

  /* bitmasks for pins (ADC port) based on probe ID */
  extern const unsigned char Pin_table[];

  /* bitmasks for ADC MUX input addresses based on probe ID */
  extern const unsigned char ADC_table[];

#endif



/* ************************************************************************
 *   EOF
 * ************************************************************************ */
