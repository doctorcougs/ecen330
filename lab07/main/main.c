#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_timer.h"

#include "hw.h"
#include "draw.h"
#include "pin.h"
#include "config.h"
#include "game.h"
#include "joy.h"
#include "starting_screen.h"



static const char *TAG = "lab07";

int tick_counter;

#define PER_MS ((uint32_t)(CONFIG_GAME_TIMER_PERIOD*1000))
#define TIME_OUT 500 // ms

TimerHandle_t update_timer; // Declare timer handle for update callback

volatile bool interrupt_flag;

// Interrupt handler for game - use flag method
void update(TimerHandle_t xTimer) {
	interrupt_flag = true;
}

// Main application
void app_main(void)
{
	//////////////////////MAIN INIT////////////////////////
	// ISR flag
	interrupt_flag = false;

	// Initialization
	lcd_init();
	lcd_frameEnable();
	lcd_fillScreen(CONFIG_COLOR_BACKGROUND);
	lcd_setFontSize(FONT_SIZE);
	joy_init();
	game_init();
	starting_screen_init();

	// Configure I/O pins for buttons
	pin_reset(HW_BTN_A);
	pin_input(HW_BTN_A, true);
	pin_reset(HW_BTN_B);
	pin_input(HW_BTN_B, true);
	pin_reset(HW_BTN_MENU);
	pin_input(HW_BTN_MENU, true);
	pin_reset(HW_BTN_OPTION);
	pin_input(HW_BTN_OPTION, true);
	pin_reset(HW_BTN_SELECT);
	pin_input(HW_BTN_SELECT, true);
	pin_reset(HW_BTN_START);
	pin_input(HW_BTN_START, true);

	// Initialize update timer
	update_timer = xTimerCreate(
		"update_timer",        // Text name for the timer.
		pdMS_TO_TICKS(PER_MS), // The timer period in ticks.
		pdTRUE,                // Auto-reload the timer when it expires.
		NULL,                  // No need for a timer ID.
		update                 // Function called when timer expires.
	);
	if (update_timer == NULL) {
		ESP_LOGE(TAG, "Error creating update timer");
		return;
	}
	if (xTimerStart(update_timer, pdMS_TO_TICKS(TIME_OUT)) != pdPASS) {
		ESP_LOGE(TAG, "Error starting update timer");
		return;
	}

	main_state_t state = MAIN_IDLE;
	//////////////////////MAIN LOOP////////////////////////


	// main loop, while menu button not pressed do everything
	while(pin_get_level(HW_BTN_MENU)) {
		//loop for starting screen
		// TODO: turn into state machine
		while (!interrupt_flag) ;
		
		switch(state) {
			case MAIN_IDLE: {
				// printf("main_idle\n");
				starting_screen_tick(&state);
				break;
			}
			case MAIN_START: {
				// printf("main_start\n");
				game_tick(&state);
				break;
			}
			case MAIN_OPTIONS: {
				break;
			}
			case MAIN_QUIT: {
				break;
			}
		}

		tick_counter++;

		lcd_writeFrame();
		
		interrupt_flag = false;
	}
}
