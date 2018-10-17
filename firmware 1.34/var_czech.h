/* ************************************************************************
 *
 *   language specific global variables: Czech (ISO 8859-1)
 *
 *   (c) 2015-2018 by Markus Reschke
 *   translation by Kapa
 *   based on code from Markus Frejek and Karl-Heinz Kübbeler
 *
 * ************************************************************************ */


/*
 *  Czech
 */

#if defined (UI_CZECH)

  /*
   *  constant strings (stored in EEPROM)
   */

  const unsigned char Probing_str[] EEMEM = "Probiha mereni..";
  const unsigned char Timeout_str[] EEMEM = "Vyprsel cas";
  const unsigned char Failed1_str[] EEMEM = "Zadna soucastka";
  const unsigned char Failed2_str[] EEMEM = "neznama - vadna";
  const unsigned char Done_str[] EEMEM = "hotovo!";
  const unsigned char Select_str[] EEMEM = "Vyber";
  const unsigned char Selftest_str[] EEMEM = "Autotest";
  const unsigned char Adjustment_str[] EEMEM = "Kalibrace";
  const unsigned char Save_str[] EEMEM = "Ulozit";
  const unsigned char Load_str[] EEMEM = "Nacist";
  const unsigned char Show_str[] EEMEM = "Zobraz hodnoty";
  const unsigned char Remove_str[] EEMEM = "Odstranit";
  const unsigned char Create_str[] EEMEM = "Udelej";
  const unsigned char ShortCircuit_str[] EEMEM = "zkrat!";
  const unsigned char DischargeFailed_str[] EEMEM = "Baterie?";
  const unsigned char Error_str[] EEMEM = "Chyba!";
  const unsigned char Exit_str[] EEMEM = "Exit";
  const unsigned char Checksum_str[] EEMEM = "Kontrolni soucet";
  const unsigned char BJT_str[] EEMEM = "Tranzistor";
  const unsigned char Thyristor_str[] EEMEM = "Tyristor";
  const unsigned char Triac_str[] EEMEM = "Triak";
  const unsigned char PUT_str[] EEMEM = "PUT";
  const unsigned char Bye_str[] EEMEM = "Nashledanou...";

  #ifndef BAT_NONE
    const unsigned char Weak_str[] EEMEM = "slaba!";
    const unsigned char Low_str[] EEMEM =  "vybita!";
  #endif

  #ifdef SW_SQUAREWAVE
    const unsigned char SquareWave_str[] EEMEM = "Gen. obdelniku";
  #endif

  #ifdef HW_ZENER
    const unsigned char Zener_str[] EEMEM = "Zenerka";
    const unsigned char Min_str[] EEMEM = "Min";
  #endif

  #ifdef HW_FREQ_COUNTER
    const unsigned char FreqCounter_str[] EEMEM = "Citac";
  #endif

  #ifdef HW_FREQ_COUNTER_EXT
    const unsigned char CounterChannel_str[] EEMEM = "Ch";
    const unsigned char FreqInput_str[] EEMEM = "BNC";
    const unsigned char LF_Crystal_str[] EEMEM = "LF crystal";
    const unsigned char HF_Crystal_str[] EEMEM = "HF crystal";
  #endif

  #ifdef SW_ENCODER
    const unsigned char Encoder_str[] EEMEM = "IRC snimac";
    const unsigned char TurnRight_str[] EEMEM = "Otoc vpravo!";
  #endif

  #ifdef SW_CONTRAST
    const unsigned char Contrast_str[] EEMEM = "Kontrast";
  #endif

  #if defined (SW_IR_RECEIVER) || defined (HW_IR_RECEIVER)
    const unsigned char IR_Detector_str[] EEMEM = "IR detektor";
  #endif

  #ifdef SW_IR_TRANSMITTER
    const unsigned char IR_Transmitter_str[] EEMEM = "IR RC";
    const unsigned char IR_Send_str[] EEMEM = "vysilam...";
  #endif

  #ifdef SW_OPTO_COUPLER
    const unsigned char OptoCoupler_str[] EEMEM = "Optron";
    const unsigned char None_str[] EEMEM = "zadny";
  #endif

  #if defined (SW_OPTO_COUPLER) || defined (SW_DS18B20)
    const unsigned char Start_str[] EEMEM = "Start";
  #endif

  #ifdef SW_UJT
    const unsigned char UJT_str[] EEMEM = "UJT";
  #endif

  #ifdef SW_SERVO
    const unsigned char Servo_str[] EEMEM = "Servo";
    const unsigned char Sweep_str[] EEMEM = "<->";
  #endif

  #ifdef SW_CAP_LEAKAGE
    const unsigned char CapLeak_str[] EEMEM = "Cap Leakage";
    const unsigned char CapCharge_str[] EEMEM = "Charging";
    const unsigned char CapHigh_str[] EEMEM = "Rl";
    const unsigned char CapLow_str[] EEMEM = "Rh";
    const unsigned char CapDischarge_str[] EEMEM = "Discharging";
  #endif

  #ifdef HW_TOUCH
    const unsigned char TouchSetup_str[] EEMEM = "Touch Setup";
  #endif

#endif


/* ************************************************************************
 *   EOF
 * ************************************************************************ */
