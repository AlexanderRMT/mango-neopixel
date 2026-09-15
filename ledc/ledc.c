//
// LEDC backend. Created by alexa on 8/22/2026.
//

#include "ledc.h"
#include <gpio.h>

#define REG(addr) (*(volatile unsigned int*)(addr)) // macro for ptr cast + dereference

// LEDC addresses
#define LEDC_CTRL_REG REG(0x02008000)
#define LED_T01_TIMING_CTRL_REG REG(0x02008004)
#define LEDC_DMA_CTRL_REG REG(0x02008018)

// CCU addresses
#define LEDC_CLK_REG REG(0x02001BF0)
#define LEDC_BUS_GATING_RST_REG REG(0x02001BFC)
#define DMA_BGR_REG REG(0x0200170C)
#define MBUS_MAT_CLK_GATING_REG REG(0x02001804)

static void config_ccu_ledc(void)
{
    LEDC_BUS_GATING_RST_REG = 1u; // pass gate
    LEDC_CLK_REG = 1u << 31; // CLK on, HOSC 24Mhz by default
    (void)LEDC_CLK_REG; // read back to ensure clk on
    LEDC_BUS_GATING_RST_REG |= (1u << 16); // De-assert reset
}

static void config_ccu_dma(void)
{
    DMA_BGR_REG = 1u; // pass DMA bus clock
    DMA_BGR_REG |= (1u << 16); // deassert reset
    (void)DMA_BGR_REG;
    MBUS_MAT_CLK_GATING_REG |= 1u;
    (void)MBUS_MAT_CLK_GATING_REG;
}

// function configures GPIO_PC0, D1-H technically also supports LEDC on PE5, PF2, PG13, PG16, PG13 is used for I2C on the board but the rest
// are unavailable on the Mango-Pi board
static void config_pin(void)
{
    gpio_set_function(GPIO_PC0, GPIO_FN_ALT4);
    // gpio_set_function(GPIO_PE5, GPIO_FN_ALT5);
    // gpio_set_function(GPIO_PF2, GPIO_FN_ALT5);
    // gpio_set_function(GPIO_PG13, GPIO_FN_ALT6);
    // gpio_set_function(GPIO_PG16, GPIO_FN_ALT7);
}

static void ledc_soft_reset(void)
{
    LEDC_CTRL_REG |= 1u << 1; // set LED_SOFT_RESET
    while (((LEDC_CTRL_REG >> 1u) & 1u) != 0)
    {
    } // read LED_SOFT_RESET until bit is set to 0
}

static void ledc_configure_protocol(void)
{
    // set timings, resulting time is 42 ns * (value_decimal + 1), the default values are adequate but this sequence is provided for any necessary changes
    // timings are first cleared, then set

    unsigned int t = LED_T01_TIMING_CTRL_REG; // save reg state for editing
    t = (t & ~0b111111u) | 0b010011u; // T0L
    t = (t & ~(0b011111u << 6)) | (0b000111u << 6); // T0H
    t = (t & ~(0b011111u << 16)) | (0b000110u << 16); // T1L
    t = (t & ~(0b111111u << 21)) | (0b010100u << 21); // T1H
    LED_T01_TIMING_CTRL_REG = t; // save configured value to reg

    // config data format, default is GRB, MSB first
}
