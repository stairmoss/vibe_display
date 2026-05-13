#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "esp_log.h"

// --- Pin Definitions 
#define PIN_NUM_MISO -1 // Not used for LCD
#define PIN_NUM_MOSI 23
#define PIN_NUM_CLK  19
#define PIN_NUM_CS   22
#define PIN_NUM_DC   21
#define PIN_NUM_RST  18
#define PIN_NUM_BCKL 5

// LCD Dimensions
#define LCD_WIDTH  128
#define LCD_HEIGHT 160

static const char *TAG = "notiplay";

// Simple SPI Send Command
void lcd_cmd(spi_device_handle_t spi, const uint8_t cmd) {
    esp_err_t ret;
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));
    t.length = 8;
    t.tx_buffer = &cmd;
    t.user = (void*)0; // DC pin low for command
    ret = spi_device_polling_transmit(spi, &t);
    assert(ret == ESP_OK);
}

// Simple SPI Send Data
void lcd_data(spi_device_handle_t spi, const uint8_t *data, int len) {
    if (len == 0) return;
    esp_err_t ret;
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));
    t.length = len * 8;
    t.tx_buffer = data;
    t.user = (void*)1; // DC pin high for data
    ret = spi_device_polling_transmit(spi, &t);
    assert(ret == ESP_OK);
}

// Callback to handle the DC  pin toggle
void lcd_spi_pre_transfer_callback(spi_transaction_t *t) {
    int dc = (int)t->user;
    gpio_set_level(PIN_NUM_DC, dc);
}

// Basic ST7735 Initialization
void lcd_init(spi_device_handle_t spi) {
    gpio_set_level(PIN_NUM_RST, 0);
    vTaskDelay(pdMS_TO_TICKS(100));
    gpio_set_level(PIN_NUM_RST, 1);
    vTaskDelay(pdMS_TO_TICKS(100));

    lcd_cmd(spi, 0x01); // Software Reset
    vTaskDelay(pdMS_TO_TICKS(150));
    lcd_cmd(spi, 0x11); // Sleep Out
    vTaskDelay(pdMS_TO_TICKS(500));
    
    lcd_cmd(spi, 0x3A); // Interface Pixel Format
    uint8_t format = 0x05; // 16-bit color
    lcd_data(spi, &format, 1);

    lcd_cmd(spi, 0x29); // Display ON
}

// Fill screen with a solid color (e.g., for "Animations" or Backgrounds)
void fill_screen(spi_device_handle_t spi, uint16_t color) {
    uint8_t data[2];
    data[0] = color >> 8;
    data[1] = color & 0xFF;

    // Set Window to full screen
    lcd_cmd(spi, 0x2A); // Column Address Set
    uint8_t caset[] = {0x00, 0x00, 0x00, LCD_WIDTH - 1};
    lcd_data(spi, caset, 4);

    lcd_cmd(spi, 0x2B); // Row Address Set
    uint8_t raset[] = {0x00, 0x00, 0x00, LCD_HEIGHT - 1};
    lcd_data(spi, raset, 4);

    lcd_cmd(spi, 0x2C); // Memory Write
    for (int i = 0; i < LCD_WIDTH * LCD_HEIGHT; i++) {
        lcd_data(spi, data, 2);
    }
}

void app_main(void) {
    // 1. Setup GPIOs
    gpio_set_direction(PIN_NUM_DC, GPIO_MODE_OUTPUT);
    gpio_set_direction(PIN_NUM_RST, GPIO_MODE_OUTPUT);
    gpio_set_direction(PIN_NUM_BCKL, GPIO_MODE_OUTPUT);
    gpio_set_level(PIN_NUM_BCKL, 1); // Turn on Backlight

    // 2. Setup SPI Bus
    spi_bus_config_t buscfg = {
        .miso_io_num = PIN_NUM_MISO,
        .mosi_io_num = PIN_NUM_MOSI,
        .sclk_io_num = PIN_NUM_CLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = LCD_WIDTH * LCD_HEIGHT * 2
    };

    spi_device_interface_config_t devcfg = {
        .clock_speed_hz = 26 * 1000 * 1000, // 26MHz
        .mode = 0,
        .spics_io_num = PIN_NUM_CS,
        .queue_size = 7,
        .pre_cb = lcd_spi_pre_transfer_callback,
    };

    spi_bus_initialize(HSPI_HOST, &buscfg, 1);
    spi_device_handle_t spi;
    spi_bus_add_device(HSPI_HOST, &devcfg, &spi);

    // 3. Init LCD
    lcd_init(spi);
    ESP_LOGI(TAG, "Display Initialized!");

    // 4. Main "Vibe" Loop
    while (1) {
        // Mocking the data update
        printf("Updating Display: Spotify: 'Moonlight', Temp: 28C, Stocks: +2%%\n");

        // Flash "Animation" effect
        fill_screen(spi, 0xF800); // Red
        vTaskDelay(pdMS_TO_TICKS(500));
        fill_screen(spi, 0x07E0); // Green
        vTaskDelay(pdMS_TO_TICKS(500));
        fill_screen(spi, 0x001F); // Blue
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}