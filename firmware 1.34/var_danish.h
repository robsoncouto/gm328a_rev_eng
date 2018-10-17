/* ************************************************************************
 *
 *   language specific global variables: Danish (ISO 8859-1)
 *
 *   (c) 2018 by Markus Reschke
 *   translation by glenndk@mikrocontroller.net
 *   based on code from Markus Frejek and Karl-Heinz K¸bbeler
 *
 * ************************************************************************ */


/*
 *  Danish
 */

#if defined (UI_DANISH)

  /*
   *  constant strings (stored in EEPROM)
   */

  const unsigned char Probing_str[] EEMEM = "Søger...";
  const unsigned char Timeout_str[] EEMEM = "Timeout";
  const unsigned char Failed1_str[] EEMEM = "Ingen komponent";
  const unsigned char Failed2_str[] EEMEM = "fundet!";
  const unsigned char Done_str[] EEMEM = "færdig!";
  const unsigned char Select_str[] EEMEM = "Vælg";
  const unsigned char Selftest_str[] EEMEM = "Test";
  const unsigned char Adjustment_str[] EEMEM = "Justering";
  const unsigned char Save_str[] EEMEM = "Gem";
  const unsigned char Load_str[] EEMEM = "Hent";
  const unsigned char Show_str[] EEMEM = "Vis værdier";
  const unsigned char Remove_str[] EEMEM = "Fjern";
  const unsigned char Create_str[] EEMEM = "Opret";
  const unsigned char ShortCircuit_str[] EEMEM = "Kortsluttet kredsløb!";
  const unsigned char DischargeFailed_str[] EEMEM = "Batteri?";
  const unsigned char Error_str[] EEMEM = "Fejl!";
  const unsigned char Exit_str[] EEMEM = "Afbryd";
  const unsigned char Checksum_str[] EEMEM = "Tjeksum";
  const unsigned char BJT_str[] EEMEM = "BJT";
  const unsigned char Thyristor_str[] EEMEM = "SCR";
  const unsigned char Triac_str[] EEMEM = "Triac";
  const unsigned char PUT_str[] EEMEM = "PUT";
  const unsigned char Bye_str[] EEMEM = "Farvel!";

  #ifndef BAT_NONE
    const unsigned char Weak_str[] EEMEM = "svag";
    const unsigned char Low_str[] EEMEM = "lav";
  #endif

  #ifdef SW_SQUAREWAVE
    const unsigned char SquareWave_str[] EEMEM = "firkantbølge";
  #endif

  #ifdef HW_ZENER
    const unsigned char Zener_str[] EEMEM = "Zener";
    const unsigned char Min_str[] EEMEM = "Min";
  #endif

  #ifdef HW_FREQ_COUNTER
    const unsigned char FreqCounter_str[] EEMEM = "Frekvenstæller";
  #endif

  #ifdef HW_FREQ_COUNTER_EXT
    const unsigned char CounterChannel_str[] EEMEM = "Ch";
    const unsigned char FreqInput_str[] EEMEM = "BNC";
    const unsigned char LF_Crystal_str[] EEMEM = "LF-krystal";
    const unsigned char HF_Crystal_str[] EEMEM = "HF-krystal";
  #endif

  #ifdef SW_ENCODER
    const unsigned char Encoder_str[] EEMEM = "Drejegiver";
    const unsigned char TurnRight_str[] EEMEM = "Drej højre om!";
  #endif

  #ifdef SW_CONTRAST
    const unsigned char Contrast_str[] EEMEM = "Kontrast";
  #endif

  #if defined (SW_IR_RECEIVER) || defined (HW_IR_RECEIVER)
    const unsigned char IR_Detector_str[] EEMEM = "IR-detektor";
  #endif

  #ifdef SW_IR_TRANSMITTER
    const unsigned char IR_Transmitter_str[] EEMEM = "IR-RC";
    const unsigned char IR_Send_str[] EEMEM = "sender...";
  #endif

  #ifdef SW_OPTO_COUPLER
    const unsigned char OptoCoupler_str[] EEMEM = "Optokopler";
    const unsigned char None_str[] EEMEM = "Ingen";
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
    const unsigned char TouchSetup_str[] EEMEM = "Touch-opsætning";
  #endif

#endif


/* ************************************************************************
 *   EOF
 * ************************************************************************ */
