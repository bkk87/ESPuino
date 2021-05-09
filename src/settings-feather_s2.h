#include "Arduino.h"

//######################### INFOS ####################################
/*
   Status: In development
*/

//################## GPIO-configuration ##############################
#define SPISD_CS   38                               // GPIO for chip select (SD)
#define SPISD_MOSI 33                               // GPIO for master out slave in (SD) => not necessary for single-SPI
#define SPISD_MISO  8                               // GPIO for master in slave ou (SD) => not necessary for single-SPI
#define SPISD_SCK   9                               // GPIO for clock-signal (SD) => not necessary for single-SPI

// RFID (via SPI)
#define RST_PIN                         99          // Not necessary but has to be set anyway; so let's use a dummy-number
#define RFID_CS                         10          // GPIO for chip select (RFID)
#define RFID_MOSI                        3          // GPIO for master out slave in (RFID)
#define RFID_MISO                        1          // GPIO for master in slave out (RFID)
#define RFID_SCK                         7          // GPIO for clock-signal (RFID)

#ifdef RFID_READER_TYPE_PN5180
    #define RFID_BUSY                   16          // PN5180 BUSY PIN
    #define RFID_RST                    22          // PN5180 RESET PIN
    #define RFID_IRQ                    39          // PN5180 IRQ PIN (only needed for low power card detection)
#endif
// I2S (DAC)
#define I2S_DOUT                        37          // Digital out (I2S)
#define I2S_BCLK                        35          // BCLK (I2S)
#define I2S_LRC                         36          // LRC (I2S)

// Rotary encoder
#ifdef USEROTARY_ENABLE
    #define DREHENCODER_CLK             99          // If you want to reverse encoder's direction, just switch GPIOs of CLK with DT (in software or hardware)
    #define DREHENCODER_DT              99          // Info: Lolin D32 / Lolin D32 pro 35 are using 35 for battery-voltage-monitoring!
    #define DREHENCODER_BUTTON          99          // (set to 99 to disable; 0->39 for GPIO; 100->115 for port-expander)
#endif

// Control-buttons (set to 99 to DISABLE; 0->39 for GPIO; 100->115 for port-expander)
#define NEXT_BUTTON                     99          // Button 0: GPIO to detect next
#define PREVIOUS_BUTTON                 99          // Button 1: GPIO to detect previous (Important: as of 19.11.2020 changed from 33 to 2; make sure to change in SD-MMC-mode)
#define PAUSEPLAY_BUTTON                99          // Button 2: GPIO to detect pause/play
#define BUTTON_4                        99          // Button 4: unnamed optional button
#define BUTTON_5                        99          // Button 5: unnamed optional button

// I2C-configuration (necessary for RC522 [only via i2c - not spi!] or port-expander)
#if defined(RFID_READER_TYPE_MFRC522_I2C) || defined(PORT_EXPANDER_ENABLE)
    #define ext_IIC_CLK                 5           // i2c-SCL (clock)
    #define ext_IIC_DATA                2           // i2c-SDA (data)
#endif

// Wake-up button => this also is the interrupt-pin if port-expander is enabled!
// Please note: only RTC-GPIOs (0, 4, 12, 13, 14, 15, 25, 26, 27, 32, 33, 34, 35, 36, 39, 99) can be used! Set to 99 to DISABLE.
// Please note #2: this button can be used as interrupt-pin for port-expander. If so, all pins connected to port-expander can wake up ESPuino.
#define WAKEUP_BUTTON                   99//DREHENCODER_BUTTON // Defines the button that is used to wake up ESPuino from deepsleep.

// (optional) Power-control
#define POWER                           21          // GPIO used to drive transistor-circuit, that switches off peripheral devices while ESP32-deepsleep

// (optional) Neopixel
#define LED_PIN                          5          // GPIO for Neopixel-signaling

// (optinal) Headphone-detection
#ifdef HEADPHONE_ADJUST_ENABLE
    #define HP_DETECT                   99          // GPIO that detects, if there's a plug in the headphone jack or not
#endif

// (optional) Monitoring of battery-voltage via ADC
#ifdef MEASURE_BATTERY_VOLTAGE
    #define VOLTAGE_READ_PIN            33          // GPIO used to monitor battery-voltage. Change to 35 if you're using Lolin D32 or Lolin D32 pro as it's hard-wired there!
    constexpr float referenceVoltage = 3.35;                  // Voltage between 3.3V and GND-pin at the develboard in battery-mode (disconnect USB!)
    constexpr float offsetVoltage = 0.1;                      // If voltage measured by ESP isn't 100% accurate, you can add an correction-value here
#endif

// (optional) For measuring battery-voltage a voltage-divider is necessary. Their values need to be configured here.
#ifdef MEASURE_BATTERY_VOLTAGE
    constexpr uint8_t rdiv1 = 129;                               // Rdiv1 of voltage-divider (kOhms) (measure exact value with multimeter!)
    constexpr uint16_t rdiv2 = 129;                              // Rdiv2 of voltage-divider (kOhms) (measure exact value with multimeter!) => used to measure voltage via ADC!
#endif

// (Optional) remote control via infrared
#ifdef IR_CONTROL_ENABLE
    #define IRLED_PIN                   22              // GPIO where IR-receiver is connected (only tested with VS1838B)
    #define IR_DEBOUNCE                 200             // Interval in ms to wait at least for next signal (not used for actions volume up/down)

    // Actions available. Use your own remote control and have a look at the console for "Command=0x??". E.g. "Protocol=NEC Address=0x17F Command=0x68 Repeat gap=39750us"
    // Make sure to define a hex-code not more than once as this will lead to a compile-error
    // https://forum.espuino.de/t/neues-feature-fernsteuerung-per-infrarot-fernbedienung/265
    #define RC_PLAY                     0x68            // command for play
    #define RC_PAUSE                    0x67            // command for pause
    #define RC_NEXT                     0x6b            // command for next track of playlist
    #define RC_PREVIOUS                 0x6a            // command for previous track of playlist
    #define RC_FIRST                    0x6c            // command for first track of playlist
    #define RC_LAST                     0x6d            // command for last track of playlist
    #define RC_VOL_UP                   0x1a            // Command for volume up (one step)
    #define RC_VOL_DOWN                 0x1b            // Command for volume down (one step)
    #define RC_MUTE                     0x1c            // Command to mute ESPuino
    #define RC_SHUTDOWN                 0x2a            // Command for deepsleep
    #define RC_BLUETOOTH                0x72            // Command to enable/disable bluetooth
    #define RC_FTP                      0x65            // Command to enable FTP-server
#endif