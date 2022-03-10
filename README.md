# ESP32-Bluetooth-Speaker
A Bluetooth speaker built from ESP32 microcontroller and PCM5102 I2S DAC

# Description
The ESP32 microcontroller will be configured as an A2DP sink to implement a Bluetooth protocol stack. It is setup with several Bluetooth profiles: the GAP profile, A2DP sink profile and AVRCP profile.

- A2DP profile will setup audio stream reception 
- AVRCP profile will setup an interface between the bluetooth controller (e.g. mobile phone) and the target (in this case would be ESP32) to allow media information notifications

The source code is taken from the original espressif-idf github repository (a2dp_sink example code). The entire main source code file is written in callback function manner to handle Bluetooth events (e.g. GAP events, A2DP events & AVRCP events), as well as multi-tasking using FreeRTOS to create parallel tasks to receive and transmit audio data. The PCM5102 IC is a digital-to-analog converter with an I2S interface. The ESP32 will receive audio data stream from a remote device via Bluetooth, converts the analog signal to digital bit-stream via its built in ADC, and then transmits that digital data stream to PCM5102 via I2S interface to be converted to analog audio signal.

# Additions 
- Volume control: added code so that a change in the mobile device's volume will reflect changes in the actual volume output at the speaker
- LCD Display: added a LCD display (AMC2004AR-B-Y6WFDY-SPI) to the build and included a driver source code. The LCD will display the following information: Status of the bluetooth speaker, the name of the bluetooth speaker and the name of the song that's currently playing. If the song name is unavailable, it'll just display "unavailable".

# Wiring
Interface between ESP32 and PCM5102:
| ESP32         | PCM5102     |
|---------------|-------------|
|GPIO 22: L/R CK|     LCK     |
|GPIO 26: BCK   |     BCK     |
|GPIO 25: Data  |     DIN     |
|      5V       |     VIN     |
|      GND      |  GND & SCK  |

Interface between ESP32 and AMC2004AR-B-Y6WFDY-SPI:
| ESP32         | LCD           |
|---------------|---------------|
|GPIO 13: MOSI  |   Pin 6       |
|GPIO 14: SCK   |   Pin 5       |
|GPIO 15: CS    |   Pin 8       |
|GPIO 5: RS     |   Pin 9       |
|      5V       | Pin 1 & Pin 4 |
|      GND      | Pin 2 & Pin 3 |

# Requirements
This project requires you to install the `ESP-IDF` Framework and Toolchain for either Windows or Linux. Once installed, you can open the ESP-IDF CMD (ESP command prompt), navigate to the project directory and modify the original source code. 

# Future Work
- Use SNTP (Simeple Network Time Protocol) to display the current time onto the LCD
- Add a LCD line shift function: if the song name is too long to fit into a LCD line, it will be shifted across the screenat a certain rate as to display the entire title.
