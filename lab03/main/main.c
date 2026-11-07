#include <driver/gptimer.h>
#include <hw.h>
#include <lcd.h>
#include <pin.h>
#include <watch.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "stdio.h"

static const char *TAG = "lab03";
// ISR volatile parameters
volatile uint64_t timer_ticks;
volatile bool running;
volatile int64_t isr_max = 0; // maximum ISR execution time (us)
volatile int32_t isr_cnt = 0; // Count of ISR invocations
volatile int64_t isr_start, isr_finish, isr_update_time;

// macro for passing in function name as string
#define time_section_macro(func) time_section(func, #func)

//timer defines
#define TIMERRESOLUTION 1 * 1000 * 1000 // 1MHz resolution, 1 tick equals 1 microsecond
#define ALARMCOUNT 10000 // 10ms alarm
#define ISRMAXCNT 500 // how long between ISR timer prints

// takes in a function and then prints out the amount of time that function took
void time_section(void (*func_ptr)(void), const char *func_name) {
    int64_t start, finish;

    start = esp_timer_get_time();
    func_ptr();
    finish = esp_timer_get_time();
    printf("Section [%s] time:%lld microseconds\n", func_name, finish-start);
}

// alarm callback that checks button presses and increments our timer_ticks variable
static bool alarm_event_callback1(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_ctx) {   
    isr_start = esp_timer_get_time();
    // Check button values
    if (!pin_get_level(HW_BTN_A)) {
        running = true;
    } else if (!pin_get_level(HW_BTN_B)) {
        running = false;
    } else if (!pin_get_level(HW_BTN_START)) {
        running = false;
        timer_ticks = 0;
    }
    // if we're running we have to increment
    if (running) {
        timer_ticks++;
    }
    // calculates the update time based on the start and finish of the isr
    isr_finish = esp_timer_get_time();
    isr_update_time = isr_finish - isr_start;
    // saves isr_max time
    if (isr_update_time > isr_max) {
        isr_max = isr_update_time;
    }
    isr_cnt++;
    return false;
} 

// pin startup for buttons
void pin_startup(void) {
    //reset and set buttons as inputs
    pin_reset(HW_BTN_A);
    pin_reset(HW_BTN_B);
    pin_reset(HW_BTN_START);
    pin_input(HW_BTN_A, true);
    pin_input(HW_BTN_B, true);
    pin_input(HW_BTN_START, true);
}

// all necessary initializations to get our timer ready
void timer_startup(void) {
    // create new timer instance
    gptimer_handle_t gptimer = NULL;
    gptimer_config_t timer_config = 
    {
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,
        .direction = GPTIMER_COUNT_UP,
        .resolution_hz = TIMERRESOLUTION,
    };
    ESP_ERROR_CHECK(gptimer_new_timer(&timer_config, &gptimer));

    // create an alarm
    gptimer_alarm_config_t alarm_config = 
    {
        .reload_count = 0,
        .alarm_count = ALARMCOUNT,
        .flags.auto_reload_on_alarm = true, // auto reload on alarm
    };
    // Sets our alarm action to be the above alarm
    ESP_ERROR_CHECK(gptimer_set_alarm_action(gptimer, &alarm_config));

    // callback function for the alarm event
    gptimer_event_callbacks_t cbs = 
    {
        .on_alarm = alarm_event_callback1, // function to call on alarm
    };
    ESP_ERROR_CHECK(gptimer_register_event_callbacks(gptimer, &cbs, NULL));

    // Enable and start the timer
    ESP_ERROR_CHECK(gptimer_enable(gptimer));
    ESP_ERROR_CHECK(gptimer_start(gptimer));
}

// Log stopwatch update
void stopwatch_log(void) {
    ESP_LOGI(TAG, "Stopwatch update");
}

void app_main(void) {
    ESP_LOGI(TAG, "Starting");

    // initialize pins for buttons
    time_section_macro(pin_startup);

    // initialize timer
    time_section_macro(timer_startup);

    // Logs stopwatch update
    time_section_macro(stopwatch_log);

    lcd_init(); // Initialize LCD display
    watch_init(); // Initialize stopwatch face
    for (;;) 
    { // forever update loop
        watch_update(timer_ticks);
        // prints isr_max time every 5 seconds
        if (isr_cnt >= ISRMAXCNT) {
            printf("Maximum isr time:%lld microseconds\n", isr_max);
            isr_cnt = 0;
            isr_max = 0;
        }
    }
}