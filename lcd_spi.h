#ifndef _LCD_SPI_H_
#define _LCD_SPI_H_

#include "driver/spi_master.h"
#include <stdint.h>

#define LCD_HOST    HSPI_HOST
#define LCD_DATA_LEN        8
#define LCD_DSP_LINE_MAX_CHAR    20

#define PIN_NUM_MOSI        13
#define PIN_NUM_SCK         14
#define PIN_NUM_CS          15
#define PIN_NUM_RS          5

#define LCD_FIRST_LINE_BASE_ADDR        0x00
#define LCD_SECOND_LINE_BASE_ADDR       0x40
#define LCD_THIRD_LINE_BASE_ADDR        0x14
#define LCD_FOURTH_LINE_BASE_ADDR       0x54

/* Each char array represents a line on the LCD display
* Since each line has a max of 20 characters, each array will hold a max of 20 chars
*
* I made the size of each array = 21 here so that it'll be easier to use sprintf() to store a 
* formatted string into the char arrays. This is because sprintf() also attachs a null-terminator
* at the end of the formatted string. In the case that all 20 elements of the char arrays are 
* filled witl chars, the 21st element will hold the null-terminator
*/
char lcd_dsp_line1[LCD_DSP_LINE_MAX_CHAR+1];
char lcd_dsp_line2[LCD_DSP_LINE_MAX_CHAR+1];
char lcd_dsp_line3[LCD_DSP_LINE_MAX_CHAR+1];
char lcd_dsp_line4[LCD_DSP_LINE_MAX_CHAR+1];

/* Device handle for the LCD */
spi_device_handle_t lcd_spi_handle;

/* Creates a variable microsecond delay. This function is taken from stack overflow */
void IRAM_ATTR delay_us(uint32_t us);

/* Sends one byte to the SPI slave device, as specified by the spi_transaction_t* transaction struct
* This function uses the traditional spi_device_polling_transmit() as opposed to the interrupt method
*/
void lcd_spi_send_byte(spi_device_handle_t spi_dev, spi_transaction_t* transaction);

/* Sends a command byte */
void lcd_spi_send_cmd(spi_device_handle_t spi_dev, uint8_t cmd);

/* Sends a data byte*/
void lcd_spi_send_data_byte(spi_device_handle_t spi_dev, uint8_t data);

/* Sends an entire string to the LCD */
void lcd_spi_send_string(spi_device_handle_t spi_dev, char* data_buf, int num_bytes);

/* Alternative to the above function.
* Instead of sending the DDRAM address as a command, followed by the actual data stream (2 operations),
* This function combines the two.
*/
void lcd_spi_send_string_at_location(spi_device_handle_t spi_dev, char* data_buf, int num_bytes, int row, int column);

#endif