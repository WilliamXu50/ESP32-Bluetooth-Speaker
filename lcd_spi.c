#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "lcd_spi.h"

uint8_t base_addr_table[4]={LCD_FIRST_LINE_BASE_ADDR,
                            LCD_SECOND_LINE_BASE_ADDR,
                            LCD_THIRD_LINE_BASE_ADDR,
                            LCD_FOURTH_LINE_BASE_ADDR
 }

//Function for generating micro-seconds delay, taken from stack overflow
unsigned long IRAM_ATTR micros(){
    return (unsigned long) (esp_timer_get_time());
}

void IRAM_ATTR delay_us(uint32_t us){
    uint32_t m = micros();
    if(us){
        uint32_t e = (m + us);
        if(m > e){ //overflow
            while(micros() > e){
                asm volatile("nop");
            }
        }
        while(micros() < e){
            asm volatile("nop");
        }
    }
}

/*To send command or data over to the LCD, we will use polling method since the data
we're sending are pretty small. 
*/
void lcd_spi_send_byte(spi_device_handle_t spi_dev, spi_transaction_t* transaction){
    esp_err_t ret;
    //Send the transaction via polling
    ret=spi_device_polling_transmit(spi_dev, transaction);
    assert(ret==ESP_OK);
}

/* Send an 8-bit command byte to the LCD
 * Since commands sent to the LCD is automatically stored in the internal command register
 * we don't need to specify the address the command is to be stored.
 * Just pull the Register Select pin to low before transferring a byte and it will be sent 
 * to the command register of the LCD
 * Even the address of the DDRAM of the location where a character is to be displayed on
 * the screen is itself, a command
*/
void lcd_spi_send_cmd(spi_device_handle_t spi_dev, uint8_t cmd){
    gpio_set_level(PIN_NUM_RS, 0);          //RS=0 for command register
    
    spi_transaction_t cmd_trans;
    memset(&cmd_trans, 0, sizeof(spi_transaction_t));

    cmd_trans.length=LCD_DATA_LEN;
    cmd_trans.tx_buffer=&cmd;

    lcd_spi_send_byte(spi_dev, &cmd_trans);
}

/* Send one byte of data to LCD data register*/
void lcd_spi_send_data_byte(spi_device_handle_t spi_dev, uint8_t data){
    gpio_set_level(PIN_NUM_RS, 1);

    spi_transaction_t data_trans;

    memset(&data_trans, 0, sizeof(spi_transaction_t));

    data_trans.length=LCD_DATA_LEN;
    data_trans.tx_buffer=&data;

    lcd_spi_send_byte(spi_dev, &data_trans);
}

/* Send a data buffer ('data_buf') to the LCD. The data buffer corresponds to a string that
 * is to be displayed on the LCD. This function will send individual chars (individual
 * elements of the data buffer) in a for-loop until the entire buffer (length as
 * indicated by num_bytes) is completed.
 * 
 * To display a single character onto a specific position on the LCD, the data must be sent
 * to the corresponding DDRAM address of that position. To send a single character to be displayed
 * on the LCD in a specific location, we first set the DDRAM address of the position by sending
 * the desired address as a command byte. Then follows the actual data (the character itself) in the
 * next transfer.
 * 
 * To send a string to the LCD, we only need to set the starting address of DDRAM, as the internal 
 * address counter of the LCD is automatically incremented to the next position on the display 
 * after a successful write operation (this is configured in 'Set Entry Mode' command).
 * 
 * To send continuous SPI transaction from ESP32 to the LCD, we can keep the CS pin low after
 * each transfer. To do this, we check if there are more then 1 byte of data left to be transferred;
 * if there are more than 1 byte, we can add the 'SPI_TRANS_CS_KEEP_ACTIVE' flag to our transaction
 * struct for that transfer. This means to keep the CS pin low after finishing that particular transction 
*/
void lcd_spi_send_string(spi_device_handle_t spi_dev, char* data_buf, int num_bytes){
    gpio_set_level(PIN_NUM_RS, 1);          //RS=1 for data register

    spi_transaction_t data_trans;

    //Send each character of the buffer sequentially
    for(int i=0; i<num_bytes; i++){
        memset(&data_trans, 0, sizeof(spi_transaction_t));

        data_trans.length=LCD_DATA_LEN;
        data_trans.tx_buffer=&(data_buf[i]);

        if(i<(num_bytes-1)){
            //If there are still more than 1 byte of data left,
            //keep CS pin low after the transfer
            data_trans.flags=SPI_TRANS_CS_KEEP_ACTIVE;
        }

        lcd_spi_send_byte(spi_dev, &data_trans);
        delay_us(19);
    }
}

/* Alternative function for sending a string to the LCD starting at a specific location
 * Instead of performing 2 operations (first to send the DDRAM address as a command byte, followed by 
 * sending the data buffer), this function combines the 2 operations. 
 * 
 * This function takes in a 'row' argument (specifying the row location on the display) and a 'column'
 * argument (specifying the column location on the display). 
 *
 * The DDRAM address would then be decoded from the row and column, and sent to the LCD as a command.
 *
*/
void lcd_spi_send_string_at_location(spi_device_handle_t spi_dev, char* data_buf, int num_bytes, int row, int column){
    uint8_t ddram_addr=0;
    int err=0;

    if(row >= 1 && row <= 4){
        ddram_addr=base_addr_table[row-1];
    }
    else{
        printf("Invalid row!\n");
        return;
    }

    if(column>=1 && column <=20){
        ddram_addr+=(column-1);
    }
    else{
        printf("Invalid column!\n");
        return;
    }

    ddram_addr|=0x80;

    printf("ddram addr = 0x%02x\n", ddram_addr);

    lcd_spi_send_cmd(spi_dev, ddram_addr);
    lcd_spi_send_string(spi_dev, data_buf, num_bytes);
}
