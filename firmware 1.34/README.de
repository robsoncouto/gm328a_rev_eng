
------------------------------------------------------------------------------

                   ReadMe für die Component Tester Firmware

           für ATmega328/324/644/1284 mit Text- oder Grafikdisplay

        (c) 2012-2018 by Markus Reschke (madires@theca-tabellaria.de)

------------------------------------------------------------------------------

Letzte Änderung:  2018-10-05

Inhaltsübersicht:
  - Über den Tester
  - Sicherheitshinweise
  - Lizenz
    - Zusätzliche Hinweise
  - Unterschiede
  - Quellcode
  - Firmware übersetzen
  - Busse & Schnittstellen
    - I2C/SPI
    - Serielle TTL-Schnittstelle
    - OneWire
  - LCD-Module
    - HD44780
    - ILI9163
    - ILI9341/ILI9342
    - PCD8544
    - SSD1306
    - ST7565R
    - ST7735
    - ST7920
    - VT100 Terminal
  - Tasten und Eingabeoptionen
    - Test-Taste
    - Drehencoder
    - Mehr/Weniger-Tasten
    - Touch-Screen
  - Kommunikation mit PC
    - Serielle Ausgabe
    - Automatisierung
    - VT100-Ausgabe
  - Einschalten
  - Bauteilesuche
  - Ausschalten
  - Menü
    - PWM-Generator
    - Rechteck-Signalgenerator
    - Zenertest
    - ESR-Messung
    - Kondensatorleckstrom
    - Frequenzzähler
      - Einfacher Zähler
      - Erweiterter Zähler
    - Drehencoder
    - Kontrast
    - Detektor/Decoder für IR-Fernbedienungen
    - IR-Fernbedienung
    - Opto-Koppler-Test
    - Modellbau-Servo-Test
    - Temperatursensor DS18B20
    - Selbsttest
    - Selbstabgleich
    - Speichern/Laden
    - Werte anzeigen
    - Exit
  - Widerstände
  - Kondensatoren
  - Induktivitäten
  - Bauteile entladen
  - ADC Oversampling
  - V_BE von Bipolartransistoren
  - Ergebnisanzeige
    - Spezielle Bauteile
      - Spezial-Bipolar-Transistor
      - CLD
  - Nicht unterstütze Bauelemente
  - Bekannte Probleme
  - Firmware-Änderungen
  - Fernsteuerungskommandos
  - Quellenverzeichnis


* Über den Tester

Der Component Tester basiert auf dem Projekt von Markus Frejek [1&2] und der
Weiterführung von Karl-Heinz Kübbeler [3&4]. Es ist eine alternative
Firmware für die aktuelle Testerschaltung von Karl-Heinz und bietet einige
Änderungen der Benutzerschnittstelle und Mess- und Testverfahren. Während
die Firmware von Karl-Heinz die offizielle Version ist und auch ältere
ATmega MCUs unterstützt, ist diese Version zum Ausprobieren und Testen neuer
Ideen. Außerdem ist sie auf ATmegas mit mindestens 32kB Flash beschränkt.
Die primären Sprachen für die Bedienung sind Englisch und Deutsch, können aber
leicht um weitere Sprachen ergänzt werden

Hinweis: Bitte den Selbstabgleich bei brandneuen Testern oder nach Firmware-
aktualisierung laufen lassen. Oder auch bei Benutzung anderer Messkabel.


* Sicherheitshinweise

Der Component Tester ist kein Multimeter! Es ist ein einfacher Tester für
Bauteile, der Verschiedenes messen kann. Die Eingänge sind nicht geschützt
und werden durch Spannungen über 5V beschädigt. Den Tester nicht für
Schaltungen in Betrieb nutzen, sondern nur für einzelne Bauteile! Bei
Kondensatoren darauf achten, daß sie entladen sind, bevor der Tester
angeklemmt wird. Benutzung auf eigene Gefahr!


* Lizenz

Der Autor der Ursprungsversion hat bzgl. der Lizenzbedingungen nur zwei
Vorgaben gemacht. Zum einen ist das Projekt Open Source, und zum anderen
sollen sich kommerzielle Benutzer beim Autor melden. Unglücklicherweise
haben wir, Karl-Heinz und ich, den Autor bisher nicht erreichen können.
Um das Problem des Fehlens einer vernünftigen Open-Source-Lizenz zu lösen,
habe ich am 1.1.2016 eine Standard-Open-Source-Lizenz ausgewählt, nachdem
der ursprüngliche Autor ausreichend zeit hatte uns seine Wünsche bzgl. einer
Lizenz mitzuteilen. Da diese Firmwareversion eine komplett neue Version ist,
die lediglich ein paar Ideen der ursprünglichen Firmware aufgreift, aber
keinen Code teilt, sollte dieses Vorgehen gerechtfertigt sein. 

Lizenziert unter der EUPL V.1.1


+ Zusätzliche Hinweise

Produkt- oder Firmennamen können geschützte Marken der jeweiligen Eigentümer
sein.


* Unterschiede

Karl-Heinz hat eine richtig gute Doumentation für den Tester geschrieben.
Unbedingt lesen! Daher zähle ich nur die Hauptunterschiede zur k-Firmware auf:

- Benutzerschnittstelle
  + Keine Panik! ;)
  + Touch Screen
  + Automatisierung (Fernsteuerkommandos)
- Adaptive Entladefunktion
- Widerstandsmessung
  + zusätliche Methode für Widerstände <10 Ohm (anstatt ESR-Messung)
- Kapazitätsmessung
  + ab 5pF
  + zusätzliche Methode für Kondensatoren zwischen 4,7µF und 47µF
  + anderes Verfahren zur Korrektur/Kompensation
- kein SamplingADC() für sehr niedrige Kapazitäten oder Induktivitäten
- Dioden
  + Erkennungslogik
- Bipolartransistoren
  + V_f wird für praxisnahe Testströme interpoliert
  + Erkennung von Germanium-Transistoren mit höherem Leckstrom
- JFETs
  + Erkennung von JFETs mit sehr niedrigem I_DSS
- TRIACs
  + Erkennung von MT1 und MT2
- Detektor/Dekoder für IR-Fernsteuerungen
- IR-Fernbedienung
- Test von Opto-Kopplern
- Test von Modellbau-Servos
- OneWire (DS18B20)
- Strukturierter Quellcode
- Und Manches mehr, was mir gerade nicht einfällt.

Mehr Details dazu findest Du in den nachfolgenden Abschnitten.


* Quellcode

Eine Menge wurde aufgeräumt, Kommentare, Variablen umbenannt, Funktionen
umstrukturiert, große Funktionen in mehrere kleine aufgeteilt und mehr. Ich
hoffe, daß der Quellcode nun einfach zu lesen und gut verständlich ist.

Du findest die Firmware auf folgenden Webseiten:
- https://www.mikrocontroller.net/svnbrowser/transistortester/Software/Markus
- https://github.com/svn2github/transistortester/tree/master/Software/Markus


* Firmware übersetzen

Als erstes solltest Du das Makefile editieren und MCU-Modell, Frequenz,
Oszillator-Typ und die Programmieradaptereinstellungen anpassen. Alle anderen
Einstellungen stehen in der globalen config.h und einer MCU-spezifischen
config-<MCU>.h. In der Datei "Clones" findest Du Einstellungen zu
verschiedenen Testerversionen bzw. Clonen. Ist Dein Tester nicht dabei,
schicke bitte die Einstellungen per EMail an den Autor, um anderen Benutzern
damit zu helfen.

In config.h wähle Hardware- und Software-Optionen, die Sprache für die
Bedienung, und ändere Standardwerte, wenn notwendig. Alle Einstellungen und
Werte sind in der Datei selber erklärt. Daher folgt hier nur eine kurze
Übersicht der wichtigsten Punkte.

Hardware-Optionen:
- zusätzliche Tasten bzw. Eingabeoptionen
  - Drehencoder
  - Mehr/Weniger-Tasten
  - Touch-Screen
- externe 2,5V Spannungsreferenz
- Schutz-Relais zur Kondensatorentladung
- Messung von Zenerdioden (DC-DC-Konverter)
- Frequenzzähler (einfache und erweiterte Version)
- Test von IR-Fernbedienungen (festes IR-Empfängermodul)
- fester Kondensator für Selbstabgleich von Spannungsoffsets
- SPI-Bus (Bit-Bang und Hardware)
- I2C-Bus (Bit-Bang und Hardware)
- TTL-Serielle (Bit-Bang und Hardware)
- OneWire Bus (Bit-Bang)

Die externe 2,5V Spannungsreferenz sollte nur genutzt werden, wenn sie
um den Faktor 10 genauer als der Spannungsregler ist. Ansonsten würde
sie die Messergebnisse eher verschlechtern als verbessern. Wenn Du einen
MCP1702 mit einer typischen Genauigkeit von 0,4% als Spannungsregler
hast, brauchst Du eigentlich keine zusätzliche Spannungsreferenz mehr.

Und natürlich die Software-Optionen:
- PWM Generator (2 Varianten)
- Messung von Induktivität
- ESR-Messung und In-Circuit ESR
- Test von Drehencodern
- Rechtecksignalgenerator (braucht zusätzliche Tasten)
- Test von IR-Fernbedienungen (IR-Empfängermodul an Testpins)
- IR-Fernbedienung (IR-LED mit Treibertransistor)
- Test von Opto-Kopplern
- Test von Modellbau-Servos (braucht zusätzliche Tasten, Display >2 Zeilen)
- Erkennung von UJTs
- Test für Kondensatorleckstrom
- DS18B20 Temperatursensor
- Farbkodierung für Testpins (benötigt Farbdisplay)
- Ausgabe der gefundenen Bauteile parallel über TTL-Serielle, z.B auf PC
  (benötigt TTL-Serielle)
- Fernsteuerkommandos über TTL-Serielle.

Bitte die Optionen entprechend Deinen Wünschen und den begrenzten Ressourcen 
der MCU, d.h. RAM, EEPROM und Flash-Speicher, auswählen. Sollte die Firmware
zu groß werden, versuche für Dich nicht so wichtige Optionen wieder zu
deaktivieren.

Verfügbare Sprachen:
- Dänisch (von glenndk@mikrocontroller.net)
  benötigt kleine Änderungen im Font
- Deutsch
- Englisch
- Italienisch (von Gino_09@EEVblog)
- Polnisch (von Szpila)
- Russisch (von indman@EEVblog und hapless@EEVblog)
  nur 8x16 Zeichensatz (horizontal ausgerichtet) und HD44780 mit kyrillischem
  Zeichensatz
- Spanisch (von pepe10000@EEVblog)
- Tschechisch (von Kapa)

Es gibt für Russisch zusätzlich eine alternative Textdatei (var_russian_2.h).
Wenn Du diese nehmen möchtest, einfach nach var_russian.h umbenennen. Ist Dir
ein Komma statt einem Punkt für die Kennzeichnung von Dezimalstellen lieber, 
gibt es dafür eine Einstellungsoption (Standard: Punkt).

Für die MCU-spezifischen Einstellungen, wie Pin-Zuordnungen und Display,
editiere config-<MCU>.h:
- ATmega 328                config_328.h
- ATmega 324/644/1284       config_644.h

Das LCD-Modul sollte min. 2 Zeilen mit min. 16 Zeichen haben. Für Grafikmodule
einen Zeichensatz wählen, der die obige Bedingung erfüllt.

Nach dem Editieren vom Makefile, config.h oder config-<MCU>.h bitte "make"
ausführen oder was-auch-immer Dein IDE will, um die Firmware zu übersetzen.
Das Makefile bietet folgende zusätzliche Targets: 
- clean    alle Objektdateien löschen
- fuses    Fuse Bits setzen
- upload   Firmware brennen


* Busse & Schnittstellen


+ I2C/SPI

Manche LCD-Module and andere Komponenten benötigen I2C oder SPI als Schnitt-
stelle zur MCU. Daher hat der Firmware Treiber für beide Bussysteme. Um
unterschiedliche Beschaltungen zu unterstützen haben die Bustreiber einen
Bit-Bang und einen Hardware-Modus. Beim Bit-Bang-Modus können beliebige IO
Pins an dem gleichen Port genutzt werden, während der Hardware-Modus die
fest vorgegeben Bus-Pins der MCU nimmt. Der Nachteil des Bit-Bang-Modus ist
seine Geschwindigkeit, er ist langsam. Der Hardware-Modus dagegen ist deutlich
schneller. Du kannst den Unterschied leicht bei Farb-LCD-Modulen mit hoher
Auflösung sehen.

Für Tester mit ATmega 328 wird fast immer der Bit-Bang-Modus aufgrund der
Beschaltung benötigt. Der ATmega 324/644/1284 hat mehr I/O Pins, plus die
veränderte Beschaltung erlauben es, die festen Bus-Pins für den Hardware-
Modus zu nutzen. 

Da SPI oder I2C primär vom LCD-Modul genutzt wird, können beide im Abschnitt
fuer LCD-Module in config-<MCU>.h direkt konfiguriert werden. Alternativ kannst
Du auch I2C bzw. SPI in config.h aktivieren und Ports & Pins in config-<MCU>.h
festlegen (schaue nach I2C_PORT bzw. SPI_PORT).


+ Serielle TTL-Schnittstelle

Der Tester kann auch eine optionale serielle TTL-Schnittstelle haben. Wird
diese zur Kommunikation mit einem PC genutzt, sollte sie mit einem USB-zu-TTL
Konverter oder einem RS-232 Treiberbaustein kombiniert werden. Die Firmware
kann den UART der MCU oder einen Software-UART (Bit-Bang) nutzen. Die TTL-
Serielle wird in config.h aktiviert (siehe Abschnitt "Busses"), und Port & Pins
sind in config-<MCU>.h definiert (schaue nach SERIAL_PORT).

Der Software-UART hat den Nachteil, daß das TX-Signal nicht ständig "high"
ist, wenn die Schnittstelle ruht. Ursache dafür ist benutze Methode wie
Port-Pins angesteuert werden. Ein Umschreiben der Ansteuerung würde die
Firmware deutlich vergrößern. Dieses Problem scheint aber keine Auswirkung
auf die meisten USB-zu-TTL Konverter zu haben. Sollte es doch Probleme
geben, kannst Du einen Pull-up-Widerstand (10-100k) am TX-Pin probieren, um
den Signalpegel im Ruhemodus auf "high" zu halten. 

Die Standardeinstellung der seriellen Schnittstele ist 9600 8N1:
  - 9600 bps
  - 8 Datenbits
  - keine Parität
  - 1 Stopbit
  - keine Flussteuerung


+ OneWire

Ein weiterer untesrstützter Bus ist OneWire, welcher entweder die Test-Pins (
ONEWIRE_PROBES) oder einen festen MCU-Pin (ONEWIRE_IO_PIN) benutzen kann.
Der Treiber ist für Standard-Busgeschwindigkeit und Clients mit externer
Stromversorgung (nicht parasitär versorgt) ausgelegt.

Beschaltung von Test-Pins:
  Probe #1:  Gnd
  Probe #2:  Vcc (Strom durch 680 Ohm Widerstand begrenzt)
  Probe #3:  DQ (Daten)
  Ein externer Pull-Up-Widerstand von 4,7kOhm zwischen DQ and Vcc wird
  benötigt!


* LCD-Module

Im Augenblick werden folgende LCD-Module bzw. Controller unterstützt:
- HD44780 (textbasiertes Display)
- ILI9163 (grafisches Farb-Display 128x160)
- ILI9341/ILI9342 (grafisches Farb-Display 240x320 oder 320x240)
- PCD8544 (grafisches Display 84x48)
- SSD1306 (grafisches Display 128x64)
- ST7565R (grafisches Display 128x64)
- ST7735 (grafisches Farb-Display 128x160)
- ST7920 (grafisches Display bis zu 256x64)
- VT100 Terminal

Beachte die Versorgungsspannung und die Logikpegel des LCD-Moduls! Benutze
Pegelumsetzer, sofern notwendig. Wenn das Display trotz korrekter Beschaltung
nichts anzeigt, versuche den Kontrast zu ändern (config_<MCU>.h).

Hinweis zu ATmega 328:
Wenn Du einen Drehencoder an PD2/PD3 hängst, dann verbinde /CS vom LCD-Modul
mit PD5 und setze LCD_CS in config_328.h (nur für grafische LCD-Module).
Anderenfalls würde der Drehencoder den Datenbus durcheinander bringen und
zu fehlerhaften Ausgaben führen.


+ HD44780

Der HD44780 wird im 4-Bit-Modus bestrieben. Die Pins für den Parallelbus sind:

  Modul       config-<MCU>.h   Standard
                               328
  DB4         LCD_DB4          PD0
  DB5         LCD_DB5          PD1
  DB6         LCD_DB6          PD2
  DB7         LCD_DB7          PD3
  RS          LCD_RS           PD4
  R/W                          Gnd
  E           LCD_EN1          PD5

Das LCD-Modul kann auch über einen I2C-Adapter mit PCF8574 bestrieben werden.
Dazu muss zum einen I2C aktiviert werden, und zum anderen wird auch die
I2C-Adresse des PCF8574 benötigt. Das Pinout beschreibt wie das LCD-Modul mit
dem PCF8574 verdrahtet ist:

  Modul       config-<MCU>.h   Standard
  DB4         LCD_DB4          PCF8574_P4
  DB5         LCD_DB5          PCF8574_P5
  DB6         LCD_DB6          PCF8574_P6
  DB7         LCD_DB7          PCF8574_P7
  RS          LCD_RS           PCF8574_P0
  R/W         LCD_RW           PCF8574_P1
  E           LCD_EN1          PCF8574_P2
  LED         LCD_LED          PCF8574_P3


+ ILI9163

Der ILI9163 wird mittels 4-Draht SPI gesteuert. Die Pins sind:

  Modul       config-<MCU>.h   Standard   Hinweis
                               328
  /RESX       LCD_RES          PD4        optional
  /CSX        LCD_CS           PD5        optional
  D/CX        LCD_DC           PD3
  SCL         LCD_SCL          PD2
  SDIO        LCD_SDA          PD1

Für eine korrekte Anzeige mußt Du evtl. mit den Einstellungen X/Y-Flip
experimentieren. Bei Bedarf kannst Du auch die X-Richtung verschieben. Wenn
LCD_LATE_ON aktiviert ist, startet der Tester mit einem gelöschten Display,
was zu einer kurzen Verzögerung beim Einschalten führt. Ansonsten sind beim
Anschalten kurz zufällige Pixel zu sehen.


+ ILI9341/ILI9342

Der ILI9341/ILI9342 wird mittels SPI gesteuert. Die Pins sind:

  Modul       config-<MCU>.h   Standard   Hinweis
                               328
  /RES        LCD_RES          PD4        optional
  /CS         LCD_CS           PD5        optional
  D/C         LCD_DC           PD3
  SCK         LCD_SCK          PD2
  SDI         LCD_SDI          PD1
  SDO         LCD_SDO          PD0        nur ILI9341, noch nicht benutzt

Für eine korrekte Anzeige mußt Du evtl. mit den Einstellungen X/Y-Flip und
Rotate experimentieren. Und nicht vergessen die X und Y-AUflösung korrekt
zu setzen (ILI9341 ist 240x320 und ILI9342 ist 320x240).

Der ILI9341/ILI9342 ist ein Farbdisplay-Controller, und Du kannst zwischen
Einfarben oder Mehrfarben-Modus über die LCD_COLOR Definition im LCD-Bereich
in config.h wählen (Standard: Einfarben-Modus).

Aufgrund der hohen Pixelzahl ist die Ausgabe etwas langsam. Das Löschen der
kompletten Anzeige dauert etwa 3 Sekunden bei 8MHz MCU-Takt, wenn Bit-Bang
SPI genutzt wird.


+ PCD8544

Der PCD8544 wird mittels SPI gesteuert. Die Pins sind:

  Modul       config-<MCU>.h   Standard   Hinweis
                               328
  /RES        LCD_RES          PD4        optional
  /SCE        LCD_SCE          PD5        optional
  D/C         LCD_DC           PD3
  SCLK        LCD_SCLK         PD2
  SDIN        LCD_SDIN         PD1

Da das Display nur 84 Punkte in X-Richtung hat, ergeben sich max. 14 Zeichen
pro Zeile bei einem 6x8-Zeichensatz. Also werden bis zu zwei Zeichen
verschluckt. Wenn das stört, kannst Du in variables.h die Texte etwas kürzen.


+ SSD1306

Der SSD1306 wird mittels 4-Line SPI oder I2C gesteuert. Die Pins für SPI sind:

  Modul       config-<MCU>.h   Standard   Hinweis
                               328

  /RES        LCD_RESET        PD4        optional
  /CS         LCD_CS           PD5        optional
  DC          LCD_DC           PD3
  SCLK (D0)   LCD_SCLK         PD2
  SDIN (D1)   LCD_SDIN         PD1

Und für I2C:

  /RES        LCD_RESET        PD4        optional
  SCL (D0)    I2C_SCL          PD1
  SDA (D1&2)  I2C_SDA          PD0
  SA0 (D/C)                    Gnd (0x3c) / 3.3V (0x3d)

Mit den X/Y-Flip Einstellungen kannst Du die Orientierung der Anzeige
anpassen, wenn notwenig.


+ ST7565R

Der ST7565R wird mittels 4/5-Line SPI gesteuert. Die Pins sind:

  Modul       config-<MCU>.h   Standard   Hinweis
                               328
  /RES        LCD_RESET        PD0        optional
  /CS1        LCD_CS           PD5        optional
  A0          LCD_A0           PD1
  SCL (DB6)   LCD_SCL          PD2
  SI (DB7)    LCD_SI           PD3

Für eine korrekte Anzeige mußt Du evtl. mit den Einstellungen X/Y-Flip und
X-Offset experimentieren.


+ ST7735

Der ST7735 wird mittels 4-Draht SPI gesteuert. Die Pins sind:

  Modul       config-<MCU>.h   Standard   Hinweis
                               328
  /RESX       LCD_RES          PD4        optional
  /CSX        LCD_CS           PD5        optional
  D/CX        LCD_DC           PD3
  SCL         LCD_SCL          PD2
  SDA         LCD_SDA          PD1

Für eine korrekte Anzeige mußt Du evtl. mit den Einstellungen X/Y-Flip
experimentieren. Wenn LCD_LATE_ON aktiviert ist, startet der Tester mit einem
gelöschten Display, was zu einer kurzen Verzögerung beim Einschalten führt.
Ansonsten sind beim Anschalten kurz zufällige Pixel zu sehen.


+ ST7920

Der ST7920 kann per 4-bit-Parallel-Modus oder SPI gesteuert werden. Die Pins
für den Parallelbus sind:

  Modul       config-<MCU>.h   Standard  Hinweis
                               328

  /XRESET     LCD_RESET        Vcc       optional
  E           LCD_EN           PD5
  RS          LCD_RS           PD4
  RW          LCD_RW           Gnd       optional
  D4          LCD_DB4          PD0
  D5          LCD_DB5          PD1
  D6          LCD_DB6          PD2
  D7          LCD_DB7          PD3

Und für SPI:

 /XRESET      LCD_RESET        PD4       optional
 CS (RS)      LCD_CS           PD5       optional
 SCLK (E)     LCD_SCLK         PD2
 SID (RW)     LCD_SID          PD1

Wegen dem schlechten Design des ST7920 können nur Zeichensätze mit einer Breite
von 8 Pixeln verwendet werden. Zur Handhabung der horizontalen Addressierung in
16-Bit Schritten mußte ich einen Bildschirmpuffer für Zeichen einrichten.


+ VT100 Terminal

Der VT100-Treiber ersetzt ein LCD-Modul, und die komplette Ausgabe erfolgt über
ein serielles VT100-Terminal. Der Konfigrationsabschnitt für VT100 aktiviert
die serielle Schnittstelle gleich mit. Bitte beachte, daß der VT100-Treiber
andere Optionen für die serielle Schnittstelle deaktiviert, welche die Ausgabe
beeinträchtigen können.


* Tasten und Eingabeoptionen

Der Tester wird über primär über die Test-Taste bedient, erlaubt aber
zusätzliche Eingabeoptionen, welche die Bedienung erleichtern oder für
manche Funktion auch notwendig sind.


+ Test-Taste

Die Test-Taste schaltet den Tester ein und dient zur Bedienung. Dazu
unterscheidet der Tester zwischen einem kurzen Tastendruck und einem langen (
0,3s). Der kurze Tastendruck wird üblicherweise zum Fortfahren einer Funktion
oder zur Auswahl des nächsten Menüpunktes benutzt. Der lange Tastendruck führt
eine kontextabhängige Aktion aus.

Wenn der Tester einen Tastendruck zum Fortfahren der aktuellen Aktion erwartet,
zeigt es dies durch einen Cursor rechts unten auf dem LCD-Modul an. Ein
statischer Cursor signalisiert, daß weitere Informationen folgen, und ein
blinkender Cursor bedeutet, daß mit der Bauteilesuche weiter gemacht wird.
Für Menüs und einige Extrafunktionen wird der Cursor nicht angezeigt, da die
erwartete Eingabe hier klar sein sollte. 


+ Drehencoder (Hardware-Option)

Mit einem Drehencoder erhält die Bedienung zusätzliche Funktionalität, die
kontextabhängig ist. Die Details werden in den weiteren Abschnitten erklärt.
Manche Funktionen erlauben über die Drehgeschwindigkeit größere Änderungen
oder Sprünge von Werten. 

Der Lese-Algorithmus berücksichtigt die Anzahl der Gray-Code-Pulse pro
Schritt (ENCODER_PULSES) und auch die Anzahl der Schritte für eine volle
360 Grad Umdrehung (ENCODER_STEPS). Mit dem letzteren Wert kannst Du
auch eine Feineinstellung der Erkennung der Drehgeschwindigkeit vornehmen.
Ein höherer Wert verlangsamt die Drehgeschwindigkeit, ein niedriger Wert
erhöht sie. Sollte die Drehrichtung verkehrt herum sein, einfach die Pin-
Definitionen für A und B in config_<MCU>.h vertauschen.

Die Erkennung der Drehgeschwindigkeit mißt die Zeit von zwei Schritten. Also
solltest Du den Encoder mindestens um zwei Schritte für mittlere
Gewindigkeiten drehen. Für höhere Geschwindigkeiten sind es drei Schritte.
Ein einzelner Schritt resultiert immer in der niedrigsten Geschwindigkeit. 


+ Mehr/Weniger-Tasten (Hardware-Option)

Wenn Dir Tasten lieber als ein Drekencoder sind, dann kannst Du alternativ
auch zwei Tasten nutzen. Die Tasten werden genauso wie ein Drehencoder
angeschlossen (Pull-Up Widerstände, Logikpegel Low bei Betätigung). Für
eine Beschleunigung, ähnlich der Drehgeschwindikeit beim Drehencoder, einfach
die Taste lange drücken. Je länger, desto höher wird die Beschleunigung.


+ Touch-Screen (Hardware-Option)

Als weitere Eingabeoption ist ein Touch-Screen möglich. Dazu sollte das
LCD-Modul groß genug sein und mindestens 8 Textzeilen je 16 Zeichen oder mehr
unterstützen. Um wertvollen Platz auf dem LCD zu sparen, verzichten wir auf
Symbole zum Berühren. Stattdessen gibt es unsichtbare Leisten links und rechts
(je 3 Zeichen breit), oben und unten (je 2 Zeilen hoch) und eine Fläche in der
Mitte. Die Leisten links und oben sind für "weniger" oder "Menüpunkt hoch",
rechts und unten entsprechend für "mehr" oder "Menüpunkt runter". Also die
gleiche Funktion wie beim Drehencoder. Ein langes Berühren beschleunigt
Änderungen in manchen Funktionen, ähnlich der Drehgeschwindigkeit beim Dreh-
encoder. Die Fläche in der Mitte ist eine Software-Version der Test-Taste,
d.h. damit wird z.B. nicht die Zener-Test-Option mit Strom versorgt.

Für die Benutzung des Touch-Screen ist ein Abgleich notwendig. Dieser wird
automatisch nach dem Einschalten des Testers gestartet, wenn noch keine
Abgleichwerte im EEPROM gespeichert sind. Man kann ihn auch über das Haupt-
menü starten. Die Prozedur ist recht einfach. Wenn Du ein Sternchen (gelbes *
bei Farb-LCDs) siehst, drauf drücken. Der Tester zeigt nach jeder Berührung
die native x/y-Position an. Den Abgleich kannst Du jeder Zeit mit der Test-
Taste abbrechen. Wenn Du Probleme mit dem Abgleich, wie z.B. seltsame x/y-
Positionen, hast, überprüfe bitte die Orientierung des Touch-Screen gegenüber
dem LCD-Modul. Der Treiber hat Schalter zum Verdrehen und Vertauschen der
Orientierung. Nacht einem erfolgreichen Abgleich nicht vergessen, die Offset-
Werte zu speichen (Hauptmenü: Speichern).

Unterstützte Touch-Screen-Controller:
- ADS7843 / XPT2046

Du findest die Konfiguration dazu unterhalb des Bereichs für LCD-Module in
config-<MCU>.h (momentan nur config_644.h, da der 328 zu wenig unbenutze
IO-Pins hat).


* Kommunikation mit PC

Der Tester kann eine serielle TTL-Schnittstelle zur Kommunilkation mit einem
PC nutzen. Dies kann eine unidrektionale Verbindung (nur Senden) zur seriellen
Ausgabe von gefundenen Bauteilen sein, oder auch eine bidrektionale zur
Automatisierung. In beiden Fällen muß die serielle Schnittstelle aktiviert
werden (siehe Abschnitt "Busses" in config.h).

Spezielle Zeichen werden durch Standardzeichen ersetzt, z.B. wird das Omega (
Ohm) zu einem R.

Konvertierungstabelle:
  Diodensymbol        |>  <|
  Kondensatorsymbol   ||
  Omega               R
  micro / µ           u
  Widerstandssymbol   []

Hinweise:
- 9600 8N1
- Newline ist <CR><LF>


+ Serielle Ausgabe

Der Tester gibt gefundene Bauteile zusätzlich über die serielle Schnitt-
stelle aus, wenn dies aktviert ist (siehe UI_SERIAL_COPY im Abschnitt "misc
settings" in config.h). Dazu reicht ein einfaches Terminalprogramm auf dem PC. 
Die Ausgabe folgt der Ausgabe auf dem LCD-Display, aber nur für gefundene
Bauteile.  Es erfolgt keine Ausgabe von Menüs und Funtionen über
die Serielle, außer für Ergebnisse vom Opto-Koppler-Test.


+  Automatisierung

Die Automatisierung ermöglicht die Fernsteuerung des Testers per Kommandos über
eine bidrektionale serielle Verbindung. Zum Aktivieren dieser Funktion siehe
bitte UI_SERIAL_COMMANDS im Abschnitt "misc settings" in config.h. Das Verhalten
des Testers ändert sich etwas. Die Automatisierung erwingt den Auto-Hold-Modus,
und der Tester sucht nach dem Einschalten nicht automatisch nach einem Bauteil. 

Die Kommandoschnitstelle ist recht einfach. Du sendest ein Komanndo und der
Tester antwortet. Die Kommunikation basiert auf ASCII-Textzeilen, und bei den
Kommandos ist auf Groß- und Kleinschreibung zu achten. Jede Kommandozeile
wird mit einem <CR><LF> oder <LF> Newline abgeschlossen. Der Tester nimmt
Kommandos nur während des Wartens auf den Benutzer nach dem Einschalten, der
Ausgabe eines Bauteils oder der Ausführung einer Menüfunktion an. Antwort-
zeilen enden mit einem <CR><LF> Newline. Für die Liste der Kommandos und
ihrer Beschreibung siehe Abschnitt "Fernsteuerungskommandos"


+ VT100-Ausgabe

Anstatt einer LCD-Anzeige kann die komplette Ausgabe über ein VT100-Terminal
erfolgen (siehe VT100 im Abschnitt LCD-Module). Um ein Durcheinander im Layout
der Ausgabe zu vemeiden, werden die anderen Optionen für die serielle Schnitt-
stelle deaktiviert.


* Einschalten

Ein langer Tastendruck beim Einschalten aktiviert den Auto-Hold-Modus. In
diesem Modus wartet der Tester nach einer Ausgabe auf einen kurzen Tastendruck,
um mit der Bauteilesuche weiter zu machen. Ansonsten läuft der Tester im
kontinuierlichen Modus. Die Auswahl des Modus läßt sich per config.h (
UI_AUTOHOLD) umdrehen. Nach dem Einschalten wird kurz die Firmwareversion
angezeigt.

Mit einem sehr langen Tastendruck (2s) beim Einschalten kannst Du die
Abgleichwerte auf ihre Standards zurück setzen. Das kann praktisch sein, wenn
z.B. der Kontrast vom LCD-Modul so verstellt ist, daß man nichts mehr sieht. 

Wenn der Tester ein Problem mit den gespeicherten Abgleichwerten entdeckt (
Problem mit dem EEPROM), zeigt er einen Prüfsummenfehler an und benutzt die
Standardwerte stattdessen.


* Bauteilesuche

Nach dem Einschalten sucht Tester automatisch nach Bauteilen. Im
kontinuierlichen Modus wiederholt der Tester die Suche nach einer kurzen
Wartepause. Wenn mehrfach hintereinander kein Bauteil gefunden wurde, schaltet
sich der Tester von selbst aus. Im Auto-Hold-Modus (durch Cursor signalisiert)
führt der Tester einen Suchvorgang aus und wartet dann auf Tastendruck bzw.
Rechtsdrehung vom Drehencoder. 

In beiden Modi kannst Du das Hauptmenü aufrufen (siehe weiter unten).


* Ausschalten

Während das Ergebnis der letzten Bauteilesuche angezeigt wird, schaltet ein
langer Tastendruck den Tester aus. Dabei zeigt der Tester ein kurzes "Auf
Wiedersehen" und schaltet sich dann selbst ab. Allerdings bleibt der Tester
solange noch eingeschaltet, wie die Taste gedrückt gehalten wird. Das liegt
am Design des Schaltungsteils der Stromversorgung. 


* Menü

Durch zweimaliges kurzes Drücken der Test-Taste nach der Ausgabe des letztes
Ergebnisses gelangt man in das Menü. Einfach zweimal kurz hintereinander drücken.
Kann vielleicht etwas Übung am Anfang benötigen ;) Wenn ein Drehencoder
vorhanden ist, startet zusätzlich eine Linksdrehung das Menü. Die alte Methode
über den Kurzschluß der drei Testpins kann ebensfalls aktiviert werden
(UI_SHORT_CIRCUIT_MENU).

Im Menü wählt ein kurzer Tastendruck den nächsten Punkt aus und ein langer
Tastendruck führt den ausgewählten Punkt aus. Bei einem LCD-Modul mit 2 Zeilen
wird unten rechts eine Navigationshilfe angezeigt. Ein ">", wenn weitere
Punkte folgen, oder ein "<" beim letzten Punkt. Geht man weiter als der letzte
Punkt, gelangt man wieder zum ersten. Bei einem LCD-Modul mit mehr als 2
Zeilen wird der ausgewählte Punkt mit einem "*" davor gekennzeichnet.

Ist ein Drehencoder vorhanden, wird mit dem Drehen der vorherige bzw. nächste
Punkt augewählt. Hier gibt es auch wieder einen Überlauf, d.h. vom ersten zum
letzten Punkt. Ein kurzer Tastendruck führt den Punkt aus, im Gegensatz zu
oben.

Manche Punkte/Extras zeigen beim Start das Pinout der benutzten Testpins kurz
an. Die Info wird für ein paar Sekunden gezeigt, kann aber mit einem kurzen
Tastendruck übersprungen werden.

Funktionen, welche Signale erzeugen, geben ihr Signal standardmäßig auf
Testpin #2 aus. Dabei werden die Pins #1 und #3 auf Masse gesetzt. Ist Dein
Tester für die Signalausgabe auf einem eigenen Ausgang (OC1B) konfiguriert, 
werden die Testpins nicht genutzt und es erfolgt auch keine Ausgabe des
Pinout.


+ PWM-Generator

Macht genaus das, was Du erwartest :-) Vor dem Übersetzen der Firmware bitte
entweder den PWN-Generator mit einfacher Bedienung oder den mit erweiterter
Bedienung auswählen. Letzterer benötigt einen Drehencoder und ein größeres
Display.

Beschaltung bei Signalsusgabe über die Testpins: 
  Pin #2:          Ausgang (680 Ohm Widerstand zur Strombegrenzung)
  Pin #1 und #3:   Masse


- Einfache PWM

Zuerst muß man aus einer vorgegeben Liste die Frequenz wählen. Kurzer Tasten-
druck für die nächste Frequenz und langer Tastendruck zum Starten, wie beim
Menü. Mit Drehencoder ein kurzer Tastendruck zum Starten.

Das Tastverhältnis startet bei 50% und kann in 5%-Schritten geändert werden.
Ein kurzer Tastendruck für +5% und ein langer für -5%. Zum Beenden die Test-
Taste zweimal kurz hintereinander drücken.

Ist ein Drehencoder vorhanden, läßt sich das Tastverhältnis in 1%-Schritten
ändern.


- Erweiterte PWM

Mit einem kurzen Tastendruck schaltest Du zwischen Frequenz und Tastverhältnis
um. Der ausgewählte Wert wird durch ein Sternchen markiert. Mit dem Dreh-
encoder änderst Du den Wert, rechts für höher, links für niedriger. Und mit
einem langen Tastendruck wird auf den Standardwert zurück gestellt (Frequenz:
1kHz, Tastverhältnis: 50%). Mit zwei kurzen Tastendrücken wird der PWM-
Generator beendet.


+ Rechteck-Signalgenerator

Der Signalgenerator gibt ein Rechtecksignal mit variabler Frequenz bis zu
einem 1/4 des MCU-Taktes aus (2MHz bei 8MHz Takt). Die Startfrequenz liegt
bei 1000Hz und kann mit dem Drehencoder geändert werden. Die Dreh-
geschwindigkeit bestimmt den Grad der Änderung, d.h. langsames Drehen ergibt
kleine Änderungen und schnelles Drehen große. Da die Signalerzeugung auf
der internen PWM-Funktion der MCU basiert, können nicht beliebige Frequenzen
generiert werden, sondern nur in Schritten. Für niedrige Frequenzen ist die
Schrittweite recht klein, erst bei hohen Frequenzen wird sie signifikant.
Ein langer Tastendruck stellt die Frequenz zurück auf 1kHz und zwei kurze
Tastendrücke beenden den Signalgenerator.

Beschaltung bei Signalsusgabe über die Testpins:
  Pin #2:          Ausgang (680 Ohm Widerstand zur Strombegrenzung)
  Pin #1 und #3:   Masse

Hinweis: Drehencoder oder andere EIngabeoption notwendig!


+ Zenertest (Hardware-Option)

Mit Hilfe eines DC-DC-Konverters wird eine Testspannung von bis zu 50V zum
Testen von Zenerdioden generiert. Der Anschluß erfolgt über zusätzliche
Testpins. Solange die Test-Taste gedrückt wird, erzeugt der Konverter die
Testspannung und die aktuelle Spannung wird angezeigt. Nach dem Loslassen
der Taste wird die kleinste gemessene Spannung angezeigt, sofern der Test
ausreichend lange für eine stabile Testspannung lief. Dieser Vorgang kann
beliebig oft wiedrholt werden. Zum Beenden die Test-Taste zweimal kurz
hintereinander drücken.

Beschaltung für Zenerdiode:
   Pin +: Kathode
   Pin -: Anode


+ ESR-Messung

Die ESR-Messung kann den Kondensator in der Schaltung messen und zeigt
neben der Kapazität den ESR an, wenn ein Kondensator tatsächlich entdeckt
wird. Stelle sicher, daß der Kondenstor vor dem Anschließen entladen wurde!
Die gemessenen Werte können von einer Messung außerhalb der Schaltung wegen
parallel geschalteter Bauteile abweichen.

Um die Messung zu starten, kurz die Test-Taste drücken. Zum Beenden die
Test-Taste zweimal kurz hintereinander drücken.

Beschaltung für Kondensator:
   Pin #1:  Plus
   Pin #3:  Minus


+ Kondensatorleckstrom

Der Test auf Leckstrom lädt einen Kondensator auf und zeigt dabei den Strom
und die Spannung über den Messwiderstand an. Das Laden beginnt mit Rl (680 Ohm)
und schaltet auf Rh (470kOhm) um, sobald der Strom einen bestimmten Grenzwert
unterschreitet.

Jeder Testzyklus beginnt mit der Anzeige der Belegung der Testpins. Nach dem
Verbinden des Kondensators startet ein Druck der Testtaste das Laden (oder
Rechtsdrehung bei einem Drehencoder). Ein weiterer Druck beendet das Laden,
und der Tester entlädt den Kondensator während die Restspannung angezeigt wird.
Sobald der Entladegrenzwert erreicht ist, startet der Tester einen neuen
Testzyklus. Zum Verlassen des Tests zweimal kurz die Testtaste drücken.

Hinweis: Auf Polarität von Elkos achten!

Beschaltung für Kondensator:
   Pin #1:  Plus
   Pin #3:  Minus


+ Frequenzzähler (Hardware-Option)

Den Frequenzzähler gibt es in zwei Versionen. Der Einfache besteht aus einem
passiven Eingang auf den T0-Pin der MCU. Und der Erweiterte hat neben einem
Eingangspuffer auch zwei Oszillatoren zum Testen von Quartzen (für niedrige
und hohe Frequenzen) und einen zusätzlichen Frequenzvorteiler. Beide
Schaltungen sind in der Dokumentation von Karl-Heinz beschrieben.


- Einfacher Zähler

Ist die Zusatzschaltung für den einfachen Frequenzzähler eingebaut, kannst Du
damit Frequenzen von ca. 10Hz bis zu 1/4 der MCU-Taktfrequenz mit einer
Auflösung von 1Hz bei Frequenzen unterhalb von 10kHz messen. Die Frequenz wird
ständig gemessen und angezeigt. Ein Tastendruck beendet die Messung. Die
automatische Bereichswahl setzt die Torzeit auf Werte zwischen 10ms und 1000ms,
je nach Frequenz.


- Erweiterter Zähler

Der erweiterte Frequenzzähler hat einen zusätzlichen Vorteiler, welcher die
Messung höherer Frequenzen erlaubt. Das theoretische Maximum liegt bei 
1/4 des MCU-Taktes multipliziert mit dem Vorteiler (16:1 or 32:1). Die Steuer-
signale werden in config_<mcu>.h definiert, und bitte nicht vergessen, in
config.h den korrekten Vorteiler auszuwählen.

Der Signaleingang (gepufferter Eingang, Quartz-Oszillator für niedrige
Frequenzen, Quartz-Oszillator für hohe Frequenzen) wird über die Testtaste oder
den Drehencoder geändert. Zwei kurze Tastendrücke beenden den Frequenzzähler.


+ Drehencoder

Diese Funktion testet Drehencoder und bestimmt das Pinout. Deine Aufgabe ist
es, die Testpins an den Drehencoder (A, B, Common) anzuschließen und den
Encoder nach rechts (also Uhrzeigersinn) zu drehen. Der Algorithmus benötigt
4 Grey-Code-Schritte zur Erkennung. Die Drehrichtung ist wichtig zur Erkennung
von A und B, da eine falsche Richtung zur Verdrehung der Pins führen würde. 

Wenn ein Drehencoder entdeckt wird, gibt der Tester die Pinbelegung aus und
wartet auf einen Tastendruck beim Auto-Hold-Modus oder wartet kurz beim
kontinuierlichen Modus. Zum Beenden die Test-Taste kurz während eines
Suchlaufs drücken.


+ Kontrast

Für manche grafische LCD-Module kannst Du den Kontrast einstellen. Ein kurzer
Tastendruck erhöht den Wert, ein langer verkleinert ihn. Zum Beenden die
Test-Taste zweimal kurz hintereinander drücken. Ist ein Drehencoder
vorhanden, kann der Kontrastwert damit ebenfalls geändert werden. 


+ Detektor/Decoder für IR-Fernbedienungen

Diese Funktion erkennt und dekodiert Signale von IR-Fernbedienungen und
benötigt ein IR-Empfängermodul, wie z.B. aus der TSOP-Serie. Beim Übersetzen
der Firmware kannst Du zwischen zwei Anschlussvarianten wählen. Bei der ersten
Variante wird das Moduls mit den normalen Testpins verbunden. Die zweite
Variante ist ein festes Modul, welches mit einem bestimmten MCU-Pin verbunden
ist.

Wenn ein bekanntes Protokoll erkannt wird , gibt der Tester das Protokoll,
Adresse (sofern verfügbar), Kommando und ggf. zusätzliche Informationen
hexadezimal aus. Das Ausgabeformat ist:
  <Protokoll> [<Adresse>:]<Kommando>[:<Extras>]
Bei einem defekten Datenpaket wird hinter dem Protokoll ein "?" ausgegeben.

Ist das Protokoll unbekannt, zeigt der Tester die Anzahl der Pausen & Pulse
und die Dauer des ersten Puls und der ersten Pause in Einheiten von 50µs an:
  ? <Pulse>:<erster Pulse>-<erste Pause>
Wenn die Anzahl der Pulse bei verschiedenen Tasten der Fernbedienung gleich
bleit, ist die Modulation sehr wahrscheinlich PDM oder PWM. Eine sich
ändernde Anzahl von Pulsen weist auf Bi-Phase-Modulation hin. 

Zum Beenden die Test-Taste einmal kurz drücken.

Unterstützte Protokolle:
  - JVC
  - Kaseikyo (Japancode, 48 Bit)
  - Matsushita (Panasonic MN6014, C6D6 / 12 bits)
  - Motorola
  - NEC (Standard & Erweitert)
  - Proton / Mitsubishi (M50560)
  - RC-5 (Standard)
  - RC-6 (Standard)
  - Samsung / Toshiba (32 Bit)
  - Sharp
  - Sony SIRC (12, 15 & 20 Bit)

Optionale Protokolle:
  - IR60 (SDA2008/MC14497)
  - Matsushita (Panasonic MN6014, C5D6 / 11 bits)
  - NEC µPD1986C
  - RECS80 (Standard & Erweitert)
  - Sanyo (LC7461)
  - Thomson

Die Trägerfrequenz vom TSOP IR-Empfängermodul muß nicht genau zur Fernsteuerung
passen. Es verringert sich eigentlich nur die Reichweite, was für unseren Zweck
aber kein Problem darstellt.


- IR-Empfängermodul an Testpins

Das IR-Empfängermodul bitte erst im IR-Fernbedienungsdetektor anschliessen!

Beschaltung für das TSOP-Modul:
  Probe #1:  Masse/Gnd
  Probe #2:  Vs (680 Ohm Widerstand zur Strombegrenzung)
  Probe #3:  Data/Out

Hinweis:
  Der Widerstand zur Strombegrenzung setzt ein IR-Empfängermodul mit einem
  Versorgungsspannungsbereich von ca. 2,5 - 5V voraus. Wenn Du ein 5V-Modul
  hast, kannst Du in config.h den Widerstand auf eigene Gefahr abschalten.
  Ein Kurzschluss kann allerdings die MCU zerstören.


- Festes IR-Empfängermodul

Für das feste Modul bitte Port und Daten-Pin in config_<MCU>.h passend setzen.


+ IR-Fernbedienung

Die IR-Fernbedienung sendet Fernbedienungscodes, welche Du zuvor eingegeben
hast, und dient zum Testen von IR-Empfängern bzw. von Geräten mit IR-
Fernbedienung. Diese Funktion benötigt eine zusätzliche Eingabeoption, wie
z.B. ein Drehencoder, ein Display mit mehr als vier Textzeilen und eine
einfache Treiberschaltung für die IR-LED.

Der Tester zeigt Dir das Protokoll, die Trägerfrequencz, das Tastverhältnis
des Trägers und ein paar Datenfelder. Mit einem kurzen Druck der Test-Taste
schaltest du zwischen den Punkten hin und her. Der ausgewählte Punkt wird
durch ein "*" gekennzeichnet. Über den Drehencoder (oder andere Eingabeoption)
änderst Du die Einstellung bzw. den Wert eines Punktes.  Bei einem langen
Druck der Test-Taste sendet der Tester den IR-Code solange die Test-Taste
gedrückt bleibt. Und wie üblich beenden zwei kurze Tastendrücke die Funktion.

Wenn Du das Protokoll änderst, werden Trägerfrequenz und Tastverhältnis auf
die Standardwerte des jeweiligen Protokolls gesetzt. Du kannst diese aber
nach Belieben ändern. Die Trägerfrequenz kann auf 30 bis 56 kHz gestellt
werden, und das Tastverhältnis auf  1/2 (50%), 1/3 (33%) oder 1/4 (25%). Die
Datenfelder sind die Teile des Fernbedienungscodes, die Du setzen kannst. Sie
werden weiter unten erklärt und sind meistens nur die Adresse und das Kommando.

Unterstützte Protokolle und ihre Datenfelder:

  - JVC
    <Adresse:8> <Kommando:8>

  - Kaseikyo (Japanese Code)
    <Hersteller:16> <System:4> <Produkt:8> <Funktion:8>

  - Matsushita (Panasonic, MN6014 12 bits)
    <Gerät:6> <Taste:6>

  - Motorola
    <Kommando:9>

  - NEC Standard
    <Adresse:8> <Kommando:8>

  - NEC Extended
    <Adresse:16)> <Kommando:8>

  - Proton / Mitsubishi (M50560)
    <Adresse:8> <Kommando:8> 

  - RC-5 Standard
    <Adresse:5> <Kommando:6>

  - RC-6 Standard, Mode 0
    <Adresse:8> <Kommando:8>

  - Samsung / Toshiba (32 bits)
    <Gerät:8> <Taste:8>

  - Sharp / Denon
    <Adresse:5> <Kommando:8> <Maskierung:1>

  - Sony SIRC-12
    <Kommando:7> <Adresse:5>

  - Sony SIRC-15
    <Kommando:7> <Adresse:8>

  - Sony SIRC-20
    <Kommando:7> <Adresse:5> <Erweitert:8>

Optionale Protokolle:

  - Thomson
    <Gerät:4> <Funktion:7>

Die Datenfelder sind durch Leerzeichen getrennt und ihre Syntax ist:
  <Feldname>:<Anzahl Bits>

Beschaltung bei Signalsusgabe über die Testpins:
  Pin #2:          Signalausgang (680 Ohm Widerstand zur Strombegrenzung)
  Pin #1 und #3:   Masse

Der Signalausgang (Test-Pin #2) hat einen Widerstand zur Strombegrenzung und
kann eine IR-LED mit nur etwa 5mA direkt schalten, was für eine typische IR-LED
mit einem If von 100mA nicht ausreichend ist. Daher wird ein einfacher Treiber
auf Basis eines Transistors, der IR-LED und einem Widerstand zur Strom-
begrenzung benötigt.

Beispielschaltung für einen Treiber, welcher die IR-LED (Vf 1.5V, If 100mA) mit
50mA schaltet:

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


+ Opto-Koppler-Test

Dieser Test prüft Opto-Koppler und gibt V_f der LED, den CTR-Wert (auch If)
und t_on bzw. t_off Zeiten (für Transistortypen) aus. Unterstützt werden
Standard-NPN-Transistoren, NPN-Darlington-Stufen und TRIACs. Für die CTR-
Messung wird der I/O-Pin der MCU kurzzeitig für ca. 3ms überlastet. Das
Datenblatt gibt einen maximalen Ausgangsstrom vom 20mA an, wir überlasten
den Pin aber bis zu ca. 100mA. Daher ist der maximale CTR-Wert begrenzt, und
Werte über 2000% sollte man mit Vorsicht geniessen.

Der maximale Strom für die LED ist 5mA, was bei TRIAC-Typen zu beachten ist.
Relais-Typen (MOSFET back to back) werden als Transistor erkannt und der
CTR-Wert ist dann bedeutungslos. Typen mit anti-parallelen LEDs werden
ignoriert.

Zum Testen brauchst Du einen einfachen Adapter mit folgenden drei Testpunkten:

Transistor-Typ:
- Anode der LED
- Kathode der LED und Emitter vom Transistor miteinander verbunden
- Kollektor vom Transistor

TRIAC-Typ:
- Anode der LED
- Kathode der LED und MT1 vom TRIAC miteinander verbunden
- MT1 vom TRIAC

Du kannst den Adapter nach Belieben mit den drei Testpins vom Tester
verbinden. Der Tester findet die Anschlußbelegung dann automatisch.

Nach dem Starten bitte den Adapter mit den Testpins vom Tester verbinden und
kurz die Taste zum Prüfen drücken. Wenn ein Opto-Koppler gefunden wurde, zeigt
der Tester den Typen und verschiedene Infos an. Wurde keiner erkannt, erfolgt
die Anzeige von "keiner". Ein blinkender Cursor weist darauf hin, daß ein
Tastendruck für die nächste Prüfung erwartet wird. Zwei kurze Testendrücke
beenden, wie üblich, den Test.


+ Modellbau-Servo-Test

Diese Funktion erzeugt ein PWM-Signal für Modellbau-Servos, welche mit einem
1-2 ms PWM-Puls gesteuert werden. Die typischen PWM-Frequenzen von 50, 125,
250 und 333Hz werden unterstützt, und die Pulselänge ist im Bereich von 0,5
bis 2,5 ms einstellbar. Zusätzlich gibt es einen Sweep-Modus für Pulse
von 1 - 2ms und wählbarer Geschwindigkeit. 

Die Pulsbreite stellst Du mit dem Drehencoder ein. Links für kürzere Pulse,
und rechts für längere. Mit einem langen Tastendruck wird die Pulsweite auf
1,5ms zurück gesetzt (mittlere Position vom Servo).

Mit einem kurzen Tastendruck wechselst Du zwischen Puls- und Frequenzauswahl (
durch ein Sternchen markiert). In der Frequenzauswahl schaltest Du mit dem
Drehencoder zwischen den Frequenzen um. Mit einem langen Tastendruck wird der
Sweep-Modus ein- bzw. ausgeschaltet (durch ein "<->" markiert).

Solange der Sweep-Modus eingeschaltet ist, wird die Pulslänge durch die
Sweep-Zeit ersetzt, welche mittels dem Drehencoder geändert werden kann.

Wie üblich beenden zwei kurze Tastendrücke die Funktion.

Beschaltung bei Signalsusgabe über die Testpins:
  Pin #2:          PWM-Ausgang (680 Ohm Widerstand zur Strombegrenzung)
  Pin #1 und #3:   Masse

Hinweis: Für den Servo benötigst Du eine zusätzliche Stromversorgung.

Pinbelegungen für typische 3-Pin-Servo-Stecker:

Hersteller     Pin 1               Pin 2               Pin 3
---------------------------------------------------------------------- 
Airtronics     PWM weiss/schwarz   Gnd schwarz         Vcc rot
Futaba         PWM weiss           Vcc rot             Gnd schwarz
hitec          PWM gelb            Vcc rot             Gnd schwarz
JR Radios      PWM orange          Vcc rot             Gnd braun


+ Temperatursensor DS18B20

Hiermit wird der OneWire Temperatursensor DS18B20 ausgelesen. Zum Einrichten
des OneWire-Busses siehe bitte den Abschnitt "Busse & Schnittstellen". Bei
Benutzung der Test-Pins informiert der Tester über die Beschaltung und wartet
bis ein externer Pull-Up-Widerstand erkannt wurde. Mit einem Tastendruck läßt
sich dies überspringen. Nach den Verbinden des DS18B20 als einziger Client am
Bus startet ein Tastendruck das Auslesen der Temeperatur (kann fast eine
Sekunde dauen). Zum Beenden zweimal kurz die Test-Taste drücken.


+ Selbsttest

Wenn Du den Selbsttest über das Menü gestartet hast, bittet dich der Tester
die drei Testpins kurz zu schließen und wartet solange, bis er dies erkennt.
Bei Problemen kannst Du das Warten mit einem Tastendruck abbrechen.

Der Selbsttest führt jeden Test 5-mal aus. Mit einem kurzen Tastendruck wird
der aktuelle Test übersprungen, mit einem langen Tastendruck der komplette
Test.

In Test #4 ist der Kurzschluß wieder zu entfernen. Der Tester wartet dann
so lange.

Die Testschritte sind:
- T1 interne Spannungsreferenz (in mV)
- T2 Vergleich der Rl-Widerstände (Offset in mV)
- T3 Vergleich der Rh-Widerstände (Offset in mV)
- T4 Entfernen des Kurzschlusses der Testpins/kabel
- T5 Leckstromtest für Testpins mit Gnd-Pegel (Spannung in mV)
- T6 Leckstromtest für Testpins mit Vcc-Pegel (Spannung in mV)


+ Selbstabgleich

Der Selbstabgleich misst den Widerstand und die Kapazität der Messkabel, d.h.
von Platine, interner Verkabelung und dem Messkabel als Summe, um einen
Nulloffset zu bestimmen. Auch wird der interne Widerstand der MCU-Portpins
im Pull-Up und Pull-Down Modus bestimmt. Wenn der Abgleich übersprungen wird 
oder unplausible Werte gemessen werden, werden die Standardwerte der Firmware
angenommen. Wenn alles sauber durch läuft, werden die neuen Werte angezeigt,
aber nicht im EEPRON gespeichert (siehe Speichern).

Der Spannungsoffset des Analogkomparators wird automatisch bei der Messung
eines Kondensators bestimmt (bei der normalen Bauteilesuche), wenn der
Kondensator einen Wert zwischen 100nF und 3,3µF hat. Außerdem wird
gleichzeitig der Offset der internen Spannungsreferenz gemessen.

Bevor der Selbstabgleich ausgeführt wird, solltest Du einen Folienkondensator
mit einer Kapazität zwischen 100nF und 3,3µF min. 3-mal messen, damit die oben
erwähnten Offsets bestimmt werden können. Typischerweise liefert die erste
Messung einen zu niedrigen Wert, die zweite einen zu hohen und erst die dritte
eine korrekten Wert. Das wird durch die sich selbst abgleichenden Offsets
verursacht.

Mit einem festen Kondensator zum Selbstabgleich wird der automatische Abgleich
in der Kapazitätsmessung durch eine eigene Funktion ersetzt, welche während
des Selbstabgleichs ausgeführt wird. Somit brauchst Du nicht mehr vorher
einen Folienkondensator zu messen.

Der Selbstabgleich ist dem Selbsttest vom Ablauf und der Bedienung her sehr
ähnlich.

Die Schritte des Selbstabgleichs sind:
- A1 Offsets für interne Spannungsreferenz und Analogkomparator
     (nur bei festem Abgleichkondensator)
- A2 Widerstand der Testpins/kabel (in 10mOhm)
- A3 Entfernen des Kurzschlusses der Testpins/kabel
- A4 interner Widerstand der Portspins für Gnd (Spannung über RiL)
- A5 interner Widerstand der Portspins für Vcc (Spannung über RiH)
- A6 Kapazität der Testpins/kabel (in pF)

Erlaubte Maximalwerte:
- Widerstand Testpin/Kabel < 1,50 Ohm (zwei in Reihe)
- Kapazität Testpin/Kabel < 100pF

Hinweis: Wenn der Widerstandswerte der Testpins zu sehr variieren, könnte ein
Kontaktproblem vorliegen.

Merke: Abgleich ist nicht Kalibrierung! Kalibrierung ist die Prozedur, 
Messergebnisse mit verfolgbaren Standards zu vergleichen und die Abweichungen
zu notieren. Der Zweck ist die Überwachung der Abweichungen über die Zeit. Der
Abgleich ist die Prozedur, ein Messgerät so einzustellen, daß es seine
Vorgaben bzgl. Genauigkeit und anderer Parameter einhält.


+ Speichern/Laden

Beim Brennen der Firmware wird ein Satz vordefinierter Standardardwerte in das
EEPROM geschrieben. Nach dem Selbstabgleich kannst mit dieser Funktion die
Standardwerte durch die korrekten Werte überschreiben. Beim nächsten Neustart
vom Tester werden dann diese Werte (Profil #1) automatisch geladen und 
benutzt.

Zur Bequemlichkeit stehen zwei Profile zum Speichern bzw. Laden zu Verfügung,
z.B. für zwei unterschiedliche Sätze an Messkabeln.

Die Idee hinter der manuellen Speichern-Funktion ist, daß man z.B. beim
temporären Wechsel der Messkabel nur einen Selbstabgleich macht und nach
dem Neustart wieder die Werte für die Haupt-Messkabel hat. Ansonsten müsste
man für die alten Kabel wieder einen neuen Selbstabgleich machen.


+ Werte anzeigen

Diese Funktion zeigt die aktuellen Abgleichwerte an. Die Nutzung einer
externen 2.5V Spannungsreferenz wird mit einem "*" nach Vcc signalisiert.


+ Exit

Damit kannst Du das Menü verlassen, wenn Du z.B. aus Versehen reingegangen
bist.


* Widerstände

Widerstände werden zweimal gemessen. d.h. in beide Richtungen, und die Werte
dann verglichen. Wenn die Werte zu unterschiedlich sind, nimmt der Tester an,
daß es zwei Widerstände sind und nicht nur einer. In dem Fall zeigt die
Ausgabe zwei Widerstände mit gleichen Pins in der Form "1 -- 2 -- 1" mit den
beiden Werten an. Für Widerstände kleiner als 10 Ohm wird eine zusätzliche
Messung mit höherer Auflösung durchgeführt. 

In seltenen Fällen kann der Tester sehr kleine Widerstände nicht erkennen. Am
besten dann die Messung einfach wiederholen. 


* Kondensatoren

Die Messung von Kondensatoren ist in drei Methoden aufgeteilt. Große
Kondensatoren >47µF werden mittels Ladezyklenmethode mit Pulsen von 10ms
gemessen. Mittelgroße zwischen 4,7µF and 47µF werden ebenfalls mit der
Ladezyklenmethode gemessen, aber mit Pulsen von 1ms. Kleine Kondensatoren
laufen über den analogen Komparator. Auf dies Weise wird die Genauigkeit der
Kapazitätsmessung optimiert. 

Die Messwerte großer Kondensatoren benötigen eine eine Korrektur. Ohne
Korrektur wären die Werte zu hoch. Ich denke, daß dies durch die Messmethode
verursacht wird, da die Analog-Digital-Wandlung nach dem Ladepuls eine gewisse
Zeit benötigt und der Kondensator in dieser Zeit etwas Ladung durch Leckströme
verliert und natürlich auch durch die Analog-Digital-Wandlung selber. So dauert
es dann länger bis der Kondensator geladen ist, und die Kapazität erscheint
höher. Eine spätere Selbstentlademessung versucht dies zu kompensieren, kann es
aber nur teilweise. Die Korrekturfaktoren (CAP_FACTOR_SMALL, CAP_FACTOR_MID
und CAP_FACTOR_LARGE in config.c) sind so gewählt, daß sie für die meisten
Testermodelle passen. In manchen Fällen kann aber eine Änderung notwendig sein. 

Eine Logik zum Verhindern, daß große Kondensatoren als Widerstände erkannt
werden, wurde hinzugefügt. Widerstände kleiner als 10 Ohm werden zusätzlich
auf Kapazität geprüft.

Der Tester versucht den ESR bei Kondensatoren größer als 18nF zu messen.
Alternativ kannst Du auch die alte Messmethode aktivieren, welche ab 180nF
mißt. Da aber die Messung nicht per Wechselstrom mit einer bestimmten Frequenz
durchgefüghrt wird, bitte keine supergenauen Ergebnisse erwarten. Die benutzte
Methode entspricht vielleicht der Messung mit 1kHz. Für die Prüfung von Elkos
ist die Messung mehr als ausreichend. Bei Filmkondensatoren mit kleinen Werten,
können je nach MCU-Taktrate unterschiedliche Werte zustande kommen. Ich denke,
Herr Fourier könnte dies erklären.

Ein gemessener Kapazitätswert von mehr als 5pF (incl. Nulloffset) wird
als gültig angesehen. Niedrigere Werte sind zu ungenau und könnten durch
Verschiedenes, wie z.B. anderes Platzieren der Messkabel, verursacht werden.

Eine weitere Messung ist der Selbstentladungsleckstrom für Kodensatoren größer
als 4,7µF. Der Wert gibt z.B. einen Hinweis auf den Zustand eines Elkos.
Von meinen Tests her scheinen folgende Werte typisch für Elkos zu sein:  
  - 10-220µF    1-3µA
  - 330-470µF   4-5µA
  - 470-820µF   4-7µA
  - >1000µF     5-7µA pro 1000µF


* Induktivitäten

The Induktivitätsmessung ist nicht sonderloch genau, und Dinge wie z.B. der
MCU-Takt und die Platine haben Auswirkungen auf das Ergebnis. Die Messung
selber basiert auf der Bestimmung der Zeit zwischen dem Einschalten des
Stroms und dem Erreichen eines bestimmten Stroms. Für große Induktivitäten
gibt es eine Niedrigstrommessung, und für kleine Induktivitäten eine Hoch-
strommessung, welche das Stromlimit der MCU-Pins kurzzeitig überschreitet (bis
zu etwa 25 Microsekunden).

Bei der Untersuchung der Effekte des MCU-Takts und anderer Dinge habe ich ein
Muster an Abweichungen gefunden, welches zum Kompensieren genutzt werden kann.
Je nach Tester kann eine Anpassung notwendig sein. In der Datei inductance.c
in der Funktion MeasureInductor() gibt es die Variable "Offset" zur
Kompensation. Sie ist ein Offset für die Referenzspannung. Ein positiver
Wert verkleinert die Induktivität, und ein negativer Wert vergrößert die
Induktivität.

Die Kompensation für die Hochstrommessung basiert auf den MCU-Takt und ist in
drei Zeitbereiche unterteilt. Für die Niedrigstrommessung gibt es momentan nur
eine einfache Kompensation, da hier noch weitere Tests notwendig sind. Wenn Du
größere Abweichungen beim Vergleich mit einem "richtigen" LCR-Meter siehst,
kannst Du die Offsetwerte entsprechend deinem Tester anpassen.

Hinweis: Bei unerwarteten Messwerten bitte die Messung wiederholen.


* Bauteile entladen

Die Entladefunktion basiert nicht auf einem festen Timeout, sondern paßt
sich automatisch dem Entladefortschritt an. Auf diese Weise wird eine
Batterie schneller erkannt und große Kondensatoren erhalten mehr Zeit zum
Entladen. Sollte ein großer Elko als Batterie erkannt werden, bitte nochmal
versuchen. In einer Umgebung mit vielen elektrischen Störungen könnte auch
der Nullwert CAP_DISCHARGED mit 3mV zu niedrig sein (ggf. anpassen). Die
Anzeige der Restspannung hilft beim Finden des optimalen Nullwertes.


* ADC Oversampling

Die ADC-Funktion unterstützt ein variables Oversampling (1 - 255). Der
Standardwert ist 25 Samples. Du kannst versuchen, durch Erhöhen des
Oversamplingwerts die Genauigkeit des Testers zu erhöhen. Allerdings
steigt mit einem höheren Wert auch die benötigte Zeit, d.h. die Messungen
werden langsamer.


* V_BE von Bipolartransistoren

Beim Test auf Dioden wird Vf zum einen mit Rl (hoher Teststrom) und zum
anderen mit Rh (niedriger Teststrom) gemessen. Die Ausgabefunktion für
Bipolartransistoren interpoliert aus den beiden Vf-Werten V_BE abhängig vom
hFE für einen virtuellen Teststrom. Somit erhält man praxisnahe Werte, da V_BE
eines Kleinsignaltransistors mit einem anderen Strom gemessen wird als bei
einem Leistungstransistor.


* Ergebnisanzeige

Einige Namen und Abkürzungen wurden geändert. Die Ausgabe mancher Bauteile
wird auf mehrere Seiten aufgeteilt, wenn das LCD-Modul nicht ausreichend viele
Zeilen hat.

Bei einer einzelnen Diode wird das Vf der Messung mit niedrigem Teststrom (
10µA) in Klammern angezeigt, wenn der Wert unter 250mV liegt. Damit erhält man
einen Hinweis auf eine Germaniumdiode. Die meisten Datenblätter von Germanium-
dioden geben einen Messtrom von 0,1mA für Vf an. Leider unterstützt der Tester
nicht diesen Messstrom. Und für höhere Ströme liegt Vf bei etwa 0,7V, was eine
Unterscheidung zu Siliziumdioden schwierig macht. 

Der Leckstrom I_R für eine einzelne Diode bzw. I_CEO für einen Bipolar-
Transistor wird ausgegeben, sofern er höher als 50nA ist. Germanium-
transistoren haben einen Leckstrom im Bereich von wenigen µA bis etwa 500µA.
Germaniumdioden liegen üblicherweise bei ein paar µA.

Für manche Bauteile wird ein Kapazitätswert angezeigt. Liegt die Kapazität
unterhalb von 5pF oder die Messung schlug fehl, wird 0pF ausgegeben.

Wenn ein Verarmungs-FET mit symmetrischem Drain und Source gefunden wird,
zeigt die Pinbelegung ein 'x' statt 'D' oder 'S', da beide vom Tester nicht
unterschieden werden können, sie sind funktional identisch. In diesem Fall
bitte im Datenblatt nach Details der Pinbelegung schauen.

Die Pinbelegung eines Triac wird mit den Pins 'G', '1' and '2'. angezeigt.
'1' ist MT1 und '2' ist MT2. Und für einen UJT, sofern die Erkennung aktiviert
ist, ist es '1' für B1, '2' für B2 und 'E' für den Emitter.

Wenn die "Fancy Pinout" Funktion aktiviert wurde (über das Setzen einer
Symboldatei in config.h), wird das Bauteilesymbol mit den entsprechenden
Testpins für 3-Pin-Halbleiter angezeigt. Sollte nicht genügend Platz auf dem
Display sein, wird die Ausgabe des Symbols übersprungen.


+ Spezielle Bauteile


Spezial-Bipolar-Transistor

Bei einem Bipolartransistor mit Basis-Emitter-Widerstand wird der Widerstand
angezeigt, aber hFE ausgelassen. Wenn der Transistor zusätzlich eine
Schutzdiode hat, kann er als Transistor oder zwei Dioden erkannt werden, je
nach Wert des Basis-Emitter-Widerstands. Im letzteren Fall werden zwei Dioden
plus Widerstand mit dem Hinweis auf einen möglichen Transistor angezeigt. Ein
niedriger Basis-Emitter-Widerstand verhindert leider die eindeutige Erkennung
des Transistors.

Ein weiterer Spezialfall ist ein Bipolartransistor mit integrierter
Schutzdiode, die sich auf dem gleichen Substrat befindet. Der integrierte
PN-Übergang erzeugt einen parasitären zweiten Transistor. Ein NPN bekommt
somit einen paristären PNP und umgekehrt. Wird ein solcher Transistor
entdeckt, wird er mit einem "+" hinter der Typenangabe gekennzeichnet.


CLD

Die Diodenprüfung erkennt eine CLD (Current Limiting Diode) als normale Diode
und gibt ihren Strom I_F als Leckstrom aus. Zu beachten ist, daß bei einer CLD
Anode und Kathode gegenüber einer normalen Diode vertauscht sind. Eine
gesonderte Erkennung von CLDs ist schwierig, da der Leckstrom einer Germanium
oder Leistungs-Schottky-Diode im Bereich von I_F (ab ca. 33µA) liegt.

Wenn eine Diode ein ungewöhnliches V_f hat, ein niedriges V_f beim Niedrig-
stromtest (zweiter Wert in Klammern) und keine Kapazität gemessen werden
konnte, dann ist es wahrscheinlich eine CLD.


* Nicht unterstütze Bauelemente

Alle Halbleiter, welche einen hohen Steuerstrom benötigen, können nicht
erkannt werden, da der Tester max. ca. 7mA Strom zum Schalten hat. Auch
liefert der Tester nur 5V, was z.B. nicht ausreichend für DIACs mit einem
V_BO von 20-200V ist.


* Bekannte Probleme

- Ein Speicher- bzw. Superkondensator, wie z.B. Panasonic NF Serie, wird als
  Diode oder zwei anti-parallele Dioden erkannt. Die Kapazitätsmessung
  kann keinen brauchbaren Wert bestimmen.
- Bei Verwendung eines Schaltnetzteiles oder DC-DC-Konverters zur Strom-
  versorgung gibt der Tester manchmal fälschlicherweise einen Elko um
  die 50µF aus, obwohl kein Bauteil angeschlossen ist.
- Der ESR kann bei Kondensatoren mit 180 - 220nF je nach MCU-Takt
  variieren.


* Firmware-Änderungen

Bitte die Datei CHANGES.de lesen!


* Fernsteuerungskommandos

Wenn der Tester ein Kommando akzeptiert, antwortet er mit spezifischen Daten
oder einem der folgenden Standardtexte: 

  ERR
  - unbekanntes Kommando
  - Kommando im aktuellen Komponentenkontext nicht unterstützt
  - Pufferüberlauf

  OK
  - Kommando ausgeführt
    (manche Kommandos benötigen etwas Zeit zum Ausführen)

  N/A
  - Information/Wert nicht verfügbar

Antworten mit Daten beginnen nie mit einem der obigen Standardtexte, um
mögliche Unklarheiten zu vermeiden.


Basiskommandos:

  VER
  - gibt Version der Firmware zurück
  - to verify connectivity and to determine command set based on version
  - Beispielantwort: "1.33m"

  OFF
  - schaltet den Tester aus
  - Tester anwortet mit "OK" vor dem Auschalten
  - Beispielantwort: "OK" <Tester schaltet ab>


Testkommandos:

  PROBE
  - sucht nach Bauteil und überspringt alle Pausen für Benutzereingaben
  - Tester antwortet mit einem "OK" nach dem Beenden der Suche
  - Beispielantwort: <some time elapses for probing> "OK"

  COMP
  - gibt ID der Bauteilart zurück
  - für IDs siehe COMP_* in common.h
  - Beispielantwort für BJT: "30"

  MSG
  - gibt Fehlermeldung zurück
  - nur falls ein Fehler aufgetreten ist (COMP: 1)
  - Antwort kann von der Sprache des Benutzerinterfaces abhängen
  - Beispielantwort: "Battery? 1:1500mV"

  QTY
  - gibt Anzahl der gefundenen Bauteile zurück
  - Beispielantwort für BJT: "1"

  NEXT
  - wählt zweites Bauteil aus
  - nur wenn zwei Bauteile gefunden wurden (QTY: 2)
  - Beispielantwort: "OK"

  TYPE
  - gibt den Typen eines Bauteils zurück
  - nur für BJT, FET und IGBT
  - verfügbare Typen:
    - NPN      NPN (BJT)
    - PNP      PNP (BJT)
    - JFET     JFET (FET)
    - MOSFET   MOSFET (FET)
    - N-ch     n-Kanal (FET, IGBT)
    - P-ch     p-Kanal (FET, IGBT)
    - enh.     Anreicherungstyp (FET, IGBT)
    - dep.     Verarmungstyp (FET, IGBT)
  - Beispielantwort für BJT: "NPN"
  - Beispielantwort für FET (MOSFET): "MOSFET n-ch enh."

  HINT
  - gibt Hinweise zu speziellen Merkmalen zurück
  - nur für Diode, BJT, FET und IGBT
  - verfügbare Merkmale:
    - NPN      möglicherweise ein NPN BJT (Diode)
    - PNP      möglicherweise ein PNP BJT (Diode)
    - R_BE     Basis-Emitter-Widerstand (Diode, BJT)
    - BJT+     integrierte Freilaufdiode auf gleichem Substrat
               erzeugt parasitären zweiten BJT (BJT)
    - D_FB     Body/Freilauf-Diode (BJT, FET, IGBT)
    - SYM      Drain und Source symmetrisch (FET) 
  - Beispielantwort für BJT: "D_FB R_BE"
  - Beispielantwort für FET (MOSFET): "D_FB"

  PIN
  - gibt Anschlußbelegung zurück
  - benutzte Kennungen:
    - Widerstand   x = verbunden, - = nicht verbunden
    - Kondensator  x = verbunden, - = nicht verbunden
    - Diode        A = Anode, C = Kathode, - = nicht verbunden
    - BJT          B = Basis, C = Kollektor, E = Emitter
    - FET          G = Gate, S = Source, D = Drain, x = Drain/Source
    - IGBT         G = Gate, C = Kollektor, E = Emitter
    - SCR          G = Gate, A = Anode, C = Kathode
    - TRIAC        G = Gate, 2 = MT2, 1 = MT1
    - PUT          G = Gate, A = Anode, C = Kathode
    - UJT          E = Emitter, 2 = B2, 1 = B1
  - Format der Antwort:
    <Test-Pin #1 Kennung><Test-Pin #2 Kennung><Test-Pin #3 Kennung>
  - Beispielantwort für Widerstand: "xx-"
  - Beispielantwort für Diode: "C-A"
  - Beispielantwort für BJT: "EBC"

  R
  - gibt Widerstandswert zurück
  - nur für Widerstand (beinhaltet auch Induktiviät)
  - Beispielantwort: "122R"

  C
  - gibt Kapazitätswert zurück
  - nur für Kondensator
  - example responses: "98nF" "462uF"

  L
  - gibt Induktiviätswert zurück
  - nur für Widerstand (beinhaltet auch Induktiviät)
  - Beispielantwort: "115uH"

  ESR
  - gibt ESR-Wert zurück (Equivalent Series Resistance)
  - requires ESR measurement to be enabled
  - nur für Kondensator
  - Beispielantwort: "0.21R"

  I_l
  - gibt I_leak zurück (Leckstromequivalent zur Selbstentladung)
  - nur für Kondensator
  - Beispielantwort: "3.25uA"

  V_F
  - gibt V_F zurück (forward voltage)
  - nur für Diode und PUT
  - auch für Body-Diode von MOSFET und Freilaufdiode von BJT oder IGBT
  - Beispielantwort: "654mV"

  V_F2
  - gibt V_F der Niedrigstrommessung zurück (forward voltage)
  - nur für Diode
  - Beispielantwort: "387mV"

  C_D
  - gibt C_D zurück (Kapazität der Diode)
  - nur für Diode
  - Beispielantwort: "8pF"

  I_R
  - gibt I_R zurück (reverse current)
  - nur für Diode
  - Beispielantwort: "4.89uA"

  R_BE
  - gibt R_BE zurück (Basis-Emitter-Widerstand)
  - nur für Diode und BJT
  - Beispielantworten: "38.2R" "5171R"

  h_FE
  - gibt h_FE zurück (DC-Stromverstärkungsfaktor)
  - nur für BJT
  - Beispielantwort: "234"

  V_BE
  - gibt V_BE zurück (Basis-Emitter-Spannung)
  - nur für BJT
  - Beispielantwort: "657mV"

  I_CEO
  - gibt I_CEO rurück (Kollektor-Emitter-Strom, offene Basis)
  - nur für BJT
  - Beispielantwort: "460.0uA"

  V_th
  - gibt V_th zurück (threshold voltage)
  - nur für FET (MOSFET) und IGBT
  - Beispielantwort: "2959mV"

  C_GS
  - gibt C_GS zurück (Gate-Source-Kapazität)
  - nur für FET (MOSFET)
  - Beispielantwort: "3200pF"

  R_DS
  - gibt R_DS_on zurück (Drain-Source-Widerstand durchgeschaltet)
  - nur für FET (MOSFET)
  - Beispielantwort: "1.20R"

  I_DSS
  - gibt I_DSS zurück (Drain-Source-Strom, kurzgeschlossenes Gate)
  - nur für Verarmungstyp-FET
  - Beispielantwort: "6430µA"

  C_GE
  - gibt C_GE zurück (Gate-Emitter-Kapazität)
  - nur für IGBT
  - Beispielantwort: "724pF"

  V_GT
  - gibt V_GT zurück (Gate-Trigger-Spannung)
  - nur für Thyristor und TRIAC
  - Beispielantwort: "865mV"

  V_T
  - gibt V_T zurück (Offset-Spannung)
  - nur für PUT
  - Beispielantwort: "699mV"

  R_BB
  - gibt R_BB zurück (interbase resistance)
  - requires UJT detection to be enabled
  - nur für UJT
  - Beispielantwort: "4758R"



* Quellenverzeichnis

[1] AVR-Transistortester, Markus Frejek,
    Embedded Projects Journal, 2011-11

[2] http://www.mikrocontroller.net/topic/131804
    Forum von Markus Frejek, Forum, 2009

[3] http://www.mikrocontroller.net/articles/AVR-Transistortester
    Online-Dokumentation des Transistortesters, Online-Artikel, 2009-2011

[4] http://www.mikrocontroller.net/articles/AVR_Transistortester
    Kurze Beschreibung des TransistorTesters, Karl-Heinz Kübbeler,
    Online-Artikel, 2012



------------------------------------ EOF -------------------------------------
