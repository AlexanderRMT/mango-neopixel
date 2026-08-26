/*
 * This module provides the regular GPIO/assembly interface for driving Neopixel/WS2812 LEDs.
 * The interface is inspired by the Adafruit Neopixel library, but all of the
 * code here is written by me, Alexander Magdaleno.
 */


#ifndef NEOPIXEL_H
#define NEOPIXEL_H

#include "gl.h"
#include "gpio.h"


struct neopixel {
    gpio_id_t data_pin;
    unsigned int num_pixels;
    uint8_t brightness; // value from 0 - 255
    color_t* pixels;
    
};

typedef struct neopixel neopixel_t; // nickname 

neopixel_t* neo_new(gpio_id_t DATA_PIN, unsigned int NUM_PIXELS, uint8_t BRIGHTNESS);

void neo_set_pixel(neopixel_t* neopixel, unsigned int i, color_t c); // set LED at index i to color c, does nothing if i would go past the last pixel

void neo_set_brightness(neopixel_t* neopixel, uint8_t brightness);

void neo_show(neopixel_t* neopixel);

#endif
