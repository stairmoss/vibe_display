#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_spiffs.h" // For your Animation folder

// #notiplay #vibe-coding #esp32 #zentalic
static const char *TAG = "notiplay";

// --- Hardware Pins ---
#define BTN_MODE    34
#define BTN_ACTION  35
#define BTN_EXTRA   32
#define LCD_DC      21
#define LCD_CS      22

// --- App States ---
typedef enum {
    MODE_SPOTIFY,
    MODE_ANIMATION,
    MODE_TIME,
    MODE_HACKATIME, // Your Wakatime/Hackatime leaderboard
    MODE_COUNT
} device_mode_t;

device_mode_t current_mode = MODE_SPOTIFY;
int animation_frame = 0;

// This is a "stub" - in a real app, you'd update these via Wi-Fi
char current_song[32] = "Loading Spotify...";
char current_artist[32] = "Please wait";

// --- Human-friendly functions ---

void handle_buttons() {
    // Switch Modes
    if (gpio_get_level(BTN_MODE) == 0) {
        current_mode = (current_mode + 1) % MODE_COUNT;
        ESP_LOGI(TAG, "Switched to mode: %d", current_mode);
        vTaskDelay(pdMS_TO_TICKS(300)); // Debounce
    }
    
    // Action Button (Next Song or Next Animation)
    if (gpio_get_level(BTN_ACTION) == 0) {
        if (current_mode == MODE_ANIMATION) {
            animation_frame = 0; // Reset or Skip
            ESP_LOGI(TAG, "Next Animation triggered");
        }
        vTaskDelay(pdMS_TO_TICKS(300));
    }
}

void draw_spotify_ui() {
    // #spotify #nowplaying
    // Logic: Draw a small square on the left for the thumbnail
    // Draw text on the right
    // Note: To show real thumbnails, you'll need the 'esp_jpeg' component
    printf("[UI] Showing: %s by %s\n", current_song, current_artist);
}

void draw_animation_mode() {
    // #vibes #gif
    // Logic: Read from your /Animation folder
    // For now, we simulate a frame tick
    animation_frame++;
    printf("[UI] Playing Animation Frame: %d\n", animation_frame);
}

void draw_time_mode() {
    // #clock #activity
    printf("[UI] 10:45 PM | Activity: Coding OREO V2\n");
}

void draw_leaderboard() {
    // #hackatime #coding-stats
    printf("[UI] 1. Adarsh: 12hrs | 2. Ghost: 10hrs\n");
}

void app_main(void) {
    // Setup Buttons
    gpio_config_t btn_config = {
        .pin_bit_mask = (1ULL << BTN_MODE) | (1ULL << BTN_ACTION) | (1ULL << BTN_EXTRA),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE
    };
    gpio_config(&btn_config);

    // Initializing SPIFFS for your "Animation" folder
    esp_vfs_spiffs_conf_t conf = {
      .base_path = "/spiffs",
      .partition_label = NULL,
      .max_files = 5,
      .format_if_mount_failed = true
    };
    esp_vfs_spiffs_register(&conf);

    ESP_LOGI(TAG, "Zentalic Display System Started!");

    while (1) {
        handle_buttons();

        // Refresh screen based on mode
        switch (current_mode) {
            case MODE_SPOTIFY:   draw_spotify_ui(); break;
            case MODE_ANIMATION: draw_animation_mode(); break;
            case MODE_TIME:      draw_time_mode(); break;
            case MODE_HACKATIME: draw_leaderboard(); break;
            default: break;
        }

        // Keep it smooth but don't melt the CPU
        vTaskDelay(pdMS_TO_TICKS(100)); 
    }
}