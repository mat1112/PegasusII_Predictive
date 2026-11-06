/* ||  \n
2026-Nov-06

With PTT processor. With SGP4 satellite tracking. With SD card file processing. With RTC time via Wifi.
   With Ham4 Super AZ rotator and KR500 EL rotator. With IC-9700 radio CIV control.
   With serial port control/monitor and web dashboard control/monitor.

NOTE all control is now via the serial port and/or web dashboard. No buttons.

 Magnetic azimuth degrees, North being 360 degrees.

 With HAM 4 brake control and HAM 4 Super voltage sensing and remote transformer power control.

 Note. Measurement can only be made when the motors are idle and PTT is off.

 Note: Issue with direction change in ham4 super rotator. Sensor rotor lags actual degrees. 

 Quadrants: 360 deg Max (360 deg = 0 deg)
                  360   1 deg North
                      :
                 4    :   1
                      :
  270 deg West --------------  90 deg East
                      :
                 3    :   2
                max v :  min v             
                  181   180 deg South (Min/Max voltage point for Ham4)

 */

// Note to self. WiFi and ESPnow do NOT work together.

 /* web links.
    https://celestrak.org/NORAD/elements/gp.php?GROUP=amateur&amp;FORMAT=tle
    http://www.celestrak.org/NORAD/elements/gp.php?CATNR=25544&FORMAT=TLE
    https://www.amsat.org/track/?lang=en&satellite=ISS&count=10&loc=kg43br&lat=26.2719&latdir=-&lng=28.1512&longdir=-&ele=0&doPredict=+Predict+&saveme=1
 */

 /* Serial Command reference. 13D 0XDH = enter / carriage return
  0 48D 0X30H, 1 49D 0X31H, 2 50D 0X32H, 3 51D 0X33H, 4 52D 0X34H
  5 53D 0X35H, 6 54D 0X36H, 7 55D 0X37H, 8 56D 0X38H, 9 57D 0X39H
  A Command - Stop the Azimuth Rotation. 65D 0X41H
  APxxx Predict next xxx passes.
  AQ predict one pass after date/time. e.g. AQ 2026 07 20 21 44 30
  AS1 display longitude, latitude and altitude. Persisted after power off.
  AS2 set longitude e.g. "AS2 28.13511"
  AS3 set latitude e.g. "AS3 -26.2719"
  AS4 set altitude in meters e.g. "AS4 1500"
  AS5 display catalogue number. Persisted after power off.
  AS6 set ISS default catalogue number 25544
  AS7 set other catalogue number e.g. "AS7 40967"
  AS8 get TLE data from internet. Celestrak. Persisted after power off.
  AS9 show TLE elements age.
  ASa set future time manually and start tracking. e.g. "ASa 2025 10 05 05 55 00".
  ASb set time to current time and start tracking.
  ASc stop tracking.
  ASe enter sat LOS time. e.g. "ASe 2025 10 01 14 30 00" (Automatic for internal tracking.)
  ASf Verify LOSunixtime.
  ASg Set / Get AOS to LOS total Doppler frequency VHF range. e.g. ASg or ASg 6000. Persisted after power off.
  ASh Show rounded doppler changes for each 10 seconds. After Predict One Pass (as in startTracking()).
  ASi Set / Get AOS to LOS total Doppler frequency UHF range. e.g. ASi or ASi 17000. Persisted after power off.
  ASs schedule tracking for HH MM. e.g. ass 14 15.
  AX - show AZdirChangeDegIn. AX X - set AZdirChangeDegIn. Persisted after power off.
  AY1 - Logging report.  AY2 - Toggle satFileLogging.
  AY3 - Toggle satSerialLogging.  AY4 - Toggle AZRotatorLogging.
  AY5 - Toggle ELRotatorLogging.  AY6 - Toggle TalkNturnLogging.
  AY7 - Delete all logs.
  B Command - Send the Current Elevation to the PC. 66D 0X42H
  B1 - brakes off. B2 - brakes on.
  C - return requested azimuth C2 - and Elevation. C3 - get ADC degrees. 67D 0X43H
  D - Rotate Elevation Down Command. 68D 0X44H
  D0 - Doppler update to radio test.
  D1 - Reset/Prepare radio doppler frequencies.
  D2 - Rotate Down for 400ms. OR D2xxxx in ms. e.g. D20500
  D3 - toggle ReadRadio switch. Normally Off.
  D4 - Get/Set cent TX radio frequency in Hertz. e.g. D4 145990000. Persisted after power off.
  D5 - Get/Set center RX radio frequency in Hertz. e.g. D5 437800000. Persisted after power off.
  D6 - LoadRadio
  D7 - Load radio test frequency.
  D8 - Set radio mode to SIMPLEX. Toggle.
  D9 - Get radio S Meter Level value.
  DA - Full Auto Polarization State. 65D 0X41H. Persisted after power off.
  DA0 - Full Auto Polarization Off. 
  DA1 - Full Auto Polarization On. Auto overrides P commands. 
  DA2 - Full Auto Polarization Test toggle.
  DBn - Dashboard 1=ON/OFF
  DC - Memorize the Radio Doppler frequencies
  DD - Highveld repeater
  E - Stop the Elevation Rotation. 69D 0X45H
  E2 - Elevate to E2XXX
  E3 - Set EL Linear measurement. Persisted after power off.
  E4 - Set EL Non-Linear measurement. Persisted after power off.
  E5 - EL report.
  F - Set the Max Azimuth calibration. Switch off overrides etc. 70D 0X46H. Persisted after power off.
  F2 - Set the Max Elevation Calibration. Persisted after power off. Not used with KR500.
  G - Set GPredict mode. 71D 0X47H
  H - Ham4 Super North Offset info. 72D 0X48H // 20934/200849 281. Persisted after power off.
  H2 - Set North here. H3 - Reset North override to OFF.
  I - AC power delay info I (second+ count to 50). 73D 0X49H
  I2 - AC power delay test
  L - Rotate Azimuth CCW. 76D 0X4CH
  L2 - Left for 150ms OR L2xxxx in ms. e.g. L20220.
  LM - Led matrix color. LM0 = off (48D). LM1 = red. LM2 = green. LM3 = blue. LM4 = white. LM6 = yellow.
  M - Stack and Heap info.
  M - Rotate to Set Point MXXX. 77D 0X4DH
  N - SD card. Note neads ext power. 78D 0X4EH N1 = On. N2 = Off. 
  N3 List Dir.
  N4 read file. e.g. n4 /test.txt
  N5 delete file. e.g. n5 /test.txt
  N6 write file line. e.g. n6 test.txt /Hello world.
  N7 append file line. e.g. n7 test.txt /Hello two.
  O - Set the Azimuth Zero Calibration. Switch off overrides etc. 79D 0X4FH. Persisted after power off.
  O2 - Set the Elevation Zero Calibration. Not used with KR500.
  P - show Auto_polarize mode. 80D 0X50H
  P1 - set Auto_polarize to AutoPolarizeUpLHCP. P2 - set Auto_polarize to AutoPolarizeUpRHCP.
  PL - LHCP. PR - RHCP.
  PP - wifi password. e.g. PP *ionosphere1* (20 chars max.) Persisted after power off.
  PS - wifi SSID e.g. PS ZS6PEG1 (10 chars max.) Persisted after power off.
  R - Rotate Azimuth CW. 82D 0X52H
  R2 - right for 150ms OR R2xxxx in ms. e.g. R20220.
  RSt - Reset CPU
  RX - Switch radio to RX mode.
  S - Stop All Rotation. Show disable_rotators state.
  S0 - Show switches state. 83D 0X52H
  S1 - toggle PTT_disable. Default off.
  S2 - Toggle disable_rotators. 83D 0X53H
  S3 - SWR test. RX (UHF) RHCP. Default.
  S4 - SWR test. RX (UHF) LHCP
  S5 - SWR test. RX (VHF) RHCP
  S6 - SWR test. RX (VHF) LHCP
  S9 - Toggle radioCatOn. Default off.
  SA - Toggle satDisableAutoCycleTime. Default off.
  T - Reset degrees.
  T1-8 - test relays. 84D 0X54H
  T0 off. T1 pwr. T2 brake. T3 left. T4 right. T5 up. T6 down. T7 TLV. T8 polarize LHCP.
  T9 - Sync RTC clock via Wifi, or display time, once synced.
  TT - toggle turn in talk.
  TX - Switch radio to TX mode.
  TY - Test Timer 1sec toggle.
  U - Rotate Elevation Up. 85D 0X55H
  U2 - Rotate UP for 400ms. OR U2xxxx in ms. e.g. U20500
  V - Get Direction and distance from XXX to YYY (NO SPACES). 86D 0X56H e.g v001 010
  W - Toggle wifi On/Off.
  W1 - report last wifi signal strength.
  W - Rotate Azimuth and Elevation to Set Point WXXX YYY. 87D 0X57H
  X is current ADC and degrees. 88D 0X58H
  X1 is AZ debug info.
  X2 is EL debug info.
  Y - PTT mode. 89D 0X59H
  Y1 is TX on VHF (Relay normally off in RX). Y2 is TX on UHF (Relay normally on in RX).
  Y3 is TX and RX on VHF (Relay always on). Y4 is TX and RX on UHF (Relay always off).
 */

const char* cmdhelp = R"rawliteral(CMD_RESPONSE
ASCII command quick reference:<br>
<b>A</b> Command - Stop the Azimuth Rotation.<br>
<b>APxxx</b> Predict next xxx passes.<br>
<b>AQ</b> predict one pass after date/time.<br> e.g. AQ 2026 07 20 21 44 30<br>
<b>AS1</b> display longitude, latitude and altitude.<br> Persisted after power off.<br>
<b>AS2</b> set longitude e.g. "AS2 28.13511"<br>
<b>AS3</b> set latitude e.g. "AS3 -26.272"<br>
<b>AS4</b> set altitude in meters e.g. "AS4 1500"<br>
<b>AS5</b> display catalogue number.<br> Persisted after power off.<br>
<b>AS6</b> set ISS default catalogue number 25544<br>
<b>AS7</b> set other catalogue number e.g. "AS7 40967"<br>
<b>AS8</b> get TLE data from internet. Celestrak.<br> Persisted after power off.<br>
<b>AS9</b> show TLE elements age.<br>
<b>ASa</b> set future time manually and start tracking.<br> e.g. "ASa 2025 10 05 05 55 00".<br>
<b>ASb</b> set time to current time and start tracking.<br>
<b>ASc</b> stop tracking.<br>
<b>ASe</b> enter sat LOS time.<br> e.g. "ASe 2025 10 01 14 30 00"<br> (Automatic for internal tracking.)<br>
<b>ASf</b> Verify LOSunixtime.<br>
<b>ASg</b> Set / Get AOS to LOS total Doppler<br> frequency range VHF.<br> e.g. ASg or ASg 6000.<br> Persisted after power off.<br>
<b>ASh</b> Show rounded doppler changes<br> for each 10 seconds.<br> After Predict One Pass<br> (as in startTracking()).<br>
<b>ASi</b> Set / Get AOS to LOS total Doppler<br> frequency UHF range.<br> e.g. ASi or ASi 17000.<br> Persisted after power off.<br>
<b>ASs</b> schedule tracking for HH MM. e.g. ass 14 15.<br>
<b>AX</b> - show AZdirChangeDegIn.<br> <b>AX X</b> - set AZdirChangeDegIn.<br> Persisted after power off.<br>
<b>AY1</b> - Logging report.<br> <b>AY2</b> - Toggle satFileLogging.<br>
<b>AY3</b> - Toggle satSerialLogging.<br> <b>AY4</b> - Toggle AZRotatorLogging.<br>
<b>AY5</b> - Toggle ELRotatorLogging.<br> <b>AY6</b> - Toggle TalkNturnLogging.<br>
<b>AY7</b> - Delete all logs.<br>
<b>B</b> Cmd - Send the Current Elevation to the PC<br>
<b>B1</b> - brakes off. <b>B2</b> - brakes on.<br>
<b>C</b> - return requested azimuth<br> <b>C2</b> - and Elevation.<br> <b>C3</b> - get ADC degrees.<br>
<b>D</b> - Rotate Elevation Down Command.<br>
<b>D0</b> - Doppler update to radio test.<br>
<b>D1</b> - Reset radio doppler frequencies.<br>
<b>D2</b> - Rotate Down for 400ms.<br> OR D2xxxx in ms. e.g. D20500<br>
<b>D3</b> - toggle ReadRadio switch. Normally Off.<br>
<b>D4</b> - Get/Set center TX radio frequency in Hertz.<br> e.g. D4 145990000.<br> Persisted after power off.<br>
<b>D5</b> - Get/Set center RX radio frequency in Hertz.<br> e.g. D5 437800000.<br> Persisted after power off.<br>
<b>D6</b> - LoadRadio<br>
<b>D7</b> - Load radio test frequency.<br>
<b>D8</b> - Set radio mode to SIMPLEX. Toggle.<br>
<b>D9</b> - Get radio S Meter Level value.<br>
<b>DA</b> - Full Auto Polarization State.<br> Persisted after power off.<br>
<b>DA0</b> - Full Auto Polarization Off.<br>
<b>DA1</b> - Full Auto Polarization On.<br> Auto overrides P commands.<br>
<b>DA2</b> - Full Auto Polarization Test toggle.<br>
<b>DBn</b> - Dashboard 1=ON/OFF<br><br>
<b>DC</b> - Memorize the Radio Doppler frequencies<br>
<b>DD</b> - Highveld repeater.<br>
<b>E</b> - Stop the Elevation Rotation.<br>
<b>E2</b> - Elevate to E2XXX<br>
<b>E3</b> - Set EL Linear measurement.<br> Persisted after power off.<br>
<b>E4</b> - Set EL Non-Linear measurement.<br> Persisted after power off.<br>
<b>E5</b> - EL report.<br>
<b>F</b> - Set the Max Azimuth calibration.<br> Switch off overrides etc.<br> Persisted after power off.<br>
<b>F2</b> - Set the Max Elevation Calibration.<br> Persisted after power off.<br>
<b>G</b> - Set GPredict mode.<br>
<b>H</b> - Ham4 Super North Offset info.<br> Persisted after power off.<br>
<b>H2</b> - Set North here.<br> <b>H3</b> - Reset North override to OFF.<br>
<b>I</b> - AC power delay info I (second+ count to 50).<br>
<b>I2</b> - AC power delay test<br>
<b>L</b> - Rotate Azimuth CCW.<br>
<b>L2</b> - Left for 150ms. OR L2xxxx in ms. e.g. L20220<br>
<b>LM</b> - Led matrix color. LM0 = off (48D).<br> LM1 = red. LM2 = green.<br> LM3 = blue. LM4 = white.<br> LM6 = yellow.<br>
<b>M</b> - Stack and Heap info.<br>
<b>M</b> - Rotate to Set Point MXXX.<br>
<b>N</b> - SD card. Note neads ext power.<br> <b>N1</b> = On. <b>N2</b> = Off.<br>
<b>N3</b> List Dir.<br>
<b>N4</b> read file. e.g. n4 /test.txt<br>
<b>N5</b> delete file. e.g. n5 /test.txt<br>
<b>N6</b> write file line. e.g. n6 test.txt /Hello world.<br>
<b>N7</b> append file line. e.g. n7 test.txt /Hello two.<br>
<b>O</b> - Set the Azimuth Zero Calibration.<br> Switch off overrides etc.<br> Persisted after power off.<br>
<b>O2</b> - Set the Elevation Zero Calibration.<br> Persisted after power off.<br>
<b>P</b> - show Auto_polarize mode. 80D 0X50H<br> 
<b>P1</b> - set Auto_polarize to AutoPolarizeUpLHCP.<br> 
<b>P2</b> - set Auto_polarize to AutoPolarizeUpRHCP.<br>
<b>PL</b> - LHCP<br>
<b>PR</b> - RHCP<br>
<b>PP</b> - wifi password. e.g. PP *ionosphere1*<br> (20 chars max.) Persisted after power off.<br>
<b>PS</b> - wifi SSID e.g. PS ZS6PEG1 (10 chars max.)<br> Persisted after power off.<br>
<b>R</b> - Rotate Azimuth CW.<br>
<b>R2</b> - right for 150ms. OR R2xxxx in ms. e.g. R20220<br>
<b>RSt</b> - Reset CPU<br>
<b>RX</b> - Switch radio to RX mode.<br>
<b>S</b> - Stop All Rotation. Show rotators state.<br>
<b>S0</b> - Show switches state.<br>
<b>S1</b> - toggle PTT_disable. Default off.<br>
<b>S2</b> - Toggle disable_rotators.<br>
<b>S3</b> - SWR test. RX (UHF) RHCP. Default.<br>
<b>S4</b> - SWR test. RX (UHF) LHCP<br>
<b>S5</b> - SWR test. RX (VHF) RHCP<br>
<b>S6</b> - SWR test. RX (VHF) LHCP<br>
<b>S9</b> - Toggle radioCatOn. Default off.<br>
<b>SA</b> - Toggle satDisableAutoCycleTime. Default off.<br>
<b>T</b> - Reset degrees.<br>
<b>T1-8</b> - test relays.<br>
<b>T0</b> off. <b>T1</b> pwr. <b>T2</b> brake.<br> <b>T3</b> left. <b>T4</b> right. <b>T5</b> up.<br> <b>T6</b> down. <b>T7</b> TLV.<br> <b>T8</b> polarize LHCP.<br>
<b>T9</b> - Sync RTC clock via Wifi,<br> or display time, once synced.<br>
<b>TT</b> - toggle turn in talk.<br>
<b>TX</b> - Switch radio to TX mode.<br>
<b>TY</b> - Test Timer 1sec toggle.<br>
<b>U</b> - Rotate Elevation Up.<br>
<b>U2</b> - Rotate UP for 400ms. OR U2xxxx in ms.<br> e.g. U20500<br>
<b>V</b> - Get Direction and distance from<br> XXX to YYY (NO SPACES).<br> e.g v001 010<br>
<b>W</b> - Toggle wifi On/Off.<br>
<b>W1</b> - report last wifi signal strength.<br>
<b>W</b> - Rotate Azimuth and Elevation to<br> Set Point WXXX YYY.<br>
<b>X</b> is current ADC and degrees.<br>
<b>X1</b> is AZ debug info.<br>
<b>X2</b> is EL debug info.<br>
<b>Y</b> - PTT mode.<br>
<b>Y1</b> is TX on VHF (Relay normally off in RX).<br> <b>Y2</b> is TX on UHF (Relay normally on).<br>
<b>Y3</b> is TX and RX on VHF (Relay always on).<br> <b>Y4</b> is TX and RX on UHF (Relay always off).<br>
<br><br><br>
)rawliteral";

#define debug false
#define is_AZ_HAM4_Super 1 // Set when using a Super ZS6PEG Ham4 modified AZ rotator.
#define is_EL_KR500 1 // Set when using a KR500 elevation rotator.

#include <esp_heap_caps.h>
#include <rom/rtc.h>
#include <esp_task_wdt.h> // Add this for esp_task_wdt_delete
#include <Adafruit_ADS1X15.h> // Include the ADS1115.h Library (Library Updated to fix errors)
#include <Adafruit_I2CDevice.h>  // Include I2CDev.h Library (Library Updated to fix errors)

Adafruit_ADS1115 adc;

#include "FS.h"
#include "LittleFS.h"
#include "SD.h"
#include "SPI.h"

// Time RTC
#include <WiFi.h>

// Satellite tracking
#include <Sgp4.h>
#include <HTTPClient.h>
#include <UnixTime.h>

#include <Adafruit_NeoPixel.h>

#include <Preferences.h>

// Dashboard
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ESPmDNS.h>

// RTOS
#include <freertos/FreeRTOS.h>  // Base FreeRTOS definitions
#include <freertos/task.h>      // Required for xTaskCreatePinnedToCore and vTaskDelete
#include <freertos/semphr.h>

// RGB led matrix pin 0
// I2C SDA is on  pin 8
// I2C SCL is on pin 9
// RX0 is on pin 44
// TX0 is on pin 43
// RX1 is on pin 15 - civ
// TX1 is on pin 16 - civ
// RX2 is on pin 19
// TX2 is on pin 20
// builtin led on pin 38
// RGB led on pin 48
// SPI SCK 17
// SPI MISO 18
// SPI MOSI 4
// SPI CS 7

// V converter by usb
// A8 - I2C SDA 8 -now direct.
// A7 - I2C SCL 9 -now direct.
// A6 - interruptiPin 36
// A5 - pini_int_Hsend 47
// A4 - pini_int_Vsend 21
// A3 - pino_amp_Hsend 5
// A2 - pino_amp_Vsend 6
// A1 - Spare

// V converter by wifi
// A8 - polarize_opin 37
// A7 - TLVantSel_opin 35
// A6 - rotate_down_opin 39
// A5 - rotate_up_opin 40
// A4 - rotate_right_opin 41
// A3 - rotate_left_opin 42
// A2 - brakes_opin 2
// A1 - ac_power_opin 1

#define BAUD_RATE 19200  // Set the Serial Port Baud rate to 115200

// Radio CIV control lines
#define RXD1 15
#define TXD1 16

// SPI pin reasignment for SD card
#define SCK 17
#define MISO 18
#define MOSI 4
#define CS 7

// I2C pin reasignment for ADS1115 ADC
#define I2C_SDA 8
#define I2C_SCL 9

#define megaByte 1048576 // (1024 * 1024)

// Pin numbers
#define rotate_up_opin 39  // Define Rotate Up as Pin 3
#define rotate_down_opin 40  // Define Rotate Down as Pin 7
#define rotate_left_opin 42  // Define Rotate Left as Pin 4
#define rotate_right_opin 41  // Define Rotate Right as Pin 5
#define brakes_opin 2   // Define Brakes off command on pin 9
//#define interruptiPin 36  // Emergency stop button
#define polarize_opin 37  // Relay to change circular polarization direction
#define ac_power_opin 1  // AC power to 24V transformer relay
#define TLVantSel_opin 35  // PTT TX low voltage output for antenna Relay selection
#define pini_int_Hsend 47  // HSEND interupt
#define pini_int_Vsend 21  // VSEND interupt
#define pino_amp_Hsend 5  // HSEND output amp driver
#define pino_amp_Vsend 6  // VSEND output amp driver
#define RGBledMatrix_opin 0  // RGB led matrix data pin

// Pin state
#define pin_active_state 0x0 // 0 volts is ON
#define pin_off_state 0x1    // 3.3V is OFF

#define MAxByte 255

// Expected EEPROM calibration values
#define AZ_CAL_0_DEFAULT 40        // Preset the Azimuth Zero Calibration Point
#define AZ_CAL_MAX_DEFAULT 26800   // Preset the Azimuth Max Calibration Point
#define EL_CAL_0_DEFAULT  23300    // Preset the Elevation Zero Calibration Point
#define EL_CAL_MAX_DEFAULT 3600    // Preset the Elevation Max Calibration Point

// EL logaritmic pot adjustments
#define EL_KR500_90_ADC 12720      // Calibration points to give 4 logarithmic segments.
#define EL_KR500_45_ADC 18736
#define KR500_Min_Voltage 3600
#define KR500_Max_Voltage 23300

// Elevator RIGHT - Clockwise towards 0 degrees.
#define el00deg 23300 // 23392
#define el05deg 22912
#define el10deg 22592 
#define el15deg 22128 
#define el20deg 21536
#define el25deg 21040
#define el30deg 20688 
#define el35deg 20128
#define el40deg 19440 
#define el45deg 18736
#define el50deg 18144
#define el55deg 17632
#define el60deg 17008
#define el65deg 16336
#define el70deg 15744
#define el75deg 15056
#define el80deg 14288
#define el85deg 13728
#define el90deg 12720
#define el95deg 12300
#define el100deg 11800
#define el135deg 8440
#define el180deg 3600

// Degree values
#define MaxAZdegrees 360
#define MidAZdegrees 180
#define MinAZdegrees 1
#define MaxELdegrees 180
#define MidELdegrees 90
#define MinELdegrees 0

// Delay times in ms
#define PowerDelaySeconds 5
#define BrakeSettleDelay 30
#define MotorSettleDelay 40
#define TurnErrorDelay 20

#define TalkSecondsDelayAZ 2
#define TalkSecondsDelayEL 3

#define AZms1deg 80
#define AZms2deg 120
#define AZms3deg 180
#define AZms4deg 360
#define AZms5deg 450
#define AZms6deg 540
#define AZms7deg 620
#define AZms8deg 700
#define AZms9deg 800
#define AZms10deg 980

// Effective measured tracking speeds, including AZ stop/start cycles.
// These are NOT the individual relay pulse durations above.
#define SAT_AZ_MS_PER_DEG 200UL
#define SAT_EL_MS_PER_DEG 400UL

#define AZms1degtnt 180 //
#define AZms2degtnt 280 // 100
#define AZms3degtnt 400 // 120
#define AZms4degtnt 520 // 120
#define AZms5degtnt 650 // 130
#define AZms6degtnt 800 // 150
#define AZms7degtnt 950 // 150
#define AZms8degtnt 1100 // 150
#define AZms9degtnt 1250 // 150
#define AZms10degtnt 1500 // 250

#define ELms1deg 400
#define ELms2deg 800
#define ELms3deg 1200
#define ELms4deg 1600
#define ELms5deg 1800
#define ELms6deg 2000
#define ELms7deg 2200
#define ELms8deg 3000
#define ELms9deg 3500
#define ELms10deg 4000

#define ELms1degtnt 420
#define ELms2degtnt 850
#define ELms3degtnt 1280
#define ELms4degtnt 1650
#define ELms5degtnt 1850
#define ELms6degtnt 2050
#define ELms7degtnt 2250
#define ELms8degtnt 3050
#define ELms9degtnt 3550
#define ELms10degtnt 4050

// PTT processor modes
#define BandModeSelectVHFforRX 0
#define BandModeSelectUHFforRX 1
#define BandModeVHFonly 3
#define BandModeUHFonly 4

#define PolarizeSecondsDelay 3
#define dopplerTenSecUpdate 10

// #define cmdLoopBack 0x01
// #define cmdLoadRadio 0x02
// #define cmdRadioTX 0x03
// #define cmdRadioRX 0x04
// #define cmdDopplerOff 0x05
// #define cmdDopplerOn 0x06
// #define cmdTXfreq 0x07
// #define cmdRXfreq 0x08
// #define cmdDeltaFreq 0x09
// #define cmdDeltaRange 0x0A
// #define cmdStartTracking 0x0B
// #define cmdStopTracking 0x0C
// #define cmdReset 0x0D
// #define cmdSetSplit 0x0E

const char *host = "zs6peg";
char ssidcharr[11] = {'Z', 'S', '6', 'P', 'E', 'G', '1', '\0'};
const char *ssid = ssidcharr;
char passwordcharr[21] = {'*', 'i', 'o','n', 'o', 's', 'p', 'h', 'e', 'r', 'e', '1', '*', '\0'};
const char *password = passwordcharr;

const char* http_username = "admin";
const char* http_password = "admin";

const char* SAT_FLASH_LOG_PATH = "/temp_sat_log.txt";
const char* SD_LOG_PATH    = "/satellite_log.txt";
const char* AZ_rotaror_log_path = "/az_rotator_log.txt";
const char* EL_rotaror_log_path = "/el_rotator_log.txt";
const char* TalkNturnAZ_log_path = "/turnntalkAZ.txt";
const char* TalkNturnEL_log_path = "/turnntalkEL.txt";

bool dashboardison = false;
bool dashboardBusy = false;
bool dbledState = LOW;
uint8_t dbtimecounter = 0;
#define dbtimeinterval 8 // send data every 1 seconds

AsyncWebServer server(80);
AsyncEventSource events("/events"); // SSE endpoint
AsyncEventSource monitor_events("/monitor_events");

bool monitorTX = false;

const char *ntpServer1 = "pool.ntp.org";
const char *ntpServer2 = "time.nist.gov";
const long gmtOffset_sec = 7200;
const int daylightOffset_sec = 0;

bool RTCtimeOn = false;

// old attempt to save memory.
const String NorthOverride = "SH4 North override ";
const String Left = "Left CCW. ";
const String Right = "Right CW. ";
const String Mapped = "_Mapped_Degrees = ";
const String Distance = "_Distance = ";
const String PTTmodeIs = "PTT processor mode is ";
const String SDcardType = "SD Card Type : ";
const String SDunav = "SD card is not available";
const String RotatorsAre = "Rotators are ";
const String On = "On";
const String Off = "Off";

byte inByte = 0;  // incoming serial byte
byte serial_buffer[25];  // incoming serial byte buffer
int serial_buffer_index = 0;  // The index pointer variable for the Serial buffer
char cmdbuffer[25];
int cmdbuffer_index = 0;

// SD card
uint8_t cardType = 0;
uint64_t cardSize = 0;
char filename[] = "/123456.789";
bool SDcardIsOn = false;

bool gpredict = false;  // GPredict requires different serial format to that of HRD

bool disable_rotators = true;
bool disable_ptt = true;

bool AZ_waitingToStop = false;
bool cw = false;
bool AZmanualmoving = false; // R or L command
bool AZcmdfirststep = false;
bool AZnewdir = false; // Initial (master) turn direction at AZcmdfirststep.
int AZ_current_ADC = AZ_CAL_0_DEFAULT;  // Current Azimuth raw adc value
int AZ_0_ADC = AZ_CAL_0_DEFAULT;  // Azimuth Zero Value from EEPROM
int AZ_MAX_ADC = AZ_CAL_MAX_DEFAULT; // Azimuth Max Value from EEPROM
int AZ_Mapped_Degrees = 0; // mapped AZ ADC value to Degrees (without offset etc.)0V Changes while moving. Internal.
int AZ_External_Degrees = 0; // When internal degrees and external degrees differ
int AZ_Requested = 0; // RS232 Requested Azimuth - M and short W command
int AZ_Old = 0;
int AZ_Distance = 0; // Distance to move AZ. Can be positive or negative, indicating direction - Rotator Internal
bool AZovershoot = false;
int AZtt_Requested = 0; // Turn and Talk specific
int AZtt_Old = 0;
int AZtt_Distance = 0;

int AZrunMS = AZms1deg;
int ELrunMS = ELms1deg;
int AZdirChangeDegIn  = 0;

int EL_set_new = -1;  // Elevation set value. Indicates that EL is auto moving 
int EL_current_ADC = EL_CAL_0_DEFAULT;  // Current Elevation raw value
int EL_0_ADC = EL_CAL_0_DEFAULT;  // Elevation 0 Value from EEPROM
int EL_MAX_ADC = EL_CAL_MAX_DEFAULT;  //Elevation Max Value from EEPROM
int EL_Mapped_Degrees = 0; // mapped EL ADC value to Degrees
int EL_Requested = 0; // Requested EL Move
int EL_Distance = 0; // Distance to move EL
int EL_Old = 90;
int EL_last_adc = 0;
bool EL_Dropping = false;
bool ELmanualmoving = false;
bool EL_waitingToStop = false;
bool ELnonLinear = false; // KR500 EL non linearity flag.
unsigned long ELstartMillis = 0;
unsigned long ELnowMillis = 0;
unsigned long ELlenMillis = 0;

bool AZ_Brakes_Disengaged = false; // True before left or right move
uint8_t BrakeOnTimer = 0;
bool stop_ipin_active = false;

int SH4_North_ADC = -1; // North override voltage. -1 = off. For North calibration. (Ham4 Super.)
int SH4InternalNdegrees = 0; // 262
int SH4IntNdegDelta = 0; // Possitive or negative.

hw_timer_t *Timer50ms = NULL;
portMUX_TYPE timer50msMux = portMUX_INITIALIZER_UNLOCKED;
hw_timer_t *AZstopTimer = NULL;
volatile bool AZstopTimerFlag = false;
portMUX_TYPE AZstopTimerMux = portMUX_INITIALIZER_UNLOCKED;

bool oneSecond = false;
byte toSecond = 0; // counter for oneSecond tick.
byte powerDelay = 0; // counter to keep power on for 1 minute.
bool powerOn = false;

bool AZwaitingToTurn = false; // AZ request update while motor is turning.

bool TestTimer = false; // 1 second timer accuracy test.
int testms = 0;
bool RgbLedRed = false;

typedef enum {
  az,
  el,
  aztt,
  eltt,
} LogType;

int ElMinMaxDegrees(int deg);
void check_start();
void check_serial();
void send_requested_az();
void send_requested_el();
void send_requested_azel();
void ARDUINO_ISR_ATTR Timer50ms_Tick_Vect();
void Setup_Timer50ms();
void ARDUINO_ISR_ATTR az_time_stop_vect();
int GetCmdStringInt(int bufferindex, int len);
double GetSerialStringDouble(int bufferindex, int len);
String GetSerialString(int bufferindex, int len);
void GetFileName(int bufferindex, int len);
void Reset_AZ_Degrees();
void Reset_EL_Degrees();
void get_AZ_North_Degrees();
void Calculate_EL_Mapped_Degrees();
void rotate_az_cw();
void rotate_az_ccw();
void rotate_el_up();
void rotate_el_down();
void az_rotate_cmd_stop();
void el_rotate_cmd_stop();
int ExternalNorthAdjustDegrees(int Internal);
void SDcardOn();
void SDcardOff();
void readTLEsetup();
void readFile(fs::FS &fs, const char * path);
void writeFile(fs::FS &fs, const char * path, const char * message, bool appendNewLine);
void appendFile(fs::FS &fs, const char * path, const char * message, bool appendNewLine);
void listDir(fs::FS &fs, const char * dirname, uint8_t levels);
void printLocalTime(bool loadRadio);
void timeavailable(struct timeval *t);
void RTCtime();
void turn_talk();
void ELtalkstop();
void logAZTurnnTalk(String extramessage, bool onlymsg);
void logELTurnnTalk(String extramessage, bool onlymsg);
void retrieveTLEelementsForSatellite(int catalogueNumber);
String determineTLEage(String line1charArray);
void Predict(int many);
void startTracking();
void Sat_Tick();
void SatSpeedAdjust(int SatAZ);
void neopixels(byte color);
String getNowTimeStr();
void logAZrotator(String addmsg, bool onlymsg);
void logELrotator(String addmsg, bool onlymsg);
void print_reset_reason(RESET_REASON reason);
void PTT_handler();
void BCDarrayFromInt(int value, uint8_t* bcdArray, int arraySize);
void radioUpdateDoppler();
void radioMemoDoppler();
void radioSetSimplex();
void check_civ();
void radioRX();
void radioTX();
void AZ_DistanceToAZrunMS();
void EL_DistanceToELrunMS();
void dashboardOn();
void dashboardOff();
void handleStopAll(AsyncWebServerRequest *request);
void handleSerialRestart(AsyncWebServerRequest *request);
void helpOn(AsyncWebServerRequest *request);
void handledbcmd(AsyncWebServerRequest *request);
void updatedbtime();
void sendDBAZ(bool requested);
void sendDBEL(bool requested);
void cmdhandler();
void processDBcmd();
void cmdDashboardOut(String response);
void cmdSerialOut(String msg);
void resetCMDbuffer();
String byteToHexString(byte byte);
void PTT_ELRotators_Stop();
void ptt_rotators_state();
void reporting();
void ELclosest30();
void EL91to135();
void EL61to90();
void EL31to60();
void EL0to30();
void switchBandMode();
void WiFiEvent(WiFiEvent_t event, WiFiEventInfo_t info);
void getShortestTurn();
int getShortestDistance(int fromDeg, int toDeg);
void logToFlash(char* path, String data);
void dumpFlashToSD(char* flashPath, char* sdPath);
void AZendofturn();
void LogQueueEnter(String message, LogType type, bool onlymsg);
void LogQueueProcess();
void setStartDopplerFrequencies();
void setStopDopplerFrequencies();

bool dbhelp = false;
bool shouldReboot = false; // Flag to indicate a reboot is needed

typedef enum {
  RHCP,
  LHCP,
  AutoPolarizeUpRHCP,
  AutoPolarizeUpLHCP
} AutoPolarizationState;

AutoPolarizationState Auto_polarize = RHCP;

bool PolarizeRight = true;

bool compassUseDirection = false;
int compassFromDeg = 0;
int compassToDeg = 0;
int compassDirectionDistance = 0;

bool Hsend = false;
bool Vsend = false; // Vsend is used for antenna changes and civ frequency updates.
bool radioTXing = false;
bool radioSideTX = false;
bool radioSidePTT = false; // Radio PTT handling in progress.
byte BandMode = BandModeSelectUHFforRX;
bool radioSimplex = false;

bool enableTurnnTalk = false;
bool talknturningAZ = false; // only true when turning in PTT.
bool talknturningEL = false; // only true when turning in PTT.
unsigned long t_tELstartMS = 0;
unsigned long t_tELnowMS = 0;
unsigned long t_tELdeltaMS = 0;

// Satellite tracking
double lat = -26.2719; // North or + for South
double lon = 28.13511; // East
double alt = 1500;

int timezone = 2 ;  //utc + 2
Sgp4 sat;
UnixTime stamp(timezone);
unsigned long unixtime = 1458950400; // NB NOT auto updated!!!

unsigned long AOSunixtime = unixtime;
unsigned long TCAunixtime = unixtime;
unsigned long LOSunixtime = unixtime;

byte almostLOS = 90; // Time for White, before LOS in seconds.
bool LOStimeSet = false;

typedef enum {
  SATSTATE_STARTED,
  SATSTATE_90stoAOS,
  SATSTATE_AOSreached,
  SATSTATE_TCAreached,
  SATSTATE_90stoLOS,
  SATSTATE_LOS,
  SATSTATE_OFF,
  SATSTATE_ERROR
} SatState;

// Create a variable of the enum type
SatState currentSatState = SATSTATE_OFF;

int year; int mon; int day; int hr; int minute; double sec;
int AOSyear; int AOSmon; int AOSday; int AOShr; int AOSminute; double AOSsec;
int TCAyear; int TCAmon; int TCAday; int TCAhr; int TCAminute; double TCAsec;
int LOSyear; int LOSmon; int LOSday; int LOShr; int LOSminute; double LOSsec;

double azAOS = 0;

//int DopplerTotalRange = 6000;
//double tensecDopplerHertz = 0;
bool TenSecondDopplerOK = false;

// Manual time entry
uint16_t myear = 2025; uint8_t mmonth = 10; uint8_t mday = 1; uint8_t mhour = 2; uint8_t mminute = 1; uint8_t msecond = 1;

int catalogueNumber = 25544;

String satelliteName = "";
String tleLine1string = "";
String tleLine2string = "";
char satname[] = "ISS (ZARYA)";
char tle_line1[] = "1 25544U 98067A   26294.04791053  .00009575  00000-0  18188-3 0  9997";  //Line one from the TLE data
char tle_line2[] = "2 25544  51.6327  37.2623 0006760 289.5321  70.4938 15.48967427591265";  //Line two from the TLE data

bool UseManualTime = false;

int SatCycleSeconds = 10;
bool SatGo = false;
int SatGoCounter = 0;
bool Satspeedchange = false;
bool satwaiting = false; // Satellite tracking due at sceduled time.
int waithour = 0;
int waitminute = 0;
int waitsecond = 0;

bool satTracking = false; // internal SGP4 tracking in progress.
bool AOSok = false;
bool SatTurnCW = false;

typedef enum {
  SATSPEED_10s,
  SATSPEED_3s,
  SATSPEED_2s,
  SATSPEED_1s,
} SatSpeed;

uint8_t SatCurSpeed = SATSPEED_10s;

int satslowdowncount = 0;
int AOStoLOSsec = 0;
int satLeftSec = 0;
bool satDisableAutoCycleTime = false;

double SatLastPeakEL = 0;
int SatLastMoveAZ = 0;
int SatLastMoveEL = 0;

// Used in Sat_Tick for Satellite internal tracking.
int MoveEveryXdegrees = 1; // Move rotator every X degrees change.
double MoveEveryYdegrees = 1;

String satHTMLmsg = "";
String satFILEmsg = "";
String tickline = "";
String sattimestr = "";

bool satFileLogging = false;
bool satSerialLogging = false;
bool AZRotatorLogging = false;
bool ELRotatorLogging = false;
bool TalkNturnLogging = false;
bool AZloggingBusy = false;
bool ELloggingBusy = false;
bool AZTTloggingBusy = false;
bool ELTTloggingBusy = false;
bool queueLoggingBusy = false;

typedef struct {
    char msg[32]; // Fixed-size string buffer
    bool onlymsg; // Boolean flag
    LogType type; // Log type
} logQueueStruct;

logQueueStruct logQueue[10]; // Queue for log messages
uint8_t logQueueStart = 0;
uint8_t LogQueueDepth = 0;

bool satELup = false;

Adafruit_NeoPixel leds(64, RGBledMatrix_opin, NEO_GRB + NEO_KHZ800);
byte lastMatrixColor = 48; // off = 0 ascii

uint32_t TXcenterFreq = 145990000; // in Hertz
uint32_t RXcenterFreq = 437800000; // in Hertz
int dopplerDelta = 93; // 93 Hz each 10 seconds.
int dopplerUhfDelta = 130;
int dopplerRange = 6000; // 6khz total.
int dopplerUhfRange = 17500; // 10khz total.
int dopplerMinutes = 10;
uint32_t TXdopplerFreq = TXcenterFreq;
uint32_t RXdopplerFreq = RXcenterFreq;
int dopplerTimeCounter = 0;


bool radioCatOn = false;
HardwareSerial civ(1); // Use UART1

// Sub command is BCD format
uint8_t radioPreampOn [8] = {0XFE, 0XFE, 0XA2, 0XE0, 0X16, 0X02, 0X01, 0XFD}; // Preamp on.
uint8_t radioPreampOff[8] = {0XFE, 0XFE, 0XA2, 0XE0, 0X16, 0X02, 0X00, 0XFD}; // Preamp off.
uint8_t radioSplitOn[7] = {0XFE, 0XFE, 0XA2, 0XE0, 0X0F, 0X01, 0XFD}; // Split/Duplex split on.
uint8_t radioSplitOff[7] = {0XFE, 0XFE, 0XA2, 0XE0, 0X0F, 0X00, 0XFD}; // Split/Duplex split off.
uint8_t radioSimplexOn[7] = {0XFE, 0XFE, 0XA2, 0XE0, 0X0F, 0X10, 0XFD}; // Simplex on.
uint8_t radioVFOA[7] = {0XFE, 0XFE, 0XA2, 0XE0, 0X07, 0x00, 0xFD}; // SelectVFOMode -> VFO A
uint8_t radioVFOB[7] = {0XFE, 0XFE, 0XA2, 0XE0, 0X07, 0x01, 0xFD}; // SelectVFOMode -> VFO B
uint8_t radioFMfilt1[8] = {0XFE, 0XFE, 0XA2, 0XE0, 0X06, 0X05, 0X01, 0XFD}; // FM FIL1
uint8_t radioRXfreq[11] = {0XFE, 0XFE, 0XA2, 0XE0, 0X05, 0X00, 0X00, 0X80, 0X37, 0X04, 0XFD}; // SetTheFrequency 437 800 000 Hz.
uint8_t radioTXfreq[11] = {0XFE, 0XFE, 0XA2, 0XE0, 0X05, 0X00, 0X00, 0X99, 0X45, 0X01, 0XFD}; // SetTheFrequency 145 990 000 Hz.
uint8_t radioTestfreq[11] = {0XFE, 0XFE, 0XA2, 0XE0, 0X05, 0X01, 0X01, 0X01, 0X95, 0X12, 0XFD}; // 1295.010.101
uint8_t radioToTX[8] = {0XFE, 0XFE, 0XA2, 0XE0, 0X1C, 0X00, 0X01, 0XFD}; // Set to TX mode
uint8_t radioToRX[8] = {0XFE, 0XFE, 0XA2, 0XE0, 0X1C, 0X00, 0X00, 0XFD}; // Set to RX mode
uint8_t radioSmeter[7] = {0XFE, 0XFE, 0XA2, 0XE0, 0X15, 0X02, 0XFD}; // Get S meter level
uint8_t radioDualWatchOn[8] = {0XFE, 0XFE, 0XA2, 0XE0, 0X16, 0X059, 0X01, 0XFD}; // Enable Sub
uint8_t radioDualWatchOff[8] = {0XFE, 0XFE, 0XA2, 0XE0, 0X16, 0X059, 0X00, 0XFD}; // Disable Sub
uint8_t radioSATon[8] = {0XFE, 0XFE, 0XA2, 0XE0, 0X16, 0X05A, 0X01, 0XFD}; // SAT on
uint8_t radioSAToff[8] = {0XFE, 0XFE, 0XA2, 0XE0, 0X16, 0X05A, 0X00, 0XFD}; // SAT off
uint8_t radioMaxSqlSubAudio[8] = {0XFE, 0XFE, 0XA2, 0XE0, 0X14, 0X03, 0XFF, 0XFD}; // Max Sub Audio balance sql
uint8_t radioSubSpeakerOff[9] = {0XFE, 0XFE, 0XA2, 0XE0, 0X1A, 0X05, 0X33, 0X00, 0XFD}; // Sub Speaker off
uint8_t radioSubSpeakerOn[9] = {0XFE, 0XFE, 0XA2, 0XE0, 0X1A, 0X05, 0X33, 0X01, 0XFD}; // Sub Speaker on
uint8_t radioSubMuteUSB[9] = {0XFE, 0XFE, 0XA2, 0XE0, 0X1A, 0X05, 0X34, 0X00, 0XFD}; // Sub Mute USB
uint8_t radioSubMuteUSBoff[9] = {0XFE, 0XFE, 0XA2, 0XE0, 0X1A, 0X05, 0X34, 0X01, 0XFD}; // Sub Mute USB off
uint8_t radioSubMuteLan[9] = {0XFE, 0XFE, 0XA2, 0XE0, 0X1A, 0X05, 0X35, 0X00, 0XFD}; // Sub Mute Lan
uint8_t radioSubMuteLanOff[9] = {0XFE, 0XFE, 0XA2, 0XE0, 0X1A, 0X05, 0X35, 0X01, 0XFD}; // Sub Mute Lan off
uint8_t radioACCafMain[9] = {0XFE, 0XFE, 0XA2, 0XE0, 0X1A, 0X05, 0X99, 0X00, 0XFD}; // ACCaf Main
uint8_t radioDispRX[10] = {0XFE, 0XFE, 0XA2, 0XE0, 0X1A, 0X05, 0X01, 0X62, 0X01, 0XFD}; // Disp RX Sub/Main
uint8_t radioDispOff[10] = {0XFE, 0XFE, 0XA2, 0XE0, 0X1A, 0X05, 0X01, 0X62, 0X00, 0XFD}; // Disp RX Sub/Main
uint8_t radioRecBoth[10] = {0XFE, 0XFE, 0XA2, 0XE0, 0X1A, 0X05, 0X02, 0X45, 0X01, 0XFD}; // Rec Both
uint8_t radio10dbAttenuator[7] = {0XFE, 0XFE, 0XA2, 0XE0, 0X11, 0X10, 0XFD}; // 10db Attenuator on
uint8_t radioAttenuatorOff[7] = {0XFE, 0XFE, 0XA2, 0XE0, 0X11, 0X00, 0XFD}; // Attenuator off
uint8_t radioMinRXGain[8] = {0XFE, 0XFE, 0XA2, 0XE0, 0X14, 0X02, 0X00, 0XFD}; // Min RX Gain
uint8_t radioMaxRXGain[8] = {0XFE, 0XFE, 0XA2, 0XE0, 0X14, 0X02, 0XFF, 0XFD}; // Max RX Gain
uint8_t radioSubVFO[7] = {0XFE, 0XFE, 0XA2, 0XE0, 0X07, 0XD1, 0XFD}; // Sub VFO
uint8_t radioMainVFO[7] = {0XFE, 0XFE, 0XA2, 0XE0, 0X07, 0XD0, 0XFD}; // Main VFO
uint8_t radioAVFO[7] = {0XFE, 0XFE, 0XA2, 0XE0, 0X07, 0X00, 0XFD}; // A VFO (VFO mode in SAT)
uint8_t radioMemoryChannel[8] = {0XFE, 0XFE, 0XA2, 0XE0, 0X08, 0X00, 0X01, 0XFD}; // Memory Channel
uint8_t radioMemoryWrite[6] = {0XFE, 0XFE, 0XA2, 0XE0, 0X09, 0XFD}; // Memory Write
uint8_t radioVFOmode[6] = {0XFE, 0XFE, 0XA2, 0XE0, 0X07, 0XFD}; // VFO mode
uint8_t radioMemorymode[6] = {0XFE, 0XFE, 0XA2, 0XE0, 0X08, 0XFD}; // Memory mode
uint8_t radioRXHVB[11] = {0XFE, 0XFE, 0XA2, 0XE0, 0X05, 0X00, 0X75, 0X78, 0X45, 0X01, 0XFD}; // 437 187 500 Hz.
//uint8_t radioTXHVB[11] = {0XFE, 0XFE, 0XA2, 0XE0, 0X05, 0X00, 0X75, 0X18, 0X45, 0X01, 0XFD}; // 145 787 500 Hz.
uint8_t radioDUPoffset[9] = {0XFE, 0XFE, 0XA2, 0XE0, 0X0D, 0X00, 0X60, 0X00, 0XFD}; // DUP offset
uint8_t radioDUPon[7] = {0XFE, 0XFE, 0XA2, 0XE0, 0X0F, 0X11, 0XFD}; // DUP on

Preferences preferences;

bool readRadioOnce = false;
byte civinByte = 0;  // incoming serial byte
byte civ_buffer[50];  // incoming serial byte buffer
int civ_buffer_index = 0;  // The index pointer variable for the Serial buffer
bool civrespOK = false;
String lastcivresp = "";
bool SmeterLevelReq = false;
int sMeterVal = 0;
uint8_t autoSmeterCount = 0;
bool autoPolarize = false;
bool radioAutoSmeter_wait = false;
bool firstAutoSmeterTest = false;
bool autoPolarizeTest = false;

bool cmdexecuting = false;
bool dbcmd = false; // ascii dashboard command received.
int8_t wifiRSSI = 0; // last wifi signal strength in dBm. Updated every 30 seconds when wifi is on.
String dashbnserialmsg = "";
String filemsg = "";
// Polarization messages
bool polarmsgOK = false;
String polarmsg = "";
// Satellite tracking messages (for command dashboard and monitor).
bool satHTMLmsgOK = false;

String tempstring = "";

// FreeRTOS Semaphores and Handles
SemaphoreHandle_t AZstopTimerSemaphore;
TaskHandle_t managerTaskHandle = NULL;

// The actual self-terminating thread
void AZstopTask(void *parameter) {

  timerStop(AZstopTimer);
  timerEnd(AZstopTimer);
  digitalWrite(rotate_right_opin, pin_off_state);  // Turn off the Rotate Right Pin
  digitalWrite(rotate_left_opin, pin_off_state);   // Turn off the Rotate Left Pin

  if (!talknturningAZ)
  {
    vTaskDelay(pdMS_TO_TICKS(MotorSettleDelay));
    get_AZ_North_Degrees(); // Where are we now?
    compassFromDeg = AZ_External_Degrees;
    compassToDeg = AZ_Requested;
    getShortestTurn();
    AZ_Distance = compassDirectionDistance;
    cw = compassUseDirection;
    AZ_Old = AZ_External_Degrees;
    // Turn more?
    if (AZ_Distance < 1)
    {
      logAZrotator("end of steps. Distance. In AZstopTask.\n", true);
      AZ_Old = AZ_Requested;
      AZendofturn();
    }

    if (AZnewdir != cw)
    {
      if (AZ_Distance > 1)
      {
        logAZrotator("AZovershoot. In AZstopTask.\n", false);
        AZovershoot = true;
      }else{
        logAZrotator("end of steps. Direction. In AZstopTask.\n", true);
        AZ_Old = AZ_Requested;
        AZendofturn();
      }
    }
  }else{
    AZtt_Old = AZtt_Requested;
    AZtt_Distance = 0;
    talknturningAZ = false; // only true when turning in PTT.
    AZendofturn();   
  }

  portENTER_CRITICAL_ISR(&AZstopTimerMux);
  AZstopTimerFlag = true;
  AZ_waitingToStop = false;
  portEXIT_CRITICAL_ISR(&AZstopTimerMux);

  vTaskDelete(NULL); // Passing NULL makes the task delete itself
}

// Worker Thread: Waits for the timer signal, then launches the task
void managerTask(void *parameter) {
    while(true) {
    // Wait indefinitely until the timer ISR gives the semaphore
    if (xSemaphoreTake(AZstopTimerSemaphore, portMAX_DELAY) == pdTRUE)
    {
      // Launch the self-terminating thread on Core 0
      xTaskCreatePinnedToCore(
        AZstopTask,
        "OneShotTask",
        4096,
        NULL,
        2, // Higher priority so it runs immediately
        NULL,
        0
      );
    }
  }
}

void Core0Timer50msTask(void *pvParameters) {

  Timer50ms = timerBegin(1000000);
  timerAttachInterrupt(Timer50ms, &Timer50ms_Tick_Vect);
  timerAlarm(Timer50ms, 20000, true, 0);

  while (true) {
    vTaskDelay(pdMS_TO_TICKS(20000)); // Yield to avoid triggering watchdog on Core 0
  }
}

const char* monitor_html = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title>Pegasus monitor.</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    body { font-family: Arial; text-align: center; margin: 0px auto; padding-top: 0px;}
    .button { background-color: #ea9a37ff; border: none; color: white; padding: 5px 5px; text-align: center; text-decoration: none; display: inline-block; font-size: 16px; margin: 4px 2px; cursor: pointer; }
    .button.rx { background-color: #4CAF50; }
    .button.tx { background-color: #f44336; }
  </style>
</head>
<body>
  <h1>Pegasus monitor.</h1>
  <button class="button" id="rxtx" onclick="rxtx()">Receive</button>
  <br><br>
  <div id="data">Waiting for data...</div>
  <script>
    var eventSource = new EventSource('/monitor_events');
    eventSource.onmessage = function(event) {
      if (event.data.startsWith("RXTX_STATE:")) {
        var state = event.data.substring(11);
        document.getElementById('rxtx').className = 'button ' + (state === 'TX' ? 'tx' : 'rx');
        document.getElementById('rxtx').innerText = (state === 'TX' ? 'Transmitting' : 'Receiving');
      } else {
        document.getElementById('data').innerHTML = event.data;
      }
    };
    function rxtx() {
      var xhr = new XMLHttpRequest();
      xhr.open("GET", "/rxtxtoggle", true);
      xhr.send();
    }
  </script>
</body>
</html>
)rawliteral";

const char* index_html = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title>Pegasus dashboard.</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    body { font-family: Arial; text-align: center; margin: 0px auto; padding-top: 0px;}
    .button { background-color: #ea9a37ff; border: none; color: white; padding: 5px 5px; text-align: center; text-decoration: none; display: inline-block; font-size: 16px; margin: 4px 2px; cursor: pointer; }
    .button.on { background-color: #4CAF50; }
    .button.off { background-color: #f44336; }
  </style>
</head>
<body>
  <h1>Pegasus dashboard.</h1>
  <label id="RTC" style="margin-right: 20px; font-size:125%;">12/11/25 18:21</label>
  <label for="AZval" id="AZ" style="margin-left: 20px; font-size:125%;" >AZ</label>
  <label id="AZval" style="font-size:125%;">000</label>
  <label for="ELval" id="EL" style="margin-left: 20px; font-size:125%;" >EL</label>
  <label id="ELval" style="font-size:125%;">000</label>
  <br><br>
  <div id="data">Waiting for data...</div>
  <br>
  <label for="inputcmd">Input :</label>
  <input type="text" id="inputcmd" name="inputcmd">
  <button id="clear" onclick="document.getElementById('inputcmd').value ='';
  document.getElementById('data').innerHTML ='&nbsp;';
  document.getElementById('result').innerHTML ='&nbsp;';
  ">Clear</button>
  <br><br>
  <button class="button" id="toggleLED" onclick="toggleLED()">Toggle LED</button>
  <button class="button" id="help" onclick="help()">help</button>
  <button class="button" id="cmd" onclick="dbcmd()">CMD</button>
  <button class="button" id="SerialRestart" onclick="SerialRestart()">Restart Serial</button>
  <button class="button" id="stop" onclick="stopall()">Stop All</button>
  <br><br>
  <div id="result">Waiting for CMD reponse...</div>

  <script>
    var gateway = `ws://${window.location.hostname}/ws`; // For WebSockets if needed, but not used in this SSE example
    var eventSource = new EventSource('/events');

    eventSource.onmessage = function(event) {
      if (event.data.startsWith("LED_STATE:")) {
        var state = event.data.substring(10);
        document.getElementById('toggleLED').className = 'button ' + (state === 'ON' ? 'on' : 'off');
      }else if (event.data.startsWith("HELP_STATE:")) {
        var state = event.data.substring(11);
        document.getElementById('help').className = 'button ' + (state === 'ON' ? 'on' : 'off');
      }else if (event.data.startsWith("RTC_TIME:")) {
        var time = event.data.substring(9);
        document.getElementById('RTC').innerHTML = time;
      }else if (event.data.startsWith("DBAZ:")) {
        var time = event.data.substring(5);
        document.getElementById('AZval').innerHTML = time;
      }else if (event.data.startsWith("DBEL:")) {
        var time = event.data.substring(5);
        document.getElementById('ELval').innerHTML = time;
      }else if (event.data.startsWith("DBCMD_STATE:")) {
        var state = event.data.substring(12);
        document.getElementById('cmd').className = 'button ' + (state === 'ON' ? 'on' : 'off');
      }else{
        var iscmd = event.data.startsWith("CMD_RESPONSE");
        if(iscmd){
          document.getElementById('result').innerHTML = event.data.substring(12);
        }else{
          document.getElementById('data').innerHTML = event.data;
        }
      }
    };

    function toggleLED() {
      var xhr = new XMLHttpRequest();
      xhr.open("GET", "/toggle", true);
      xhr.send();
    }

    function help() {
      var xhr = new XMLHttpRequest();
      xhr.open("GET", "/help", true);
      xhr.send();
    }

    function stopall() {
      var xhr = new XMLHttpRequest();
      xhr.open("GET", "/stopall", true);
      xhr.send();
    }

    function SerialRestart() {
      var xhr = new XMLHttpRequest();
      xhr.open("GET", "/serial", true);
      xhr.send();
    }

    function dbcmd() {
      var xhr = new XMLHttpRequest();
      xhr.open("GET", "/dbcmd", true);
      xhr.setRequestHeader("cmd", document.getElementById("inputcmd").value);
      xhr.send();
    }

  </script>
</body>
</html>
)rawliteral";

void setup() 
{
  Serial.begin(BAUD_RATE); // initialize serial communication

  preferences.begin("mat", false); // Open Preferences with mat namespace. RW mode (permanent storage).
  TXcenterFreq = preferences.getULong("TXcenterFreqHz", 145990000);
  RXcenterFreq = preferences.getULong("RXcenterFreqHz", 437800000);
  dopplerRange = preferences.getInt("dopplerRange", 6000);
  dopplerUhfRange = preferences.getInt("dopplerUhfRange", 10000);
  lat = preferences.getDouble("latitude", -26.2719);
  lon = preferences.getDouble("longitude", 28.13511);
  alt = preferences.getDouble("altitude", 1500);
  AZdirChangeDegIn = preferences.getInt("hs4changedeg", 2);
  strcpy(ssidcharr, preferences.getString("wifissid", "ZS6PEG1").c_str());
  strcpy(passwordcharr, preferences.getString("wifipassword", "*ionosphere1*").c_str());
  autoPolarize = preferences.getBool("autoPolarize", false);
  ELnonLinear = preferences.getBool("ELnonLinear", false);

  SH4_North_ADC = preferences.getInt("NOffsADC", -1);
  SH4InternalNdegrees = preferences.getInt("NOffsDeg", 0);
  SH4IntNdegDelta = preferences.getInt("NOffsDelta", 0);
  AZ_0_ADC = preferences.getInt("azZeroADC", AZ_CAL_0_DEFAULT);
  AZ_MAX_ADC = preferences.getInt("azMaxADC", AZ_CAL_MAX_DEFAULT);
  EL_0_ADC = preferences.getInt("elZeroADC", EL_CAL_0_DEFAULT);
  EL_MAX_ADC = preferences.getInt("elMaxADC", EL_CAL_MAX_DEFAULT);

  preferences.end(); // Close the Preferences

  setStartDopplerFrequencies();

  uint8_t bcdA[5] = {0, 0, 0, 0, 0}; // Array to hold BCD representation
  BCDarrayFromInt(TXcenterFreq, bcdA, 5);

  radioTXfreq[9] = bcdA[0]; // Ten MHz
  radioTXfreq[8] = bcdA[1]; // One MHz
  radioTXfreq[7] = bcdA[2]; // Hundred kHz
  radioTXfreq[6] = bcdA[3]; // Ten kHz
  radioTXfreq[5] = bcdA[4]; // One kHz

  BCDarrayFromInt(RXcenterFreq, bcdA, 5);

  radioRXfreq[9] = bcdA[0]; // Ten MHz
  radioRXfreq[8] = bcdA[1]; // One MHz
  radioRXfreq[7] = bcdA[2]; // Hundred kHz
  radioRXfreq[6] = bcdA[3]; // Ten kHz
  radioRXfreq[5] = bcdA[4]; // One kHz

  pinMode(RXD1, INPUT);
  pinMode(TXD1, OUTPUT);

  civ.begin(BAUD_RATE, SERIAL_8N1, RXD1, TXD1); // Initialize UART1 for CI-V

  // SPI pin reasignment for SD card
  pinMode(MOSI, OUTPUT);
  pinMode(MISO, INPUT);
  pinMode(SCK, OUTPUT);
  pinMode(CS, OUTPUT);  
  
  // Rotator control pins
  pinMode(rotate_up_opin, OUTPUT);  // Define the Control Pins as Outputs
  pinMode(rotate_down_opin, OUTPUT);
  pinMode(rotate_left_opin, OUTPUT);
  pinMode(rotate_right_opin, OUTPUT);
  pinMode(brakes_opin, OUTPUT);
  pinMode(polarize_opin, OUTPUT);
  pinMode(ac_power_opin, OUTPUT);

  // PTT controller
  pinMode(pini_int_Hsend, INPUT);
  pinMode(pini_int_Vsend, INPUT);
  pinMode(pino_amp_Hsend, OUTPUT);
  pinMode(TLVantSel_opin, OUTPUT);
  pinMode(pino_amp_Vsend, OUTPUT);
  
  // Reset all Rotator command pins
  digitalWrite(rotate_up_opin, pin_off_state);  // Turn off all the Control Pins
  digitalWrite(rotate_down_opin, pin_off_state);
  digitalWrite(rotate_left_opin, pin_off_state);
  digitalWrite(rotate_right_opin, pin_off_state);
  digitalWrite(brakes_opin, pin_off_state);
  digitalWrite(polarize_opin, pin_off_state);
  digitalWrite(ac_power_opin, pin_off_state);
  BandMode = BandModeSelectUHFforRX;
  digitalWrite(TLVantSel_opin, pin_off_state); //activate for TX on VHF / RX on UHF.
  digitalWrite(pino_amp_Hsend, pin_active_state); //normal HF amp output
  digitalWrite(pino_amp_Vsend, pin_active_state); //normal VHF amp output

  // I2C init
  Wire.begin(I2C_SDA, I2C_SCL);
  Wire.setClock(100000); // 100kHz I2C
  
  if (!adc.begin(0x48))  // Start the ADC
  {
    Serial.println("Failed to initialize ADS.");
    while (1);
  }
  adc.setGain(GAIN_TWOTHIRDS);
  adc.setDataRate(RATE_ADS1115_475SPS);

  readTLEsetup();

  // rmt_ch_dir_t rmt_dir = RMT_TX_MODE;
  // if (!rmtInit(RGBledMatrix_opin, rmt_dir, RMT_MEM_64)) {
  //   Serial.println("RMT Initialization Failed!");
  // } else {
  //   Serial.println("RMT Initialized Successfully for 64 LEDs!");
  // }
  leds.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
  leds.show();  // Turn OFF all pixels ASAP

  Serial.print("Loop stack size is ");
  Serial.println(getArduinoLoopTaskStackSize());
  // Check integrity of all heaps
  Serial.printf("biggest free block: %i\n", heap_caps_get_largest_free_block(MALLOC_CAP_8BIT));

  delay(500);

  // Register the event handler
  WiFi.onEvent(WiFiEvent);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  RTCtime();

  delay(500);

  if(!LittleFS.begin(true))
  {
    Serial.println("LittleFS Mount Failed. Formatting...");
    return;
  }
  Serial.println("LittleFS Mounted Successfully");

  dashboardOn();

  // Create binary semaphore for ISR-to-Task sync
  AZstopTimerSemaphore = xSemaphoreCreateBinary();

  // Create the background Manager Thread on Core 0
  xTaskCreatePinnedToCore(managerTask, "ManagerTask", 4096, NULL, 3, &managerTaskHandle, 0);

  get_AZ_North_Degrees();
  AZ_Old = AZ_Requested = AZ_External_Degrees;

  Calculate_EL_Mapped_Degrees();
  EL_Old = EL_Requested = EL_Mapped_Degrees;

  xTaskCreatePinnedToCore(
  Core0Timer50msTask,   /* Task function */
  "Core0TimerTask", /* Name */
  4096,             /* Stack size */
  NULL,             /* Parameters */
  1,                /* Priority */
  NULL,             /* Task handle */
  0                 /* Core ID (0 or 1) */
  );

  Serial.println("Setup Done");
}

void LogQueueEnter(String message, LogType type, bool onlymsg)
{
  queueLoggingBusy = true;
  logQueueStruct logEntry;
  strlcpy(logEntry.msg, message.c_str(), sizeof(logEntry.msg)); // Copy the message to the log entry
  logEntry.type = type;
  logEntry.onlymsg = onlymsg;

  if (LogQueueDepth < 10) {
    logQueue[(logQueueStart + LogQueueDepth) % 10] = logEntry;
    LogQueueDepth++;
  } else {
    Serial.println("Log queue is full! Dropping log entry.");
  }
  queueLoggingBusy = false;
}

void LogQueueProcess()
{
  if (queueLoggingBusy) {return;} // Avoid re-entrancy
  if (LogQueueDepth > 0) {
    logQueueStruct logEntry = logQueue[logQueueStart];
    String logMessage = "Queued: " + String(logEntry.msg);
    // Process the log entry (e.g., write to SD card)
    if (logEntry.type == az)
    {
      if (!AZloggingBusy)
      {
        logAZrotator(logMessage, logEntry.onlymsg);
        // Move to the next entry
        logQueueStart = (logQueueStart + 1) % 10;
        LogQueueDepth--;
      }
    }else if (logEntry.type == el)
    {
      if (!ELloggingBusy)
      {
        logELrotator(logMessage, logEntry.onlymsg);
        // Move to the next entry
        logQueueStart = (logQueueStart + 1) % 10;
        LogQueueDepth--;
      }
    }else if (logEntry.type == aztt)
    {
      if (!AZTTloggingBusy)
      {
        logAZTurnnTalk(logMessage, logEntry.onlymsg);
        // Move to the next entry
        logQueueStart = (logQueueStart + 1) % 10;
        LogQueueDepth--;
      }
    }else if (logEntry.type == eltt)
    {
      if (!ELTTloggingBusy)
      {
        logELTurnnTalk(logMessage, logEntry.onlymsg);
        // Move to the next entry
        logQueueStart = (logQueueStart + 1) % 10;
        LogQueueDepth--;
      }
    }
  }
}

// Event handler for Wi-Fi changes
void WiFiEvent(WiFiEvent_t event, WiFiEventInfo_t info) {
  switch (event) {
    case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
      Serial.println("Wi-Fi disconnected. Triggering reconnect...");
      WiFi.reconnect(); // Built-in reconnect function
      break;
    case ARDUINO_EVENT_WIFI_STA_GOT_IP:
      Serial.println("Connected! Got IP address.");
      break;
  }
}

void dashboardOn()
{
  Serial.println("dashboardOn()");
  if (dashboardison) return; // already on

    /*use mdns for host name resolution*/
  if (!MDNS.begin(host)) { //http://zs6peg.local
    Serial.println("Error setting up MDNS responder!");
    while (1) {
      delay(1000);
    }
  }

  Serial.println("mDNS responder started");

    // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    if(!request->authenticate(http_username, http_password))
      return request->requestAuthentication();
      String s = index_html;
      s.replace("%STATE%", dbledState ? "ON" : "OFF");
      request->send(200, "text/html", s);
  });

  server.on("/toggle", HTTP_GET, [](AsyncWebServerRequest *request){
    dashbnserialmsg += "Toggle LED button pressed.<br />";
    String message = "";
    if (dbhelp)
    {
      dbhelp = false;
      events.send("HELP_STATE:" + String(dbhelp ? "ON" : "OFF"), "message", millis());
      message  = "Help: Test function to validate Dashboard operation.";
      events.send(message.c_str(), "message", millis()); // "message" is the event type
    }else{
    dbledState = !dbledState;
    neopixels(dbledState ? 50 : 0); // Green or off
    events.send("LED_STATE:" + String(dbledState ? "ON" : "OFF"), "message", millis());
    message = "LED Toggled " + String(dbledState ? "ON" : "OFF");;
    events.send(message.c_str(), "message", millis()); // "message" is the event
    }
    request->send(204); // No content response
  });

  server.on("/stopall", HTTP_GET, handleStopAll);

  server.on("/serial", HTTP_GET, handleSerialRestart);

  server.on("/help", helpOn);

  server.on("/dbcmd", HTTP_GET, handledbcmd);

  events.onConnect([](AsyncEventSourceClient *client){
    if(client->lastId()){
      Serial.printf("Client reconnected! Last message ID: %u\n", client->lastId());
    }
    // send event with message "hello!", id current millis
    // and set reconnect interval to 1 second
    client->send("hello!", NULL, millis(), 1000);
    client->send("LED_STATE:" + String(dbledState ? "ON" : "OFF"), "message", millis());
    client->send("HELP_STATE:" + String(dbhelp ? "ON" : "OFF"), "message", millis());
    client->send("DBCMD_STATE:" + String(dbcmd ? "ON" : "OFF"), "message", millis());
  });

  server.on("/monitor", HTTP_GET, [](AsyncWebServerRequest *request){
  request->send(200, "text/html", monitor_html);
  });

  monitor_events.onConnect([](AsyncEventSourceClient *client){
    if(client->lastId()){
      Serial.printf("monitor client reconnected! Last message ID: %u\n", client->lastId());
    }
    // send event with message "hello!", id current millis
    // and set reconnect interval to 1 second
    client->send("RXTX_STATE:" + String(monitorTX ? "TX" : "RX"), "message", millis());
    client->send("hello! Waiting for tracking to start.", NULL, millis(), 1000);
  });

  server.on("/rxtxtoggle", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(204); // No content response
    monitorTX = !monitorTX;
    monitor_events.send("RXTX_STATE:" + String(monitorTX ? "TX" : "RX"), "message", millis());
    if (monitorTX)
    {
      radioTX();
    }else{
      radioRX();
    }
  });

  server.addHandler(&events);
  server.addHandler(&monitor_events);

  server.begin();
  dashboardison = true; 
}

void dashboardOff()
{
  events.close();
  monitor_events.close();
  server.end();
  MDNS.end();
  dashboardison = false;
}

// Appends data directly into the internal Flash memory
void logToFlash(const char* path, String data) {
  File file = LittleFS.open(path, FILE_APPEND);
  if (!file) {
    dashbnserialmsg += "Failed to open flash file for appending!\n";
    return;
  }
  file.print(data);
  file.print('\n');
  file.close(); // Ensures data is committed securely to flash
}

// Streams the data block from Flash to SD, then clears Flash buffer
void dumpFlashToSD(const char* flashPath, const char* sdPath) {
  Serial.println("\n--- Initiating Flash to SD Dump ---");

  if (!SDcardIsOn)
  {
    SDcardOn();
  }

  if (!SDcardIsOn){ dashbnserialmsg += SDunav + "<br />"; return; }

  // Open the flash cache log file for reading
  File flashFile = LittleFS.open(flashPath, FILE_READ);
  if (!flashFile || flashFile.size() == 0) {
    dashbnserialmsg += "Flash cache log is empty or inaccessible.<br />";
    if (flashFile) flashFile.close();
    return;
  }

  // Open the master data log file on the SD card for appending
  File sdFile = SD.open(sdPath, FILE_APPEND);
  if (!sdFile) {
    dashbnserialmsg += "Critical error: Failed to open master file on SD card.<br />";
    flashFile.close();
    return;
  }

  // Stream data by reading bytes out of flash directly into the SD card file
  uint8_t buffer[64];
  while (flashFile.available()) {
    int bytesRead = flashFile.read(buffer, sizeof(buffer));
    sdFile.write(buffer, bytesRead);
  }

  // Explicitly close files to ensure the hardware writes all dirty buffers
  sdFile.close();
  flashFile.close();

  // Delete/Wipe the internal flash log to reset storage usage
  if (!LittleFS.remove(flashPath))
  {
    dashbnserialmsg += "Warning: Failed to erase the Flash buffer file.<br />";
  }
}

void handleStopAll(AsyncWebServerRequest *request) {
  request->send(204); // No content response
  String message = "";
  if (dbhelp)
  {
    dbhelp = false;
    events.send("HELP_STATE:" + String(dbhelp ? "ON" : "OFF"), "message", millis());
    message  = "Help: Stop All rotation.";
    events.send(message.c_str(), "message", millis()); // "message" is the event type
  }else{
    az_rotate_cmd_stop();
    el_rotate_cmd_stop();
    message = "Rotators stopped.";
    events.send(message.c_str(), "message", millis()); // "message" is the event type
  }
}

void handleSerialRestart(AsyncWebServerRequest *request) {
  String message = "";
  if (dbhelp)
  {
    dbhelp = false;
    events.send("HELP_STATE:" + String(dbhelp ? "ON" : "OFF"), "message", millis());
    message  = "Help: Abort, Stop and Restart USB serial Comms. Loose connection.";
    events.send(message.c_str(), "message", millis()); // "message" is the event type
  }else{
    while(!Serial.availableForWrite()){delay(10);} // Ensure all data is written
    request->send(204); // No content response
    delay(100); // Small delay to ensure the message is sent
    Serial.end();
    delay(50); 
    Serial.flush();
    delay(50);
    Serial.begin(BAUD_RATE);
    String message = "Serial Restarted. Reestablich connection.";
    events.send(message.c_str(), "message", millis()); // "message" is the event type
  }
}

void helpOn(AsyncWebServerRequest *request) {
  String message = "";
  if (dbhelp)
  {
    message  = "Help: Click any button for a quick help description.";
    events.send(message.c_str(), "message", millis()); // "message" is the event type
  }else{
    dbhelp = true;
    message = "Help is On";
    events.send(message.c_str(), "message", millis()); // "message" is the event type
    events.send("HELP_STATE:" + String(dbhelp ? "ON" : "OFF"), "message", millis());
    request->send(204); // No content response
  }
}

void handledbcmd(AsyncWebServerRequest *request) {
  String message = "";
  if (dbhelp)
  {
    dbhelp = false;
    events.send("HELP_STATE:" + String(dbhelp ? "ON" : "OFF"), "message", millis());
    message  = "Help: Enter ASCII command in input box and click CMD button to send to Pegasus.";
    events.send(message.c_str(), "message", millis()); // "message" is the event type
    events.send(cmdhelp, "message", millis());
  }else{
    if (cmdexecuting) {return;}
    dbcmd = !dbcmd;
    message = dbcmd ? "Cmd sent." : "&nbsp;";
    events.send(message.c_str(), "message", millis()); // "message" is the event type
    events.send("DBCMD_STATE:" + String(dbcmd ? "ON" : "OFF"), "message", millis());
    request->send(204); // No content response
    if(dbcmd)
    {
      resetCMDbuffer();
      request->getHeader("cmd")->value().toCharArray(cmdbuffer, sizeof(cmdbuffer));
      cmdbuffer_index = request->getHeader("cmd")->value().length();
      processDBcmd();
    }
  }
}

void processDBcmd()
{
  cmdhandler();
  dbcmd = false;
  events.send("DBCMD_STATE:OFF", "message", millis());
}

void print_reset_reason(RESET_REASON reason)
{
  switch ( reason)
  {
    case 1 : Serial.println ("POWERON_RESET");break;          /**<1, Vbat power on reset*/
    case 3 : Serial.println ("SW_RESET");break;               /**<3, Software reset digital core*/
    case 4 : Serial.println ("OWDT_RESET");break;             /**<4, Legacy watch dog reset digital core*/
    case 5 : Serial.println ("DEEPSLEEP_RESET");break;        /**<5, Deep Sleep reset digital core*/
    case 6 : Serial.println ("SDIO_RESET");break;             /**<6, Reset by SLC module, reset digital core*/
    case 7 : Serial.println ("TG0WDT_SYS_RESET");break;       /**<7, Timer Group0 Watch dog reset digital core*/
    case 8 : Serial.println ("TG1WDT_SYS_RESET");break;       /**<8, Timer Group1 Watch dog reset digital core*/
    case 9 : Serial.println ("RTCWDT_SYS_RESET");break;       /**<9, RTC Watch dog Reset digital core*/
    case 10 : Serial.println ("INTRUSION_RESET");break;       /**<10, Instrusion tested to reset CPU*/
    case 11 : Serial.println ("TGWDT_CPU_RESET");break;       /**<11, Time Group reset CPU*/
    case 12 : Serial.println ("SW_CPU_RESET");break;          /**<12, Software reset CPU*/
    case 13 : Serial.println ("RTCWDT_CPU_RESET");break;      /**<13, RTC Watch dog Reset CPU*/
    case 14 : Serial.println ("EXT_CPU_RESET");break;         /**<14, for APP CPU, reseted by PRO CPU*/
    case 15 : Serial.println ("RTCWDT_BROWN_OUT_RESET");break;/**<15, Reset when the vdd voltage is not stable*/
    case 16 : Serial.println ("RTCWDT_RTC_RESET");break;      /**<16, RTC Watch dog reset digital core and rtc module*/
    default : Serial.println ("NO_MEAN");
  }
}

void switchBandMode()
{
  switch (BandMode)
  {
    case BandModeSelectVHFforRX:
    {
      //activate remote relay
      if (!Vsend)
      {
        digitalWrite(TLVantSel_opin, pin_active_state); //activate for RX on VHF
      }else{
        digitalWrite(TLVantSel_opin, pin_off_state); //de-activate for TX on UHF
      }
      break;
    }
    case BandModeSelectUHFforRX:
    {
      //de-activate remote relay
      if (!Vsend)
      {
        digitalWrite(TLVantSel_opin, pin_off_state); //de-activate for RX on UHF
      }else{
        digitalWrite(TLVantSel_opin, pin_active_state); //activate for TX on VHF
      }
      break;     
    }
    case BandModeVHFonly:
    {
      //always use vhf antenna = relay in
      digitalWrite(TLVantSel_opin, pin_active_state); //relay stays operated     
      break;
    }
    case BandModeUHFonly:
    {
      //always use UHF antenna = relay out
      digitalWrite(TLVantSel_opin, pin_off_state); //relay stays off         
      break;
    }
  }
}

void radioRX()
{
  if (monitorTX)
  {
    monitorTX = false;
    monitor_events.send("RXTX_STATE:RX", "message", millis());
  }

  if (radioCatOn && !radioSidePTT)
  {
    if (radioSimplex)
    {
      civ.write(radioToRX, 8); // Set to RX mode
      delay(30);
      civ.write(radioVFOA, 7); // Set to VFO A
      delay(30);
      civ.write(radioRXfreq, 11); // Set RX frequency
      delay(30);
    }else{
      civ.write(radioToRX, 8); // Set to RX mode
      delay(30);
      civ.write(radioMainVFO, 7); // Set to Main VFO
      delay(30);
      civ.write(radioRXfreq, 11); // Set RX frequency
      delay(30);
      civ.write(radioAttenuatorOff, 7); // Set 10dB gain off
      delay(30);
      civ.write(radioMaxRXGain, 8); // Set RX gain to maximum
      delay(10);
    }

    Vsend = false; // Vsend is used for antenna changes and civ frequency updates.

    // Antenna control.
    digitalWrite(pino_amp_Vsend, pin_active_state); // Amp is off with 0v on gate.
    // Antenna selection mode.
    switchBandMode();

    ptt_rotators_state(); // Start turn measurement

    neopixels(lastMatrixColor); // Restore previous color

    AZ_Old = AZtt_Old;
    if (!talknturningAZ)
    {
      delay(MotorSettleDelay);
      get_AZ_North_Degrees(); // Where are we now?
      AZ_Old = AZ_External_Degrees;
    }

    ptt_rotators_state();

    logAZTurnnTalk("Radio now RX", true);
    logELTurnnTalk("Radio now RX", true);
    logAZrotator("Radio now RX", true);
    logELrotator("Radio now RX", true);
  }
}

void radioTX() // True to (radio)TX without radio PTT down.
{
  if (!monitorTX)
  {
    monitorTX = true;
    monitor_events.send("RXTX_STATE:TX", "message", millis());
  }

  if (radioCatOn && !radioSidePTT)
  {
    logAZTurnnTalk("Radio now TX", true);
    logELTurnnTalk("Radio now TX", true);
    logAZrotator("Radio now TX", true);
    logELrotator("Radio now TX", true);

    Vsend = true; // Vsend is used for antenna changes and civ frequency updates.
    Hsend = false;

    // Antenna control.
    digitalWrite(pino_amp_Vsend, pin_off_state); // Amp on with positive on gate.
    // Antenna selection mode.
    switchBandMode();

    if (radioSimplex)
    {
      civ.write(radioVFOA, 7); // Set to VFO A
      delay(30);
      civ.write(radioTXfreq, 11); // Set TX frequency
      delay(30);
      civ.write(radioToTX, 8); // Set to TX mode
      delay(10);
    }else{
      civ.write(radioMainVFO, 7); // Set to Main VFO
      delay(30);
      civ.write(radio10dbAttenuator, 7); // Set 10dB -gain
      delay(30);
      civ.write(radioMinRXGain, 7); // Set RX gain to minimum
      delay(30);
      civ.write(radioSubVFO, 7); // Set to Sub VFO
      delay(30);
      civ.write(radioTXfreq, 11); // Set TX frequency
      delay(30);
      civ.write(radioToTX, 8); // Set to TX mode
      delay(10);
    }

    ptt_rotators_state(); // Stop turn measurement

    neopixels(49); // RED for PTT TX

    AZtt_Requested = AZ_Requested;
    AZtt_Old = AZ_Old;
    AZtt_Distance = 0;
  }
}

void PTT_handler()
{
  if (disable_ptt) { return; }

  radioSideTX = !digitalRead(pini_int_Vsend); // gnd is ptt.

  if (radioSideTX && !radioSidePTT && !Vsend)
  {
    radioSidePTT = true;
  }

  if (radioSidePTT)
  {
    if (radioSideTX)
    {
      radioTX();
    }else{
      radioSidePTT = false;
      radioRX();
    }
  }
}

void turn_talk()
{
  if (!enableTurnnTalk) { return; }
  if (disable_rotators) { return; }
  if (!satTracking) { return; }

  if (!radioTXing) // radio gone to RX
  {
    if (talknturningEL) // only true when turning in PTT.
    {
      ELtalkstop(); // PTTinTX has just gone off.
    }    
    return;
  }else{ // PTT is down.
    // talktimeout is set in onesecond elapse when PTT is down. 
    // This allows for a delay before Talk n Turn starts, to allow for quick PTT taps without turning.
    if (!talknturningAZ) // Not Talk Turning now.
    {
      AZtt_Requested = AZ_Requested;
      // Make sure that there has been a command update before turning blindly.
      if (AZtt_Requested != AZtt_Old)
      {
        // Distance to turn while talking.
        compassFromDeg = AZtt_Old;
        compassToDeg = AZtt_Requested;
        getShortestTurn();
        AZtt_Distance = compassDirectionDistance;
        cw = compassUseDirection;
        
        if (SatTurnCW == cw)
        {
          talknturningAZ = true; // only true when turning in PTT.
          cw ? rotate_az_cw() : rotate_az_ccw();
        }else{
          logAZTurnnTalk("Wrong way in turn_talk.", false);
        }
      }
    }

    // talktimeout is set in onesecond elapse when PTT is down. 
    // This allows for a delay before Talk n Turn starts, to allow for quick PTT taps without turning.
    if (!talknturningEL)
    {
      // Make sure that there has been a command update before turning blindly.
      if (EL_Requested != EL_Old)
      {
        talknturningEL = true; // only true when turning in PTT.

        if (talknturningEL)
        {
          t_tELstartMS = millis();
          satELup ? rotate_el_up() : rotate_el_down();
        }
      }
    }

    if (talknturningEL) // only true when turning in PTT.
    {
      t_tELnowMS = millis();
      t_tELdeltaMS = t_tELnowMS - t_tELstartMS;
      if (t_tELdeltaMS > ELrunMS)
      {
        ELtalkstop();
      }
    }
  }
}

void ELtalkstop()
{
  talknturningEL = false; // only true when turning in PTT.
  digitalWrite(rotate_up_opin, pin_off_state); // Turn off the Rotate Up Pin
  digitalWrite(rotate_down_opin, pin_off_state);   // Turn off the Rotate Down Pin
  logELTurnnTalk("in ELtalkstop.", false);
  EL_Old = EL_Requested;
  EL_Distance = 0;
}

void sendDBAZ(bool requested)
{
  if (!dashboardison) { return; }
  char dbazStringBuff[6];
  if (requested)
  {
    snprintf(dbazStringBuff, sizeof(dbazStringBuff), "%03d", AZ_Requested);
  }else{
    snprintf(dbazStringBuff, sizeof(dbazStringBuff), "%03d", AZ_External_Degrees);
  }
  events.send("DBAZ:" + String(dbazStringBuff), "message", millis());
}

void sendDBEL(bool requested)
{
  if (!dashboardison) { return; }
  char dbelStringBuff[4];
  if (requested)
  {
    snprintf(dbelStringBuff, sizeof(dbelStringBuff), "%03d", EL_Requested);
  }else{
    snprintf(dbelStringBuff, sizeof(dbelStringBuff), "%03d", EL_Mapped_Degrees);
  }
  events.send("DBEL:" + String(dbelStringBuff), "message", millis());
}

void cmdDashboardOut(String response)
{
  if (!dashboardison) { return; }
  String temp = response;

  temp.replace("\r\n", "<br>");
  temp.replace("\n", "<br>");

  String tosend = "CMD_RESPONSE" + temp;

  int overflow = 0;

  // "message" is the event
  uint8_t status = events.send(tosend.c_str(), "message", millis());

  while (status == 2) { // PARTIALLY_ENQUEUED
    delay(10); // Wait until the event is sent successfully
    overflow++;
    if (overflow > 3) {
      Serial.println("Dashboard event overflow. Could not send message.");
      break;
    }
  }
}

void updatedbtime()
{
  if (!RTCtimeOn || !dashboardison) { return; }
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("No time available (yet)");
    return;
  }
  char timeStringBuff[20];
  strftime(timeStringBuff, sizeof(timeStringBuff), "%y/%m/%d %H:%M", &timeinfo);
  events.send("RTC_TIME:" + String(timeStringBuff), "message", millis());
}

// No reporting here
void printLocalTime(bool loadRadio) {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("No time available (yet)");
    return;
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  time_t tt = mktime(&timeinfo); // update unixtime
  unixtime = tt;

  Serial.println("Time loaded OK");

  if (!radioCatOn && loadRadio)
  {
    Serial.println("Turn on radioCatOn switch FIRST!");
    return;
  }

  if (loadRadio)
  {
    uint8_t radioDate[13] = {0XFE, 0XFE, 0XA2, 0XE0, 0X1A, 0X05, 0X01, 0X79, 0X20, 0X25, 0X10, 0X24, 0XFD}; // Date
    uint8_t year[2] = {0, 0};
    BCDarrayFromInt(timeinfo.tm_year + 1900, year, 2);
    uint8_t month = 0;
    BCDarrayFromInt(timeinfo.tm_mon + 1, &month, 1);
    uint8_t day = 0;
    BCDarrayFromInt(timeinfo.tm_mday, &day, 1);
    radioDate[8] = year[0];
    radioDate[9] = year[1];
    radioDate[10] = month;
    radioDate[11] = day;

    civ.write(radioDate, 13);
    delay(50);

    uint8_t radioTime[11] = {0XFE, 0XFE, 0XA2, 0XE0, 0X1A, 0X05, 0X01, 0X80, 0X12, 0X15, 0XFD}; // Time
    uint8_t hour = 0;
    BCDarrayFromInt(timeinfo.tm_hour, &hour, 1);
    uint8_t min = 0;
    BCDarrayFromInt(timeinfo.tm_min, &min, 1);
    radioTime[8] = hour; // 12
    radioTime[9] = min; // 15

    civ.write(radioTime, 11);
    delay(50);

    uint8_t radioUTC[12] = {0XFE, 0XFE, 0XA2, 0XE0, 0X1A, 0X05, 0X01, 0X84, 0X02, 0X00, 0X00, 0XFD}; // UTC

    civ.write(radioUTC, 12);
  }
}

// Callback function (gets called when time adjusts via NTP)
void timeavailable(struct timeval *t) {
  if (RTCtimeOn){ return; }
  Serial.println("Got time adjustment from NTP!");
  printLocalTime(false);
  RTCtimeOn = true;
}

void RTCtime()
{
  if (RTCtimeOn)
  {
    printLocalTime(false);  // it will take some time to sync time :)
    return;
  }

  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer1, ntpServer2);
  delay(10);
  printLocalTime(false);
  RTCtimeOn = true;
}

void check_start()
{
  bool AZtostart = false; // Only run when true.
  bool ELtostart = false;

  if (disable_rotators || radioTXing ||  talknturningAZ || talknturningEL) { return; }
  if (AZmanualmoving || ELmanualmoving) { return; }

  portENTER_CRITICAL_ISR(&AZstopTimerMux);
  bool temp = AZ_waitingToStop;
  portEXIT_CRITICAL_ISR(&AZstopTimerMux);

  // Must AZ move or not?
  // Old_Requested_AZ = Requested_AZ = AZ_External_Degrees; when stopped here. 
  if (!temp) // Has stopped already.
  { // Ignore No Change commands.
    if (AZ_Requested == AZ_Old)
    {
      AZtostart = false;
    }else{
      compassFromDeg = AZ_External_Degrees;
      compassToDeg = AZ_Requested;
      getShortestTurn();
      cw = compassUseDirection;
      AZ_Distance = compassDirectionDistance;
      if (AZcmdfirststep)
      {
        AZnewdir = cw;
      }else{
        if (AZnewdir != cw)
        {
          AZtostart = false;
        }
      }
      if (AZ_Distance <= 0)
      {
        AZtostart = false; // ???
        AZ_Old = AZ_Requested;
      }else{
        AZtostart = true; // continue to run.
      }

      if (AZtostart)
      {
        if (AZovershoot)
        {
          AZtostart = true;
          AZcmdfirststep = true;
          logAZrotator("AZ overshoot in check_start", false);
        }else{
          if (satTracking)
          {
            if (SatTurnCW != cw)
            {
              AZovershoot = true;
              AZtostart = true;
              AZcmdfirststep = true;
              logAZrotator("AZovershootSat. In check_start.\n", false);
            }
          }        
        }
      }
    }
  }else{ // Busy turning. Do no more.
    AZtostart = false;
  }

  // Must EL move or not?
  // Old_Requested_EL = Requested_EL; when stopped here.

  if (!EL_waitingToStop) // Has stopped already.
  { // Ignore No Change commands.
    if (EL_Requested == EL_Old)
    {
      ELtostart = false;
    }else{
      ELtostart = true;
    }
  }else{
    ELtostart = false;
  }

  if (ELtostart)
  {
    Calculate_EL_Mapped_Degrees();
    if ((EL_current_ADC < KR500_Min_Voltage) || (EL_current_ADC > KR500_Max_Voltage))
    {
      if ((EL_Requested >= MinELdegrees) && (EL_Requested <= MaxELdegrees))
      {
        ELtostart = true;
        logELrotator("EL range exceeded in check_start", true);
      }else{
        ELtostart = false;
        logELrotator("EL range error in check_start", true);
      }
    }
    // Also use EL_Mapped_Degrees because that can be different from Old_Requested_EL.
    if (EL_Requested == EL_Mapped_Degrees)
    {
      EL_Old = EL_Requested;
      ELtostart = false;
    } 
  }

  if (ELtostart)
  { // Stopped by recent timeout.
    EL_Distance = EL_Requested - EL_Mapped_Degrees;

    if (EL_Distance == 0)
    {
      ELtostart = false;
    }else{
      if (EL_Distance < 0) // Down
      {
        EL_Dropping = true;
      }else{
        EL_Dropping = false;
      }
    }
  }

  if (AZtostart) // All OK
  {
    cw ? rotate_az_cw() : rotate_az_ccw();
    AZcmdfirststep = false;
  }

  if (ELtostart) // All OK
  {
    if (satTracking)
    {
      if (satELup)
      {
        rotate_el_up(); // Rotate Up if positive
      }else{
        rotate_el_down(); // Rotate Down if negative
      }
    }else{
      if (!EL_Dropping)   //We need to move UP
      {
        rotate_el_up(); // Rotate Up if positive
      } else {
        rotate_el_down(); // Rotate Down if negative
      }
    }
  }
}

void AZendofturn()
{
  AZcmdfirststep = false;
  sendDBAZ(false);
}

void AZ_DistanceToAZrunMS()
{
  if (AZmanualmoving) { return; }
  if (talknturningAZ)
  {
    if (AZtt_Distance <= 0){ AZrunMS = 0; return; }
  }else{
    if (AZ_Distance <= 0){ AZrunMS = 0; return; }
  }

  if (talknturningAZ)
  {
    if (AZtt_Distance >= 10)
    {
      AZrunMS = AZms10degtnt;
      return;
    }

    switch (AZtt_Distance)
    {
      case 1:
        AZrunMS = AZms1degtnt;
        break;
      case 2:
        AZrunMS = AZms2degtnt;
        break;
      case 3:
        AZrunMS = AZms3degtnt;
        break;
      case 4:
        AZrunMS = AZms4degtnt;
        break;
      case 5:
        AZrunMS = AZms5degtnt;
        break;
      case 6:
        AZrunMS = AZms6degtnt;
        break;
      case 7:
        AZrunMS = AZms7degtnt;
        break;
      case 8:
        AZrunMS = AZms8degtnt;
        break;
      case 9:
        AZrunMS = AZms9degtnt;
        break;      
    }
  }else{
    if (AZ_Distance >= 10)
    {
      AZrunMS = AZms10deg;
      return;
    }

    switch (AZ_Distance)
    {
      case 1:
        AZrunMS = AZms1deg;
        break;
      case 2:
        AZrunMS = AZms2deg;
        break;
      case 3:
        AZrunMS = AZms3deg;
        break;
      case 4:
        AZrunMS = AZms4deg;
        break;
      case 5:
        AZrunMS = AZms5deg;
        break;
      case 6:
        AZrunMS = AZms6deg;
        break;
      case 7:
        AZrunMS = AZms7deg;
        break;
      case 8:
        AZrunMS = AZms8deg;
        break;
      case 9:
        AZrunMS = AZms9deg;
        break;      
    }
  }
}

void EL_DistanceToELrunMS()
{
  if (ELmanualmoving) { return; }

  if (talknturningEL)
  {
    EL_Distance = abs(EL_Requested - EL_Old);
    if (EL_Distance >= 10)
    {
      ELrunMS = ELms10degtnt;
      return;
    }

    switch (EL_Distance)
    {
      case 1:
        ELrunMS = ELms1degtnt;
        break;
      case 2:
        ELrunMS = ELms2degtnt;
        break;
      case 3:
        ELrunMS = ELms3degtnt;
        break;
      case 4:
        ELrunMS = ELms4degtnt;
        break;
      case 5:
        ELrunMS = ELms5degtnt;
        break;
      case 6:
        ELrunMS = ELms6degtnt;
        break;
      case 7:
        ELrunMS = ELms7degtnt;
        break;
      case 8:
        ELrunMS = ELms8degtnt;
        break;
      case 9:
        ELrunMS = ELms9degtnt;
        break;      
    }
  }else{
    EL_Distance = abs(EL_Requested - EL_Mapped_Degrees);
    if (EL_Distance >= 10)
    {
      ELrunMS = ELms10deg;
      return;
    }

    switch (EL_Distance)
    {
      case 1:
        ELrunMS = ELms1deg;
        break;
      case 2:
        ELrunMS = ELms2deg;
        break;
      case 3:
        ELrunMS = ELms3deg;
        break;
      case 4:
        ELrunMS = ELms4deg;
        break;
      case 5:
        ELrunMS = ELms5deg;
        break;
      case 6:
        ELrunMS = ELms6deg;
        break;
      case 7:
        ELrunMS = ELms7deg;
        break;
      case 8:
        ELrunMS = ELms8deg;
        break;
      case 9:
        ELrunMS = ELms9deg;
        break;      
    }
  }
}

void TestTimer1()
{
  if (testms < 1) { testms = millis(); } else{
    int m = millis() - testms;
    dashbnserialmsg += "TestTimer1 ms = " + String(m) + "ms.";
  }
  
  if (RgbLedRed)
  {
    RgbLedRed = false;
    neopixelWrite(48,0,0,0);
  }else{
    RgbLedRed = true;
    neopixelWrite(49,RGB_BRIGHTNESS,0,0); // Red
  }
}

void read_adc_az() // Function to read the ADC
{
  AZ_current_ADC = adc.readADC_SingleEnded(0);
}

int AzMinMaxDegrees(int deg)
{
  if (deg == 0) {return MaxAZdegrees;}
  if (deg > MaxAZdegrees) {return MaxAZdegrees;}
  if (deg < MinAZdegrees) {return MinAZdegrees;}
  return deg;
}

// returns a north calibrated int degree
int ExternalNorthAdjustDegrees(int Internal)
{
  int external = 0;
  if (is_AZ_HAM4_Super)
  {
    external = (Internal + SH4IntNdegDelta);
    if (external > MaxAZdegrees) { external = external - MaxAZdegrees; }
    if (external < 1) { external = external + MaxAZdegrees; }
    return external;
  }
  return Internal;   
}

// Use current ADC voltage degrees to find the Real AZ_External_Degrees.
void get_AZ_North_Degrees() //AZ_Mapped_Degrees internal. AZ_External_Degrees.
{
  int x = 0;

  read_adc_az();  // Read the ADC in current_AZ_ADC

  x = map(AZ_current_ADC, AZ_0_ADC, AZ_MAX_ADC, MinAZdegrees, MaxAZdegrees); // Map ADC to Deg

  x = AzMinMaxDegrees(x);
  AZ_Mapped_Degrees = x; //internal

  AZ_External_Degrees = ExternalNorthAdjustDegrees(AZ_Mapped_Degrees);
}

void read_adc_el() // Function to read the ADC
{
  EL_current_ADC = adc.readADC_SingleEnded(1);
}

// Read the ADC and convert it into an approximate logarithmic degree value.
void Calculate_EL_Mapped_Degrees()
{
  read_adc_el();

  if (EL_current_ADC < KR500_Min_Voltage)
  {
    delay(TurnErrorDelay);
    read_adc_el();
  }

  if (EL_current_ADC > KR500_Max_Voltage)
  {
    delay(TurnErrorDelay);
    read_adc_el();
  }

  if (is_EL_KR500)
  {
    if (!ELnonLinear)
    {
      EL_Mapped_Degrees = map(EL_current_ADC, EL_CAL_0_DEFAULT, EL_CAL_MAX_DEFAULT, MinELdegrees, MaxELdegrees);  // Map the Elevation Value to Degrees
    }else{
      ELclosest30(); // EL non linear mapping.
    }
  }else{
    EL_Mapped_Degrees = map(EL_current_ADC, EL_0_ADC, EL_MAX_ADC, MinELdegrees, MaxELdegrees);  // Map the Elevation Value to Degrees
  }

  if (EL_Mapped_Degrees < 0){ EL_Mapped_Degrees = 0; }
}

void ELclosest30() // EL non linear mapping.
{
  if (EL_current_ADC > EL_0_ADC)
  {
    EL_Mapped_Degrees = 0;
    return;
  }

  if (EL_current_ADC < el135deg)
  {
    EL_Mapped_Degrees = map(EL_current_ADC, el135deg, el180deg, 135, 180);
    return;
  }

  if (EL_current_ADC < el90deg)
  {
    EL91to135(); // From 91 to 135 degrees.
    return;
  }

  if (EL_current_ADC < el60deg)
  {
    EL61to90(); // From 61 to 90 degrees.
  }else if (EL_current_ADC < el30deg)
  {
    EL31to60(); // From 31 to 60 degrees.
  }else{
    EL0to30(); // From 0 to 30 degrees.
  }
}

void EL91to135()
{
  if (EL_current_ADC <= el100deg)
  {
    EL_Mapped_Degrees = map(EL_current_ADC, el135deg, el100deg, 135, 100);
  }
  else if (EL_current_ADC <= el95deg)
  {
    EL_Mapped_Degrees = map(EL_current_ADC, el95deg, el90deg, 95, 90);
  }
  else
  {
    EL_Mapped_Degrees = map(EL_current_ADC, el100deg, el95deg, 100, 95);
  }
}

void EL61to90()
{
  if (EL_current_ADC <= el85deg)
  {
    EL_Mapped_Degrees = map(EL_current_ADC, el90deg, el85deg, 90, 85);
  }else if (EL_current_ADC <= el80deg)
  {
    EL_Mapped_Degrees = map(EL_current_ADC, el85deg, el80deg, 85, 80);
  }else if (EL_current_ADC <= el75deg)
  {
    EL_Mapped_Degrees = map(EL_current_ADC, el80deg, el75deg, 80, 75);
  }else if (EL_current_ADC <= el70deg)
  {
    EL_Mapped_Degrees = map(EL_current_ADC, el75deg, el70deg, 75, 70);
  }else if (EL_current_ADC <= el65deg)
  {
    EL_Mapped_Degrees = map(EL_current_ADC, el70deg, el65deg, 70, 65);
  }else{
    EL_Mapped_Degrees = map(EL_current_ADC, el65deg, el60deg, 65, 60);
  }
}

void EL31to60()
{
  if (EL_current_ADC <= el55deg)
  {
    EL_Mapped_Degrees = map(EL_current_ADC, el60deg, el55deg, 60, 55);
  }else if (EL_current_ADC <= el50deg)
  {
    EL_Mapped_Degrees = map(EL_current_ADC, el55deg, el50deg, 55, 50);
  }else if (EL_current_ADC <= el45deg)
  {
    EL_Mapped_Degrees = map(EL_current_ADC, el50deg, el45deg, 50, 45);
  }else if (EL_current_ADC <= el40deg)
  {
    EL_Mapped_Degrees = map(EL_current_ADC, el45deg, el40deg, 45, 40);
  }else if (EL_current_ADC <= el35deg)
  {
    EL_Mapped_Degrees = map(EL_current_ADC, el40deg, el35deg, 40, 35);
  }else{
    EL_Mapped_Degrees = map(EL_current_ADC, el35deg, el30deg, 35, 30);
  }
}

void EL0to30()
{
  if (EL_current_ADC <= el25deg)
  {
    EL_Mapped_Degrees = map(EL_current_ADC, el30deg, el25deg, 30, 25);
  }else if (EL_current_ADC <= el20deg)
  {
    EL_Mapped_Degrees = map(EL_current_ADC, el25deg, el20deg, 25, 20);
  }else if (EL_current_ADC <= el15deg)
  {
    EL_Mapped_Degrees = map(EL_current_ADC, el20deg, el15deg, 20, 15);
  }else if (EL_current_ADC <= el10deg)
  {
    EL_Mapped_Degrees = map(EL_current_ADC, el15deg, el10deg, 15, 10);
  }else if (EL_current_ADC <= el05deg)
  {
    EL_Mapped_Degrees = map(EL_current_ADC, el10deg, el05deg, 10, 5);
  }else{
    EL_Mapped_Degrees = map(EL_current_ADC, el05deg, el00deg, 5, 0);
  }
}

void Reset_AZ_Degrees()
{
  get_AZ_North_Degrees();
  AZ_Old = AZ_Requested = AZ_External_Degrees;
  AZ_Distance = 0;
  AZmanualmoving = false;
  portENTER_CRITICAL_ISR(&AZstopTimerMux);
  AZ_waitingToStop = false;
  portEXIT_CRITICAL_ISR(&AZstopTimerMux);
  AZendofturn();
}

void Reset_EL_Degrees()
{
  Calculate_EL_Mapped_Degrees();
  EL_Old = EL_Requested = EL_Mapped_Degrees;
  EL_Distance = 0;
  ELmanualmoving = false;
  EL_waitingToStop = false;
  sendDBEL(false);
}

void acPowerOn()
{
  powerDelay = 0;
  if (!powerOn)
  {
    powerOn = true;
    digitalWrite(ac_power_opin, pin_active_state);
    delay(BrakeSettleDelay); // Time for 24v transformer to turn on
  }
}

void AZPutBrakesOff()
{
  if (!is_AZ_HAM4_Super){return;}
  if (AZ_Brakes_Disengaged == false)
  {
    acPowerOn();
    digitalWrite(brakes_opin, pin_active_state);
    delay(BrakeSettleDelay); // Time for brakes to disengage
    AZ_Brakes_Disengaged = true;
  }
}

void logAZrotator(String addmsg, bool onlymsg)
{
  if (!AZRotatorLogging) { return; }
  if (!SDcardIsOn || AZmanualmoving) { return; }

  if (AZloggingBusy)
  {
    LogQueueEnter(addmsg, LogType::az, onlymsg);
    return;
  }

  AZloggingBusy = true;

  String logAZ = "";
  String logTime = "";
  String extraMessage = "";

  if (addmsg.length() > 1) { extraMessage = addmsg + '\n'; }

  logTime = getNowTimeStr();

  if (!onlymsg)
  {
    logAZ += "Requested_AZ = ";
    logAZ += (AZ_Requested);
    logAZ += "\n AZ_External_Degrees = ";
    logAZ += (AZ_External_Degrees);
    logAZ += "\n AZ_Distance = ";
    logAZ += (AZ_Distance);
    logAZ += "\n Old_Requested_AZ = ";
    logAZ += (AZ_Old);
    logAZ += "\n Direction = ";
    cw ? logAZ += "cw" : logAZ += "ccw";
    logAZ += "\n new AZ Direction = ";
    AZnewdir ? logAZ += "cw" : logAZ += "ccw";
    logAZ += "\n AZrunMS = ";
    logAZ += (AZrunMS);
    logAZ += "\n New AZ Command = ";
    AZcmdfirststep ? logAZ += "Yes" : logAZ += "No";
    logAZ += "\n SatTurnCW = ";
    SatTurnCW ? logAZ += "Yes\n" : logAZ += "No\n";
  }

  if (satTracking)
  {
    logToFlash(AZ_rotaror_log_path, logTime);
    if (!onlymsg)
    {
      logToFlash(AZ_rotaror_log_path, logAZ);
    }
    if (addmsg.length() >= 1) { logToFlash(AZ_rotaror_log_path, extraMessage); }
  }else{
    appendFile(SD, AZ_rotaror_log_path, logTime.c_str(), true);
    if (!onlymsg)
    {
      appendFile(SD, AZ_rotaror_log_path, logAZ.c_str(), true);
    }
    if (addmsg.length() >= 1) { appendFile(SD, AZ_rotaror_log_path, extraMessage.c_str(), true); }
  }

  logTime = "";
  logAZ = "";
  AZloggingBusy = false;
}

void rotate_az_cw() // Function to Rotate Azimuth CW
{
  if (AZovershoot)
  {
    AZovershoot = false;
  }else{
    if (satTracking)
    {
      if (!SatTurnCW)
      {
        if (talknturningAZ)
        {
          logAZTurnnTalk("Sat Turn wrong way in rotate_az_cw.", true);
        }else{
          logAZrotator("Sat Turn wrong way in rotate_az_cw.", true);
        }
        AZ_Old = AZ_Requested;
        return;
      } // Don't turn CW if not allowed.
    }
  }

  if (!cw) // Has the AZ turning direction changed?
  {
    cw = true;
  }

  if (AZmanualmoving)
  {
    AZPutBrakesOff();
    digitalWrite(rotate_left_opin, pin_off_state);    // Make sure the Rotate Left Pin Low
    digitalWrite(rotate_right_opin, pin_active_state);    // Set the Rotate Right Pin High
    return;
  }

  AZ_DistanceToAZrunMS();
  if (AZrunMS == 0)
  {
    if (talknturningAZ)
    {
      logAZTurnnTalk("rotate_az_cw distance error.", false);
    }else{
      logAZrotator("rotate_az_cw distance error.", false);
    } 
    return;
  }

  AZstopTimer = timerBegin(1000000);
  timerAttachInterrupt(AZstopTimer, &az_time_stop_vect);
  timerAlarm(AZstopTimer, AZrunMS * 1000, false, 0);

  AZPutBrakesOff();
  digitalWrite(rotate_left_opin, pin_off_state);    // Make sure the Rotate Left Pin Low
  digitalWrite(rotate_right_opin, pin_active_state);    // Set the Rotate Right Pin High
  portENTER_CRITICAL_ISR(&AZstopTimerMux);
  AZ_waitingToStop = true;
  portEXIT_CRITICAL_ISR(&AZstopTimerMux);

  if (talknturningAZ)
  {
    logAZTurnnTalk("in rotate_az_cw", false);
  }else{
    logAZrotator("in rotate_az_cw", false);
  }
}

void rotate_az_ccw() // Function to Rotate Azimuth CCW
{
  if (AZovershoot)
  {
    AZovershoot = false;
  }else{
    if (satTracking)
    { 
      if (SatTurnCW)
      {
        if (talknturningAZ)
        {
          logAZTurnnTalk("Sat Turn wrong way in rotate_az_ccw.", true);
        }else{
          logAZrotator("Sat Turn wrong way in rotate_az_ccw.", true);
        }
        AZ_Old = AZ_Requested;
        return;
      } // Don't turn CW if not allowed.
    }
  }

  if (cw) // Has the AZ turning direction changed?
  {
    cw = false;
  }

  if (AZmanualmoving)
  {
    AZPutBrakesOff();
    digitalWrite(rotate_left_opin, pin_active_state);
    digitalWrite(rotate_right_opin, pin_off_state);
    return;
  }

  AZ_DistanceToAZrunMS();
  if (AZrunMS == 0)
  {
    if (talknturningAZ)
    {
      logAZTurnnTalk("rotate_az_ccw distance error.", false);
    }else{
      logAZrotator("rotate_az_ccw distance error.", false);
    } 
    return;
  }

  AZstopTimer = timerBegin(1000000);
  timerAttachInterrupt(AZstopTimer, &az_time_stop_vect);
  timerAlarm(AZstopTimer, AZrunMS * 1000, false, 0);

  AZPutBrakesOff();
  digitalWrite(rotate_left_opin, pin_active_state);    // Make sure the Rotate Left Pin Low
  digitalWrite(rotate_right_opin, pin_off_state);    // Set the Rotate Right Pin High
  portENTER_CRITICAL_ISR(&AZstopTimerMux);
  AZ_waitingToStop = true;
  portEXIT_CRITICAL_ISR(&AZstopTimerMux);

  if (talknturningAZ)
  {
    logAZTurnnTalk("in rotate_az_ccw", false);
  }else{
    logAZrotator("in rotate_az_ccw", false);
  }
}

String getNowTimeStr()
{
  struct tm timeinfo;
  String nowTime = "";
  
  if (!getLocalTime(&timeinfo)) {
    nowTime = "No time available (yet). In getNowTimeStr.";
    nowTime += '\n';
    dashbnserialmsg += nowTime;
    appendFile(SD, AZ_rotaror_log_path, nowTime.c_str(), true);
    return nowTime;
  }

  int64_t time_us = esp_timer_get_time();
  int current_ms = (time_us / 1000LL) % 1000LL;

  nowTime = (timeinfo.tm_year + 1900);
  nowTime += ":";
  nowTime += (timeinfo.tm_mon + 1);
  nowTime += ":";
  nowTime += (timeinfo.tm_mday);
  nowTime += ":";
  nowTime += (timeinfo.tm_hour);
  nowTime += ":";
  nowTime += (timeinfo.tm_min);
  nowTime += ":";
  nowTime += (timeinfo.tm_sec);
  nowTime += ".";
  nowTime += String(current_ms);

  return nowTime;
}

void logAZTurnnTalk(String addmsg, bool onlymsg)
{
  if (!TalkNturnLogging) { return; }

  if (AZTTloggingBusy)
  {
    LogQueueEnter(addmsg, LogType::aztt, onlymsg);
    return;
  }

  AZTTloggingBusy = true;

  String log = "";
  String logTime = "";
  String extraMessage = "";

  if (addmsg.length() > 1) { extraMessage = addmsg + '\n'; }

  logTime = getNowTimeStr();

  if (!onlymsg)
  {
    log += "\n AZ Rotator estimation start.";
    log += "\n AZtt_Requested = ";
    log += (AZtt_Requested);
    log += "\n AZtt_Old = ";
    log += (AZtt_Old);
    log += "\n AZtt_Distance = ";
    log += (AZtt_Distance);
    log += "\n AZrunMS = ";
    log += (AZrunMS);
    log += "\n Direction = ";
    cw ? log += "cw\n" : log += "ccw\n";
  }

  logToFlash(TalkNturnAZ_log_path, (logTime + log + extraMessage));

  AZTTloggingBusy = false;
}

void logELTurnnTalk(String addmsg, bool onlymsg)
{
  if (!TalkNturnLogging) { return; }

  if (ELTTloggingBusy)
  {
    LogQueueEnter(addmsg, LogType::eltt, onlymsg);
    return;
  }

  ELTTloggingBusy = true;

  String log = "";
  String logTime = "";
  String extraMessage = "";

  if (addmsg.length() > 1) { extraMessage = addmsg + '\n'; }

  logTime = getNowTimeStr();

  if (!onlymsg)
  {
    log += "\n EL Rotator estimation stop.";
    log += "\n EL_Requested = ";
    log += (EL_Requested);
    log += "\n EL_Old = ";
    log += (EL_Old);
    log += "\n EL_Distance = ";
    log += (EL_Distance);
    log += "\n ELrunMS = ";
    log += (ELrunMS);
    EL_Dropping ? log += "down\n\n" : log += "up\n\n";
  }
  
  logToFlash(TalkNturnEL_log_path, (logTime + log + extraMessage));
  ELTTloggingBusy = false;
}

int ElMinMaxDegrees(int deg)
{
  if (deg > MaxELdegrees) {return MaxELdegrees;}
  if (deg < MinELdegrees) {return MinELdegrees;}
  return deg;
}

// One second + timer tick
void ARDUINO_ISR_ATTR Timer50ms_Tick_Vect()
{
  portENTER_CRITICAL_ISR(&timer50msMux);
  if (toSecond > 48)
  {
    oneSecond = true;
    toSecond = 0;
  }else{
    toSecond++;
  }
  portEXIT_CRITICAL_ISR(&timer50msMux);
}

void ARDUINO_ISR_ATTR az_time_stop_vect()
{
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;
  
  // Unblock the manager task from the interrupt safely
  xSemaphoreGiveFromISR(AZstopTimerSemaphore, &xHigherPriorityTaskWoken);
  
  if (xHigherPriorityTaskWoken) {
    portYIELD_FROM_ISR();
  }
}

void az_rotate_cmd_stop() // Function to Stop Azimuth Rotation
{
  digitalWrite(rotate_right_opin, pin_off_state);  // Turn off the Rotate Right Pin
  digitalWrite(rotate_left_opin, pin_off_state);   // Turn off the Rotate Left Pin
  Reset_AZ_Degrees();
}

void el_time_stop() // Note. Measurement can only be made when the motors are idle.
{
  digitalWrite(rotate_up_opin, pin_off_state); // Turn off the Rotate Up Pin
  digitalWrite(rotate_down_opin, pin_off_state);   // Turn off the Rotate Down Pin
  EL_waitingToStop = false;
  if (talknturningEL)
  {
    logELTurnnTalk("Time stop from check_stop.", true);
  }else{
    logELrotator("Time stop from check_stop.", true);
  }
}

void el_rotate_cmd_stop() // Function to Stop Elevation Rotation
{
  digitalWrite(rotate_up_opin, pin_off_state); // Turn off the Rotate Up Pin
  digitalWrite(rotate_down_opin, pin_off_state);   // Turn off the Rotate Down Pin
  Reset_EL_Degrees();
}

void PTT_ELRotators_Stop()
{
  if (EL_waitingToStop)
  {
    digitalWrite(rotate_up_opin, pin_off_state); // Turn off the Rotate Up Pin
    digitalWrite(rotate_down_opin, pin_off_state);   // Turn off the Rotate Down Pin
    ELmanualmoving = false;
    EL_waitingToStop = false;
  }

  // Dont take voltage measurements.
  EL_Old = EL_Requested;
  EL_Distance = 0;
}

void Setup_Timer50ms()
{
  Timer50ms = timerBegin(1000000);
  timerAttachInterrupt(Timer50ms, &Timer50ms_Tick_Vect);
  timerAlarm(Timer50ms, 20000, true, 0);

  timerStop(Timer50ms);
}

// Serial print format.
void Padding_360_Degrees(int x, bool printnewline)
{
  if (x > 99)
  {
    printnewline ? Serial.println(x) : Serial.print(x);
    return;
  }

  if (x > 9)
  {
    Serial.print("0"); 
    printnewline ? Serial.println(x) : Serial.print(x);
    return;
  }

    Serial.print("00");
    printnewline ? Serial.println(x) : Serial.print(x);
}

void send_requested_azel() // Function to Send the Current Azimuth and Elevation
{
  Serial.print("AZ=");
  Padding_360_Degrees(AZ_Requested, false);   // Serial print format.

  Serial.print(" EL=");  // Send the Azimuth in Degrees
  Padding_360_Degrees(EL_Requested, true);   // Serial print format.

  sendDBAZ(true);
  sendDBEL(true);
}

void set_max_az_cal() // Set the Max Azimuth Calibration Function
{  

  read_adc_az();

  AZ_MAX_ADC = AZ_current_ADC;  // Set the Azimuth Maximum Calibration to Current Azimuth Reading
  
  preferences.begin("mat", false); // Open Preferences with mat namespace. RW mode (permanent storage).
  preferences.putInt("azMaxADC", AZ_MAX_ADC);
  preferences.end(); // Close the Preferences

  if (gpredict)
  {
    Serial.print("Max Azimyth Calibration Voltage percent = "); Serial.println(String(AZ_MAX_ADC));
  }
}  

void set_max_el_cal()   // Set the Max Elevation Calibration Function
{
 
  read_adc_el();

  EL_MAX_ADC = EL_current_ADC;  // Set the Elevation Max Calibration to the Current Elevation Reading
  
  preferences.begin("mat", false); // Open Preferences with mat namespace. RW mode (permanent storage).
  preferences.putInt("elMaxADC", EL_MAX_ADC);
  preferences.end(); // Close the Preferences

  if (gpredict)
  {
    Serial.print("Max Elevation Calibration Voltage percent = "); Serial.println(String(EL_MAX_ADC));
  }
}

void logELrotator(String addmsg, bool onlymsg)
{
  if (!ELRotatorLogging) { return; }
  if (!SDcardIsOn || ELmanualmoving) { return; }

  if (ELloggingBusy)
  {
    LogQueueEnter(addmsg, LogType::el, onlymsg);
    return;
  }

  ELloggingBusy = true;

  String logEL = "";
  String logTime = "";
  String extraMessage = "";

  if (addmsg.length() > 1) { extraMessage = addmsg + '\n'; }

  logTime = getNowTimeStr();

  if (!onlymsg)
  {
    logEL += "\n Requested_EL = ";
    logEL += (EL_Requested);
    logEL += "\n EL_Mapped_Degrees = ";
    logEL += (EL_Mapped_Degrees);
    logEL += "\n EL_Distance = ";
    logEL += (EL_Distance);
    logEL += "\n Old_Requested_EL = ";
    logEL += (EL_Old);
    logEL += "\n Direction = ";
    logEL += (EL_Dropping ? "Down" : "Up");
    logEL += "\n ELrunMS = ";
    logEL += (ELrunMS);
    logEL += "\n SatELup = ";
    logEL += (satELup ? "Yes\n" : "No\n");
  }

  if (satTracking)
  {
    logToFlash(EL_rotaror_log_path, logTime);
    if (!onlymsg) { logToFlash(EL_rotaror_log_path, logEL); }
    if (addmsg.length() >= 1) { logToFlash(EL_rotaror_log_path, extraMessage); }
  }else{
    appendFile(SD, EL_rotaror_log_path, logTime.c_str(), true);
    if (!onlymsg) { appendFile(SD, EL_rotaror_log_path, logEL.c_str(), true); }
    if (addmsg.length() >= 1) { appendFile(SD, EL_rotaror_log_path, extraMessage.c_str(), true); }
  }
  logTime = "";
  logEL = "";
  ELloggingBusy = false;
}

void rotate_el_up() // Function to Rotate Elevation Up
{
  if (!ELmanualmoving)
  {
    if (EL_Mapped_Degrees >= EL_Requested)
    {
      EL_Old = EL_Requested = EL_Mapped_Degrees;
       if (talknturningEL)
      {
        logELTurnnTalk("Too far in rotate_el_up.", false);
      }else{
        logELrotator("Too far in rotate_el_up.", false);
      }
      return;     
    }
    if (EL_Old >= EL_Requested)
    {
      if (talknturningEL)
      {
        logELTurnnTalk("Wrong way in rotate_el_up.", false);
      }else{
        logELrotator("Wrong way in rotate_el_up.", false);
      }
      return;
    }
  }

  EL_DistanceToELrunMS();
  digitalWrite(rotate_down_opin, pin_off_state);  // Make sure the Rotate Down Pin is Low
  digitalWrite(rotate_up_opin, pin_active_state);  // Set the Rotate Up Pin High
  acPowerOn();
  EL_Dropping = false;
  EL_waitingToStop = true;

  if (Auto_polarize == AutoPolarizeUpLHCP)
  {
    if (PolarizeRight) // Relay not on. RHCP is default.
    {
      digitalWrite(polarize_opin, pin_active_state); // Left hand polarization going up;
      PolarizeRight = false;
    }
  }

  if (Auto_polarize == AutoPolarizeUpRHCP)
  {
    if (!PolarizeRight) // Relay not on. RHCP is default.
    {
      digitalWrite(polarize_opin, pin_off_state); // Right hand polarization going up;
      PolarizeRight = true;
    }
  }

  if (!ELmanualmoving)
  {
    ELstartMillis = millis();
    if (talknturningEL)
    {
      logELTurnnTalk("in rotate_el_up.", false);
    }else{
      logELrotator("in rotate_el_up.", false);
    }
  }  
}

void rotate_el_down()  // Function to Rotate Elevation Down
{
  if (!ELmanualmoving)
  {
    if (EL_Mapped_Degrees <= EL_Requested)
    {
      EL_Old = EL_Requested = EL_Mapped_Degrees;
       if (talknturningEL)
      {
        logELTurnnTalk("Too far in rotate_el_up.", false);
      }else{
        logELrotator("Too far in rotate_el_up.", false);
      }
      return;     
    }
    if (EL_Old <= EL_Requested)
    {
      if (talknturningEL)
      {
        logELTurnnTalk("Wrong way in rotate_el_up.", false);
      }else{
        logELrotator("Wrong way in rotate_el_up.", false);
      }
      return;
    }
  }

  EL_DistanceToELrunMS();
  digitalWrite(rotate_up_opin, pin_off_state);    // Make sure the Rotate Up Pin is Low
  digitalWrite(rotate_down_opin, pin_active_state);   // Set the Rotate Down Pin High
  acPowerOn();
  EL_Dropping = true;
  EL_waitingToStop = true;

  if (Auto_polarize == AutoPolarizeUpRHCP)
  {
    if (PolarizeRight) // Relay not on. RHCP is default.
    {
      digitalWrite(polarize_opin, pin_active_state); // Left hand polarization going up;
      PolarizeRight = false;
    }
  }

  if (Auto_polarize == AutoPolarizeUpLHCP)
  {
    if (!PolarizeRight) // Relay not on. RHCP is default.
    {
      digitalWrite(polarize_opin, pin_off_state); // Right hand polarization going up;
      PolarizeRight = true;
    }
  }

  if (!ELmanualmoving)
  {
    ELstartMillis = millis();
    if (talknturningEL)
    {
      logELTurnnTalk("in rotate_el_down.", false);
    }else{
      logELrotator("in rotate_el_down.", false);
    }
  }
}

void send_requested_el() // Function to Send the Requested Elevation
{
  Serial.print(" EL=");  // Send the Azimuth in Degrees
  Padding_360_Degrees(EL_Requested, true);  // Serial print format.

  sendDBEL(true);
}

void set_0_az_cal() // Set Azimuth Zero Calibration
{

  read_adc_az();  // Read the ADC

  AZ_0_ADC = AZ_current_ADC;  // Set the Azimuth Zero Calibration to current position

  preferences.begin("mat", false); // Open Preferences with mat namespace. RW mode (permanent storage).
  preferences.putInt("azZeroADC", AZ_0_ADC);
  preferences.end(); // Close the Preferences

  if (gpredict)
  {
    Serial.print("Zero Azimuth Calibration Voltage percentage = "); Serial.println(String(AZ_0_ADC)); 
  }
}

void set_0_el_cal() //  Set the Elevation Zero Calibration
{

  read_adc_el();  // Read the ADC

  EL_0_ADC = EL_current_ADC;  // Set the Elevation Zero Calibration to current position
  
  preferences.begin("mat", false); // Open Preferences with mat namespace. RW mode (permanent storage).
  preferences.putInt("elZeroADC", EL_0_ADC);
  preferences.end(); // Close the Preferences

  if (gpredict)
  {
    Serial.print("Zero Elevation Calibration Voltage percentage = "); Serial.println(String(EL_0_ADC)); 
  }
}

// Note. Measurement can only be made when the motors are idle.
void check_stop() // Check to see if we've been commanded to move
{
  if (radioTXing) { return; }

  // AZ stop is done in task

  if ((EL_waitingToStop) && (!ELmanualmoving) && (!talknturningEL)) // EL is moving
  {
    ELnowMillis = millis();
    ELlenMillis = ELnowMillis - ELstartMillis;
    if (ELlenMillis > ELrunMS)
    {
      el_time_stop();
    }
  }
}

void X_routine()
{
  if (cmdbuffer_index == 2)
  {
    if (cmdbuffer[1] == 49) // X1 debug routine
    {     
      dashbnserialmsg += "AZ" + String(Mapped) + String(AZ_Mapped_Degrees) + "<br />" +
            "Requested_AZ = " + String(AZ_Requested) + "<br />" +
            "Old_Requested_AZ = " + String(AZ_Old) + "<br />" +
            "AZ_External_Degrees = " + String(AZ_External_Degrees) + "<br />" +
            "AZ" + String(Distance) + String(AZ_Distance) + "<br />" +
            (cw ? Right : Left);
      return;
    }
    if (cmdbuffer[1] == 50) // X2 debug routine
    {     
      dashbnserialmsg += "EL" + String(Mapped) + String(EL_Mapped_Degrees) + "<br />" +
            "Old_Requested_EL = " + String(EL_Old) + "<br />" +
            "Requested_EL = " + String(EL_Requested) + "<br />" +
            "EL" + String(Distance) + String(EL_Distance);
      return;
    }
    return;
  }

  get_AZ_North_Degrees();
  Calculate_EL_Mapped_Degrees();

  dashbnserialmsg += "AZ Current ADC voltage count = " + String(AZ_current_ADC) + "<br />" +
        "AZ Current internal ADC degrees = " + String(AZ_Mapped_Degrees) + "<br />" +
        "EL Current ADC voltage count = " + String(EL_current_ADC) + "<br />" +
        "EL Current internal ADC degrees = " + String(EL_Mapped_Degrees);
}

void ptt_rotators_state()
{
  if (Vsend && !radioTXing)
  {
    radioTXing = true;
    PTT_ELRotators_Stop();
    return;
  }

  if (radioTXing && !Vsend)
  {
    radioTXing = false;
  }
}

void one_second()
{
  portENTER_CRITICAL_ISR(&timer50msMux);
  bool onestemp = oneSecond;
  portEXIT_CRITICAL_ISR(&timer50msMux);
  portENTER_CRITICAL_ISR(&AZstopTimerMux);
  bool azrun = AZ_waitingToStop;
  portEXIT_CRITICAL_ISR(&AZstopTimerMux);

  if (onestemp)
  {
    portENTER_CRITICAL_ISR(&timer50msMux);
    oneSecond = false;
    toSecond = 0;
    portEXIT_CRITICAL_ISR(&timer50msMux);

    if (satTracking)
    {
      if (Satspeedchange || SatGo)
      {
        SatGoCounter = 0;
      }else{
        if (SatGoCounter >= SatCycleSeconds)
        {
          SatGoCounter = 0;
          SatGo = true;
        }else{
          SatGoCounter++;
        }
      }

      if (AOSok && (dopplerTimeCounter > dopplerTenSecUpdate))
      {
        dopplerTimeCounter = 0;
        switch (BandMode)
        {
          case BandModeSelectUHFforRX:
          {
            TXdopplerFreq = TXdopplerFreq + dopplerDelta;
            RXdopplerFreq = RXdopplerFreq - dopplerUhfDelta;
            break;
          }
          case BandModeSelectVHFforRX:
          {
            TXdopplerFreq = TXdopplerFreq + dopplerUhfDelta;
            RXdopplerFreq = RXdopplerFreq - dopplerDelta;
            break;
          }
          case BandModeVHFonly:
          {
            TXdopplerFreq = TXdopplerFreq + dopplerDelta;
            RXdopplerFreq = RXdopplerFreq - dopplerDelta;
            break;
          }
          case BandModeUHFonly:
          {
            TXdopplerFreq = TXdopplerFreq + dopplerUhfDelta;
            RXdopplerFreq = RXdopplerFreq - dopplerUhfDelta;
            break;
          }
        }
        radioUpdateDoppler();
      }else{
        dopplerTimeCounter++;
      }
    }

    if (satwaiting)
    {
      tm st;
      getLocalTime(&st);
      if (st.tm_hour == waithour && st.tm_min == waitminute && st.tm_sec == waitsecond)
      {
        satwaiting = false;
        startTracking();
      }
    }

    if ((satTracking || autoPolarizeTest) && autoPolarize && radioCatOn)
    {
      if (autoSmeterCount > PolarizeSecondsDelay)
      {
        autoSmeterCount = 0;
        autoPolarize = false;
        radioAutoSmeter_wait = true;
        firstAutoSmeterTest = true;
        readRadioOnce = true;
        SmeterLevelReq = true;
        civ.write(radioSmeter, 7);
      }else{
        autoSmeterCount++;
      }
    }

    if (TestTimer)
    {
      TestTimer1();
    }

    if (powerOn)
    {
      if (powerDelay > PowerDelaySeconds)
      {
        digitalWrite(ac_power_opin, pin_off_state);
        powerOn = false;
        powerDelay = 0;
      }else if (!AZmanualmoving && !ELmanualmoving && !azrun && !EL_waitingToStop)
      {
        powerDelay++;
      }

      if (AZ_Brakes_Disengaged)
      {
        if (azrun || AZmanualmoving)
        {
          BrakeOnTimer = 0;
        }else{
          BrakeOnTimer++;
          if (BrakeOnTimer > 2)
          {
            digitalWrite(brakes_opin, pin_off_state);
            delay(BrakeSettleDelay); // Time for brake to settle
            AZ_Brakes_Disengaged = false;            
          }
        }
      }
    }

    if (dashboardison){
      if (dbtimecounter >= dbtimeinterval)
      {
        updatedbtime();
        dbtimecounter = 0;
        sendDBAZ(false);
        sendDBEL(false);
      }else{
        dbtimecounter++;
      }
    }
  }
}

void send_requested_az() // Send the Requested Azimuth Function
{
  int x = AZ_Requested;;
  Serial.print("+0");
  Padding_360_Degrees(x, true);  // Send the Azimuth in Degrees. Serial print format.
  sendDBAZ(true);
}

// Function to convert an integer to a BCD byte array
void BCDarrayFromInt(int value, uint8_t* bcdArray, int arraySize) {
  // Ensure the input value is non-negative
  if (value < 0) {
    value = 0; // Or handle as an error
  }

  // Iterate from the last element of the BCD array (least significant BCD byte)
  for (int i = arraySize - 1; i >= 0; i--) {
    uint8_t upperNibble = (value / 10) % 10; // Extract tens digit
    uint8_t lowerNibble = value % 10;       // Extract units digit

    bcdArray[i] = (upperNibble << 4) | lowerNibble; // Combine into a BCD byte
    value /= 100; // Move to the next pair of digits
  }
}

int GetCmdStringInt(int bufferindex, int len)
{
  int i = 0;
  char oneChar;
  String SerialString = "";

  if (len == 1)
  {
    oneChar = cmdbuffer[bufferindex];
    SerialString.concat(oneChar);
  }else{
    while(i < len)
    {
      oneChar = char(cmdbuffer[i + bufferindex]);
      SerialString.concat(oneChar);
      i++;
    }
  }

  return SerialString.toInt();
}

double GetSerialStringDouble(int bufferindex, int len)
{
  int i = 0;
  char oneChar;
  String SerialString = "";

  if (len == 1)
  {
    oneChar = cmdbuffer[bufferindex];
    SerialString.concat(oneChar);
  }else{
    while(i < len)
    {
      oneChar = char(cmdbuffer[i + bufferindex]);
      SerialString.concat(oneChar);
      i++;
    }
  }

  return SerialString.toDouble();
}

String GetSerialString(int bufferindex, int len)
{
  int i = 0;
  char oneChar;
  String SerialString = "";

  if (len == 1)
  {
    oneChar = cmdbuffer[bufferindex];
    SerialString.concat(oneChar);
  }else{
    while(i < len)
    {
      oneChar = char(cmdbuffer[i + bufferindex]);
      SerialString.concat(oneChar);
      i++;
    }
  }

  return SerialString;
}

void GetFileName(int bufferindex, int len)
{
  int i = 0;
  char oneChar;

  if (len == 1)
  {
    oneChar = cmdbuffer[bufferindex];
    filename[1] = oneChar;
    filename[2] = '\0';
  }else{
    while(i < len)
    {
      oneChar = char(cmdbuffer[i + bufferindex]);
      // first char is /
      filename[i+1] = oneChar;
      i++;
    }
    filename[i+1] = '\0';
  }
}

void SDcardOn()
{
  if (SDcardIsOn){ return; }
  SPI.begin(SCK, MISO, MOSI, CS);
  if (!SD.begin(CS)) 
  {
    Serial.println("SD Card MOUNT FAIL");
  } 
  else 
  {
    Serial.println("SD Card MOUNT SUCCESS");
    Serial.println("");
    uint32_t cardSize = SD.cardSize() / megaByte;
    String str = "SDCard Size: " + String(cardSize) + "MB";
    Serial.println(str);
    uint8_t cardType = SD.cardType();
    if(cardType == CARD_NONE)
    {
      Serial.println("No SD card attached");
    }
    Serial.print("SD Card Type: ");
    if(cardType == CARD_MMC)
    {
        Serial.println("MMC");
    } 
    else if(cardType == CARD_SD)
    {
        Serial.println("SDSC");
    } 
    else if(cardType == CARD_SDHC)
    {
        Serial.println("SDHC");
    } 
    else 
    {
        Serial.println("UNKNOWN");
    }
    SDcardIsOn = true;
  } 
}

void SDcardOff()
{
  SD.end();
  SPI.end();
  SDcardIsOn == false;
  Serial.println("SD Card is OFF");
}

void listDir(fs::FS &fs, const char * dirname, uint8_t levels)
{
  if (!SDcardIsOn){ dashbnserialmsg += SDunav + "<br />"; return; }
  dashbnserialmsg += "Listing directory: ";
  dashbnserialmsg += String(dirname);
  dashbnserialmsg += "<br />";
  File root = fs.open(dirname);
  if(!root){
    dashbnserialmsg += "Failed to open directory<br />";
    return;
  }
  if(!root.isDirectory()){
    dashbnserialmsg += "Not a directory<br />";
    return;
  }

  File file = root.openNextFile();
  while(file){
    if(file.isDirectory()){
      dashbnserialmsg += "  DIR : ";
      dashbnserialmsg += String(file.name());
      dashbnserialmsg += "<br />";
      if(levels){
        listDir(fs, file.name(), levels -1);
      }
    } else {
      dashbnserialmsg += "  FILE: ";
      dashbnserialmsg += String(file.name());
      dashbnserialmsg += "  SIZE: ";
      dashbnserialmsg += String(file.size());
      dashbnserialmsg += "<br />";
    }
    file = root.openNextFile();
  }
  dashbnserialmsg += "Total space: ";
  dashbnserialmsg += String(SD.totalBytes() / megaByte);
  dashbnserialmsg += "MB<br />";
  dashbnserialmsg += "Used space: ";
  dashbnserialmsg += String(SD.usedBytes() / megaByte);
  dashbnserialmsg += "MB<br />";
}

void readFile(fs::FS &fs, const char * path){
  dashbnserialmsg += "Reading file: ";
  dashbnserialmsg += String(path);
  dashbnserialmsg += "<br />";
  File file = fs.open(path);
  if(!file){
    dashbnserialmsg += "Failed to open file for reading<br />";
    return;
  }

  while(file.available()){
    String line = file.readStringUntil('\n');
    dashbnserialmsg += line;
    dashbnserialmsg += "<br />";
  }
  file.close();
}

void readTLEsetup()
{
  if (!SDcardIsOn)
  {
    SDcardOn();
  }

  if (!SDcardIsOn){ Serial.println(SDunav); return; }

  if (!SD.exists("/TLE.txt"))
  {
    Serial.println("No TLE file found on SD card");
    return;
  }

  File file = SD.open("/TLE.txt");
  if(!file){
    Serial.println("Failed to open TLE file for reading");
    return;
  }

  String line1 = "";
  String line2 = "";
  String name = "";

  if (file.available())
  {
    name = file.readStringUntil('\n');
    name.trim();
  }
  if (file.available())
  {
    line1 = file.readStringUntil('\n');
    line1.trim();
  }
  if (file.available())
  {
    line2 = file.readStringUntil('\n');
    line2.trim();
  }

  file.close();

  if (name.length() > 0 && line1.length() > 0 && line2.length() > 0)
  {
      satelliteName = name;
      tleLine1string = line1;
      tleLine2string = line2;

      // format for sgpd4
      satelliteName.toCharArray(satname, sizeof(satname));
      tleLine1string.toCharArray(tle_line1, sizeof(tle_line1));
      tleLine2string.toCharArray(tle_line2, sizeof(tle_line2));
  }else{
      Serial.println("Error: TLE file is incomplete or improperly formatted.");
  }
}

void writeFile(fs::FS &fs, const char * path, const char * message, bool appendNewLine){

  dashbnserialmsg += ("Writing file: " + String(path));

  if (!SDcardIsOn)
  {
    SDcardOn();
  }

  if (!SDcardIsOn){ dashbnserialmsg += SDunav + "<br />"; return; }

  File file = fs.open(path, FILE_WRITE);
  if(!file){
    dashbnserialmsg += "Failed to open file for writing<br />";
    return;
  }
  if(!file.print(message)){
    dashbnserialmsg += "Write failed<br />";
  }
  if (appendNewLine)
  {
    file.print("\n");
  }
  file.close();
}

void appendFile(fs::FS &fs, const char * path, const char * message, bool appendNewLine){
  File file = fs.open(path, FILE_APPEND);
  if(!file){
    Serial.println("Failed to open file for appending");
    return;
  }
  if(!file.print(message)){
    Serial.println("Append failed");
  }
  if (appendNewLine)
  {
    file.print("\n");
  }
  file.close();
}

void appendFileDouble(fs::FS &fs, const char * path, double message, bool appendNewLine){

  File file = fs.open(path, FILE_APPEND);
  if(!file){
    Serial.println("Failed to open file for appending");
    return;
  }
  if(!file.print(message)){
    Serial.println("Append failed");
  }
  if (appendNewLine)
  {
    file.print("\n");
  }
  file.close();
}

void deleteFile(fs::FS &fs, const char * path)
{
  dashbnserialmsg += ("<br />Deleting file: " + String(path) + "<br />");
  if(fs.remove(path)){
    dashbnserialmsg += "File deleted<br />";
  } else {
    dashbnserialmsg += "Delete failed<br />";
  }
}

void retrieveTLEelementsForSatellite(int catalogueNumber)
{
  bool errorOccurred = false;

  // http://www.celestrak.org/NORAD/elements/gp.php?CATNR=25544&FORMAT=TLE
  String url = "http://celestrak.org/NORAD/elements/gp.php?CATNR=" + String(catalogueNumber) + "&FORMAT=TLE";
  HTTPClient http;
  http.begin(url);
  int httpResponseCode = http.GET();

  if (httpResponseCode == 200)
  {
      String payload = http.getString();

      int firstLineEnd = payload.indexOf('\n');
      int secondLineEnd = payload.indexOf('\n', firstLineEnd + 1);

      if (firstLineEnd > 0 && secondLineEnd > firstLineEnd)
      {
          satelliteName = payload.substring(0, firstLineEnd);
          satelliteName.trim(); // Trim whitespace after assignment

          tleLine1string = payload.substring(firstLineEnd + 1, secondLineEnd);
          tleLine1string.trim(); // Trim whitespace after assignment

          tleLine2string = payload.substring(secondLineEnd + 1);
          tleLine2string.trim(); // Trim whitespace after assignment

          // format for sgpd4
          satelliteName.toCharArray(satname, sizeof(satname));
          tleLine1string.toCharArray(tle_line1, sizeof(tle_line1));
          tleLine2string.toCharArray(tle_line2, sizeof(tle_line2));

          if (!SDcardIsOn){ dashbnserialmsg += SDunav + "<br />"; return; }

          writeFile(SD, "/TLE.txt", satelliteName.c_str(), true);
          appendFile(SD, "/TLE.txt", tleLine1string.c_str(), true);
          appendFile(SD, "/TLE.txt", tleLine2string.c_str(), true); 
      }
      else
      {
          dashbnserialmsg += "Error: Unable to parse TLE data.<br />";
          errorOccurred = true;
      }
  }
  else
  {
      dashbnserialmsg += "Error: HTTP response code " + String(httpResponseCode);
      errorOccurred = true;
  }
  http.end();

  dashbnserialmsg += "<br />TLE data has been downloaded.<br />";
}

String determineTLEage(String line1charArray)
{
    time_t tleEpochUnix = 0;
    String line1 = String(line1charArray); // Convert char array to String

    // Extract timestamp from Line 1 (Plain Text)
    int epochYear = line1.substring(18, 20).toInt() + 2000; // Convert 2-digit year to full year
    float epochDay = line1.substring(20, 32).toFloat();     // Extract fractional day of the year

    // Convert to Unix timestamp
    struct tm tleTime = {0};
    tleTime.tm_year = epochYear - 1900; // tm_year is years since 1900
    tleTime.tm_mday = 1;                // Start from January 1st
    tleTime.tm_mon = 0;
    tleEpochUnix = mktime(&tleTime) + (unsigned long)((epochDay - 1) * 86400); // Add fractional days

    // Get current time
    time_t currentTime = time(NULL);
    unixtime = currentTime;

    // Calculate TLE age
    //
    unsigned long ageInSeconds = unixtime - tleEpochUnix;
    unsigned long ageInDays = ageInSeconds / 86400;
    unsigned long remainingSeconds = ageInSeconds % 86400;
    unsigned long ageInHours = remainingSeconds / 3600;
    unsigned long ageInMinutes = (remainingSeconds % 3600) / 60;

    char ageFormatted[50];
    //sprintf(ageFormatted, "%lu day(s), %lu hour(s), %lu minute(s)", ageInDays, ageInHours, ageInMinutes);

    unsigned long totalHours = ageInSeconds / 3600;          // Total hours (can exceed 24)
    unsigned long totalMinutes = (ageInSeconds % 3600) / 60; // Remaining minutes

    char ageHHMM[10];                                        // Buffer to hold the formatted string
    sprintf(ageHHMM, "%lu:%02lu", totalHours, totalMinutes); // Format hours and minutes

    return String(ageHHMM);
}

void startTracking()
{
  Reset_AZ_Degrees();
  Reset_EL_Degrees();
  // Reset 1 second timer.
  portENTER_CRITICAL_ISR(&timer50msMux);
  oneSecond = false;
  toSecond = 0;
  portEXIT_CRITICAL_ISR(&timer50msMux);
  SatLastPeakEL = 0;
  SatCurSpeed = SATSPEED_10s;
  satslowdowncount = 0;
  SatCycleSeconds = 10;
  dashbnserialmsg += "startTracking()<br />";
  if(!SDcardIsOn)
  {
    SDcardOn();
  }
  if (!SDcardIsOn){ dashbnserialmsg += SDunav + "<br />"; return; }

  satTracking = true;
  AOSok = false;
  currentSatState = SATSTATE_STARTED;

  if (satFileLogging) { deleteFile(SD, SD_LOG_PATH); }
  if (TalkNturnLogging) { deleteFile(SD, TalkNturnAZ_log_path); }
  if (TalkNturnLogging) { deleteFile(SD, TalkNturnEL_log_path); }
  if (AZRotatorLogging) { deleteFile(SD, AZ_rotaror_log_path); }
  if (ELRotatorLogging) { deleteFile(SD, EL_rotaror_log_path); }
  
  neopixels(56); // Orange.

  Predict(1);

  sat.site(lat,lon,alt); //set site latitude[°], longitude[°] and altitude[m]
  
  sat.init(satname,tle_line1,tle_line2);     //initialize satellite parameters

  if (!UseManualTime)
  {
    // Get current time
    time_t currentTime = time(NULL);
    unixtime = currentTime;
  }else{
    stamp.setDateTime(myear, mmonth, mday, mhour, mminute, msecond);
    unixtime = stamp.getUnix();
  }

  // //Display TLE epoch time
  // double jdC = unixtime / 86400;
  // jdC = jdC + 2440587.5;
  // invjday(jdC , timezone, false, year, mon, day, hr, minute, sec);
  // Serial.println("Epoch day: " + String(day) + '/' + String(mon) + '/' + String(year));
  // Serial.println();

  dashbnserialmsg += "Satellite tracking started. Orange";
  EL_Dropping = false;
  satELup = true;
  SatGo = false;
  SatGoCounter = 0;

  SatLastMoveAZ = AZ_Old;
  SatLastMoveEL = EL_Old;
}

// Estimate the time required for both rotator axes to reach a target.
// The axes move simultaneously, so total travel time is the slower axis.
// AZ distance uses the shortest 360-degree path.
int SatPredictiveLookAheadSeconds(int targetAZ, int targetEL)
{
  // The elevation rotator cannot follow a negative satellite elevation.
  if (targetEL < 0) { targetEL = 0; }

  int azDistance = 0;
  int elDistance = 0;
  compassToDeg = targetAZ;
  compassFromDeg = SatLastMoveAZ;
  getShortestTurn();
  if ((compassUseDirection == SatTurnCW) && (compassDirectionDistance > 0))
  {
    azDistance = compassDirectionDistance;
  }

  if (satELup)
  {
    elDistance = targetEL - SatLastMoveEL;
    if (elDistance < 0) { elDistance = 0; } // The elevation rotator cannot follow a negative satellite elevation.
  }else{
    elDistance = SatLastMoveEL - targetEL;
    if (elDistance < 0) { elDistance = 0; } // The elevation rotator cannot follow a negative satellite elevation.  
  }

  int azMoveMS = azDistance * SAT_AZ_MS_PER_DEG;
  int elMoveMS = elDistance * SAT_EL_MS_PER_DEG;
  float longestMoveMS = (azMoveMS > elMoveMS) ? (float)azMoveMS : (float)elMoveMS;

  // Round up to a whole second because the SGP4 finder's time input is in seconds.
  return int(round(longestMoveMS / 1000));
}

void Sat_Tick()
{
  if (!SatGo) { return; }

  bool AZtomove = false;
  bool ELtomove = false;
  unsigned long togo = 0;
  int tominutes = 0;
  int toseconds = 0;
  int satReqAZ = 0;
  int satReqEL = 0;
  int lookAheadAZ = 0;
  int lookAheadEL = 0;
  unsigned long satLookAheadSeconds = 0;

  tickline = "";
  satFILEmsg = "";
  satHTMLmsg = "";
  sattimestr = "";

  if (!UseManualTime)
  {
    // Get current time
    time_t currentTime = time(NULL);
    unixtime = currentTime;
  }else{
    unixtime += SatCycleSeconds;
  }

  sat.findsat(unixtime);
  satReqAZ = round(sat.satAz);
  satReqEL = round(sat.satEl);
    
  invjday(sat.satJd , timezone,false, year, mon, day, hr, minute, sec);

  // tickline = "latitude = ";
  // tickline += String( sat.satLat);
  // tickline += " longitude = ";
  // tickline += String( sat.satLon);
  // tickline += "<br /> altitude = ";
  // tickline += String( sat.satAlt);

  // switch(sat.satVis)
  // {
  //   case -2:
  //       tickline = "Visible : Under horizon";
  //       break;
  //   case -1:
  //       tickline = "Visible : Daylight";
  //       break;
  //   default:
  //       tickline = "Visible : ";
  //       tickline += String(sat.satVis);   //0:eclipsed - 1000:visible
  //       tickline += " (0 = eclipsed - 1000 = visible)";
  //       break;
  // }

  sattimestr = String(day);
  sattimestr += '/';
  sattimestr += String(mon);
  sattimestr += '/';
  sattimestr += String(year);
  sattimestr += ' ';
  sattimestr += String(hr);
  sattimestr += ':';
  sattimestr += String(minute);
  sattimestr += ':';
  sattimestr += String(sec); 

  switch (currentSatState)
  {
    case SATSTATE_STARTED:
    if (unixtime > (AOSunixtime - almostLOS)) // 90 seconds before AOS
    {
      tickline = "90 seconds before AOS. Yellow";
      satFILEmsg += ('\n' + tickline + '\n');
      satHTMLmsg += ("<br />" + tickline + "<br />");
      tickline += "<br /><br />";
      currentSatState = SATSTATE_90stoAOS;
      AZ_Requested = SatLastMoveAZ = satReqAZ;
      AZcmdfirststep = true; 
      neopixels(54); // Yellow.
    }else{
      togo = AOSunixtime - unixtime;
      tominutes = togo / 60;
      toseconds = togo % 60;
      if (togo > 70000000)
      {
        tominutes = 0;
        toseconds = 0;
      }

      tickline = "Orange STARTED - waiting for AOS.<br /><br />";
      tickline += String(tominutes);
      tickline += " minutes and ";
      tickline += String(toseconds);
      tickline += " seconds to AOS";
      satFILEmsg += ('\n' + tickline + '\n');
      satHTMLmsg += ("<br />" + tickline + "<br />");
    }
    break;
    case SATSTATE_90stoAOS:
      togo = AOSunixtime - unixtime;
      tominutes = togo / 60;
      if (tominutes > 1)
      {
        tominutes = 0;
        toseconds = 0;
        currentSatState = SATSTATE_AOSreached;
        AOSok = true;
        neopixels(50); // Green
        tickline = "AOS - Acquisition of Signal. Green";
        satFILEmsg += ('\n' + tickline + '\n');
        satHTMLmsg += ("<br />" + tickline + "<br />");
        tickline += "<br /><br />";
      }else{
        toseconds = togo % 60;
        tickline = "90 seconds to AOS. Yellow<br /><br />";
        tickline += String(tominutes);
        tickline += " minutes and ";
        tickline += String(toseconds);
        tickline += " seconds to AOS\n\n (AOS < 90 seconds)";
        satFILEmsg += ('\n' + tickline + '\n');
        satHTMLmsg += ("<br />" + tickline + "<br />");
      }
    break;
    case SATSTATE_AOSreached:
      togo = TCAunixtime - unixtime;
      tominutes = togo / 60;
      toseconds = togo % 60;
      if (togo > 70000000)
      {
        tominutes = 0;
        toseconds = 0;
        currentSatState = SATSTATE_TCAreached;
        neopixels(51); // TCA (time of closest approach) Blue.
        EL_Dropping = true;
        satELup = false;
        tickline = "TCA (time of closest approach) Blue.";
        satFILEmsg += ('\n' + tickline + '\n');
        satHTMLmsg += ("<br />" + tickline + "<br />");
        tickline += "<br /><br />";
      }else{
        tickline = "AOS reached. Green<br /><br />";
        tickline += String(tominutes);
        tickline += " minutes and ";
        tickline += String(toseconds);
        tickline += " seconds to TCA";
        satFILEmsg += ('\n' + tickline + '\n');
        satHTMLmsg += ("<br />" + tickline + "<br />");
      }
    break;
    case SATSTATE_TCAreached:
      if (unixtime > (LOSunixtime - almostLOS)) // 90 seconds before LOS
      {
        tickline = "90 seconds before LOS. White";
        satFILEmsg += ('\n' + tickline + '\n');
        satHTMLmsg += ("<br />" + tickline + "<br />");

        currentSatState = SATSTATE_90stoLOS;
        LOStimeSet = false;
        // tickline = "LOStime unixtime is ";
        // tickline += LOSunixtime;
        neopixels(52); // White
      }else{
        togo = LOSunixtime - unixtime;
        tominutes = togo / 60;
        toseconds = togo % 60;
        if (togo > 70000000)
        {
          tominutes = 0;
          toseconds = 0;
        }
        tickline = "TCA reached Blue.<br /><br />";
        tickline += String(tominutes);
        tickline += " minutes and ";
        tickline += String(toseconds);
        tickline += " seconds to LOS";
        satFILEmsg += ('\n' + tickline + '\n');
        satHTMLmsg += ("<br />" + tickline + "<br />");
      }
    break;
    case SATSTATE_90stoLOS:
      togo = LOSunixtime - unixtime;
      tominutes = togo / 60;
      toseconds = togo % 60;
      if (tominutes > 1)
      {
        tominutes = 0;
        toseconds = 0;
        currentSatState = SATSTATE_LOS;
        neopixels(48); // Off
        satTracking = false;
        AOSok = false;
        LOStimeSet = false;
        TenSecondDopplerOK = false;
        tickline = "LOS - Loss of Signal";
        satFILEmsg += ('\n' + tickline + '\n');
        satHTMLmsg += ("<br />" + tickline + "<br />");
      }else{
        tickline = "90 seconds before LOS. White<br /><br />";
        tickline += String(tominutes);
        tickline += " minutes and ";
        tickline += String(toseconds);
        tickline += " seconds to LOS\n\n (LOS < 90 seconds)";
        satFILEmsg += ('\n' + tickline + '\n');
        satHTMLmsg += ("<br />" + tickline + "<br />");
      }
    break;
  }

  // Predict the satellite position for the time the rotators should need to reach it.
  // Keep the pass-state/AOS/LOS logic above based on the actual current time and position.
  // The prediction is only applied once active tracking has started.
  if (AOSok && satTracking)
  {
    // Use the most recent valid idle-position readings. The original sketch notes
    // that position measurements must not be taken while the motors are running.

    switch(SatCurSpeed)
    {
      case SATSPEED_10s:
      {
        satLookAheadSeconds += 6;
        break;
      }
      case SATSPEED_3s:
      {
        satLookAheadSeconds += 2;
        break;
      }
      case SATSPEED_2s:
      {
        satLookAheadSeconds += 1;
        break;
      }
      case SATSPEED_1s:
      {
        satLookAheadSeconds = SatPredictiveLookAheadSeconds(satReqAZ, satReqEL);
        break;
      }
    }

    sat.findsat(unixtime + satLookAheadSeconds);
    lookAheadAZ = round(sat.satAz);
    lookAheadEL = round(sat.satEl);

    // Restore current-time satellite data for timestamps, range and other reporting.
    sat.findsat(unixtime);
  }

  // Rotators action.
  if (AOSok && satTracking)
  {
    // Rotate Azimuth
    compassFromDeg = SatLastMoveAZ;
    compassToDeg = lookAheadAZ;
    getShortestTurn();

    if (compassDirectionDistance >= MoveEveryXdegrees)
    {
      AZtomove = true;
    }

    if (lookAheadEL >= 0)
    {
      if (satELup)
      {
        // Rotate Elevation UP
        if (lookAheadEL >= (EL_Old + MoveEveryYdegrees))
        {
          ELtomove = true;
        }
      }else{
        // Rotate Elevation DOWN
        if (lookAheadEL <= (EL_Old - MoveEveryYdegrees))
        {
          ELtomove = true;
        }
      }   
    }
  }

  satFILEmsg = sattimestr;
  satFILEmsg +=  "\n\n";
  satFILEmsg += tickline;
  satFILEmsg +=  "\n\n";

  satFILEmsg += "Pass to go = ";
  satFILEmsg += String(satLeftSec);
  satFILEmsg += " seconds, or";
  satFILEmsg += String(satLeftSec / 60);
  satFILEmsg += " minutes and ";
  satFILEmsg += String(satLeftSec % 60);
  satFILEmsg += " seconds";
  satFILEmsg += '\n';

  satFILEmsg += "azimuth = ";
  satFILEmsg += String(satReqAZ);
  satFILEmsg +=  '\n';
  satFILEmsg += " elevation = ";
  satFILEmsg += String(satReqEL);
  satFILEmsg +=  '\n';
  satFILEmsg += " distance = ";
  satFILEmsg += String(sat.satDist);
  satFILEmsg +=  '\n';
  satFILEmsg += " look-ahead = ";
  satFILEmsg += String(satLookAheadSeconds);
  satFILEmsg += " seconds\n";
  satFILEmsg +=  "look-ahead azimuth = ";
  satFILEmsg += String(lookAheadAZ);
  satFILEmsg +=  '\n';
  satFILEmsg += "look-ahead elevation = ";
  satFILEmsg += String(lookAheadEL);
  satFILEmsg +=  '\n';
  satFILEmsg +=  String(SatCycleSeconds);
  satFILEmsg += " second cycle time\n";

  satHTMLmsg = sattimestr;
  satHTMLmsg += "<br /><br />";
  satHTMLmsg += tickline;
  satHTMLmsg += "<br /><br />";

  satHTMLmsg += "Pass to go = ";
  satHTMLmsg += String(satLeftSec);
  satHTMLmsg += " seconds,<br /> or ";
  satHTMLmsg += String(satLeftSec / 60);
  satHTMLmsg += " minutes and <br />";
  satHTMLmsg += String(satLeftSec % 60);
  satHTMLmsg += " seconds";
  satHTMLmsg += "<br /><br />";

  satHTMLmsg += "azimuth = ";
  satHTMLmsg += String(satReqAZ);
  satHTMLmsg += "<br />";
  satHTMLmsg += " elevation = ";
  satHTMLmsg += String(satReqEL);
  satHTMLmsg += "<br />";
  satHTMLmsg += " distance = ";
  satHTMLmsg += String(sat.satDist);
  satHTMLmsg += "<br /><br />";
  satHTMLmsg += " look-ahead = ";
  satHTMLmsg += String(satLookAheadSeconds);
  satHTMLmsg += " seconds<br />";
  satHTMLmsg += "look-ahead azimuth = ";
  satHTMLmsg += String(lookAheadAZ);  
  satHTMLmsg += "<br />";
  satHTMLmsg += "look-ahead elevation = ";
  satHTMLmsg += String(lookAheadEL);
  satHTMLmsg += "<br /><br />";
  satHTMLmsg +=  String(SatCycleSeconds);
  satHTMLmsg += " second cycle time<br />";

  sattimestr = "";
  tickline = "";

  if (!satTracking)
  {
    Reset_AZ_Degrees();
    Reset_EL_Degrees();
    dashbnserialmsg += "Satellite tracking ended.<br />";
    satHTMLmsg += "<br />Satellite tracking ended.<br />";
    satFILEmsg += "\nSatellite tracking ended.\n";
    EL_Requested = 0;
    AZtomove = false;
    ELtomove = false;
  }

  SatSpeedAdjust(lookAheadAZ);

  if (satFileLogging)
  {
    logToFlash(SAT_FLASH_LOG_PATH, satFILEmsg);
    satFILEmsg = "";
  }
  if (satSerialLogging) { Serial.print(satFILEmsg); }

  satHTMLmsgOK = true;

  if (AZtomove)
  {
    AZ_Requested = SatLastMoveAZ = lookAheadAZ;
    AZcmdfirststep = true;
  }

  if (ELtomove)
  {
    if (satReqEL < 0)
    {
      EL_Requested = 0;
    }else{
      EL_Requested = SatLastMoveEL = lookAheadEL;
      if (EL_Requested > SatLastPeakEL) { SatLastPeakEL = EL_Requested; }
    }
  }

  if (!satTracking)
  {
    if (satFileLogging) { dumpFlashToSD(SAT_FLASH_LOG_PATH, SD_LOG_PATH); }
    if (TalkNturnLogging) { dumpFlashToSD(TalkNturnAZ_log_path, TalkNturnAZ_log_path); }
    if (TalkNturnLogging) { dumpFlashToSD(TalkNturnEL_log_path, TalkNturnEL_log_path); }
    if (AZRotatorLogging) { dumpFlashToSD(AZ_rotaror_log_path, AZ_rotaror_log_path); }
    if (ELRotatorLogging) { dumpFlashToSD(EL_rotaror_log_path, EL_rotaror_log_path); }    
  }

  SatGo = false;
  if (AOSok) { satLeftSec -= SatCycleSeconds; } else { satLeftSec = 0; }
}

void SatSpeedAdjust(int SatAZ)
{
  if (!satTracking) { return; }
  if (SatLastMoveAZ == SatAZ) { return; }
  if (satDisableAutoCycleTime) { return; }

  int distance = getShortestDistance(SatLastMoveAZ, SatAZ);

  // Speed UP
  if (currentSatState == SATSTATE_AOSreached)
  {
    if ((SatCurSpeed != SATSPEED_1s) && (distance > 3))
    {
      SatCurSpeed++;
      Satspeedchange = true;
    }
  }else if (currentSatState == SATSTATE_TCAreached)
  {
    if ((SatCurSpeed != SATSPEED_10s) && (distance < 4))
    {
      SatCurSpeed--;
      Satspeedchange = true;
    }
  }

  if (Satspeedchange)
  {
    switch(SatCurSpeed)
    {
      case SATSPEED_10s:
      {
        SatCycleSeconds = 10;
        break;
      }
      case SATSPEED_3s:
      {
        SatCycleSeconds = 3;
        break;
      }
      case SATSPEED_2s:
      {
        SatCycleSeconds = 2;
        break;
      }
      case SATSPEED_1s:
      {
        SatCycleSeconds = 1;
        break;
      }
    }
    Satspeedchange = false;
    satFILEmsg += ("SatSpeed change to " + String(SatCycleSeconds) + " Seconds.");
    satHTMLmsg += ("SatSpeed change to " + String(SatCycleSeconds) + " Seconds.<br />");
  }
}

void radioUpdateDoppler()
{
  if (!radioCatOn){ return; }
  if (radioSidePTT) { return; }

  uint8_t bcdA[5] = {0, 0, 0, 0, 0}; // Array to hold BCD representation

    BCDarrayFromInt(TXdopplerFreq, bcdA, 5);

  radioTXfreq[9] = bcdA[0]; // Ten MHz
  radioTXfreq[8] = bcdA[1]; // One MHz
  radioTXfreq[7] = bcdA[2]; // Hundred kHz
  radioTXfreq[6] = bcdA[3]; // Ten kHz
  radioTXfreq[5] = bcdA[4]; // One kHz    

  BCDarrayFromInt(RXdopplerFreq, bcdA, 5);

  radioRXfreq[9] = bcdA[0]; // Ten MHz
  radioRXfreq[8] = bcdA[1]; // One MHz
  radioRXfreq[7] = bcdA[2]; // Hundred kHz
  radioRXfreq[6] = bcdA[3]; // Ten kHz
  radioRXfreq[5] = bcdA[4]; // One kHz

  if (!radioSidePTT)
  {
    if (radioSimplex)
    {
      civ.write(radioAVFO, 7);
      delay(30);
      civ.write(radioRXfreq, 11);
      delay(30);
    }else{
      civ.write(radioMainVFO, 7);
      delay(30);
      civ.write(radioRXfreq, 11);
      delay(30);
      civ.write(radioSubVFO, 7);
      delay(30);
      civ.write(radioTXfreq, 11);
      delay(10);
    }
  }
}

void radioMemoDoppler()
{
  if (radioSidePTT) { return; }

  int DopplerPerMinute = dopplerDelta * 6; // Doppler change per minute
  int DopplerUhfPerMinute = dopplerUhfDelta * 6; // Doppler change per minute for UHF
  int radioChannel = 2; // Start from channel 2
  uint8_t bcdA[1] = {0};

  civ.write(radioVFOmode, 6);
  delay(30);

  setStartDopplerFrequencies();
  delay(50);
  civ.write(radioMemoryChannel, 8);
  delay(30);
  civ.write(radioMemoryWrite, 6);
  delay(30);

  for (int i = 1; i < (dopplerMinutes + 1); i++)
  {
    switch (BandMode)
    {
      case BandModeSelectUHFforRX:
      {
        TXdopplerFreq += DopplerPerMinute;
        RXdopplerFreq -= DopplerUhfPerMinute;
        break;
      }
      case BandModeSelectVHFforRX:
      {
        TXdopplerFreq += DopplerUhfPerMinute;
        RXdopplerFreq -= DopplerPerMinute;
        break;
      }
      case BandModeVHFonly:
      {
        TXdopplerFreq += DopplerPerMinute;
        RXdopplerFreq -= DopplerPerMinute;
        break;
      }
      case BandModeUHFonly:
      {
        TXdopplerFreq += DopplerUhfPerMinute;
        RXdopplerFreq -= DopplerUhfPerMinute;
        break;
      }
    }

    radioUpdateDoppler();
    delay(50);

    BCDarrayFromInt(radioChannel, bcdA, 1);
    radioMemoryChannel[6] = bcdA[0]; // Channel number

    civ.write(radioMemoryChannel, 8);
    delay(30);
    civ.write(radioMemoryWrite, 6);
    delay(30);

    radioChannel++;
  }

  setStopDopplerFrequencies();
  delay(50);

  BCDarrayFromInt(radioChannel, bcdA, 1);
  radioMemoryChannel[6] = bcdA[0]; // Channel number

  civ.write(radioMemoryChannel, 8);
  delay(30);
  civ.write(radioMemoryWrite, 6);
  delay(30);  

  radioMemoryChannel[6] = 1;

  civ.write(radioMemorymode, 6);
  delay(30);
}

void radioSetSimplex()
{
  if (radioSimplex)
  {
    radioSimplex = false;
    dashbnserialmsg += "Radio Mode SIMPLEX OFF";
    return;
  }
  civ.write(radioSAToff, 8);
  delay(40);
  civ.write(radioDualWatchOff, 8);
  delay(40);
  civ.write(radioSubMuteUSBoff, 9);
  delay(40);
  civ.write(radioSubMuteLanOff, 9);
  delay(40);
  civ.write(radioSubSpeakerOff, 9);
  delay(40);
  civ.write(radioDispOff, 10);
  delay(40);
  civ.write(radioSimplexOn, 7);
  delay(40);
  civ.write(radioAttenuatorOff, 7);
  delay(40);

  dashbnserialmsg += "Radio Mode set to SIMPLEX";
  radioSimplex = true;
}

void Predict(int many){

  filemsg = "";
  satHTMLmsg = "";
  satFILEmsg = "";
  String filepath = "/predict.txt";
  if (many == 1) { filepath = "/predictOne.txt"; }

  deleteFile(SD, filepath.c_str());

  sat.site(lat,lon,alt); //set site latitude[°], longitude[°] and altitude[m]
  
  sat.init(satname,tle_line1,tle_line2);     //initialize satellite parameters

  if (!UseManualTime)
  {
    // Get current time
    time_t currentTime = time(NULL);
    unixtime = currentTime;
  }else{
    stamp.setDateTime(myear, mmonth, mday, mhour, mminute, msecond);
    unixtime = stamp.getUnix();
  }

  //Display TLE epoch time
  // double jdC = sat.satrec.jdsatepoch;
  // double jdC = unixtime / 86400;
  // jdC = jdC + 2440587.5;
  double jdC = getJulianFromUnix(unixtime);
  invjday(jdC , timezone, false, year, mon, day, hr, minute, sec);
  sattimestr = "Epoch: " + String(day) + '/' + String(mon) + '/' + String(year) + ' ' + String(hr) + ':' + String(minute) + ':' + String(sec);
    satFILEmsg = (sattimestr + "\n\n");
    satHTMLmsg = (sattimestr + "<br /><br />");
  passinfo overpass;                       //structure to store overpass info
  sat.initpredpoint( unixtime , 0.0 );     //finds the startpoint
  
  bool error;
  unsigned long start = millis();
  for (int i = 0; i<many ; i++){
    error = sat.nextpass(&overpass,20);     //search for the next overpass, if there are more than 20 maximums below the horizon it returns false
    delay(0);
      
    if ( error == 1){ //no error, prints overpass information
      
      invjday(overpass.jdstart ,timezone ,false , AOSyear, AOSmon, AOSday, AOShr, AOSminute, AOSsec);

      filemsg = "Month = " + String(AOSmon) + "  Day = " + String(AOSday);
      satFILEmsg += (filemsg + '\n');
      satHTMLmsg += (filemsg + "<br />");

      filemsg = "Start: az=" + String(overpass.azstart) + "° " + String(AOShr) + ':' + String(AOSminute) + ':' + String(AOSsec);
      satFILEmsg += (filemsg + '\n');
      satHTMLmsg += (filemsg + "<br />");

      azAOS = overpass.azstart;

      invjday(overpass.jdmax ,timezone ,false , TCAyear, TCAmon, TCAday, TCAhr, TCAminute, TCAsec);
      filemsg = "Max: elev=" + String(overpass.maxelevation) + "° " + String(TCAhr) + ':' + String(TCAminute) + ':' + String(TCAsec);
      satFILEmsg += (filemsg + '\n');
      satHTMLmsg += (filemsg + "<br />");

      invjday(overpass.jdstop ,timezone ,false , LOSyear, LOSmon, LOSday, LOShr, LOSminute, LOSsec);
      filemsg = "Stop: az=" + String(overpass.azstop) + "° " + String(LOShr) + ':' + String(LOSminute) + ':' + String(LOSsec);
      satFILEmsg += (filemsg + '\n');
      satHTMLmsg += (filemsg + "<br />");

      switch(overpass.transit){
          case none:
              break;
          case enter:
              invjday(overpass.jdtransit ,timezone ,true , year, mon, day, hr, minute, sec);
              filemsg = "  Enter earth shadow: " + String(hr) + ':' + String(minute) + ':' + String(sec);
              break;
          case leave:
              invjday(overpass.jdtransit ,timezone ,true , year, mon, day, hr, minute, sec);
              filemsg = "  Leave earth shadow: " + String(hr) + ':' + String(minute) + ':' + String(sec);
              break;
      }
      switch(overpass.sight){
          case lighted:
              filemsg += "  Visible";
              break;
          case eclipsed:
              filemsg += "  Not visible";
              break;
          case daylight:
              filemsg += "  Daylight";
              break;
      }
      satFILEmsg += (filemsg + "\n\n");
      satHTMLmsg += (filemsg + "<br /><br />");
    }else{
      filemsg = "Prediction error";
      satFILEmsg += (filemsg + '\n');
      satHTMLmsg += (filemsg + "<br />");
    }
    delay(0);
  }

  // Durations
  if (many == 1)
  {
    UnixTime stamp2(timezone); // remember unixtime is gmt.
    stamp2.setDateTime(AOSyear, AOSmon, AOSday, AOShr, AOSminute, AOSsec);
    AOSunixtime = stamp2.getUnix();
    stamp2.setDateTime(TCAyear, TCAmon, TCAday, TCAhr, TCAminute, TCAsec);
    TCAunixtime = stamp2.getUnix();
    stamp2.setDateTime(LOSyear, LOSmon, LOSday, LOShr, LOSminute, LOSsec);
    LOSunixtime = stamp2.getUnix();

    LOStimeSet = true;

    AOStoLOSsec = LOSunixtime - AOSunixtime;
    satLeftSec = AOStoLOSsec;

    filemsg = "AOS to LOS = ";
    filemsg += String(AOStoLOSsec);
    filemsg += " seconds, or";
    satFILEmsg += (filemsg + '\n');
    satHTMLmsg += (filemsg + "<br />");

    filemsg = String(AOStoLOSsec / 60);
    filemsg += " minutes and ";
    filemsg += String(AOStoLOSsec % 60);
    filemsg += " seconds";
    satFILEmsg += (filemsg + '\n');
    satHTMLmsg += (filemsg + "<br />");

    dopplerMinutes = AOStoLOSsec / 60;
    dopplerDelta = dopplerRange / (AOStoLOSsec / 10);
    dopplerUhfDelta = dopplerUhfRange / (AOStoLOSsec / 10);

    setStartDopplerFrequencies();
    TenSecondDopplerOK = true;

    filemsg = "10 sec doppler delta = ";
    filemsg += String(dopplerDelta);
    filemsg += " Hertz.";
    satFILEmsg += (filemsg + '\n');
    satHTMLmsg += (filemsg + "<br />");
    filemsg = "10 sec UHF doppler delta = ";
    filemsg += String(dopplerUhfDelta);
    filemsg += " Hertz.";
    satFILEmsg += (filemsg + '\n');
    satHTMLmsg += (filemsg + "<br />");

    sat.findsat(AOSunixtime + 60);

    compassFromDeg = round(overpass.azstart);
    compassToDeg = round(sat.satAz);
    // compassToDeg = round(overpass.azstop);
    getShortestTurn();
    SatTurnCW = compassUseDirection;
    EL_Dropping = false;
    satELup = true;

    filemsg = "Sat turn direction = ";
    filemsg += SatTurnCW ? "CW" : "CCW";
    satFILEmsg += (filemsg + '\n');
    satHTMLmsg += (filemsg + "<br />");

    filemsg = "Pre Start degrees ";
    if (SatTurnCW)
    {
      filemsg += " < ";
    }else{
      filemsg += " > ";
    }
    filemsg += String(azAOS);
    satFILEmsg += (filemsg + '\n');
    satHTMLmsg += (filemsg + "<br />");
  }

  unsigned long einde = millis();
  filemsg = "Time: " + String(einde-start) + " milliseconds";
  satFILEmsg += (filemsg + '\n');
  satHTMLmsg += (filemsg + "<br />");

  filemsg = "Prediction complete.";
  satFILEmsg += (filemsg + '\n');
  satHTMLmsg += (filemsg + "<br />");

  appendFile(SD, filepath.c_str(), satFILEmsg.c_str(), true);
  satFILEmsg = "";
  dashbnserialmsg += satHTMLmsg;
  satHTMLmsgOK = true;
}

void getShortestTurn() {
  // 1. Normalize the difference to -180 to +180 range
  int diff = compassToDeg - compassFromDeg;
  diff = fmod(diff + 180.0, 360.0) - 180.0;

  if (diff > 180) {
    diff -= 360;
  } else if (diff < -180) {
    diff += 360;
  }

  if (diff > 0) {
    compassUseDirection = true; // Turn right (clockwise)
  } else {
    compassUseDirection = false;
  }
  
  compassDirectionDistance = abs(diff);
}

int getShortestDistance(int fromDeg, int toDeg)
{
  // 1. Normalize the difference to -180 to +180 range
  int diff = toDeg - fromDeg;
  diff = fmod(diff + 180.0, 360.0) - 180.0;

  if (diff > 180) {
    diff -= 360;
  } else if (diff < -180) {
    diff += 360;
  }

  return abs(diff);
}

void neopixels(byte color)
{
  if (color == leds.getPixelColor(0)) { return; } // dont redo same color.

  if (color != 49) //  Not Red for PTT. Remeber last color for after PTT.
  {
    lastMatrixColor = color;
  }

  uint32_t c;
  switch (color)
  {
    case 48: // LM0 off
      c = leds.Color(0, 0, 0);
    break;
    case 49: // LM1 red
      c = leds.Color(255, 0, 0);
    break;
    case 50: // LM2 green
      c = leds.Color(0, 255, 0);
    break;
    case 51: // LM3 blue
      c = leds.Color(0, 0, 255);
    break;
    case 52: // LM4 white
      c = leds.Color(255, 255, 255);
    break;
    case 53: // LM5 purple
      c = leds.Color(128, 0, 128);
    break;
    case 54: // LM6 yellow
      c = leds.Color(255, 255, 0);
    break;
    case 55: // LM7 cyan
      c = leds.Color(0, 255, 255);
    break;
    case 56: // LM8 orange
      c = leds.Color(255, 165, 0); 
    break;
    case 57: // LM9 pink
      c = leds.Color(255, 192, 203);
    break;
    default:
      c = leds.Color(0, 0, 0); // off
    break;
  }

  for (int i = 0; i < leds.numPixels(); i++)
  {                                  // For each pixel in strip...
      leds.setPixelColor(i, c); //  Set pixel's color (in RAM)
  }

  leds.show();                  //  Update strip to match
  if (satFileLogging)
  {
    satFILEmsg += "Neopixels color set to " + String(color) + ".\n";
  }
}

void cmdSerialOut(String msg)
{
  String serialmsg = msg;
  serialmsg.replace("<br />", "~");
  serialmsg.replace("<br>", "~");
  serialmsg.replace("<b>", "");
  serialmsg.replace("</b>", "");
  serialmsg.replace('~', '\n');
  Serial.println(serialmsg);
  // Wait until the transmission is fully complete
  Serial.flush();
  serialmsg = "";
}

void satelliteCmds()
{
  switch (cmdbuffer[2])
  {
    case 49: // AS1 display longitude, latitude, altitude
    {
      dashbnserialmsg += "AS1 cmd<br />";
      dashbnserialmsg += "Longitude = " + String(lon, 10) + ", Latitude = " + String(lat, 10) + ", Altitude = " + String(alt, 10);
    }
    break;
    case 50: // AS2 set longitude e.g. "AS2 28.13511"
    {
      dashbnserialmsg += "AS2 cmd<br />";
      lon = GetSerialStringDouble(4, cmdbuffer_index - 4);
      preferences.begin("mat", false); // Open Preferences with mat namespace. RW mode (permanent storage).
      preferences.putDouble("longitude", lon);
      preferences.end(); // Close the Preferences
      dashbnserialmsg += "Longitude set to " + String(lon, 10);
    }
    break;
    case 51: // AS3 set latitude
    {
      dashbnserialmsg += "AS3 cmd<br />";
      lat = GetSerialStringDouble(4, cmdbuffer_index - 4);
      preferences.begin("mat", false); // Open Preferences with mat namespace. RW mode (permanent storage).
      preferences.putDouble("latitude", lat);
      preferences.end(); // Close the Preferences
      dashbnserialmsg += "Latitude set to " + String(lat, 10);
    }
    break;
    case 52: // AS4 set altitude
    {
      dashbnserialmsg += "AS4 cmd<br />";
      alt = GetSerialStringDouble(4, cmdbuffer_index - 4);
      preferences.begin("mat", false); // Open Preferences with mat namespace. RW mode (permanent storage).
      preferences.putDouble("altitude", alt);
      preferences.end(); // Close the Preferences
      dashbnserialmsg += "Altitude set to " + String(alt, 10) + " meters.";
    }
    break;
    case 53: // AS5 display catalogue number
    {
      dashbnserialmsg += "AS5 cmd<br />";
      dashbnserialmsg += "catalogueNumber  = " + String(catalogueNumber);
    }
    break;
    case 54: // AS6 set ISS default catalogue number
    {
      dashbnserialmsg += "AS6 cmd<br />";
      catalogueNumber  = 25544; // default to ISS
      dashbnserialmsg += "catalogueNumber set to ISS (25544).";
    }
    break;
    case 55: // AS7 set catalogue number from command
    {
      dashbnserialmsg += "AS7 cmd<br />";
      catalogueNumber  = GetCmdStringInt(4, cmdbuffer_index - 4);
      dashbnserialmsg += "catalogueNumber set to " +  String(catalogueNumber) + ".";
    }
    break;
    case 56: // AS8 Retrieve TLE elements from wifi
    {
      dashbnserialmsg += "AS8 cmd<br />";
      dashbnserialmsg += "Retrieving TLE elements for catalogue number " + String(catalogueNumber) + " from Celestrak.";
      retrieveTLEelementsForSatellite(catalogueNumber);
    }
    break;
    case 57: // AS9 show TLE elements age
    {
      dashbnserialmsg += "AS9 cmd<br />";
      dashbnserialmsg += ("TLE Age H:M = " + determineTLEage(tle_line1));
    }
    break;
    case 97: // ASa input future time and track.
      {
        dashbnserialmsg += "ASa cmd<br />";
        myear = GetCmdStringInt(4, 4);
        mmonth = GetCmdStringInt(9, 2);
        mday = GetCmdStringInt(12, 2);
        mhour = GetCmdStringInt(15, 2);
        mminute = GetCmdStringInt(18, 2);
        msecond = GetCmdStringInt(21, 2);

        dashbnserialmsg += "Future Time set to: " + String(myear) + "-" + String(mmonth) + "-" + String(mday) + " " +
                     String(mhour) + ":" + String(mminute) + ":" + String(msecond) + "<br />";

        UseManualTime = true;
        startTracking();
      }
    break;
    case 98: // ASb set to current time and track
    {
      dashbnserialmsg += "ASb cmd<br />";
      UseManualTime = false;
      dashbnserialmsg += "Set to current time.";
      startTracking();
    }
    break;
    case 99: // ASc stop tracking
    {
      dashbnserialmsg += "ASc cmd<br />";
      neopixels(48); // Off
      satTracking = false;
      AOSok = false;
      LOStimeSet = false;
      TenSecondDopplerOK = false;

      if (satFileLogging) { dumpFlashToSD(SAT_FLASH_LOG_PATH, SD_LOG_PATH); }
      if (TalkNturnLogging) { dumpFlashToSD(TalkNturnAZ_log_path, TalkNturnAZ_log_path); }
      if (TalkNturnLogging) { dumpFlashToSD(TalkNturnEL_log_path, TalkNturnEL_log_path); }
      if (AZRotatorLogging) { dumpFlashToSD(AZ_rotaror_log_path, AZ_rotaror_log_path); }
      if (ELRotatorLogging) { dumpFlashToSD(EL_rotaror_log_path, EL_rotaror_log_path); }

      dashbnserialmsg += "Tracking stopped.";
    }
    break;
    case 101: // ASe set LOS time before tracking.
      {
        dashbnserialmsg += "ASe cmd<br />";
        struct tm LOStimeinfo;

        LOStimeinfo.tm_year = GetCmdStringInt(4, 4) - 1900;
        LOStimeinfo.tm_mon = GetCmdStringInt(9, 2) - 1;
        LOStimeinfo.tm_wday = GetCmdStringInt(12, 2);
        LOStimeinfo.tm_hour = GetCmdStringInt(15, 2);
        LOStimeinfo.tm_min = GetCmdStringInt(18, 2);
        LOStimeinfo.tm_sec = GetCmdStringInt(21, 2);

        LOSunixtime = mktime(&LOStimeinfo);
        LOStimeSet = true;

        dashbnserialmsg += String(LOStimeinfo.tm_wday) + "/" + String(LOStimeinfo.tm_mon) + "/" + String(LOStimeinfo.tm_year + 1900) + " ";
        dashbnserialmsg += String(LOStimeinfo.tm_hour) + ":" + String(LOStimeinfo.tm_min) + ":" + String(LOStimeinfo.tm_min) + ":" + String(LOStimeinfo.tm_sec);
      }
    break;
    case 102: // ASf Verify LOSunixtime
    {
      dashbnserialmsg += "ASf cmd<br />";
      if (LOStimeSet)
      {
        UnixTime stamp1(0);
        stamp1.getDateTime(LOSunixtime);

        dashbnserialmsg += "LOS time set to: " + String(stamp1.year) + "-" + String(stamp1.month) + "-" + String(stamp1.day) + " " +
                     String(stamp1.hour) + ":" + String(stamp1.minute) + ":" + String(stamp1.second);
      }else{
        dashbnserialmsg += "LOS time not set.";
      }
    }
    break;
    case 103: // ASg Set doppler frequency total rage from AOS to LOS
    {
      dashbnserialmsg += "ASg cmd<br />";
      if (cmdbuffer_index < 4)
      { // e.g. ASg
        dashbnserialmsg += "AOS to LOS DopplerTotalRange is " + String(dopplerRange) + " Hertz.";
      }else{ //e.g. ASg 6000
        dopplerRange = GetCmdStringInt(4, cmdbuffer_index - 4);
        setStartDopplerFrequencies();

        preferences.begin("mat", false); // Open Preferences with mat namespace. RW mode (permanent storage).
        preferences.putInt("dopplerRange", dopplerRange);
        preferences.end();

        dashbnserialmsg += "AOS to LOS DopplerTotalRange set to " + String(dopplerRange) + " Hertz.";
      }
    }
    break;
    case 104: // ASh Show rounded doppler changes for each 10 seconds.
    {
      dashbnserialmsg += "ASh cmd<br />";
      dashbnserialmsg += "Doppler changes for each 10 seconds are " + String(dopplerDelta) + " Hertz.<br />";
      dashbnserialmsg += "For UHF, Doppler changes per 10 seconds are " + String(dopplerUhfDelta) + " Hertz.";
    }
    break;
    case 105: // ASi Set doppler frequency UHF total rage from AOS to LOS
    {
      dashbnserialmsg += "ASi cmd<br />";
      if (cmdbuffer_index < 4)
      { // e.g. ASi
        dashbnserialmsg += "AOS to LOS DopplerTotalUHFRange is " + String(dopplerUhfRange) + " Hertz.";
      }else{ //e.g. ASi 6000
        dopplerUhfRange = GetCmdStringInt(4, cmdbuffer_index - 4);
        setStartDopplerFrequencies();

        preferences.begin("mat", false); // Open Preferences with mat namespace. RW mode (permanent storage).
        preferences.putInt("dopplerUhfRange", dopplerUhfRange);
        preferences.end();

        dashbnserialmsg += "AOS to LOS DopplerTotalUHFRange set to " + String(dopplerUhfRange) + " Hertz.";
      }
    }
    break;
    case 115: // ASs start tracking at HH:MM:SS
    {
      dashbnserialmsg += "ASs cmd<br />";
      waithour = GetCmdStringInt(4, 2);
      waitminute = GetCmdStringInt(7, 2);
      waitsecond = GetCmdStringInt(10, 2);

      dashbnserialmsg += "Will start tracking at " + String(waithour) + ":" + String(waitminute) + ":" + String(waitsecond);
      satwaiting = true;
    }
    break;
  }
}

void setStartDopplerFrequencies()
{
  switch (BandMode)
  {
    case BandModeSelectUHFforRX:
    {
      TXdopplerFreq = TXcenterFreq - (dopplerRange / 2);
      RXdopplerFreq = RXcenterFreq + (dopplerUhfRange / 2);
      break;
    }
    case BandModeSelectVHFforRX:
    {
      TXdopplerFreq = TXcenterFreq - (dopplerUhfRange / 2);
      RXdopplerFreq = RXcenterFreq + (dopplerRange / 2);
      break;
    }
    case BandModeVHFonly:
    {
      TXdopplerFreq = TXcenterFreq - (dopplerRange / 2);
      RXdopplerFreq = RXcenterFreq + (dopplerRange / 2);
      break;
    }
    case BandModeUHFonly:
    {
      TXdopplerFreq = TXcenterFreq - (dopplerUhfRange / 2);
      RXdopplerFreq = RXcenterFreq + (dopplerUhfRange / 2);
      break;
    }
  }

  radioUpdateDoppler();
}

void setStopDopplerFrequencies()
{
  switch (BandMode)
  {
    case BandModeSelectUHFforRX:
    {
      TXdopplerFreq = TXcenterFreq + (dopplerRange / 2);
      RXdopplerFreq = RXcenterFreq - (dopplerUhfRange / 2);
      break;
    }
    case BandModeSelectVHFforRX:
    {
      TXdopplerFreq = TXcenterFreq + (dopplerUhfRange / 2);
      RXdopplerFreq = RXcenterFreq - (dopplerRange / 2);
      break;
    }
    case BandModeVHFonly:
    {
      TXdopplerFreq = TXcenterFreq + (dopplerRange / 2);
      RXdopplerFreq = RXcenterFreq - (dopplerRange / 2);
      break;
    }
    case BandModeUHFonly:
    {
      TXdopplerFreq = TXcenterFreq + (dopplerUhfRange / 2);
      RXdopplerFreq = RXcenterFreq - (dopplerUhfRange / 2);
      break;
    }
  }

  radioUpdateDoppler();
}

void check_civ()
{
  if (!radioCatOn || !readRadioOnce){ return;}

  if (civ.available())
  {
    civinByte = civ.read();

    if (civinByte == 0XFD)  // civ response end line.
    {
      civ_buffer[civ_buffer_index] = civinByte;
          
      civ_buffer_index++;  // Increment the Serial Buffer pointer

      if ((civ_buffer[2] == 0XA2) || (civ_buffer[4] == 0XFB)) // Dont record sent data
      {
        civ_buffer[0] = 0;
        civinByte = 0;
        civ_buffer_index = 0;
        return;
      }

      lastcivresp = "";
      for (int i = 0; i < civ_buffer_index; i++)
      {
        Serial.print(civ_buffer[i], HEX);
        lastcivresp += byteToHexString(civ_buffer[i]);
        lastcivresp += ", ";
      }
      dashbnserialmsg += lastcivresp;
      civrespOK = true;
      readRadioOnce = false;
      
    }else{
      civ_buffer[civ_buffer_index] = civinByte;
          
      civ_buffer_index++;  // Increment the Serial Buffer pointer      
    }
  }

  if(civrespOK)
  {
    civrespOK = false;
    polarmsgOK = false;
    polarmsg = "Radio CIV reponse: ";
    polarmsg += lastcivresp;
    polarmsg += "<br />";
    if (civ_buffer_index > 7)
    {
      if (SmeterLevelReq)
      {
        SmeterLevelReq = false;
        int sMeterVal1 = ((civ_buffer[6]/16*10) + (civ_buffer[6]%16)); // First byte: convert from BCD to decimal.
        int sMeterVal2 = ((civ_buffer[7]/16*10) + (civ_buffer[7]%16));
        sMeterVal = (sMeterVal1 * 100) + sMeterVal2;
        polarmsg += "S Meter Level is ";
        polarmsg += String(sMeterVal);
        if (sMeterVal > 120)
        {   
          polarmsg += " or S9+ (120 = S9)";
        }else{
          polarmsg += " or S";
          polarmsg += String(map(sMeterVal, 0, 120, 0, 9));
          polarmsg += "<br />";
        }
      }

      if (radioAutoSmeter_wait)
      {
        radioAutoSmeter_wait = false;
        int sMeterVal1 = ((civ_buffer[6]/16*10) + (civ_buffer[6]%16)); // First byte: convert from BCD to decimal.
        int sMeterVal2 = ((civ_buffer[7]/16*10) + (civ_buffer[7]%16));
        if (firstAutoSmeterTest)
        {
          firstAutoSmeterTest = false;
          sMeterVal = (sMeterVal1 * 100) + sMeterVal2;
          if (autoPolarizeTest || (sMeterVal > 10))
          {
            // If first test is 0 db do not test again. Keep polarization as is.
            delay(5);
            radioAutoSmeter_wait = true;
            readRadioOnce = true;
            SmeterLevelReq = true;
            civ.write(radioSmeter, 7);
          }
        }else{
          int newSMeterVal = (sMeterVal1 * 100) + (sMeterVal2);
          bool isPolarizedRight = digitalRead(polarize_opin) == pin_off_state;
          if ((newSMeterVal > 10) && (newSMeterVal > sMeterVal))
          { // Switch polarity now.
            if (isPolarizedRight)
            {
              digitalWrite(polarize_opin, pin_active_state); // Switch to left
              polarmsg += "Antenna polarization switched to LEFT LHCP.<br />";
              Serial.print(String(sMeterVal));
              Serial.print(" to ");
              Serial.print(String(newSMeterVal));
              Serial.println(" switched to LEFT LHCP.");
            }else{
              digitalWrite(polarize_opin, pin_off_state); // Switch to right
              polarmsg += "Antenna polarization switched to RIGHT RHCP.<br />";
              Serial.print(String(sMeterVal));
              Serial.print(" to ");
              Serial.print(String(newSMeterVal));
              Serial.println(" switched to RIGHT RHCP.");
            }
            sMeterVal = newSMeterVal;
          }else{
            polarmsg = "Antenna polarization is ";
            polarmsg += isPolarizedRight ? "RIGHT RHCP.<br />" : "LEFT LHCP.<br />";
          }
        }
      }

      civ_buffer[0] = 0;
      civinByte = 0;
      civ_buffer_index = 0;

      polarmsgOK = true;
    }
  }
}

// Manage satelite tracking messages as well as intermediate auto polartization messages.
void reporting()
{
  //String wifiStatusmsg = "WiFi Signal Strength: " + String(WiFi.RSSI()) + " dBm<br />";

  String combinedmsg = "";
  bool sendcmd = false;

  if (dashbnserialmsg.length() > 0)
  {
    combinedmsg = dashbnserialmsg;
    sendcmd = true;
    dashbnserialmsg = "";
  }

  if (polarmsgOK)
  {
    polarmsgOK = false;
    combinedmsg += polarmsg;
    combinedmsg += "<br />";
    sendcmd = true;
    polarmsg = "";
  }

  if (sendcmd)
  {
    sendcmd = false;
    cmdSerialOut(combinedmsg);
    cmdDashboardOut(combinedmsg);
    combinedmsg = "";
  }

  if (satHTMLmsgOK)
  {
    satHTMLmsgOK = false;
    satHTMLmsg.replace("\n", "<br />");
    monitor_events.send(satHTMLmsg.c_str(), "message", millis());
    satHTMLmsg = "";
  }

  LogQueueProcess();
}

// Function to convert a single byte to a two-character hex string
String byteToHexString(byte byte) {
  char hexChar[3]; // Two characters for hex, plus null terminator
  sprintf(hexChar, "%02X", byte); // %02X formats as two uppercase hex digits, padded with leading zero if needed
  return String(hexChar);
}

void check_serial() // Function to check for data on the Serial port
{
  if (Serial.available() < 1)
  {
    return;
  }

  inByte = Serial.read();  // Get the Serial Data

  // You may need to comment out the following line if your PC software
  // will not communicate properly with the controller
  // SatPC32 wants the command echoed, Ham Radio Deluxe does not
  //Serial.print(char(inByte));  // Echo back to the PC

  if (inByte == 10)  // ignore Line Feeds
  {
    return;
  }
  
  if (inByte != 13)  // Add to buffer if not CR 0XDH
  {
    serial_buffer[serial_buffer_index] = inByte;
        
    serial_buffer_index++;  // Increment the Serial Buffer pointer
    return;
  }

  for (int i = 0; i < serial_buffer_index; i++)
  {
    cmdbuffer[i] = serial_buffer[i];  // Copy Serial Buffer to Command Buffer
  }

  cmdbuffer_index = serial_buffer_index; // copy command buffer to serial buffer for processing
  dbcmd = false; // This is a serial command

  cmdhandler();  // Call the Command Handler Function

  serial_buffer_index = 0;  // Clear the Serial Buffer and Reset the Buffer Index Pointer
  serial_buffer[0] = 0;
}

void cmdhandler()  // Command Handler Function
{
  if (cmdexecuting) { resetCMDbuffer(); return; }
  cmdexecuting = true;

  if (cmdbuffer_index == 0) { return; }

  if ((cmdbuffer[0] > 96) && (cmdbuffer[0] < 123))  //If first character of command is lowercase, convert to uppercase
  {
    cmdbuffer[0] = cmdbuffer[0] - 32;
  }

  if (cmdbuffer_index >= 2)
  {
    if ((cmdbuffer[1] > 96) && (cmdbuffer[1] < 123))  //If second character of command is lowercase, convert to uppercase
    {
      cmdbuffer[1] = cmdbuffer[1] - 32;
    }   
  }
  
  switch (cmdbuffer[0]) {  // Decode first character of command
  
    case 65:  // A Command - Stop the Azimuth Rotation 0X41H
      if (cmdbuffer_index == 1) // A only
      {
        dashbnserialmsg += "A? cmd<br />";
        az_rotate_cmd_stop();
      }else{
        switch (cmdbuffer[1])
        {
          case 80: // APxxx
          {
            dashbnserialmsg += "AP? cmd<br />";
            Predict(GetCmdStringInt(2, 3));
          }
          break;
          case 81: // AQ predict one pass after date/time
          {
            dashbnserialmsg += "AQ? cmd<br />";
            myear = GetCmdStringInt(3, 4);
            mmonth = GetCmdStringInt(8, 2);
            mday = GetCmdStringInt(11, 2);
            mhour = GetCmdStringInt(14, 2);
            mminute = GetCmdStringInt(17, 2);
            msecond = GetCmdStringInt(20, 2);

            dashbnserialmsg += "Future Time set to: " + String(myear) + "-" + String(mmonth) + "-" + String(mday) + " " +
                        String(mhour) + ":" + String(mminute) + ":" + String(msecond) + "<br />";

            UseManualTime = true;
            Predict(1);
            UseManualTime = false;
          }
          break;
          case 83: // AS
          {
            satelliteCmds();
          }
          break;
          case 88: // AX
          {
            dashbnserialmsg += "AX? cmd<br />";
            if (cmdbuffer_index >= 3) // AX + space + 1 digit for degrees
            {
              AZdirChangeDegIn  = GetCmdStringInt(4, 1);
              preferences.begin("mat", false); // Open Preferences with mat namespace. RW mode (permanent storage).
              preferences.putInt("hs4changedeg", AZdirChangeDegIn);
              preferences.end();
            }
            dashbnserialmsg += "AZdirChangeDegIn  = " + String(AZdirChangeDegIn);
          }
          break;
          case 89: // AY
          {
            switch (cmdbuffer[2])
            {
              case 49: // AY1 log status report
              {
                dashbnserialmsg += "AY1? cmd<br />";
                dashbnserialmsg += "<br />";
                dashbnserialmsg += "2 satFileLogging " + String(satFileLogging ? "ON" : "OFF") + "<br />";
                dashbnserialmsg += "3 satSerialLogging " + String(satSerialLogging ? "ON" : "OFF") + "<br />";
                dashbnserialmsg += "4 AZRotatorLogging " + String(AZRotatorLogging ? "ON" : "OFF") + "<br />";
                dashbnserialmsg += "5 ELRotatorLogging " + String(ELRotatorLogging ? "ON" : "OFF") + "<br />";
                dashbnserialmsg += "6 TalkNturnLogging " + String(TalkNturnLogging ? "ON" : "OFF");
              }
              break;
              case 50: // AY2 satFileLogging
              {
                dashbnserialmsg += "AY2? cmd<br />";
                satFileLogging = !satFileLogging;
                dashbnserialmsg += "2 satFileLogging " + String(satFileLogging ? "ON" : "OFF") + "<br />";
                dashbnserialmsg += "3 satSerialLogging " + String(satSerialLogging ? "ON" : "OFF") + "<br />";
                dashbnserialmsg += "4 AZRotatorLogging " + String(AZRotatorLogging ? "ON" : "OFF") + "<br />";
                dashbnserialmsg += "5 ELRotatorLogging " + String(ELRotatorLogging ? "ON" : "OFF") + "<br />";
                dashbnserialmsg += "6 TalkNturnLogging " + String(TalkNturnLogging ? "ON" : "OFF");
              }
              break;
              case 51: // AY3 satSerialLogging
              {
                dashbnserialmsg += "AY3? cmd<br />";
                satSerialLogging = !satSerialLogging;
                dashbnserialmsg += "2 satFileLogging " + String(satFileLogging ? "ON" : "OFF") + "<br />";
                dashbnserialmsg += "3 satSerialLogging " + String(satSerialLogging ? "ON" : "OFF") + "<br />";
                dashbnserialmsg += "4 AZRotatorLogging " + String(AZRotatorLogging ? "ON" : "OFF") + "<br />";
                dashbnserialmsg += "5 ELRotatorLogging " + String(ELRotatorLogging ? "ON" : "OFF") + "<br />";
                dashbnserialmsg += "6 TalkNturnLogging " + String(TalkNturnLogging ? "ON" : "OFF");
              }
              break;
              case 52: // AY4 satAZRotatorLogging
              {
                dashbnserialmsg += "AY4? cmd<br />";
                AZRotatorLogging = !AZRotatorLogging;
                dashbnserialmsg += "2 satFileLogging " + String(satFileLogging ? "ON" : "OFF") + "<br />";
                dashbnserialmsg += "3 satSerialLogging " + String(satSerialLogging ? "ON" : "OFF") + "<br />";
                dashbnserialmsg += "4 AZRotatorLogging " + String(AZRotatorLogging ? "ON" : "OFF") + "<br />";
                dashbnserialmsg += "5 ELRotatorLogging " + String(ELRotatorLogging ? "ON" : "OFF") + "<br />";
                dashbnserialmsg += "6 TalkNturnLogging " + String(TalkNturnLogging ? "ON" : "OFF");
              }
              break;
              case 53: // AY5 ELRotatorLogging
              {
                dashbnserialmsg += "AY5? cmd<br />";
                ELRotatorLogging = !ELRotatorLogging;
                dashbnserialmsg += "2 satFileLogging " + String(satFileLogging ? "ON" : "OFF") + "<br />";
                dashbnserialmsg += "3 satSerialLogging " + String(satSerialLogging ? "ON" : "OFF") + "<br />";
                dashbnserialmsg += "4 AZRotatorLogging " + String(AZRotatorLogging ? "ON" : "OFF") + "<br />";
                dashbnserialmsg += "5 ELRotatorLogging " + String(ELRotatorLogging ? "ON" : "OFF") + "<br />";
                dashbnserialmsg += "6 TalkNturnLogging " + String(TalkNturnLogging ? "ON" : "OFF");
              }
              break;
              case 54: // AY6 TalkNturnLogging
              {
                dashbnserialmsg += "AY6? cmd<br />";
                TalkNturnLogging = !TalkNturnLogging;
                dashbnserialmsg += "2 satFileLogging " + String(satFileLogging ? "ON" : "OFF") + "<br />";
                dashbnserialmsg += "3 satSerialLogging " + String(satSerialLogging ? "ON" : "OFF") + "<br />";
                dashbnserialmsg += "4 AZRotatorLogging " + String(AZRotatorLogging ? "ON" : "OFF") + "<br />";
                dashbnserialmsg += "5 ELRotatorLogging " + String(ELRotatorLogging ? "ON" : "OFF") + "<br />";
                dashbnserialmsg += "6 TalkNturnLogging " + String(TalkNturnLogging ? "ON" : "OFF");
              }
              break;
              case 55: // Delete all logs
                dashbnserialmsg += "AY7? cmd<br />";
                deleteFile(SD, SD_LOG_PATH);
                deleteFile(SD, AZ_rotaror_log_path);
                deleteFile(SD, EL_rotaror_log_path);
                deleteFile(SD, TalkNturnAZ_log_path);
                deleteFile(SD, TalkNturnEL_log_path);
                dashbnserialmsg += "All log files deleted.";
              break;
            } 
            break;
          }
          break;
        }
      }
      break;
    case 66:  // B Command - Send the Current Elevation to the PC 0X42H
      if (cmdbuffer_index == 2)
      {
        if (cmdbuffer[1] == 49) // B1 - brakes off.
        {
          dashbnserialmsg += "B1? cmd<br />";
          AZPutBrakesOff();
          dashbnserialmsg += "Brakes are off. Solenoid is in.<br />";
        }else{ // B2 - brakes on.
          dashbnserialmsg += "B2? cmd<br />";
          portENTER_CRITICAL_ISR(&AZstopTimerMux);
          bool temp = AZ_waitingToStop;
          portEXIT_CRITICAL_ISR(&AZstopTimerMux);
          if (temp || AZmanualmoving) { dashbnserialmsg += "can't AZ is turning.<br />"; break; }
          digitalWrite(brakes_opin, pin_off_state);
          delay(BrakeSettleDelay); // Time for brake to settle
          AZ_Brakes_Disengaged = false;
          dashbnserialmsg += "Brakes are on. Solenoid is off.<br />";
        }
      }else{
        dashbnserialmsg += "B? cmd<br />";
        send_requested_el();  // Call the Send Requested Elevation Function
      }
      break;
    
    case 67:      // C - return current azimuth 0X43H
      
      if (cmdbuffer_index == 2)
      {
        if (cmdbuffer[1] == 50) // C2
        {
          dashbnserialmsg += "C2? cmd<br />";
          send_requested_azel();
        }else{ // C3 calculate AZ & EL
          dashbnserialmsg += "C3? cmd<br />";
          get_AZ_North_Degrees();
          AZ_Old = AZ_Requested = AZ_External_Degrees;
          sendDBAZ(false);
          
          Calculate_EL_Mapped_Degrees();
          EL_Old = EL_Requested = EL_Mapped_Degrees;
          sendDBEL(false);
          dashbnserialmsg += "<br />AZ External Degrees is " + String(AZ_External_Degrees);
          dashbnserialmsg += "<br />EL Mapped Degrees is " + String(EL_Mapped_Degrees);
        }
      }else{
        dashbnserialmsg += "C? cmd<br />";
        send_requested_az();
      }

      break;
    
    case 68:  // D - Rotate Elevation Down Command 0X44H
      
      if (cmdbuffer_index >= 2)
      {
        switch (cmdbuffer[1])
        {
          case 48: // D0 - Doppler update to radio test.
          {
            dashbnserialmsg += "D0? cmd<br />";
            switch (BandMode)
            {
              case BandModeSelectUHFforRX:
              {
                TXdopplerFreq = TXdopplerFreq + dopplerDelta;
                RXdopplerFreq = RXdopplerFreq - dopplerUhfDelta;
                break;
              }
              case BandModeSelectVHFforRX:
              {
                TXdopplerFreq = TXdopplerFreq + dopplerUhfDelta;
                RXdopplerFreq = RXdopplerFreq - dopplerDelta;
                break;
              }
              case BandModeVHFonly:
              {
                TXdopplerFreq = TXdopplerFreq + dopplerDelta;
                RXdopplerFreq = RXdopplerFreq - dopplerDelta;
                break;
              }
              case BandModeUHFonly:
              {
                TXdopplerFreq = TXdopplerFreq + dopplerUhfDelta;
                RXdopplerFreq = RXdopplerFreq - dopplerUhfDelta;
                break;
              }
            }

            radioUpdateDoppler();
          }
          break;
          case 49: // D1 - Reset/Prepare radio doppler frequencies.
          {
            dashbnserialmsg += "D1? cmd<br />";
            setStartDopplerFrequencies();
            dashbnserialmsg += "Radio doppler frequencies reset";
          }

          case 50: // D2 - down for 400ms.
          {
            dashbnserialmsg += "D2? cmd<br />";
            int testrunms = ELms1deg;
            if (cmdbuffer_index == 6)
            {
              testrunms = GetCmdStringInt(2, 4);
            }
            ELmanualmoving = true;
            rotate_el_down();  // Call the Rotate Elevation Down Function
            delay(testrunms);
            el_rotate_cmd_stop();
          }
          break;

          case 51: // D3 - toggle ReadRadio switch.
          {
            dashbnserialmsg += "D3? cmd<br />";
            if (radioCatOn)
            {
              readRadioOnce = !readRadioOnce;
              if (readRadioOnce)
              {
                dashbnserialmsg += "ReadRadio once ON";
                civinByte = 0;
                civ_buffer_index = 0;
              }else{
                dashbnserialmsg += "ReadRadio OFF";
                civ.flush();
                civinByte = 0;
                civ_buffer_index = 0;
                memset(civ_buffer, 0, sizeof(civ_buffer));
              }
            }else{
              dashbnserialmsg += "Turn on radioCatOn switch FIRST!";
            }
          }
          break;
 
          case 52: // D4 - Get/Set center TX radio frequency in Hertz. e.g. D4 145990000
          {
            dashbnserialmsg += "D4? cmd<br />";
            if (cmdbuffer_index >= 12) // D4 + space + 9 digits for frequency
            {
              TXcenterFreq  = GetCmdStringInt(3, cmdbuffer_index - 3);
              preferences.begin("mat", false); // Open Preferences with mat namespace. RW mode (permanent storage).
              preferences.putULong("TXcenterFreqHz", TXcenterFreq);
              preferences.end(); // Close the Preferences

              uint8_t bcdA[5] = {0, 0, 0, 0, 0}; // Array to hold BCD representation

              BCDarrayFromInt(TXcenterFreq, bcdA, 5);

              radioTXfreq[9] = bcdA[0]; // Ten MHz
              radioTXfreq[8] = bcdA[1]; // One MHz
              radioTXfreq[7] = bcdA[2]; // Hundred kHz
              radioTXfreq[6] = bcdA[3]; // Ten kHz
              radioTXfreq[5] = bcdA[4]; // One kHz

              dashbnserialmsg += "TXcenterFreq set to " + String(TXcenterFreq);
            }else{
              dashbnserialmsg += " Old TXcenterFreq is " + String(TXcenterFreq);
            }
          }
          break;

          case 53: // D5 - Get/Set center RX radio frequency in Hertz. e.g. D4 437800000
          {
            dashbnserialmsg += "D5? cmd<br />";
            if (cmdbuffer_index >= 12) // D5 + space + 9 digits for frequency
            {
              RXcenterFreq = GetCmdStringInt(3, cmdbuffer_index - 3);
              preferences.begin("mat", false); // Open Preferences with mat namespace. RW mode (permanent storage).
              preferences.putULong("RXcenterFreqHz", RXcenterFreq);
              preferences.end(); // Close the Preferences

              uint8_t bcdA[5] = {0, 0, 0, 0, 0}; // Array to hold BCD representation

              BCDarrayFromInt(RXcenterFreq, bcdA, 5);

              radioRXfreq[9] = bcdA[0]; // Ten MHz
              radioRXfreq[8] = bcdA[1]; // One MHz
              radioRXfreq[7] = bcdA[2]; // Hundred kHz
              radioRXfreq[6] = bcdA[3]; // Ten kHz
              radioRXfreq[5] = bcdA[4]; // One kHz

              dashbnserialmsg += "RXcenterFreq set to " + String(RXcenterFreq);
            }else{
              dashbnserialmsg += " Old RXcenterFreq is " + String(RXcenterFreq);
            }
          }
          break;

          case 54: // D6 - LoadRadioSettings
          {
            dashbnserialmsg += "D6? cmd<br />";
            if (radioCatOn && !radioSidePTT)
            {
              if (radioSimplex)
              {
                civ.write(radioAVFO, 7);
                delay(30);
                civ.write(radioRXfreq, 11);
                delay(50);
              }else{
                civ.write(radioDualWatchOn,8);
                delay(30);
                civ.write(radioSATon, 8);
                delay(30);
                civ.write(radioMaxSqlSubAudio, 8);
                delay(30);
                civ.write(radioSubSpeakerOff, 9);
                delay(30);
                civ.write(radioSubMuteUSB, 9);
                delay(30);
                civ.write(radioSubMuteLan, 9);
                delay(30);
                civ.write(radioACCafMain, 9);
                delay(30);
                civ.write(radioDispRX, 10);
                delay(30);
                civ.write(radioRecBoth, 10);
                delay(50);

                civ.write(radioMainVFO, 7);
                delay(30);
                civ.write(radioRXfreq, 11);
                delay(30);
                civ.write(radioSubVFO, 7);
                delay(30);
                civ.write(radioTXfreq, 11);
                delay(30);
                civ.write(radio10dbAttenuator, 7);
                delay(30);
                civ.write(radioMinRXGain, 8);
                delay(50);
              }

              printLocalTime(true);
              dashbnserialmsg += "Radio settings loaded";
            }else{
              dashbnserialmsg += "Turn radioCatOn switch on, FIRST!";
            }
          }
          break;

          case 55: // D7 - Load radio test frequency.
          {
            dashbnserialmsg += "D7? cmd<br />";
            if (radioCatOn && !radioSidePTT)
            {
              civ.write(radioTestfreq, 11);
              dashbnserialmsg += "Radio test frequency loaded";
            }else{
              dashbnserialmsg += "Turn radioCatOn switch on FIRST!";
            }
          }
          break;

          case 56: // D8 - Set Radio Mode to Simplex.
          {
            dashbnserialmsg += "D8? cmd<br />";
            if (radioCatOn)
            {
              radioSetSimplex();
            }else{
              dashbnserialmsg += "Turn on the radioCatOn switch FIRST!";
            }
          }
          break;

          case 57: // D9 Get radio S level meter value
            dashbnserialmsg += "D9? cmd<br />";
            if (radioCatOn)
            {
              SmeterLevelReq = true;
              readRadioOnce = true;
              civ.write(radioSmeter, 7);
            }else{
              dashbnserialmsg += "Turn on the radioCatOn switch FIRST!";
            }
          break;

          case 65: // DA - Auto_polarization state
          {
            dashbnserialmsg += "DA? cmd<br />";
            if (cmdbuffer_index >= 3)
            {
              if (cmdbuffer[2] == 50) // DA2 - toggle test mode.
              {
                autoPolarizeTest = !autoPolarizeTest;
                dashbnserialmsg += "Test mode is ";
                dashbnserialmsg += (autoPolarizeTest ? "ON" : "OFF");
                dashbnserialmsg += "<br />";
              }
              else
              {
                if (cmdbuffer[2] == 49) // DA1
                {
                  autoPolarize = true;
                }else{
                  autoPolarize = false;
                }

                preferences.begin("mat", false); // Open Preferences with mat namespace. RW mode (permanent storage).
                preferences.putBool("autoPolarize", autoPolarize);
                preferences.end(); // Close the Preferences
              }
            }
            dashbnserialmsg += "Auto polarization is ";
            dashbnserialmsg += (autoPolarize ? "ON" : "OFF");
          }

          break;

          case 66: // DB - Dashboard ON/OFF
          {
            dashbnserialmsg += "DB? cmd<br />";
            if (cmdbuffer_index == 3)
            {
              if (cmdbuffer[2] == 49) // DB1
              {
                dashbnserialmsg += "DB1? cmd<br />";
                dashboardOn();
                Serial.println("Dashboard ON");
              }else{
                dashbnserialmsg += "DB2? cmd<br />";
                dashboardOff();
                Serial.println("Dashboard OFF");
              }
            }
          }
          break;

          case 67: // DC - Memorize radio doppler steps.
          {
            dashbnserialmsg += "DC? cmd<br />";
            radioMemoDoppler();
            dashbnserialmsg += "Radio doppler steps memorized.";
          }
          break;

          case 68: // DD - Highveld repeater
          {
            dashbnserialmsg += "DD? cmd<br />";
            radioSetSimplex();
            civ.write(radioRXHVB, 11);
            delay(40);
            civ.write(radioDUPoffset, 9);
            delay(40);
            civ.write(radioDUPon, 7);
            delay(40);
            dashbnserialmsg += "Radio to ZS6HVB.";
          }
        }
      }else{
        dashbnserialmsg += "D? cmd<br />";
        ELmanualmoving = true;
        rotate_el_down();  // Call the Rotate Elevation Down Function
      }

      break;
    
    case 69:  // E - Stop the Elevation Rotation 0X45H
      
      if (cmdbuffer_index >= 2) // E2, E3, E4
      {
        switch (cmdbuffer[1])
        {
          case 50: // E2 - Set requested EL from command and stop rotation.
          {
            dashbnserialmsg += "E2? cmd<br />";
            if (cmdbuffer_index != 5)
            {
              dashbnserialmsg += "E2 command requires a value. e.g. E2045";
              break;
            }

            EL_Requested = GetCmdStringInt(2, 3);
            EL_Requested = ElMinMaxDegrees(EL_Requested);
          }
          break;
          case 51: // E3 - Set EL Linear measurement.
          {
            dashbnserialmsg += "E3? cmd<br />";
            ELnonLinear = false;
            preferences.begin("mat", false); // Open Preferences with mat namespace. RW mode (permanent storage).
            preferences.putBool("ELnonLinear", ELnonLinear);
            preferences.end();
            dashbnserialmsg += "EL linear measurement persisted.";
          }
          break;
          case 52: // E4 - Set EL Non-Linear measurement.
          {
            dashbnserialmsg += "E4? cmd<br />";
            ELnonLinear = true;
            preferences.begin("mat", false); // Open Preferences with mat namespace. RW mode (permanent storage).
            preferences.putBool("ELnonLinear", ELnonLinear);
            preferences.end();
            dashbnserialmsg += "EL non-linear measurement persisted.";
          }
          break;
          case 53: // E5 - EL report.
          {
            dashbnserialmsg += "E5? cmd<br />";
            dashbnserialmsg += "ELnonLinear is ";
            dashbnserialmsg += (ELnonLinear ? "ON" : "OFF");
            Calculate_EL_Mapped_Degrees();
            dashbnserialmsg += "\nEL Mapped Degrees is " + String(EL_Mapped_Degrees);
            dashbnserialmsg += "\nEL ADC is " + String(EL_current_ADC);
          }
          break;
        }
      }else{
        dashbnserialmsg += "E? cmd<br />";
        el_rotate_cmd_stop(); // E only
      }
      break;     
    case 70:  // F - Set the Max Calibration 0X46H
      
      if ((cmdbuffer_index == 2) && (cmdbuffer[1] == 50)) // Check for F2 Command
      {  
        dashbnserialmsg += "F2 cmd<br />";     
        set_max_el_cal();  // F2 - Set the Max Elevation Calibration
      } else { 
        dashbnserialmsg += "F? cmd<br />";
        set_max_az_cal();  // F - Set the Max Azimuth Calibration
      }

    break;

    case 71: // G - Set GPredict mode 0X47H
      dashbnserialmsg += "G? cmd<br />";
      gpredict = 1;
      dashbnserialmsg += "GPredict mode set";
    break;

    case 72: // H - Ham4 Super North Offset info. H2 - Set North here. 0X48H
      
      if (cmdbuffer_index == 2)
      {
        if (cmdbuffer[1] == 51) // Check for H3 Command. RESET.
        {
          dashbnserialmsg += "H3? cmd<br />";
          SH4_North_ADC = -1;
          SH4InternalNdegrees = 0;
          SH4IntNdegDelta = 0;

          preferences.begin("mat", false); // Open Preferences with mat namespace. RW mode (permanent storage).
          preferences.putInt("NOffsADC", SH4_North_ADC);
          preferences.putInt("NOffsDeg", SH4InternalNdegrees);
          preferences.putInt("NOffsDelta", SH4IntNdegDelta);
          preferences.putBool("azdirchanged", false);
          preferences.end();

          dashbnserialmsg += (NorthOverride + " reset to default. <br />");

          break;  
        }

        if (cmdbuffer[1] == 50) // Check for H2 Command
        {
          dashbnserialmsg += "H2? cmd<br />";
          SH4_North_ADC = -1;
          SH4InternalNdegrees = 0;
          SH4IntNdegDelta = 0;

          get_AZ_North_Degrees();  // Read the ADC
          SH4_North_ADC = AZ_current_ADC;
          SH4InternalNdegrees = AZ_External_Degrees;

          compassFromDeg = SH4InternalNdegrees;
          compassToDeg = MaxAZdegrees;
          getShortestTurn();
          if (!compassUseDirection)
          {
            SH4IntNdegDelta = -1 * compassDirectionDistance;
          }else{
            SH4IntNdegDelta = compassDirectionDistance;
          }

          preferences.begin("mat", false); // Open Preferences with mat namespace. RW mode (permanent storage).
          preferences.putInt("NOffsADC", SH4_North_ADC);
          preferences.putInt("NOffsDeg", SH4InternalNdegrees);
          preferences.putInt("NOffsDelta", SH4IntNdegDelta);
          preferences.putBool("azdirchanged", false);
          preferences.end();

          dashbnserialmsg += (NorthOverride + "set here.<br />");

          break;
        }
      }else{
        dashbnserialmsg += "H? cmd<br />";
      }
      
      dashbnserialmsg += "\n";
      dashbnserialmsg += (NorthOverride + "voltage = " + String(SH4_North_ADC) + '\n');
      dashbnserialmsg += (NorthOverride + "degrees internal = " + String(SH4InternalNdegrees) + '\n');
      dashbnserialmsg += (NorthOverride + "degrees delta = " + String(SH4IntNdegDelta));

      break;

    case 73: // AC power delay info I 0X49H
      
      if ((cmdbuffer_index == 2) && (cmdbuffer[1] == 50)) // Check for I2 Command
      {
        dashbnserialmsg += "I2? cmd<br />";
        acPowerOn();
        dashbnserialmsg += "AC power delay off running";
      }else{ 
        dashbnserialmsg += "I? cmd<br />"; 
        dashbnserialmsg += "AC power is ";
        dashbnserialmsg += (powerOn ? "ON " : "OFF ");
        dashbnserialmsg += "\nDelay up count = ";
        dashbnserialmsg += String(powerDelay);
        dashbnserialmsg += '\n';     
      }

      break;
    
    case 76:  // L - Rotate Azimuth CCW 0X4CH
      
      if (cmdbuffer_index >= 2) // L only
      {
        if (cmdbuffer[1] == 50)
        {
          dashbnserialmsg += "L2? cmd<br />";
          int testrunms = AZms1deg;
          if (cmdbuffer_index == 6)
          {
            testrunms = GetCmdStringInt(2, 4);
          }
          AZmanualmoving = true;
          rotate_az_ccw();  // Call the Rotate Azimuth CCW Function
          delay(testrunms);
          az_rotate_cmd_stop();
        }else if (cmdbuffer[1] == 77){ // LMx Led Matrix color
          dashbnserialmsg += "LM? cmd<br />";
          neopixels(cmdbuffer[2]);
        }
      }else{
        dashbnserialmsg += "L? cmd<br />";
        AZmanualmoving = true;
        rotate_az_ccw();  // Call the Rotate Azimuth CCW Function
      }

      break;
    
    case 77:  // M - Rotate to Set Point 0X4DH
      
      if (cmdbuffer_index == 1)
      {
        dashbnserialmsg += "M? cmd<br />";
        dashbnserialmsg += "LoopTaskStackSize = " + String(getArduinoLoopTaskStackSize()) +'\n';
        // Check integrity of all heaps
        dashbnserialmsg += "biggest free heap block: " + String(heap_caps_get_largest_free_block(MALLOC_CAP_8BIT));

      }else{
        dashbnserialmsg += "Mxxx cmd<br />";
        AZ_Requested = GetCmdStringInt(1, 3);
        AZ_Requested = AzMinMaxDegrees(AZ_Requested);
        AZcmdfirststep = true;    
      }

      break;

    case 78: // N - SD card 0X4EH
      
      if (cmdbuffer_index >= 2) // Check for 2nd Command
      {
        switch (cmdbuffer[1])
        {
          case 49: //N1
            dashbnserialmsg += "N1? cmd<br />";
            SDcardOn();
          break;
          case 50: //N2
            dashbnserialmsg += "N2? cmd<br />";
            SDcardOff();
          break;
          case 51: //N3
            dashbnserialmsg += "N3? cmd<br />";
            listDir(SD, "/", 0);
          break;
          case 52: //N4 read file.
            dashbnserialmsg += "N4? cmd<br />";
            GetFileName(3, cmdbuffer_index - 3);
            readFile(SD, filename);
          break;
          case 53: //N5 delete file.
            dashbnserialmsg += "N5? cmd<br />";
            GetFileName(3, cmdbuffer_index - 3);
            deleteFile(SD, filename);
          break;
          case 54: //N6 write file.
            {
              dashbnserialmsg += "N6? cmd<br />";
              int slashIndex = 0;
              for (int i = 3; i < cmdbuffer_index; i++)
              {
                if (cmdbuffer[i] == '/')
                {
                  slashIndex = i;
                  break;
                }
              }
              if (slashIndex > 3)
              {
                GetFileName(3, slashIndex - 3);
                String message = "";
                for (int j = slashIndex + 1; j < cmdbuffer_index; j++)
                {
                  message += char(cmdbuffer[j]);
                }
                writeFile(SD, filename, message.c_str(), true);
              }else{
                dashbnserialmsg += "Error: No '/' found in command.";
              }
            }
          break;
          case 55: //N7 append file.
            {
              dashbnserialmsg += "N7? cmd<br />";
              int slashIndex = 0;
              for (int i = 3; i < cmdbuffer_index; i++)
              {
                if (cmdbuffer[i] == '/')
                {
                  slashIndex = i;
                  break;
                }
              }
              if (slashIndex > 3)
              {
                GetFileName(3, slashIndex - 3);
                String message = "";
                for (int j = slashIndex + 1; j < cmdbuffer_index; j++)
                {
                  message += char(cmdbuffer[j]);
                }
                appendFile(SD, filename, message.c_str(), true);
              }else{
                dashbnserialmsg += "Error: No '/' found in command.";
              }
            }
          break;
        }
      }else{
        dashbnserialmsg += "N? cmd<br />";
        dashbnserialmsg += "SD card is ";
        dashbnserialmsg += SDcardIsOn ? On : Off;
      }

    break;
          
    case 79:  // O - Set Zero Calibration 0X4FH
      
      if ((cmdbuffer_index == 2) && (cmdbuffer[1] == 50)) // Check for O2 Command
      {
        dashbnserialmsg += "O2? cmd<br />";
        set_0_el_cal();  // O2 - Set the Elevation Zero Calibration
      } else {
        dashbnserialmsg += "O? cmd<br />";
        set_0_az_cal();  // O - Set the Azimuth Zero Calibration
      }

    break;

    case 80: // P commands. 0X50H
      
      if (cmdbuffer_index >= 2) // !P only
      {
        switch (cmdbuffer[1])
        {
          case 49: // P1 set Auto_polarize to AutoPolarizeUpLHCP
            dashbnserialmsg += "P1? cmd<br />";
            Auto_polarize = AutoPolarizeUpLHCP;
            dashbnserialmsg += "Auto_polarize is ";
            dashbnserialmsg += "AutoPolarizeUpLHCP";
          break;
          case 50: // P2 set Auto_polarize to AutoPolarizeUpRHCP
            dashbnserialmsg += "P2? cmd<br />";
            Auto_polarize = AutoPolarizeUpRHCP;
            dashbnserialmsg += "Auto_polarize is ";
            dashbnserialmsg += "AutoPolarizeUpRHCP";
          break;
          case 76: // PL LHCP
            dashbnserialmsg += "PL? cmd<br />";
            Auto_polarize = LHCP;
            dashbnserialmsg += "Auto_polarize is ";
            dashbnserialmsg += "LHCP";

            digitalWrite(polarize_opin, pin_active_state); // Left hand polarization
            PolarizeRight = false;
          break;

          case 80: // PP wifi password. e.g. PP *ionosphere1* (20 chars max.)
            dashbnserialmsg += "PP? cmd<br />";
            tempstring = GetSerialString(3, (cmdbuffer_index - 3));
            strcpy(passwordcharr, tempstring.c_str());
            preferences.begin("mat", false); // Open Preferences with mat namespace. RW mode (permanent storage).
            preferences.putString("wifipassword", tempstring);
            preferences.end();
            tempstring = "";
            dashbnserialmsg += "Wifi pwd is " + tempstring;
          break;

          case 82: // PR RHCP
            dashbnserialmsg += "PR? cmd<br />";
            Auto_polarize = RHCP;
            dashbnserialmsg += "Auto_polarize is ";
            dashbnserialmsg += "RHCP";

            digitalWrite(polarize_opin, pin_off_state); // Right hand polarization
            PolarizeRight = true;
          break;

          case 83: // PS wifi SSID e.g. PS ZS6PEG1 (10 chars max.)
            dashbnserialmsg += "PS? cmd<br />";
            tempstring = GetSerialString(3, (cmdbuffer_index - 3));
            strcpy(ssidcharr, tempstring.c_str());
            preferences.begin("mat", false); // Open Preferences with mat namespace. RW mode (permanent storage).
            preferences.putString("wifissid", tempstring);
            preferences.end();
            tempstring = "";
            dashbnserialmsg += "Wifi ssid is " + tempstring;
          break;
        }
      }else{
        dashbnserialmsg += "P? cmd<br />";
        switch (Auto_polarize)
        {
          case RHCP:
            dashbnserialmsg += "RHCP";
          break;
          case LHCP:
            dashbnserialmsg += "LHCP";
          break;
          case AutoPolarizeUpLHCP:
            dashbnserialmsg += "AutoPolarizeUpLHCP";
          break;
          case AutoPolarizeUpRHCP:
            dashbnserialmsg += "AutoPolarizeUpRHCP";
          break;
        }
      }
    break;
    
    case 82:  // R - Rotate Azimuth CW 0X52H
      
      if (cmdbuffer_index >= 2) // !R only
      {
        if (cmdbuffer[1] == 50) // R2 short right.
        {
          dashbnserialmsg += "R2? cmd<br />";
          int testrunms = AZms1deg;
          if (cmdbuffer_index == 6)
          {
            testrunms = GetCmdStringInt(2, 4);
          }
          AZmanualmoving = true;
          rotate_az_cw();  // Call the Rotate Azimuth CW Function
          delay(testrunms);
          az_rotate_cmd_stop();
        } else if ((cmdbuffer[1] == 83) && (cmdbuffer[2] == 116)) // RSt - Reset the CPU
        {
          dashbnserialmsg += "RSt? cmd<br />";
          Serial.println("Resetting the CPU");
          ESP.restart();

        }else if (cmdbuffer[1] == 82) // RR - Report the CPU reset reason.
        {
          dashbnserialmsg += "RR? cmd<br />";
          Serial.println("CPU0 reset reason: ");
          print_reset_reason(rtc_get_reset_reason(0));

          Serial.println("CPU1 reset reason: ");
          print_reset_reason(rtc_get_reset_reason(1));
        } else if (cmdbuffer[1] == 88){ // RX - Switch radio to RX mode.
          dashbnserialmsg += "RX? cmd<br />";
          radioRX();
        }
      }else{
        dashbnserialmsg += "R? cmd<br />";
        AZmanualmoving = true;
        rotate_az_cw();  // Call the Rotate Azimuth CW Function
      }

    break;  
   
    case 83:  // S - Stop All Rotation 0X53H
      
      if (cmdbuffer_index == 2)
      {
        switch (cmdbuffer[1])
        {
          case 48: // S0 - Switches status.
            dashbnserialmsg += "S0? cmd<br />";
            dashbnserialmsg += "\nRotators are ";
            dashbnserialmsg += (disable_rotators ? Off : On);
            dashbnserialmsg += "\nPTT interupts are ";
            dashbnserialmsg += disable_ptt ? Off : On;
            dashbnserialmsg += "\nradioCatOn is ";
            radioCatOn ? dashbnserialmsg += "YES ON" : dashbnserialmsg += "NO OFF";
            dashbnserialmsg += "\nsatDisableAutoCycleTime is ";
            satDisableAutoCycleTime ? dashbnserialmsg += "Constant" : dashbnserialmsg += "NO OFF";
            break;
          case 49: // S1 - toggle PTT_disable. Default off.
            dashbnserialmsg += "S1? cmd<br />";
            if (disable_ptt)
            {
              disable_ptt = false;
            }else{
              disable_ptt = true;
            }
            dashbnserialmsg += "\nRotators are ";
            dashbnserialmsg += (disable_rotators ? Off : On);
            dashbnserialmsg += "\nPTT interupts are ";
            dashbnserialmsg += disable_ptt ? Off : On;
            dashbnserialmsg += "\nradioCatOn is ";
            radioCatOn ? dashbnserialmsg += "YES ON" : dashbnserialmsg += "NO OFF";
            dashbnserialmsg += "\nsatDisableAutoCycleTime is ";
            satDisableAutoCycleTime ? dashbnserialmsg += "Constant" : dashbnserialmsg += "NO OFF";            
            break;
          case 50: // S2 - toggle rotator enable/disable
            dashbnserialmsg += "S2? cmd<br />";
            if (disable_rotators)
            {
              Reset_AZ_Degrees();
              Reset_EL_Degrees();
              radioTXing = false;
              disable_rotators = false;
            }else{
              disable_rotators = true;
            }
            dashbnserialmsg += "\nRotators are ";
            dashbnserialmsg += (disable_rotators ? Off : On);
            dashbnserialmsg += "\nPTT interupts are ";
            dashbnserialmsg += disable_ptt ? Off : On;
            dashbnserialmsg += "\nradioCatOn is ";
            radioCatOn ? dashbnserialmsg += "YES ON" : dashbnserialmsg += "NO OFF";
            dashbnserialmsg += "\nsatDisableAutoCycleTime is ";
            satDisableAutoCycleTime ? dashbnserialmsg += "Constant" : dashbnserialmsg += "NO OFF";
            break;
          case 51: // S3 - SWR test. RX (UHF) RHCP. Default.
            dashbnserialmsg += "S3? cmd<br />";
            BandMode = BandModeSelectUHFforRX;
            switchBandMode();
            digitalWrite(polarize_opin, pin_off_state); // Right hand polarization.
            PolarizeRight = true;
            dashbnserialmsg += "Band mode set to RX on UHF for SWR test. RHCP";
           break;
          case 52: // S4 - SWR test. RX (UHF) LHCP
            dashbnserialmsg += "S4? cmd<br />";
            BandMode = BandModeSelectUHFforRX;
            switchBandMode();
            digitalWrite(polarize_opin, pin_active_state); // Left hand polarization.
            PolarizeRight = false;
            dashbnserialmsg += "Band mode set to RX on UHF for SWR test. LHCP";
            break;
          case 53: // S5 - SWR test. RX (VHF) RHCP
            dashbnserialmsg += "S5? cmd<br />";
            BandMode = BandModeSelectVHFforRX;
            switchBandMode();
            digitalWrite(polarize_opin, pin_off_state); // Right hand polarization.
            PolarizeRight = true;
            dashbnserialmsg += "Band mode set to RX on VHF for SWR test. RHCP";
            break;
          case 54: // S6 - SWR test. RX (VHF) LHCP
            dashbnserialmsg += "S6? cmd<br />";
            BandMode = BandModeSelectVHFforRX;
            switchBandMode();
            digitalWrite(polarize_opin, pin_active_state); // Left hand polarization.
            PolarizeRight = false;
            dashbnserialmsg += "Band mode set to RX on VHF for SWR test. LHCP";
            break;
          case 57: // S9 - Toggle radioCatOn switch
            dashbnserialmsg += "S9? cmd<br />";
            if (radioCatOn)
            {
              radioCatOn = false;
            }else{
              radioCatOn = true;
            }
            dashbnserialmsg += "\nRotators are ";
            dashbnserialmsg += (disable_rotators ? Off : On);
            dashbnserialmsg += "\nPTT interupts are ";
            dashbnserialmsg += disable_ptt ? Off : On;
            dashbnserialmsg += "\nradioCatOn is ";
            radioCatOn ? dashbnserialmsg += "YES ON" : dashbnserialmsg += "NO OFF";
            dashbnserialmsg += "\nsatDisableAutoCycleTime is ";
            satDisableAutoCycleTime ? dashbnserialmsg += "Constant" : dashbnserialmsg += "NO OFF";          
            break;
          case 65: // Sa - Toggle satDisableAutoCycleTime switch
            satDisableAutoCycleTime = !satDisableAutoCycleTime;
            dashbnserialmsg += "\nRotators are ";
            dashbnserialmsg += (disable_rotators ? Off : On);
            dashbnserialmsg += "\nPTT interupts are ";
            dashbnserialmsg += disable_ptt ? Off : On;
            dashbnserialmsg += "\nradioCatOn is ";
            radioCatOn ? dashbnserialmsg += "YES ON" : dashbnserialmsg += "NO OFF";
            dashbnserialmsg += "\nsatDisableAutoCycleTime is ";
            satDisableAutoCycleTime ? dashbnserialmsg += "Constant" : dashbnserialmsg += "NO OFF";
              break;
        }
      }else{
        dashbnserialmsg += "S? cmd<br />";
        az_rotate_cmd_stop();
        el_rotate_cmd_stop();
        dashbnserialmsg += "All rotation stopped.<br />";
      }
      break;

    case 84: // T - test relays 0X54H
      
      if (cmdbuffer_index == 2) // Check for 2nd Command
      {
        switch (cmdbuffer[1])
        {
          case 49: //T1
            dashbnserialmsg += "T1? cmd<br />";
            acPowerOn();
          break;
          case 50: //T2
            dashbnserialmsg += "T2? cmd<br />";
            AZPutBrakesOff();
          break;
          case 51: //T3
            dashbnserialmsg += "T3? cmd<br />";
            digitalWrite(rotate_left_opin, pin_active_state);
          break;
          case 52: //T4
            dashbnserialmsg += "T4? cmd<br />";
            digitalWrite(rotate_right_opin, pin_active_state);
          break;
          case 53: //T5
            dashbnserialmsg += "T5? cmd<br />";
            digitalWrite(rotate_up_opin, pin_active_state);
          break;
          case 54: //T6
            dashbnserialmsg += "T6? cmd<br />";
            digitalWrite(rotate_down_opin, pin_active_state);
          break;
          case 55: //T7
            dashbnserialmsg += "T7? cmd<br />";
            BandMode = BandModeSelectUHFforRX;
            switchBandMode();
          break;
          case 56: //T8
            dashbnserialmsg += "T8? cmd<br />";
            digitalWrite(polarize_opin, pin_active_state);
          break;
          case 57: //T9
            dashbnserialmsg += "T9? cmd<br />";
            RTCtime();
          break;
          case 84: //TT
            dashbnserialmsg += "TT? cmd<br />";
            dashbnserialmsg += "TT is ";
            if (enableTurnnTalk)
            {
              dashbnserialmsg += Off;
              enableTurnnTalk = false;
            }else{
              dashbnserialmsg += On;
              enableTurnnTalk = true;
            }
          break;
          case 88: // TX - Switch radio to TX mode. VSend.
            dashbnserialmsg += "TX? cmd<br />";
            radioTX();
          break;
          case 89: // TY - Timer50ms 1 second test toggle
            dashbnserialmsg += "TY? cmd<br />";
            if (TestTimer)
            {
              TestTimer = false;
              testms = 0;
              dashbnserialmsg += "TestTimer OFF<br />";
            }else{
              TestTimer = true;
              dashbnserialmsg += "TestTimer ON<br />";
            }
          break;
          default:
            digitalWrite(rotate_left_opin, pin_off_state);
            digitalWrite(rotate_right_opin, pin_off_state);
            digitalWrite(rotate_up_opin, pin_off_state);
            digitalWrite(rotate_down_opin, pin_off_state);
            digitalWrite(polarize_opin, pin_off_state);
            digitalWrite(brakes_opin, pin_off_state);
            BandMode = BandModeSelectUHFforRX;
            switchBandMode();
          break;
        }
      }else{
        // Reset
        dashbnserialmsg += "T? cmd<br />";
        Reset_AZ_Degrees();
        Reset_EL_Degrees();
        dashbnserialmsg += "Rotators reset. AZ and EL degrees set to 0.";
      }

    break;
    
    case 85:  // U - Rotate Elevation Up 0X55H
      
      if (cmdbuffer_index >= 2)
      {
        if (cmdbuffer[1] == 50) // U2 - up for 150ms.
        {
          dashbnserialmsg += "U2? cmd<br />";
          int testrunms = ELms1deg;
          if (cmdbuffer_index == 6)
          {
            testrunms = GetCmdStringInt(2, 4);
          }
          ELmanualmoving = true;
          rotate_el_up();
          delay(testrunms);
          el_rotate_cmd_stop();
        }
      }else{
        dashbnserialmsg += "U? cmd<br />";
        ELmanualmoving = true;
        rotate_el_up();
      }

    break;

    case 86:  // V - Get Direction and distance to XXX 0X56H e.g v001 010
      dashbnserialmsg += "V? cmd<br />";
      compassFromDeg = GetCmdStringInt(1, 3);
      compassToDeg = GetCmdStringInt(5, 3);
      getShortestTurn();
      dashbnserialmsg += "Direction is ";
      dashbnserialmsg += (compassUseDirection ? Right : Left) +'\n';
      dashbnserialmsg += " Distance is ";
      dashbnserialmsg += compassDirectionDistance;
      break;
          
    case 87:  // W - Rotate Azimuth and Elevation to Set Point 0X57H
      
      if (cmdbuffer_index == 8)
      {
        dashbnserialmsg += "WXXX YYY? cmd<br />";

        AZ_Requested = GetCmdStringInt(1, 3);
        AZ_Requested = AzMinMaxDegrees(AZ_Requested);
        AZcmdfirststep = true;   

        EL_Requested = GetCmdStringInt(5, 3);
        EL_Requested = ElMinMaxDegrees(EL_Requested);

      }else{ // W1 - report wifi signal strength.
        dashbnserialmsg += "W1? cmd<br />";
        wifiRSSI = WiFi.RSSI();
        dashbnserialmsg += String(wifiRSSI);
        dashbnserialmsg += " dBm";
      }

    break;

    case 88: // X get ham4 degrees 0X58H
      dashbnserialmsg += "X? cmd<br />";
      X_routine();

    break;

    case 89: // Y - PTT mode 0X59H
      
      if (cmdbuffer_index == 2) // Check for 2nd Command
      {
        switch (cmdbuffer[1])
        {
          case 49: //1
            dashbnserialmsg += "Y1? cmd<br />";
            BandMode = BandModeSelectUHFforRX;
            switchBandMode();
            dashbnserialmsg += "BandMode set to BandModeSelectUHF. Relay normally off to RX on UHF.";
          break;
          case 50: //2
            dashbnserialmsg += "Y2? cmd<br />";
            BandMode = BandModeSelectVHFforRX;
            switchBandMode();
            dashbnserialmsg += "BandMode set to BandModeSelectVHF. Relay normally on to RX on VHF.";
          break; 
          case 51: //3
            dashbnserialmsg += "Y3? cmd<br />";
            BandMode = BandModeVHFonly;
            switchBandMode();
            dashbnserialmsg += "BandMode set to BandModeVHFonly. Relay always on.";
          break;
          case 52: //4
            dashbnserialmsg += "Y4? cmd<br />";
            BandMode = BandModeUHFonly;
            switchBandMode();
            dashbnserialmsg += "BandMode set to BandModeUHFonly. Relay always off.";
          break;          
        }
        break;
      }else{
        dashbnserialmsg += "Y? cmd<br />";
      }

      dashbnserialmsg +=  '\n' + "VSend Status is ";
      dashbnserialmsg += (Vsend ? "TX." : "RX.") +'\n'; 
      dashbnserialmsg += "HSend Status is ";
      dashbnserialmsg += Hsend ? "TX." : "RX.";

      break;

  }
  dashbnserialmsg += "<br />";
  dashbnserialmsg += '\n';
  resetCMDbuffer();
  cmdexecuting = false;
}

void resetCMDbuffer()
{
  cmdbuffer_index = 0; // Reset command buffer index after command processed 
  cmdbuffer[0] = 0;
}

void loop() 
{
  Sat_Tick();
  PTT_handler(); // PTT radio interupt test 
  check_stop();  // Check to see if executing move command
  check_serial();  // Check the Serial Port for Data
  check_start(); // turn rotators here
  one_second();  // power delay handling
  turn_talk(); // Turn while talking
  check_civ(); // Check for data from the radio
  reporting();
}