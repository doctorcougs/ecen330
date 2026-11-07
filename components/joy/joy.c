#include "esp_adc/adc_oneshot.h"
#include "joy.h"
#include <stdio.h>
#include <stdint.h>

// ADC channel defines and num samples 
#define ADC1_CHAN6 ADC_CHANNEL_6
#define ADC1_CHAN7 ADC_CHANNEL_7
#define ADC1_ATTEN ADC_ATTEN_DB_12
#define NUM_SAMPLES 10


// two channels on adc1
// first channel is for x second channel is for y
static int_fast32_t adc_raw[2];
static int32_t joystick_center[2];
static const adc_channel_t xy_channels[2] = { ADC1_CHAN6, ADC1_CHAN7};

adc_oneshot_unit_handle_t adc1_handle = NULL;

// initialize the joystick
int32_t joy_init(void) {
    // return 0 if already initialized
    if (adc1_handle != NULL) {
        return 0;
    }

    // Creates an ADC in normal oneshot mode
    adc_oneshot_unit_init_cfg_t init_config1 = {
        .unit_id = ADC_UNIT_1,
        .ulp_mode = ADC_ULP_MODE_DISABLE,
    };
    // wrap in error check so we are safe
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config1, &adc1_handle));

    // return 1 if not initialized
    // ESP_ERROR_CHECK should just actually crash and reboot the esp
    // but if it doesn't this will handle it
    if (adc1_handle == NULL) {
        return 1;
    }

    // setting the config
    adc_oneshot_chan_cfg_t config = {
        .bitwidth = ADC_BITWIDTH_DEFAULT,
        .atten = ADC1_ATTEN,
    };
    //wrap in error check so we are safe
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, ADC1_CHAN6, &config));
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, ADC1_CHAN7, &config));

    // calibrate the center joystick position based on ADC read
    for (int8_t i = 0; i < 2; i++) {
        int32_t total_offset = 0;
        // checks for each channel and gets our offset values
        for (int8_t j = 0; j < NUM_SAMPLES; j++) {
            ESP_ERROR_CHECK(adc_oneshot_read(adc1_handle, xy_channels[i], &adc_raw[i]));
            total_offset += adc_raw[i];
        }
        // move the joystick for calibration
        joystick_center[i] = total_offset / NUM_SAMPLES;
    }
    return 0;
}

// deinitialize the joystick and free space
int32_t joy_deinit(void) {
    // delete an adc
    if (adc1_handle != NULL) {
        ESP_ERROR_CHECK(adc_oneshot_del_unit(adc1_handle));
        adc1_handle = NULL;
    }
    // if it is still not NULL return 1 otherwise return 0
    if (adc1_handle != NULL) {
        return 1;
    }
    return 0;
}

// gets the displacement from each channel and then compares to our configured center
void joy_get_displacement(int32_t *dcx, int32_t *dcy) {
    // for loop handles both adc's
    for (int8_t i = 0; i < 2; i++) {
        ESP_ERROR_CHECK(adc_oneshot_read(adc1_handle, xy_channels[i], &adc_raw[i]));
    }
    // store joystick positions
    *dcx = adc_raw[0] - joystick_center[0];
    *dcy = adc_raw[1] - joystick_center[1];
}
    