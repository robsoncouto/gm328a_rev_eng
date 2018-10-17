/* ************************************************************************
 *
 *   language specific global variables: Polish (ISO 8859-1)
 *
 *   (c) 2012-2018 by Markus Reschke
 *   translation by Szpila (sszpila@interia.pl)
 *   based on code from Markus Frejek and Karl-Heinz Kübbeler
 *
 * ************************************************************************ */


/*
 *  Polish
 */

#if defined (UI_POLISH)

  /*
   *  constant strings (stored in EEPROM)
   */

  const unsigned char Probing_str[] EEMEM = "Testuje...";
  const unsigned char Timeout_str[] EEMEM = "Brak odpowiedzi";
  const unsigned char Failed1_str[] EEMEM = "Nie wykryto";
  const unsigned char Failed2_str[] EEMEM = "elementu!";
  const unsigned char Done_str[] EEMEM = "zrobione!";
  const unsigned char Select_str[] EEMEM = "Wybierz";
  const unsigned char Selftest_str[] EEMEM = "Autotest";
  const unsigned char Adjustment_str[] EEMEM = "Ustawienia";
  const unsigned char Save_str[] EEMEM = "Zapisz";
  const unsigned char Load_str[] EEMEM = "Wczytaj";
  const unsigned char Show_str[] EEMEM = "Pokaz parametry";
  const unsigned char Remove_str[] EEMEM = "Usun";
  const unsigned char Create_str[] EEMEM = "Utworz";
  const unsigned char ShortCircuit_str[] EEMEM = "zwarcie sond!";
  const unsigned char DischargeFailed_str[] EEMEM = "Bateria?";
  const unsigned char Error_str[] EEMEM = "Blad!";
  const unsigned char Exit_str[] EEMEM = "Wyjscie";
  const unsigned char Checksum_str[] EEMEM = "Suma kontrolna";
  const unsigned char BJT_str[] EEMEM = "Tranzystor";
  const unsigned char Thyristor_str[] EEMEM = "Tyrystor";
  const unsigned char Triac_str[] EEMEM = "Triak";
  const unsigned char PUT_str[] EEMEM = "Programow. UJT";
  const unsigned char Bye_str[] EEMEM = "Czesc!";

  #ifndef BAT_NONE
    const unsigned char Weak_str[] EEMEM = "slaba";
    const unsigned char Low_str[] EEMEM = "niska";
  #endif

  #ifdef SW_SQUAREWAVE
    const unsigned char SquareWave_str[] EEMEM = "Gen. prostokata";
  #endif

  #ifdef HW_ZENER
    const unsigned char Zener_str[] EEMEM = "Dioda Zenera";
    const unsigned char Min_str[] EEMEM = "Min";
  #endif

  #ifdef HW_FREQ_COUNTER
    const unsigned char FreqCounter_str[] EEMEM = "Licznik czest.";
  #endif

  #ifdef HW_FREQ_COUNTER_EXT
    const unsigned char CounterChannel_str[] EEMEM = "Ch";
    const unsigned char FreqInput_str[] EEMEM = "BNC";
    const unsigned char LF_Crystal_str[] EEMEM = "LF kwarc";
    const unsigned char HF_Crystal_str[] EEMEM = "HF kwarc";
  #endif

  #ifdef SW_ENCODER
    const unsigned char Encoder_str[] EEMEM = "Enkoder";
    const unsigned char TurnRight_str[] EEMEM = "Pokrec w prawo!";
  #endif

  #ifdef SW_CONTRAST
    const unsigned char Contrast_str[] EEMEM = "Kontrast";
  #endif

  #if defined (SW_IR_RECEIVER) || defined (HW_IR_RECEIVER)
    const unsigned char IR_Detector_str[] EEMEM = "Detektor IR";
  #endif

  #ifdef SW_IR_TRANSMITTER
    const unsigned char IR_Transmitter_str[] EEMEM = "Koder IR";
    const unsigned char IR_Send_str[] EEMEM = "wysylam...";
  #endif

  #ifdef SW_OPTO_COUPLER
    const unsigned char OptoCoupler_str[] EEMEM = "Opto-izolator";
    const unsigned char Start_str[] EEMEM = "Start";
    const unsigned char None_str[] EEMEM = "Brak";
  #endif

  #ifdef SW_UJT
    const unsigned char UJT_str[] EEMEM = "UJT";
  #endif

  #ifdef SW_SERVO
    const unsigned char Servo_str[] EEMEM = "Serwo";
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
    const unsigned char TouchSetup_str[] EEMEM = "Ust. dotyku";
  #endif

#endif


/* ************************************************************************
 *   EOF
 * ************************************************************************ */
