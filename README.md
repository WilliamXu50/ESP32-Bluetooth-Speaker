# ESP32-Bluetooth-Speaker
A Bluetooth speaker built from ESP32 microcontroller and PCM5102 I2S DAC

# Description
The ESP32 microcontroller will be configured as an A2DP sink to implement a Bluetooth protocol stack. It is setup with several Bluetooth profiles: the GAP profile, A2DP sink profile and AVRCP profile.

- A2DP profile will setup audio stream reception 
- AVRCP profile will setup an interface between the bluetooth controller (e.g. mobile phone) and the target (in this case would be ESP32) to allow media information notifications

The source code is taken from the original espressif-idf github repository (a2dp_sink example code). The entire main source code file is written in callback function manner to handle Bluetooth events (e.g. GAP events, A2DP events & AVRCP events), as well as multi-tasking using FreeRTOS to create parallel tasks to receive and transmit audio data. The PCM5102 IC is a digital-to-analog converter with an I2S interface. The ESP32 will receive audio data stream from a remote device via Bluetooth, converts the analog signal to digital bit-stream via its built in ADC, and then transmits that digital data stream to PCM5102 via I2S interface to be converted to analog audio signal.

The original source code doesn't have volume control: if I increased the volume on my mobile phone, the actual volume wouldn't change on the actual speaker output. I added some code to the original source file (`bt_app_av.c`) that will modify the PCM audio sample according to the volume change, before writing that data to the I2S DMA buffer to be transmitted to the PCM5102 IC.

# Requirements
This project requires you to install the `ESP-IDF` Framework and Toolchain for either Windows or Linux. Once installed, you can open the ESP-IDF CMD (ESP command prompt), navigate to the project directory and modify the original source code. 

