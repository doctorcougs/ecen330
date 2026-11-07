#include <stdlib.h>
#include <math.h>
#include "tone.h"
#include "sound.h"

//defines for BIAS and PI
#define BIAS 0x80U
#define PI (acosf(0.0) * 2)
// have to define three to use to multiply for third quarter
#define UINT8_MAX 0xFF

// global waveform buffer and static sample_rate
uint8_t *waveform_buffer = NULL;
static uint32_t sample_rate = 0;


// initializes the tone and makes a waveform buffer
int32_t tone_init(uint32_t sample_hz) {
    // checks to make sure sample_hz is a good value
    if (sample_hz  < 2 * LOWEST_FREQ) {
        return 1;
    }
    // checks to see if the sound_init returns a 1
    if (sound_init(sample_hz)) {
        return 1;
    }
    // frees our waveform buffer if we already have one
    if (waveform_buffer != NULL) {
        free(waveform_buffer);
        waveform_buffer = NULL;
    }
    // makes a new waveform buffer
    if (waveform_buffer == NULL) {
        // malloc new space for waveform_buffer
        waveform_buffer = malloc(sizeof(uint8_t) * (sample_hz / LOWEST_FREQ));
    }
    //set sample rate
    sample_rate = sample_hz;
    return 0;
}

// frees the space from waveform_buffer
int32_t tone_deinit(void) {
    // if branch to free space
    if (waveform_buffer != NULL) {
        // freeing space
        free(waveform_buffer);
        waveform_buffer = NULL;
    }
    //checks sound_deinit and passes it on
    if (sound_deinit()) {
        return 1;
    }
    return 0;
}

// makes a tone depending on wave type and frequency
void tone_start(tone_t tone, uint32_t freq) {
    // error checking
    if (waveform_buffer == NULL) {
        return;
    }
    if (tone < SINE_T || tone > LAST_T){
        return;
    }
    // makes sure frequency will be a playable one
    if (freq < LOWEST_FREQ || freq > sample_rate / 2) {
        return;
    }
    // setting up some variables that will be used repeatedly
    uint32_t num_samples = sample_rate / freq;
    uint32_t half_num_samples = num_samples / 2;  
    switch(tone) {
        // all cases for the switch statement
        case SINE_T: {
            // for each sample we will add that uint8 to our buffer
            for (uint32_t i = 0; i < num_samples; i++) {
                //find specific angle for the sine wave
                float angle = 2.f * PI * i / num_samples;
                float waveform_pos = sinf(angle);
                //convert to a uint8_t while shifting for bias
                uint8_t converted_waveform_pos = (uint8_t)(BIAS + waveform_pos * (BIAS - 1));
                waveform_buffer[i] = converted_waveform_pos;
            }
            break;
        }
        case SQUARE_T: {
            // for the first half of the buffer put in the max value, for the second half put the min
            for (uint32_t i = 0; i < num_samples; i ++) {
                // if statement for logic of up or down on square wave
                if (i < half_num_samples) {
                    waveform_buffer[i] = UINT8_MAX;
                } else {
                    waveform_buffer[i] = 0x00;
                }
            }
            break;
        }
        case TRIANGLE_T: {
            // need these extra variables to calculate the correct triangle slope
            uint32_t quarter = half_num_samples / 2;
            uint32_t three_quarter = quarter + half_num_samples;
            // cycles through all samples
            for (uint32_t i = 0; i < num_samples; i++) {
                float waveform_pos;
                //for the first quarter we are going up 
                if (i < quarter) {
                    waveform_pos = (float)i / quarter;
                }
                // for the second and third quarters we are going negative  
                else if (i < three_quarter) {
                    // starting from the top and minusing depending on how far we are
                    waveform_pos = 1.0f - 2.0f * (float)((i - quarter) / (2 * quarter));
                } 
                // for the last quarter we are going down
                else {
                    //starting from the bottom and going up depending on how far we are
                    waveform_pos = -1.0f + (float)(i - three_quarter) / (num_samples - three_quarter);
                }
                // convert while shifting for bias
                uint8_t converted_waveform_pos = (uint8_t)(BIAS + waveform_pos * (BIAS - 1));
                waveform_buffer[i] = converted_waveform_pos;
            }
            break;
        }
        case SAW_T: {
            for (uint32_t i = 0; i < num_samples; i++) {
                float waveform_pos;
                // if we are less than half num samples just draw an up line
                if (i < half_num_samples) {
                    waveform_pos = (float)i / half_num_samples;
                } 
                // if not below halfnum samples just start from -1 and then increment up
                else {
                    waveform_pos = -1.0f + (float)(i - half_num_samples) / (num_samples - half_num_samples);
                }
                //shift for bias and convert
                uint8_t converted_waveform_pos = (uint8_t)(BIAS + waveform_pos * (BIAS - 1));
                waveform_buffer[i] = converted_waveform_pos;
            }
            break;
        }
        case LAST_T: {
            break;
        }
    }
    //call the sound function that will cycle through our buffer
    sound_cyclic(waveform_buffer, num_samples * sizeof(uint8_t));

}
