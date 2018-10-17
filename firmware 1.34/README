
------------------------------------------------------------------------------

                   ReadMe for the Component Tester firmware

        for ATmega 328/324/644/1284 with character or graphic display

        (c) 2012-2018 by Markus Reschke (madires@theca-tabellaria.de)

------------------------------------------------------------------------------

Last edit:  2018-10-05

Content
  - About
  - Safety Advice
  - License
    - Additional Disclaimer
  - Whats different?
  - Source Code
  - Building the firmware
  - Busses & Interfaces
    - I2C/SPI
    - TTL Serial
    - OneWire
  - Displays
    - HD44780
    - ILI9163
    - ILI9341/ILI9342
    - PCD8544
    - SSD1306
    - ST7565R
    - ST7735
    - ST7920
    - VT100 Terminal
  - Test push button and other input options
    - Test Key
    - Rotary Encoder
    - Increase/Decrease Buttons
    - Touch Screen
  - Communication with PC
    - Serial Output
    - Automation
    - VT100 Output
  - Startup
  - Probing
  - Power Off
  - Menu
    - PWM Tool
    - Square Wave Generator
    - Zener Tool
    - ESR Tool
    - Capacitor Leakage Check
    - Frequency Counter
      - Basic Counter
      - Extended Counter
    - Rotary Encoder
    - Contrast
    - IR RC Detector/Decoder
    - IR RC Transmitter
    - Opto Coupler Tool
    - Servo Check
    - Temperature Sensor DS18B20
    - Self Test
    - Self Adjustment
    - Save/Load
    - Show Values
    - Exit
  - Resistors
  - Capacitors
  - Inductors
  - Discharging Components
  - ADC Oversampling
  - V_BE of BJTs
  - Displaying Results
    - Special Components
      - Non-standard BJT
      - CLD
  - Unsupported Components
  - Known Issues
  - Change Log
  - Remote Commands
  - References


* About

The Component Tester is based on the project of Markus Frejek [1&2] and the
successor of Karl-Heinz Kübbeler [3&4]. It's an alternative software for the
current circuit by Karl-Heinz and offers some changes in the user interface
and the methods used for probing and measuring. While Karl-Heinz provides an
official release supporting also older ATmega MCUs, this is a playground
version with requires an ATmega with 32kB flash at least. The primary UI
languages are English and German, but can be extended easily.

Hint: Run the self-adjustment for a new tester or if you've done any
modifications, like a firmware update or changing probe leads.


* Safety Advice

The Component Tester is no DMM! It's a simple tester for components capable
of measuring several things. The probes aren't protected in any way and
won't survive higher voltages than 5V. Don't use the tester for live circuits!
Just use it for unsoldered electronic components! If you test a capacitor make
sure it's discharged before connecting the probes. This isn't just the Safety
Sally, your life may be at risk if you connect the probes to a live circuit
or a power supply (or even mains). 


* License

The original author hasn't provided any information about the licence under
which the firmware is distributed. He only stated that it's open source and
any commercial user should contact him. Unfortunately we (Karl-Heinz and I)
haven't found any way to contact him. To remedy this problem I've chosen an
open source license at 2016-01-01, after giving the original author more than
sufficient time to tell us his wishes regarding the license. Since the source
code of this firmware version is a major rewrite with tons of new code and
features, I think that this approach is justified.

Licensed under the EUPL V.1.1


+ Additional Disclaimer

Product or company names are possibly trademarks of the respective owners.


* What's different?

Karl-Heinz has done a really great documentation of the tester. I recommend
to read it. Therefore I'll tell you just about the major differences to the
official release:

- user interface
  + No worries! ;)
  + touch screen
  + remote commands
- adaptive component discharging function
- resistance measurement
  + dedicated method for resistances <10 Ohms (instead of using ESR check)
- capacitance measurement
  + starts at 5pF
  + additional method for caps from 4.7µF up to 47µF
  + correction/compensation method
- no SamplingADC() for very low capacitance or inductance
- diodes
  + detection logic
- BJTs
  + V_f is interpolated for a more suitable (virtual) I_b based on hFE
  + detection of Germanium BJTs with high leakage current
- JFETs
  + detection of JFETs with very low I_DSS
- TRIACs
  + detection of MT1 and MT2
- IR RC detector and decoder
- IR RC transmitter
- opto coupler check
- RC servo check
- OneWire (DS18B20)
- structured source code
- some more I couldn't think of right now

There are more details in the sections below.


* Source Code

A lot of cleaning up was done, like more remarks, renamed variables,
re-structured functions, large functions splitted up into several smaller
ones and what have you. I hope the code is easy to read and maintain now.

You can download the firmware from following sites:
- https://www.mikrocontroller.net/svnbrowser/transistortester/Software/Markus
- https://github.com/svn2github/transistortester/tree/master/Software/Markus


* Building the firmware

First edit the Makefile to specify your MCU model, frequency, oscilator
type and programmer settings. All other settings are moved to a global
config.h and a MCU specific config-<MCU>.h. The file 'Clones' lists
settings for various tester versions/clones. If you have a tester not listed,
please email the settings to the author to help other users.

In config.h please choose hardware and software options, the language for the
UI, and change any default values if required. All settings and values are
explained in the file, so I won't discuss them here in depth. 

Hardware options:
- additional keys
  - rotary encoder
  - increase/decrease push buttons
  - touch screen
- 2.5V voltage reference
- relay based cap discharger
- Zener voltage measurement
- frequency counter (basic and extendend version)
- IR detector/decoder for remote controls
  (fixed IR receiver module)
- fixed cap for self-adjustment of voltage offsets
- SPI bus (bit-bang and hardware)
- I2C bus (bit-bang and hardware)
- TTL Serial (bit-bang and hardware)
- OneWire bus (Bit-Bang)

The external 2.5V voltage reference should be only enabled if it's at least
10 times more precise than the voltage regulator. Otherwise it would make
the results worse. If you're using a MCP1702 with a typical tolerance of
0.4% as voltage regulator you really don't need a 2.5V voltage reference.

And of course the software options:
- PWM generator (2 variants)
- inductance measurement
- ESR measurement and in-circuit ESR measurement
- check for rotary encoders
- squarewave signal generator (requires additional keys)
- IR detector/decoder for remote controls
  (IR receiver module connected to probes)
- IR RC transmitter (IR LED with driver trabsistor)
- check for opto couplers
- servo check (requires additional keys, display with >2 lines)
- detection of UJTs
- cap leakage check
- DS18B20 temperature sensor
- color coding for probes (requires color graphics display)
- output of components found also via TTL serial, e.g. to a PC
  (requires TTL serial)
- remote commands for automation via TTL serial

Please choose the options carefully to match your needs and the MCU's
ressources, i.e. RAM, EEPROM and flash memory. If the firmware exceeds the
MCU's flash size, try to disable some options you don't need.

Available UI languages:
- Czech (provided by Kapa)
- Danish (provided by glenndk@mikrocontroller.net)
  needs minor changes in the font
- English (default)
- German
- Italian (privided by Gino_09@EEVblog)
- Polish (provided by Szpila)
- Spanish (provided by pepe10000@EEVblog)
- Russian (provided by indman@EEVblog and hapless@@EEVblog)
  just 8x16 font (horizontally aligned) and HD44780 with Cyrillc font

There's also an alternative Russian text file (var_russian_2.h). If you prefer
that one, simply rename it to var_russian.h. A decimal fraction is indicated
by a dot, but you can change that to a comma if you like by enabling the
corresponding setting.

For MCU specific settings, like pin assignments and display, edit 
config-<MCU>.h:
- ATmega 328                config_328.h
- ATmega 324/644/1284       config_644.h

The display has to provide 2 lines with 16 characters each, at least. For
graphic displays select a font which is small enough to match the
requirements.

After editing the Makefile, config.h or config-<MCU>.h please run 'make' or
whatever toolchain you have to compile the firmware. The Makefiles provides
following additional targets:
- clean    to remove all object and firmware files
- fuses    to set the ATmega's fuse bits
- upload   to upload the firmware to the ATmega


* Busses & Interfaces

+ I2C/SPI

Some displays and other hardware might need I2C or SPI for connecting to the
MCU. Therefore the firmware includes drivers for both bus systems. To cope
with different pin assignments of the various testers the bus drivers support
bit-bang and hardware operation modes. The bit-bang mode can use any IO pins
on the same port, while the hardware mode uses the dedicated bus pins of the
MCU. The drawback of the bit-bang mode is its speed, it's slow. The hardware
mode is much faster. You can spot the difference in speeds easily for high
resolution color LCD modules.

For ATmega 328 based testers the bit-bang mode is needed in most cases due to
the circuit. The ATmega 324/644/1284 has more I/O pins and the different pin
assignment for the circuit allows to use the dedicated bus pins for the
hardware mode.

Since SPI or I2C are primarily used by the LCD module, they can be configured
in the display section of config-<MCU>.h. Alternatively you can also enable I2C
and SPI in config.h, and set ports and pins in dedicated sections in
config-<MCU>.h (look for I2C_PORT or SPI_PORT).


+ TTL Serial

The tester can also provide a TTL serial interface. In case it's used for
communication with a PC it should be combined with a USB to TTL serial
converter or a classic RS-232 driver. The firmware makes use of the MCU's
hardware UART or a bit-bang software UART. The TTL serial interface is enabled
in config.h (see section "Busses") and the port pins are defined in
config-<MCU>.h (look for SERIAL_PORT).

The software UART has the drawback that the TX line will not stay high all
the time when idle. This happens because of the way the MCU port pin are
driven. To remedy this the port pin driving would have to be changed causing
a larger firmware. But this issue doesn't seem to cause any trouble with most
USB to TTL serial converters. In case you see any problem you could try to
add a pull-up resistor (10-100k) to the TX pin to keep the signal at high
level when idle.

The default setting for the TTL serial is 9600 8N1:
  - 9600 bps
  - 8 data bits
  - no parity
  - 1 stop bit
  - no flow control


+ OneWire

Another supported bus is OneWire which can use either the probes/test pins (
ONEWIRE_PROBES) or a dedicated I/O pin (ONEWIRE_IO_PIN). The driver is designed
for standard bus speed and clients to be powered externally (not parasitic-
powered).

Pin assignment for probes:
  Probe #1:  Gnd
  Probe #2:  Vcc (current limited by 680 ohms resistor)
  Probe #3:  DQ (data)
  An external pull-up resistor of 4.7kOhms between DQ and Vcc is required!


* Displays

At the moment following LCD controllers are supported:
- HD44780 (character display)
- ILI9163 (color graphic display 128x160)
- ILI9341/ILI9342 (color graphic display 240x320 or 320x240)
- PCD8544 (graphic display 84x48)
- SSD1306 (graphic display 128x64)
- ST7565R (graphic display 128x64)
- ST7735 (color graphic display 128x160)
- ST7920 (graphic display up to 256x64)
- VT100 Terminal

Take care about the LCD's supply voltage and logic levels! Use a level shifter
if required. If the display doesn't show anything after double checking the
wiring, please try different contrast settings (config_<MCU>.h).

Hint for ATmega 328:
If you connect a rotary encoder to PD2/PD3, please connect the module's /CS to
PD5 and set LCD_CS in config_328.h (applies to graphic displays). Otherwise
the rotary encoder would screw up the display by interfering with the data bus.


+ HD44780

The HD44780 is driven in 4 bit mode. The pin assignment for the parallel
port is:

  display     config-<MCU>.h   default
                               ATmega 328
  DB4         LCD_DB4          PD0
  DB5         LCD_DB5          PD1
  DB6         LCD_DB6          PD2
  DB7         LCD_DB7          PD3
  RS          LCD_RS           PD4
  R/W                          Gnd
  E           LCD_EN1          PD5

You can also drive the LCD via a PCF8574 based I2C backpack which requires
I2C to be enabled. The I2C address has to be specified too. The pin
assignment defines how the LCD is connected to the PCF8574:

  display     config-<MCU>.h   default
  DB4         LCD_DB4          PCF8574_P4
  DB5         LCD_DB5          PCF8574_P5
  DB6         LCD_DB6          PCF8574_P6
  DB7         LCD_DB7          PCF8574_P7
  RS          LCD_RS           PCF8574_P0
  R/W         LCD_RW           PCF8574_P1
  E           LCD_EN1          PCF8574_P2
  LED         LCD_LED          PCF8574_P3


+ ILI9163

The ILI9163 is driven by 4-wire SPI. The pin assignment is:

  display     config-<MCU>.h   default   remark
                               328
  /RESX       LCD_RES          PD4       optional
  /CSX        LCD_CS           PD5       optional
  D/CX        LCD_DC           PD3
  SCL         LCD_SCL          PD2
  SDIO        LCD_SDA          PD1

You might need to play with the x/y flip settings to get the correct
orientation for your display. If necessary you can also offset the x
direction. With LCD_LATE_ON enabled the tester starts with a cleared display
causing a slight delay at power-on. Otherwise you'll see some random pixels
for a moment.


+ ILI9341/ILI9342

The ILI9341/ILI9342 is driven by SPI. The pin assignment is:

  display     config-<MCU>.h   default   remark
                               328
  /RES        LCD_RES          PD4       optional
  /CS         LCD_CS           PD5       optional
  D/C         LCD_DC           PD3
  SCK         LCD_SCK          PD2
  SDI         LCD_SDI          PD1
  SDO         LCD_SDO          PD0       ILI9341 only, not used yet

You might need to play with the x/y flip and rotate settings to get the
correct orientation for your display. And don't forget to set x and y dots
based on the controller (ILI9341 is 240x320 and ILI9342 is 320x240).

The ILI9341/ILI9342 is a color display controller and you can select whether
you like single color (default) or multi color support by the "LCD_COLOR"
definition in the display section of config.h.

Based on the relative high number of pixels the display output is somewhat
slow. A complete screen clear takes about 3 seconds with a 8MHz MCU clock,
when using the bit-bang SPI.


+ PCD8544

The PCD8544 is driven by SPI. The pin assignment is:

  display     config-<MCU>.h   default   remark
                               328
  /RES        LCD_RES          PD4       optional
  /SCE        LCD_SCE          PD5       optional
  D/C         LCD_DC           PD3
  SCLK        LCD_SCLK         PD2
  SDIN        LCD_SDIN         PD1

Since the display has just 84 pixels in x direction you'll get 14 chars per
line with a 6x8 font. So up to two chars might be not displayed. To mitigate
that you could shorten some texts in variables.h.


+ SSD1306

The SSD1306 is driven by 4-wire SPI or I2C. The pin assignment for SPI is:

  display     config-<MCU>.h   default   remark
                               328
  /RES        LCD_RESET        PD4       optional
  /CS         LCD_CS           PD5       optional
  DC          LCD_DC           PD3
  SCLK (D0)   LCD_SCLK         PD2
  SDIN (D1)   LCD_SDIN         PD1

And for I2C:

  /RES        LCD_RESET        PD4       optional
  SCL (D0)    I2C_SCL          PD1
  SDA (D1&2)  I2C_SDA          PD0
  SA0 (D/C)                    Gnd (0x3c) / 3.3V (0x3d)

Using the x/y flip settings you can change the output orientation if
neccessary.


+ ST7565R

The ST7565R is driven by 4/5 line SPI. The pin assignment is:

  display     config-<MCU>.h   default    remark
                               328
  /RES        LCD_RESET        PD0        optional
  /CS1        LCD_CS           PD5        optional
  A0          LCD_A0           PD1
  SCL (DB6)   LCD_SCL          PD2
  SI (DB7)    LCD_SI           PD3

You might need to play with the x/y flip and x-offset settings to get
the correct orientation for your display. 


+ ST7735

The ST7735 is driven by 4-wire SPI. The pin assignment is:

  display     config-<MCU>.h   default   remark
                               328
  /RESX       LCD_RES          PD4       optional
  /CSX        LCD_CS           PD5       optional
  D/CX        LCD_DC           PD3
  SCL         LCD_SCL          PD2
  SDA         LCD_SDA          PD1

You might need to play with the x/y flip settings to get the correct
orientation for your display. With LCD_LATE_ON enabled the tester starts
with a cleared display causing a slight delay at power-on. Otherwise you'll
see some random pixels for a moment.


+ ST7920

The ST7920 can be driven in 4 bit parallel mode or SPI. The pin assignment
for the 4 bit paralllel interface is:

  display     config-<MCU>.h   default   remark
                               328

  /XRESET     LCD_RESET        Vcc       optional
  E           LCD_EN           PD5
  RS          LCD_RS           PD4
  RW          LCD_RW           Gnd       optional
  D4          LCD_DB4          PD0
  D5          LCD_DB5          PD1
  D6          LCD_DB6          PD2
  D7          LCD_DB7          PD3

And for SPI:

 /XRESET      LCD_RESET        PD4       optional
 CS (RS)      LCD_CS           PD5       optional
 SCLK (E)     LCD_SCLK         PD2
 SID (RW)     LCD_SID          PD1

Because of the ST7920's poor design only fonts with a width of 8 pixels can be
used. To cope with the horizontal 16 bit addressing grid I had to add a screen
buffer for characters.


+ VT100 Terminal

The VT100 driver replaces a LCD display and outputs everything to a VT100 serial
terminal. The configuration section for VT100 includes already the activation of
the TTL serial interface. Be aware that the VT100 driver will disable other
options related to the serial interface which might interfere with the output.


* Test push button and other input options

The tester's primary control is the test key, but additional input options
are supported also for a more convenient usage, while some functions require
those.


+ Test Key

The test key starts the tester and also controls the user interface. For that
purpose the tester differentiates between a short and a long key press (0.3s).
The short key press is typically used to proceed with something or to select
a menu item. The long key press performs a context specific action.   

If the tester expects you to press a key it will tell you that by displaying
a cursor at the last position in the second row of the LCD (bottom right).
A steady cursor signals that more information will be displayed and a
blinking cursor informs you that the tester will resume the probing loop. The
cursor is supressed for menus and some tools, because it's obvious that
a key press is neccessary.  


+ Rotary Encoder (hardware option)

With a rotary encoder you'll get some extra functionality with the user
interface, but that's context specific. The additional functionality is
described in the sections below, if applicable. Some functions make use of
the encoder's turning velocity to allow larger changes or steps of values.

The algorithm for reading the encoder considers the number of Gray code
pulses per step or detent (ENCODER_PULSES). Most rotary encoders have 2 or 4
Gray code pulses per detent. Also the number of steps or detents per complete
360 degrees turn is taken into account (ENCODER_STEPS). You can use that
value to finetune the detection of the turning velocity to optimize the
feedback. A higher value slows the velocity down, while a lower value speeds
it up. In case the encoder's turning direction is reversed, simply swap the
MCU pin definitions for A and B in config_<MCU>.h.

The detection of the turning velocity measures the time for two steps. So you
need to turn the encoder at least by two steps for a mid-range velocity. For
very high velocities it's three steps. A single step results in the lowest
velocity.


+ Increase/Decrease Buttons (hardware option)

If you prefer push buttons over a rotary encoder you can add a pair of push
buttons as alternative. The push buttons are wired the same way as the rotary
encoder (pull-up resistors, low active). For a speed-up functionality similar
to the encoder's turning velocity keep pressing the push button. A long button
press will increase the "speed" as long as you keep pressing the button.


+ Touch Screen (hardware option)

Another input option is a touch screen. Please note that the screen should be
large enough and support approximately 8 text lines with 16 characters each.
To save precious space on the display the user interface doesn't show icons
to touch. It simply has invisible touch bars at the left and right (each 3
characters wide), also at the top and the bottom (2 lines high) and one at the
center area. The left and top bars are for decreasing a value or moving up in
a menu, while the bottom and right bars are for increasing a value or moving
down in a menu. Actually they do the same as a rotary encoder. Touching a bar
longer results in a speed-up if supported by a function or tool (similar to
turning the rotary encoder faster). The center bar acts as a software version
of the test key, i.e. it won't power the Zener diode test option for example.

The touch screen needs an adjustment for proper operation. The adjustment is
automatically started after powering the tester on, when no adjustment values
are stored in the EEPROM. You can also run the adjustment via the main menu.
The procedure is straight forward. If you see an asterisk (yellow * on color
displays), simply touch it. The tester displays the native x/y position after
each touch event. You can skip the procedure any time by pressing the test key.
If you have problems with the adjustment like strange x/y positions, please
check the orientation of the touch screen in relation to the display. The
driver has options to flip or rotate the orientation. Don't forget to save
the offsets after a successful adjustemnt (main menu: save).

Supported touch screen controllers:
- ADS7843 / XPT2046

You'll find the configuration options below the display section in
config-<MCU>.h (currently just config_644.h because of the lack of unused
IO pins of the 328).  


* Communication with PC

The tester can support a TTL serial interface for communication with a PC.
This could be a TX-only connection for outputting components found or a
bidrectional one for automation. In both cases the TTL serial interface needs
to be enabled in config.h (see section "Busses").

Special characters are replaced with standard ones, for example the omega (
Ohms) becomes a simple R.

conversion table:
  diode symbols       |>  <|
  capacitor symbol    ||
  omega               R
  micro / µ           u
  resistor symbol     []

Hints:
- 9600 8N1
- newline is <CR><LF>


+ Serial Output

The tester outputs components found to a PC running a simple terminal program
when this feature is enabled (see UI_SERIAL_COPY in section "misc settings"
in config.h). The serial output follows the output on the LCD display but only
for the components found. There is no serial output for menus and tools besides
the results of the opto coupler check.


+ Automation

The automation feature allows you to control the tester by remote commands
via a bidirectional serial connection. For enabling this feature please see
UI_SERIAL_COMMANDS in section "misc settings" in config.h. The default
behaviour of the tester will change slightly. The automation enforces the
auto-hold mode and the tester won't automatically check for a component after
powering on.

The command interface is fairly simply. You send a command and the tester will
respond. The communication is based on ASCII textlines and the commands are
case sensitive. Each command line has to be ended by a <CR><LF> or <LF> newline.
Be aware that the tester will only accept commands when waiting for user
feedback after powering on, displaying a component or running a menu function.
Response lines end with a <CR><LF> newline. See section "Remote Commands" for
a list of commands and their explanation.


+ VT100 Output

The tester can output everything to a VT100 terminal instead of a LCD display (
see VT100 in section "Displays"). To keep the layout of the output undisturbed
all other options for the serial interface are disabled.


* Startup

A long key press while starting the tester selects the auto-hold mode. In that
mode the tester waits for a short key press after displaying a result before
it will continue. Otherwise the tester chooses the continous (looping) mode by
default. You can reverse the operation mode selection in config.h (UI_AUTOHOLD).
After powering on, the firmware version is shown briefly.

A very long key press (2s) will reset the tester to firmware defaults. This
might be handy if you have misadjusted the LCD contrast for example and can't
read the display anymore.

If the tester detects a problem with the stored adjustments values, it will
display a checksum error. That error indicates a corrupted EEPROM, and the
tester will use the firmware defaults instead.


* Probing

After the startup the testers enters the probing mode and looks for a
connected component. In continous mode it will automatically repeat the
probing after a short pause. If no component is found for several times the
tester will power itself off. In auto-hold mode (signaled by cursor) the
tester runs one round and waits for a key press or a right turn of the
rotary encoder.

In both modes you can enter a menu with additional functions or power off
the tester. For details please see below.


* Power Off

While displaying the result of the last test a long key press powers the
tester off. The tester will show a good bye message and then power off. As
long as you press the key the tester stays powered on. This is caused by the
implementaion of the power control circuit.


* Menu

You'll enter the menu by two short key presses after the display of the last
component found or function performed. Simply press the test key twice quickly
(might need some practice :). If the rotary encoder option is enabled, a left
turn will also enter the menu. The old method by short circuiting all three
probes can be enabled too (UI_SHORT_CIRCUIT_MENU).

While in the menu, a short key press shows the next item in the menu and a
long key press runs the shown item. On a 2-line display you'll see a
navigation help at the bottom right. A '>' if another item follows, or
a '<' for the very last item (will roll over to the first item). On a
display with more than 2 lines the selected item is marked with an '*' at
the left side. 

With a rotary encoder you can move the items up or down based on the turning
direction and a short key press will run the displayed item, instead of moving
to the next item. Roll over is also enabled for the first item.

Some tools show you the pinout of the probe pins used before doing anything.
That info will be displayed for a few seconds, but can be skipped by a short
press of the test button.

For tools which create a signal probe #2 is used as output by default. In that
case probe #1 and #3 are set to ground. If your tester is configured for a
dedicated signal output (OC1B) the probes aren't used and no probe pinout will
be displayed.


+ PWM Tool

This does what you would expect :) Before compiling the firmware please
choose either the PWM generator with the simple user interface or the one with
the fancy interface for testers with rotary encoder and large displays.

Pinout for signal output via probes:
  Probe #2:         output (with 680 Ohms resistor to limit current)
  Probe #1 and #3:  Ground


- Simple PWM

First you have to select the desired PWM frequency in a simple menu. Short key
press for the next frequency and a long key press starts the PWM output for
the shown frequency.

The duty ratio of the PWM starts at 50%. A short key press of the test button
increases the ratio by 5%, a long key press decreases the ratio by 5%. To exit
the PWM tool press the test key twice quickly.

If you have a rotary encoder you can use it to select the frequency in the menu
and to change the PWM ratio in 1% steps.


- Fancy PWM

Switch between frequency and ratio by pressing the test button. The selected
value is marked by an asterisk. Turn the rotary encoder clockwise to increase
the value or anti-clockwise to decrease it. As faster you turn the rotary
encoder as larger the step size becomes. A long key press sets the default
value (frequency: 1kHz, ratio: 50%). Two short button presses exit the PWM
tool. 


+ Square Wave Signal Generator

The signal generator creates a square wave signal with variable frequency up
to 1/4 of the MCU clock rate (2MHz for 8MHz MCU clock). The default frequency
is 1000Hz and you can change it by turning the rotary encoder, The turning
velocity determines the frequency change, i.e. slow turning results in small
changes und fast turning in large changes. Since the signal generation is
based on the MCU's internal PWM mode you can't select the frequency
continously, but in steps. For low frequencies the steps are quite small,
but for high frequencies they become larger and larger. A long button press
sets the frequency back to 1kHz, and two brief button presses exit the
signal generator, as usual.

Pinout for signal output via probes:
  Probe #2:         output (with 680 Ohms resistor to limit current)
  Probe #1 and #3:  Ground

Hint: Rotary encoder or other input option required!


+ Zener Tool (hardware option)

An onboard DC-DC boost converter creates a high test voltage for measuring the
breakdown voltage of a Zener diode connected to dedicated probe pins. While
the test button is pressed the boost converter runs and the tester displays
the current voltage. After releasing the test button the minimum voltage
measured is shown if the test button was pressed long enough for a stable test
voltage. You may repeat this as long as you like :-) To exit the Zener tool
press the test button twice quickly.

How to connect the Zener diode:
   Probe +: cathode
   Probe -: anode


+ ESR Tool

The ESR tool measures capacitors in-circuit and displays the capacity and ESR
if the measurement detects a valid capacitor. Make sure that the capacitor is
discharged before connecting the tester! Values could differ from the standard
measurement (out-of-circuit) because any component in parallel with the
capacitor will affect the measurement.

For triggering a measurement please press the test key. Two quick short key
presses will exit the tool.

How to connect the capacitor:
   Probe #1: positive
   Probe #3: negative (Gnd)


+ Capacitor Leakage Check

The cap leakage check charges a cap and displays the current and the voltage
across the current shunt. It starts charging the cap using Rl (680 Ohms) and
switches to Rh (470kOhms) when the current drops below a specific threshold.

Each cycle begins with the display of the pinout. After connecting the cap
press the test button (or right turn in case of a rotary encoder) to start
the charging process. To end charging press the test button again and the
tester will discharge the cap while displaying its voltage until the voltage
drops below the discharge threshold. To exit the check press the test button
twice.

Hint: Pay attention to the polarity of polarized caps!

How to connect the capacitor:
   Probe #1: positive
   Probe #3: negative (Gnd)


+ Frequency Counter (hardware option)

There are two versions of the frequency counter. The basic one is a simple
passive input to the T0 pin of the MCU. The extended version has an input
buffer, two oscillators for testing crystals (low and high frequency) and
an additional prescaler. The circuit diagrams for both are depicted in
Karl-Heinz' documentation.


- Basic Counter

With the basic frequency counter hardware option installed you can measure
frequencies from about 10Hz up to 1/4 of the MCU clock with a resolution
of 1Hz for frequencies below 10kHz. The frequency is measured and displayed
continously until you end the measurement by a key press. The autoranging
algorithm selects a gate time between 1ms and 1000ms based on the frequency.


- Extended Counter

The extended frequency counter has an additional prescaler and allows to
measure higher frequencies. The theoretical upper limit is 1/4 of the MCU's
clock rate multiplied by the prescaler (16:1 or 32:1). The control lines
are configured in config_<mcu>.h, and don't forget to set the correct
prescaler in config.h.

The input channel (buffered input, low frequency crystal oscillator, high
frequency crystal oscillator) is changed by pressing the test push button or
turning the rotary encoder. And as always, two short button presses will exit
the frequency counter.


+ Rotary Encoder

This test checks rotary encoders while determining the pin-out. Your job is
to connect the probes to the A, B and Common pin and to turn the encoder
a few steps clockwise. The algorithm needs four greycode steps to determine
the proper function and pin-out. The turning direction is important to
distinguish the A nd B pins, because reversed pins cause a reversed direction.

When a rotary encoder is detected the tester will display the pin-out and
wait for a key press (or a moment for continous mode) before resuming testing.
To exit the rotary encoder test please press the test push button once while
testing.


+ Contrast

You can adjust the contrast for some graphic LCD modules. A short key press
increases the value and and a long key press decreases it. Two short key
presses will exit the tool. With a rotary encoder installed the value
can also be adjusted by turning the encoder. 


+ IR RC Detector/Decoder

This function detects and decodes signals from IR remote controls, and
requires an IR receiver module, for example the TSOP series. When compiling
the firmware you can choose between two variants how the IR receiver module is
connected to the tester. The first one is to connect the IR module to the
standard testpins. The second one is a fixed IR module connected to a
dedicated MCU pin.

If a known protocol is detected the tester displays the protocol, address
(when available), command, and in some cases optional data in hexadecimal.
The usual format is:
  <protocol> [<addr>:]<command>[:<extra data>]
For a malformend packet the protocol and a "?" are shown.

For a unknown protocol the tester displays the number of pauses and pulses,
the duration of the first pulse and the first pause in units of 50µs:
  ? <pulses>:<first pulse>-<first pause>
When the number of pulses stay the same for different buttons of the RC, the
modulation is most likely PDM or PWM. A changing number of pulses indicates
bi-phase modulation.  

To exit the tool please press the test key. 

Supported protocols:
  - JVC
  - Kaseikyo (aka Japanese Code, 48 bit)
  - Matsushita (Panasonic MN6014, C6D6 / 12 bits)
  - Motorola
  - NEC (standard & extended)
  - Proton / Mitsubishi (M50560)
  - RC-5 (standard)
  - RC-6 (standard)
  - Samsung / Toshiba (32 bits)
  - Sharp / Denon
  - Sony SIRC (12, 15 & 20 bits)

Optional protocols:
  - IR60 (SDA2008/MC14497)
  - Matsushita (Panasonic MN6014, C5D6 / 11 bits)
  - NEC µPD1986C
  - RECS80 (standard & extended)
  - Sanyo (LC7461)
  - Thomson

The carrier frequency of the TSOP receiver module doesn't have to match the RC
exactly. A mismatch reduces the possible range, but that doesn't matter much
for this application.


- IR receiver module connected to probes

Please connect the IR receiver module after entering the IR detector function.

How to connect the TSOP module:
  Probe #1:  Gnd
  Probe #2:  Vs (current limited by 680 ohms resistor)
  Probe #3:  Data/Out

Hint:
  The current limiting resistor for Vs implies an IR receiver module with a
  supply voltage range of about 2.5 to 5V. If you have a 5V only module you can
  disable the resistor in the config.h file on your own risk. Any short
  circuit may destroy the MCU.


- Fixed IR receiver module

For the fixed IR module please set the port and pin used in config_<MCU>.h


+ IR RC Transmitter

The IR RC transmitter sends RC codes you've entered, and is meant to check
IR RC receivers or remote controlled devices. This tool requires additional
keys, such as a rotary encoder, a display with more than 4 lines, and a simple
driver circuit for the IR LED.

The display shows you the protocol, the carrier frequency, the duty cycle of
the carrier and a few data fields. By a short press of the test button you
switch between the items. The selected item is indicated by an '*'. Use the
rotary encoder (or other input option) to change the setting/value of an item.
A long press of the test button and the tester sends the IR code as long as
you keep the button pressed. And as usual, two short presses exit the tool.

When you change the protocol the carrier frequency and duty cycle are set to
the protocol's default values. But you can change them if you wish. The carrier
frequency can be set to 30 up to 56 kHz and the duty cycle to 1/2 (50%),
1/3 (33%) or 1/4 (25%). The data fields are the user settable parts of the IR
code and are explained later on. In most cases it's just the address and the
command.

Supported protocols and their data fields:

  - JVC
    <address:8> <command:8>

  - Kaseikyo (Japanese Code)
    <manufacturer code:16> <system:4> <product:8> <function:8>

  - Matsushita (Panasonic, MN6014 12 bits)
    <custom code:6> <key data:6>

  - Motorola
    <command:9>

  - NEC Standard
    <address:8> <command:8>

  - NEC Extended
    <address:16)> <command:8>

  - Proton / Mitsubishi (M50560)
    <address:8> <command:8> 

  - RC-5 Standard
    <address:5> <command:6>

  - RC-6 Standard, Mode 0
    <address:8> <command:8>

  - Samsung / Toshiba (32 bits)
    <custom code:8> <key data:8>

  - Sharp / Denon
    <address:5> <command:8> <mask:1>

  - Sony SIRC-12
    <command:7> <address:5>

  - Sony SIRC-15
    <command:7> <address:8>

  - Sony SIRC-20
    <command:7> <address:5> <extended:8>

Optional protocols:

  - Thomson
    <device:4> <function:7>

The data fields are separated by spaces and their syntax is:
  <field name>:<number of bits>

Pinout for signal output via probes:
  Probe #2:         signal output (with 680 Ohms resistor to limit current)
  Probe #1 and #3:  Ground

The signal output (probe #2) has a current limiting resistor and can drive an
IR LED with only about 5mA directly, which isn't sufficient for the IR LED's
typical rating of 100mA. Therefore you need a simple driver circuit based on
a switching transistor, the IR LED and a current limiting resistor for the LED.

Example for driving an IR LED (Vf 1.5V, If 100mA) with about 50mA:

                           + +5V
                           |
                           |
                           -
                          | |
                       75 | |
                          | |
                           -
                           |
                           |
                          ___ ->
                   IR LED \ / ->
                          ---
                           |
                           | C
              -----    B |/
  Signal ----| 3k9 |-----|  NPN
              -----      |\
                           | E
                    BC548  |
                           |
                           - Gnd


+ Opto Coupler Tool

This tool checks opto couplers and shows you the LED's V_f, the CTR (also If),
and t_on/t_off delays (BJT types). It supports standard NPN BJTs, NPN 
Darlington stages and TRIACs. For the CTR measurement the MCU's I/O pin is
overloaded for about 3ms. The datasheet specifies a maximum output current of
20mA, but we overload the I/O pin up to about 100mA. Therefore the maximal
CTR value is limited and any CTR over 2000% should be considered with caution.

The maximum drive current for the LED is about 5mA, which should be considered
for TRIAC types. Relay types (MOSFET back to back) are detected as BJT and the
CTR will be meaningless. Types with anti-parallel LEDs are ignored.

For testing you need a simple adapter with following three test points:

BJT type:
- LED's anode
- LED's cathode and BJT's emitter connected together
- BJT's collector

TRIAC type:
- LED's anode
- LED's cathode and TRIAC's MT1 connected together
- TRIAC's MT2

You may connect the adapter any way to the three probes of the component
tester. It will detect the pinout automatically.

After entering the tool please connect the adapter and press the test key
briefly to scan for an opto coupler. If one is found the tester displays
the type and various details. Or it displays "none" when no opto coupler was
detected. A blinking cursor indicates that you have to press the test key
(or turn the rotary encoder) for a new scan. Two short key presses end the
tool as usual.


+ Servo Check

This function outputs a PWM signal for RC servos which are driven by a 1-2ms
PWM pulse. It supports the typical PWM frequencies of 50, 125, 250 and 333 Hz
while the pulse length can be between 0.5 and 2.5 ms. There is also a sweep
mode for sweeping between 1 and 2 ms pulse length with an adjustable sweep
speed.

Please adjust the pulse width with the rotary encoder. Clockwise for a longer
pulse, and counter-clockwise for a shorter pulse. A long button press resets
the pulse to 1.5 ms.  

You can switch between pulse and frequency selection mode with a short button
press (mode marked by an asterisk). When in frequency selection mode use the 
rotary encoder to choose the PWM frequency. A long button press enables or
disables the sweep mode (marked by a "<->" after the frequency).

As long as the sweep mode is enabled, the pulse selection is replaced by the
sweep period. The rotary encoder allows you to change the period. 

As usual, two short button presses exit the function.

Pinout for signal output via probes:
  Probe #2:         PWM output (with 680 Ohms resistor to limit current)
  Probe #1 and #3:  Ground

Hint: You have to provide an additional power supply for the servo.

Some pinouts of typical 3pin servo connectors:

Vendor         pin 1               pin 2               pin 3
---------------------------------------------------------------------- 
Airtronics     PWM White/Black     Gnd Black           Vcc Red
Futaba         PWM White           Vcc Red             Gnd Black
hitec          PWM Yellow          Vcc Red             Gnd Black
JR Radios      PWM Orange          Vcc Red             Gnd Brown


+ Temperature Sensor DS18B20

This tool reads the OneWire temperature sensor DS18B20 and displays the
temperature. Please see section "Busses & Interfaces" for the setup of
the OneWire bus. When using the probes the tester will inform you about
the pin assignment and waits until it can detect the external pull-up
resistor. You can skip this by a key press. After connecting the DS18B20
as the only client on the OneWire bus push the test button for reading the
sensor (this may take nearly a second). To exit the tool press the test button
twice quickly.


+ Self Test

If you entered the self-test by the menu you'll be asked to short circuit all
three probes and the tester will wait until you have. In case of any problem
you can abort that by a key press. That will also skip the complete self-test.

The self-test function runs each test just 5 times. You can skip a test by a
short key press or skip the entire selfttest by a long key press.

In test #4 you have to remove the short circuit of the probes. The tester will
wait until you removed the short circuit.

The test steps are:
- T1 internal bandgap reference (in mV)
- T2 comparison of Rl resistors (offset in mV)
- T3 comparison of Rh resistors (offset in mV)
- T4 remove short circuit of probes
- T5 leakage check for probes in pull-down mode (voltage in mV)
- T6 leakage check for probes in pull-up mode (voltage in mV)


+ Self Adjustment

The self-adjustment measures the resistance and the capacitance of the probe
leads, i.e. the PCB, internal wiring and probe leads as a sum, for creating
a zero offset. It also measures the internal resistance of the MCU port pins
in pull-down and pull-up mode. If the tests are skipped or strange values are
measured the default values defined in config.h are used. If everything went
fine the tester will display and use the new values gained by the self
adjustment (they will be not stored in the EEPROM, see "Save" below).

The voltage offset of the analog comparator is automatically adjusted in the
capacitance measurement (in normal probing mode, outside of the self
adjustment) if the capacitor is in the range of 100nF up to 3.3µF. Also the
offset of the internal bandgap reference is determined in the same way.

Before running the self-adjustment the first time, please measure a film
capacitor with a value between 100nF and 3.3µF three times at least to let the
tester self-adjust the offsets mentioned above. Typically the first
measurement will result in a slightly low value, the second in a high one
and the third will be fine. That is caused by the self adjusting offsets.
Both offsets are displayed at the end of the self-adjustment.

With a fixed cap for self-adjustment the automatic offset handling in the
capacitance measurement is replaced by a dedicated function run during the
self-adjustment procedure. So you don't have to measure a film cap before
that. 

The self-adjustment is very similar to the self-test regarding the procedure
and user interface.

The adjustments steps are:
- A1 offsets for bandgap reference and analog comparator
     (only for fixed cap option)
- A2 resistance of probe leads/pins (in 10mOhms)
- A3 remove short circuit of probes
- A4 MCU's internal pin resistance for Gnd (voltage across RiL)
- A5 MCU's internal pin resistance for Vcc (voltage across RiH)
- A6 capacitance of probe leads/pins (in pF)

Expected limits:
- probe resistance   < 1.50 Ohms for two probes in series
- probe capacitance  < 100pF

Hint: When the resistance values for the probe leads/pins vary too much,
there could be a contact problem.

Remember: Adjustment is not calibration! Calibration is the procedure to
compare measurement results with a known traceable standard and noting the
differences. The goal is to monitor the drift over time. Adjustment is the
procedure to adjust a device to meet specific specs.


+ Save/Load

By flashing the firmware the pre-defined values given in config.h are stored
in the EEPROM of the MCU. After running the self-adjustment you may update
those default values using the "Save" function. The next time you power
on the tester the updated values (profile #1) will be loaded and used
automatically.

For your convenience you can save and load two profiles, e.g. if you two sets
of different probes.

The idea of the save function is to prevent automatic saving of adjustment
values. If you need to use other probe leads for some tests, you'll simply
adjust the tester for the temporary probe leads and perform the tests. If you
switch back to the standard probe leads you don't need to re-adjust because
the old values are still stored. Just powercycle the tester.


+ Show Values

This displays the current adjustment values and offsets used. The usage of an
external 2.5V voltage reference is indicated by a '*' behind Vcc.


+ Exit

If you entered the menu by mistake you can exit it by this command.


* Resistors

Resistors are measured twice (both directions) and the values are compared.
If the values differ too much the tester assumes that there are two resistors
instead of just a single one. In that case the tester displays the result as
two resistors with the same pins, like "1 -- 2 -- 1", and the two different
resistance values. For resistors lower than 10 Ohms an extra measurement with
a higher resolution is performed.

In some rare cases the tester might not be able to detect a very low
resistance. In that case simply re-run the test.


* Capacitors

The measurement of capacitance is split into three methods. Large caps >47µF
are measured by the charging cycle method with 10ms pulses. Mid-sized
caps between 4.7µF and 47µF are processed the same way but with 1ms charging
pulses. And small caps are done by the analog comparator method. That
way the accuracy of the measurement of caps is optimized.

Large capacitances require a correction. Without correction the measured values
are too large. IMHO, that is caused by the measurement method, i.e. the ADC
conversion after each charging pulse needs some time and the cap looses charge
due to internal leakage during the same time. Also the ADC conversion itself
needs some charge to operate. So it takes longer to charge the cap, and the cap
seems to have a larger capacitance. A discharge measurement later on tries to
compensate this, but is able to do it just partially. The correction factors (
CAP_FACTOR_SMALL, CAP_FACTOR_MID and CAP_FACTOR_LARGE in config.c) are choosen
to work with most tester models. In some cases you might have to change them.

A logic for preventing large caps to be detected as resistors was added.
Resistors < 10 Ohms are checked for being large caps.

A measured capacitance value more than 5pF (incl. the zero offset) is
considered valid. Lower values are too uncertain and could be caused by
placing the probe leads a little bit differently and things like that.

The tester tries to measure the ESR for capacitors with more than 10nF.
Alternatively you can also enable the old ESR measurement method starting
at 180nF. But since the ESR measurement isn't done via an AC signal with a
specific frequency, please don't expect a solid result. The method used might
be comparable with a 1kHz test. Anyway, the results are good enough to check
electrolytic caps. For low value film caps you could get different results
based on the MCU clock rate. I'd guess Mr. Fourier is able to explain this.

Alternatively you can also enable the old ESR measurement method.

Another measurement taken is the self-discharge leakage current for
capacitors larger than 4.7µF. It gives a hint about the state of an 
electrolytic cap. From my tests the typical value for a good electrolytic
cap seems to be about:
  - 10-220µF    1-3µA
  - 330-470µF   4-5µA
  - 470-820µF   4-7µA
  - >1000µF     5-7µA per 1000µF


* Inductors

The inductance measurement isn't very accurate, and things like the MCU clock
speed and the PCB layout has an impact on the results. Basically it's based
on measuring the time between switching on current flow and reaching a specific
current. For high inductances there's a low current check, and for low
inductances a high current check, which exceeds the MCU's pin drive limit
for a very short time (up to about 25 micro seconds).

While investigating the effects of the MCU clock and other things I've found a
pattern of deviations, which can be used for compensation. Based on the tester
you have some custom tweaking might be necessary. In inductance.c in the
function MeasureInductor() there the variable "Offset" for compensation. That
variable is an offset for the reference voltage. A positive offset will
decrease the inductance, and a negative value will increase the inductance.

The compensation for the high current check is based on the MCU clock, and its
divided in three time ranges, each one with a dedicated offset. For the low
current check there's just a simple compensation at the moment, as it needs
further tests. If you see any major deviations when compairing the measurement
results with a proper LCR meter, you can adjust the offsets to match your
tester.

Hint: When getting unexpected results please re-run the test.


* Discharging Components

The discharge function isn't based on a fixed timeout, it adapts itself
to the discharging rate. That way a battery will be identified faster (about
2s) and large caps have more time to discharge. If a large cap is identified
as a battery please repeat the check. In a noisy environment you might need
to adjust CAP_DISCHARGED to about 3mV. The display of the remaining voltage
will help you to choose an appropriate value.


* ADC Oversampling

The ADC function is modified to support a variable oversampling (1-255 times).
The default value is 25 samples. You can try to improve the accuracy of the
measurements by increasing the number of samples. Note that more samples will
take more time resulting in slower measurements.


* V_BE of BJTs

When checking for diodes Vf is measured with Rl (high test current) and Rh (
low test current), and both voltages are stored. The output function for BJTs
looks up the matching diode for V_BE and interpolates the two Vf measurements
based on the transistors hfe for a virtual test current. That way we get more
suitable results for different kinds of transistors, since Vf of a small signal
BJT isn't measured with the same test current as for a power BJT.


* Displaying Results

Some names and abbreviations are changed. The output for several parts might
be splitted into multiple pages to support displays with just a few lines.

For a single diode the low current Vf (measured with 10µA) is shown in braces
if the voltage is below 250mV. That should give you a hint for germanium
diodes. Most datasheets of germanium diodes specify Vf at 0.1mA which the
tester doesn't support. At a higher current Vf is expected to be around 0.7V
which makes it hard to distinguish germanium from silicon diodes.

The leakage current I_R for a single diode or I_CEO for a BJT will be
displayed if it exeeds 50nA. Germanium BJTs have a leakage current of a few µA
up to around 500µA. Germanium diodes are around a few µA usually.

For some components the capacitance is shown also. In case the capacitance is
below 5pF or the measurement failed for some reason the value displayed
will be 0pF.

If a depletion-mode FET with symmetrical Drain and Source is found, e.g. a
JFET, the pinout shows an 'x' instead of a 'D' or 'S' because both can't be
distinguished, they are functionally identical. Please see the FET's datasheet
if you need more details about the pinout.

The pinout for a Triac is shown with the pin IDs 'G', '1' and '2'. '1' is MT1
and '2' is MT2. And for a UJT, in case the detection is enabled, it's '1' for
B1, '2' for B2 and 'E' for the Emitter.

When the fancy pinout option is enabled (by selecting a symbols file in
config.h) a component symbol with the corresponding probe pin numbers will be
shown for 3-pin semiconductors. If there's not enough space left on the display
for the symbol, the pinout will be skipped. 


+ Special Components


Non-standard BJT

For a BJT with a base emitter resistor the tester displays the base emitter
resistor and skips the hFE output. If the BJT also has a freewheeling diode the
BJT might be detected as BJT or two diodes based on the value of the base
emitter resistor (low value resistor -> 2 diodes). In the latter case the tester
shows the two diodes and the resistor while hinting at a possible NPN or PNP
BJT, The problem is that the low value base emitter resistor prevents the proper
identification of the BJT.

Another special case is a BJT with an integrated freewheeling diode on the same
subtrate as the BJT. That integrated diode junction creates a parasitic
transistor. A NPN BJT will have a parasitic PNP and vice versa. If such a BJT
is found the tester prints a '+' behind the BJT type.


CLD

The diode check identifies a CLD (Current Limiting Diode) as a standard diode
and displays I_F as the leakage current. Note that anode and cathode of a CLD
are reversed vs. a standard diode. A dedicated check for a CLD is hard to
implement, since the leakage current of a Germanium or high-current Schottky
diode is in the range of I_F (>= 33µA).

If a diode has an unusual forward voltage, a quite low V_f for the low current
check (2nd value in braces) and no capacitance could be measured then it's
most likely a CLD.


* Unsupported Components

Any semiconductor which requires a high current to trigger conduction can't
be supported, since the tester only provides about 7mA at maximum. Also the
tester provides just a voltage of 5V, which isn't sufficient for DIACs with
a V_BO of 20-200V.


* Known Issues

- A storage cap (like Panasonic NF series) is detected as a diode or two
  anti-parallel diodes. The capacitance measurement isn't able to determine  
  an acceptable value either.
- When using a SMPS or DC-DC converter as power supply the tester will
  sometimes detect a capacitor around 50µF even if no component is connected.
- The ESR of a cap with a low capacitance may vary with the MCU clock.


* Change Log

Please see the CHANGES file!


* Remote Commands

When the tester accepts remote commands it will respond with following
text strings besides command specific answers containing data:

  ERR
  - unknown command
  - command unsupported in component specific context
  - buffer overflow

  OK
  - command executed
    (some commands may need some time for processing)

  N/A
  - information/value not available

Responses with data will never start with any of the standard text strings
above to prevent possible confusion.


Basic Commands:

  VER
  - returns firmware version
  - to verify connectivity and to determine command set based on version
  - example response: "1.33m"

  OFF
  - powers off tester
  - tester responds with an "OK" before powering off
  - example response: "OK" <tester powers off>


Probing Commands:

  PROBE
  - probes component and skips any pauses waiting for user feedback
  - tester responds with an "OK" after probing is finished
  - example response: <some time elapses for probing> "OK"

  COMP
  - returns component type ID
  - see COMP_* in common.h for IDs
  - example response for BJT: "30"

  MSG
  - returns error message
  - applies only when an error has occured (COMP: 1)
  - response may vary with the language of the user interface
  - example response: "Battery? 1:1500mV"

  QTY
  - returns component quantity
  - example response for BJT: "1"

  NEXT
  - selects second component
  - applies if two components are found (QTY: 2)
  - example response: "OK"

  TYPE
  - returns more specific type of component
  - applies to BJT, FET and IGBT
  - types available:
    - NPN      NPN (BJT)
    - PNP      PNP (BJT)
    - JFET     JFET (FET)
    - MOSFET   MOSFET (FET)
    - N-ch     n-channel (FET, IGBT)
    - P-ch     p-channel (FET, IGBT)
    - enh.     enhancement mode (FET, IGBT)
    - dep.     depletion mode (FET, IGBT)
  - example response for BJT: "NPN"
  - example response for FET (MOSFET): "MOSFET n-ch enh."

  HINT
  - returns hints on special features
  - applies to diode, BJT, FET and IGBT
  - hints available:
    - NPN      possibly a NPN BJT (diode)
    - PNP      possibly a PNP BJT (diode)
    - R_BE     base-emitter resistor (diode, BJT)
    - BJT+     integrated flyback diode on same substrate
               creating a parasitic second BJT (BJT)
    - D_FB     body/flyback diode (BJT, FET, IGBT)
    - SYM      symmetrical drain and source (FET) 
  - example response for BJT: "D_FB R_BE"
  - example response for FET (MOSFET): "D_FB"

  PIN
  - returns pinout of component
  - identifiers used:
    - resistor    x = connected, - = not connected
    - capacitor   x = connected, - = not connected
    - diode       A = anode, C = cathode, - = not connected
    - BJT         B = base, C = collector, E = emitter
    - FET         G = gate, S = source, D = drain, x = drain/source
    - IGBT        G = gate, C = collector, E = emitter
    - SCR         G = gate, A = anode, C = cathode
    - TRIAC       G = gate, 2 = MT2, 1 = MT1
    - PUT         G = gate, A = anode, C = cathode
    - UJT         E = emitter, 2 = B2, 1 = B1
  - format of response:
    <probe #1 identifier><probe #2 identifier><probe #3 identifier>
  - example response for resistor: "xx-"
  - example response for diode: "C-A"
  - example response for BJT: "EBC"

  R
  - returns resistance value
  - applies to resistor (includes inductor)
  - example response: "122R"

  C
  - returns capacitance value
  - applies to capacitor
  - example responses: "98nF" "462uF"

  L
  - returns inductance value
  - applies to resistor (includes inductor)
  - example response: "115uH"

  ESR
  - returns ESR value (Equivalent Series Resistance)
  - requires ESR measurement to be enabled
  - applies to capacitor
  - example response: "0.21R"

  I_l
  - returns I_leak value (self-discharge equivalent leakage current)
  - applies to capacitor
  - example response: "3.25uA"

  V_F
  - returns V_F value (forward voltage)
  - applies to diode and PUT
  - also applies to body diode of MOSFET and flyback diode of BJT or IGBT
  - example response: "654mV"

  V_F2
  - returns V_F value of low current measurement (forward voltage)
  - applies to diode
  - example response: "387mV"

  C_D
  - returns C_D value (diode capacitance)
  - applies to diode
  - example response: "8pF"

  I_R
  - returns I_R value (reverse current)
  - applies to diode
  - example response: "4.89uA"

  R_BE
  - returns R_BE value (base-emitter resistor)
  - applies to diode and BJT
  - example responses: "38.2R" "5171R"

  h_FE
  - returns h_FE value (DC current gain)
  - applies to BJT
  - example response: "234"

  V_BE
  - returns V_BE value (base-emitter voltage)
  - applies to BJT
  - example response: "657mV"

  I_CEO
  - returns I_CEO value (collector-emitter current, open base)
  - applies to BJT
  - example response: "460.0uA"

  V_th
  - returns V_th value (threshold voltage)
  - applies to FET (MOSFET) and IGBT
  - example response: "2959mV"

  C_GS
  - returns C_GS value (gate-source capacitance)
  - applies to FET (MOSFET)
  - example response: "3200pF"

  R_DS
  - returns R_DS_on value (drain-source on-resistance)
  - applies to FET (MOSFET)
  - example response: "1.20R"

  I_DSS
  - returns I_DSS value (drain-source current, zero bias / shorted gate)
  - applies to FET (depletion mode)
  - example response: "6430µA"

  C_GE
  - returns C_GE value (gate-emitter capacitance)
  - applies to IGBT
  - example response: "724pF"

  V_GT
  - returns V_GT value (gate trigger voltage)
  - applies to SCR and TRIAC
  - example response: "865mV"

  V_T
  - returns V_T value (offset voltage)
  - applies to PUT
  - example response: "699mV"

  R_BB
  - returns R_BB value (interbase resistance)
  - requires UJT detection to be enabled
  - applies to UJT
  - example response: "4758R"


* References

[1] AVR-Transistortester, Markus Frejek,
    Embedded Projects Journal, 2011-11

[2] http://www.mikrocontroller.net/topic/131804
    thread of Markus Frejek, Forum, 2009

[3] http://www.mikrocontroller.net/articles/AVR-Transistortester
    Online documentation of the Transistortester, Online Article, 2009-2011

[4] http://www.mikrocontroller.net/articles/AVR_Transistortester
    Short description of the TransistorTester, Karl-Heinz Kübbeler,
    Online Article, 2012



------------------------------------ EOF -------------------------------------
