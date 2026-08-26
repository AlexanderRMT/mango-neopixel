#include "neopixel.h"
#include "gl.h" // used for color definitions
#include "timer.h"


#define NUM_PIXELS 64
#define BRIGHTNESS 255
int main(void)
{
    neopixel_t* strip = neo_new(GPIO_PC0, NUM_PIXELS, BRIGHTNESS);
    neo_clear(strip);
    while (true)
    {
        for(int i = 0; i < NUM_PIXELS; i++)
        {
            neo_set_pixel(strip, i, GL_BLUE);
            neo_show(strip);
            timer_delay(1);
            neo_set_pixel(strip, i, 0); // clear pixel
        }
    }
    return 0;
}
