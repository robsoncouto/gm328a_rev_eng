/* ************************************************************************
 *
 *   OneWire bus
 *
 *   (c) 2018 by Markus Reschke
 *
 * ************************************************************************ */


/* ************************************************************************
 *   ROM commands (common to all clients)
 * ************************************************************************ */


/*
 *  ROM code
 *  - byte #0:   family code (device type)
 *  - byte #1-6: serial number
 *  - byte #7:   CRC (of the first 7 bytes)
 */


/*
 *  search ROM
 *  - search for client(s) on the bus
 *  - tree-like search starting with ROM bit #0
 *    - master's first read: clients send ROM bit
 *    - master's second read: clients send inverted ROM bit
 *    - master sends ROM bit to select a part of the tree
 *    - this process is repeated for the remaining ROM bits
 *      and will get the ROM code of one client
 *  - ROM bit logic (ROM bit, inverted ROM bit)
 *    - 00  devices with confliction ROM bits
 *    - 01  all devices have ROM bit 0
 *    - 10  all devices have ROM bit 1
 *    - 11  no devices on bus
 */

#define CMD_SEARCH_ROM        0xF0


/*
 *  read ROM
 *  - can be used only when one single client is on the bus
 *  - client sends its ROM code to the master (byte 0 first)
 */

#define CMD_READ_ROM          0x33


/*
 *  match ROM
 *  - address client with specific ROM code
 *  - master sends client's ROM code (byte 0 first)
 */

#define CMD_MATCH_ROM         0x55


/*
 *  skip ROM
 *  - address all clients on the bus
 */

#define CMD_SKIP_ROM          0xCC


/*
 *  alarm search
 *  - search for clients with an alarm condition
 *  - works like the "search ROM" command 
 */

#define CMD_ALARM_SEARCH      0xEC



/* ************************************************************************
 *   DS18B20 commands
 * ************************************************************************ */


/* ROM family code */
#define DS18B20_FAMILY_CODE   0x28


/*
 *  scatchpad memory map
 *  - byte #0: temperature LSB
 *  - byte #1: temperature MSB
 *  - byte #2: T_H register or user byte #1
 *  - byte #3: T_L register or user byte #2
 *  - byte #4: configuration register
 *  - byte #5: reserved (0xFF)
 *  - byte #6: reserved
 *  - byte #7: reserved (0x10)
 *  - byte #8: CRC (of the first 8 bytes)
 */


/*
 *  temperature register
 *  - bit      7    6    5    4     3     2     1     0
 *    t LSB  2^3  2^2  2^1  2^0  2^-1  2^-2  2^-3  2^-4
 *    t MSB    S    S    S    S     S   2^6   2^5   2^4
 *  - format
 *    - two's complement
 *    - sign bit S = 2^7 (- 2^11)
 *  - resolution   undefined bits      step for decimal places
 *    12 bits      -                   0.0625°C
 *    11 bits      2^-4                0.125°C  
 *    10 bits      2^-4 & 2^-3         0.25°C
 *     9 bits      2^-4, 2^-3 & 2^-2   0.5°C
 */


/*
 *  configuration register
 *  - bit  7   6   5   4   3   2   1   0
 *         0  R1  R0   1   1   1   1   1   
 */

/* thermometer resolution */
#define FLAG_DS18B20_R0       0b00100000     /* R0 bit */
#define FLAG_DS18B20_R1       0b01000000     /* R1 bit */
#define FLAG_DS18B20_RES_9    0b00000000     /* 9 bits */
#define FLAG_DS18B20_RES_10   0b00100000     /* 10 bits */
#define FLAG_DS18B20_RES_11   0b01000000     /* 11 bits */
#define FLAG_DS18B20_RES_12   0b01100000     /* 12 bits (default) */


/*
 *  alarm trigger registers T_H and T_L
 *  - T_H: alarm when T >= T_H
 *  - T_L: alarm when T <= T_L
 *  - bit   7    6    5    4    3    2    1    0
 *          S   #6   #5   #4   #3   #2   #1   #0
 *  - sign bits S
 *    - positive temperature: S = 0
 *    - negative temperature: S = 1
 */


/*
 *  convert T
 *  - start conversion of temperature
 *  - DS18B20 sends conversion status to master
 *    (not available when parasitic-power mode is used)
 */

#define CMD_DS18B20_CONVERT_T           0x44

/* response */
#define FLAG_CONV_IN_PROGRESS      0    /* conversion (still) in progress */
#define FLAG_CONV_DONE             1    /* conversion finished */


/*
 *  write scratchpad
 *  - write data from master to scratchpad
 *    (T_H, T_L and configuration register)
 *  - master sends 3 bytes
 *  - no feedback from DS18B20 to master
 */

#define CMD_DS18B20_WRITE_SCRATCHPAD    0x4E


/*
 *  read scratchpad
 *  - read entire scratchpad including CRC
 *  - DS18B20 sends up to 9 bytes to master (byte 0 first)
 */

#define CMD_DS18B20_READ_SCRATCHPAD     0xBE


/*
 *  copy scratchpad
 *  - copy T_H, T_L and configuration register
 *    from scatchpad to EEPROM
 *  - no feedback from DS18B20 to master
 */

#define CMD_DS18B20_COPY_SCRATCHPAD     0x48


/*
 *  recall E² (EEPROM)
 *  - copy T_H, T_L and configuration register
 *    from EEPROM to scatchpad
 *  - DS18B20 sends recall status to master
 */

#define CMD_DS18B20_RECALL_E2           0xB8

/* response */
#define FLAG_RECALL_IN_PROGRESS    0    /* recall (still) in progress */
#define FLAG_RECALL_DONE           1    /* recall finished */



/*
 *  read power supply
 *  -  DS18B20 sends supply status to master
 */

#define CMD_DS18B20_READ_POWER_SUPPLY   0xB4

/* response */
#define FLAG_PWR_PARASITIC         0    /* parsitic-powered */
#define FLAG_PWR_EXTERNAL          1    /* powered externally */



/* ************************************************************************
 *   EOF
 * ************************************************************************ */
