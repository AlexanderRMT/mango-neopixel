 /*
 * Neopixel/WS2812 driver
 *
 * Alexander Magdaleno
 *
 * The bit-level send path is adapted from sample code by Julie Zelenski, Feb 2024.
 * send_bit is hers verbatim (assembly, see neo_timing.s); send_byte and
 * send_pixel_color follow her structure, extended by me to take the target pin's
 * data register and to apply brightness scaling. Everything else here is mine.
 */
#include "gl.h"
#include "timer.h"
#include "uart.h"
#include "malloc.h"
#include "neopixel.h"

// Taken from internal gpio.c functions to allow for any pin to be used as the LED data output pin without having to internally change memory addresses or bit masks
// These functions are not available to the caller

#define LATCH_DELAY 55 // in micro seconds
#define MAX_BRIGHTNESS 255

enum { GROUP_B = 0, GROUP_C, GROUP_D, GROUP_E, GROUP_F, GROUP_G };

typedef struct  {
    unsigned int group;
    unsigned int pin_index;
} gpio_pin_t;

static gpio_pin_t get_group_and_index(gpio_id_t gpio) {
    gpio_pin_t gp;
    gp.group = gpio >> 8;
    gp.pin_index = gpio & 0xff; // lower 2 hex digits
    return gp;
}

static volatile unsigned int *get_data_reg(unsigned int group) {
    unsigned int *base = (unsigned int *)0x02000010; // base gpio address + 16 since dat is 16 from cfg
    unsigned int offset = 12 * (group + 1); // The distance between between pin group dat is 0x30 = 48. unsigned int is scaled by 4 so we want to add 12 between each pin
    return base + offset;
}

// End of gpio.c functions

// Julie Zelenski
// this function written in assembly for precise timing, see neo_timing.s
void send_bit(volatile unsigned int *data, unsigned int data_bit_to_flip, unsigned int zero_or_one);

// Julie Zelenski; extended by me to take the pin's data register and bit mask
static void send_byte(uint8_t byte, volatile unsigned int* data, unsigned int data_bit_to_flip) {
    for (uint8_t mask = 1 << 7; mask != 0 ; mask >>= 1 ) {
        send_bit(data, data_bit_to_flip, (byte & mask) != 0);
    }
    // inter-bit gap not v sensitive, but must not exceed latch delay (see below)    
}

// Julie Zelenski; extended by me to take the pin's data register and bit mask,
// and to scale each channel by brightness
static void send_pixel_color(color_t c, volatile unsigned int* data, unsigned int data_bit_to_flip, uint8_t brightness)   {
    // neopixel bytes sent in order G-R-B
    uint8_t G = c >> 8;
    uint8_t R = c >> 16;
    uint8_t B = c;
    send_byte((brightness * G) / MAX_BRIGHTNESS, data, data_bit_to_flip);  // G 
    send_byte((brightness * R) / MAX_BRIGHTNESS, data, data_bit_to_flip); // R
    send_byte((brightness * B) / MAX_BRIGHTNESS, data, data_bit_to_flip);       // B
    // inter-byte gap not v sensitive, but must not exceed latch delay (see below)    
}

neopixel_t* neo_new(gpio_id_t data_pin, unsigned int num_pixels, uint8_t brightness) {
    neopixel_t* neopixel = (neopixel_t*)malloc(sizeof(neopixel_t));
    neopixel->data_pin = data_pin;
    neopixel->num_pixels = num_pixels;
    neopixel->brightness = brightness;
    neopixel->pixels = (color_t*)malloc(sizeof(color_t) * num_pixels);
    return neopixel;
}

void neo_set_pixel(neopixel_t* neopixel, unsigned int i, color_t c) {
    if (i >= neopixel->num_pixels) {
        return;
    }
    neopixel->pixels[i] = c;
}

void neo_set_brightness(neopixel_t* neopixel, uint8_t brightness) {
    neopixel->brightness = brightness;
}

void neo_show(neopixel_t* neopixel) {
    unsigned int n = 0;
    gpio_set_output(neopixel->data_pin);
    gpio_pin_t gp = get_group_and_index(neopixel->data_pin);
    volatile unsigned int* data = get_data_reg(gp.group); // memory address of pin
    unsigned int data_bit_to_flip = 1 << (gp.pin_index);
    while (n < neopixel->num_pixels) {
        send_pixel_color(neopixel->pixels[n], data, data_bit_to_flip, neopixel->brightness);
        n++;
    }
    timer_delay_us(LATCH_DELAY);
}
