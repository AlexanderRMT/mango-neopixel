//
// LEDC backend. Created by alexa on 8/22/2026.
//

#include "ledc.h"
#include <gpio.h>

// LEDC addresses
#define LEDC_CTRL_REG 0x02008000
#define LED_T01_TIMING_CTRL_REG 0x02008004
#define LEDC_DMA_CTRL_REG 0x02008018

// CCU addresses
#define LEDC_CLK_REG 0x02001BF0
#define LEDC_BUS_GATING_RST_REG 0x02001BFC
#define DMA_BGR_REG 0x0200170C
#define MBUS_MAT_CLK_GATING_REG 0x02001804

static void config_ccu_ledc(void)
{
    *(volatile unsigned int*)LEDC_BUS_GATING_RST_REG = 1u; // pass gate
    *(volatile unsigned int*)LEDC_CLK_REG = 1u << 31; // CLK on, HOSC 24Mhz by default
    (void)*(volatile unsigned int*)LEDC_CLK_REG; // read back to ensure clk on
    *(volatile unsigned int*)LEDC_BUS_GATING_RST_REG = *(volatile unsigned int*)LEDC_BUS_GATING_RST_REG | (1u << 16); // De-assert reset
}

static void config_ccu_dma(void)
{
    *(volatile unsigned int*)DMA_BGR_REG = 1u; // pass DMA bus clock
    *(volatile unsigned int*)DMA_BGR_REG = *(volatile unsigned int*)DMA_BGR_REG | (1u << 16); // deassert reset
    (void)*(volatile unsigned int*)DMA_BGR_REG;
    *(volatile unsigned int*)MBUS_MAT_CLK_GATING_REG = *(volatile unsigned int*)MBUS_MAT_CLK_GATING_REG | 1u;
    (void)*(volatile unsigned int*)MBUS_MAT_CLK_GATING_REG;

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
static void ledc_soft_reset()
{
    *(volatile unsigned int*)LEDC_CTRL_REG = *(volatile unsigned int*)LEDC_CTRL_REG | (1u << 1); // set LED_SOFT_RESET
    while (((*(volatile unsigned int*)LEDC_CTRL_REG >> 1u) & 1u) != 0){} // read LED_SOFT_RESET until bit is set to 0
}

static void ledc_configure_protocol(void)
{
    // set timings, resulting time is 42 ns * (value_decimal + 1), the default values are adequate but this sequence is provided for any necessary changes
    // timings are first cleared, then set
    *(volatile unsigned int*)LED_T01_TIMING_CTRL_REG = *(volatile unsigned int*)LED_T01_TIMING_CTRL_REG & ~(0b111111u); // T0L_TIME
    *(volatile unsigned int*)LED_T01_TIMING_CTRL_REG = *(volatile unsigned int*)LED_T01_TIMING_CTRL_REG | (0b010011u);
    *(volatile unsigned int*)LED_T01_TIMING_CTRL_REG = *(volatile unsigned int*)LED_T01_TIMING_CTRL_REG & ~(0b111111u << 21); // T1H_TIME
    *(volatile unsigned int*)LED_T01_TIMING_CTRL_REG = *(volatile unsigned int*)LED_T01_TIMING_CTRL_REG | (0b010100u << 21);

    *(volatile unsigned int*)LED_T01_TIMING_CTRL_REG = *(volatile unsigned int*)LED_T01_TIMING_CTRL_REG & ~(0b11111u << 6); // T0H_TIME
    *(volatile unsigned int*)LED_T01_TIMING_CTRL_REG = *(volatile unsigned int*)LED_T01_TIMING_CTRL_REG | (0b00111u << 6);
    *(volatile unsigned int*)LED_T01_TIMING_CTRL_REG = *(volatile unsigned int*)LED_T01_TIMING_CTRL_REG & ~(0b11111u << 16); // T1L_TIME
    *(volatile unsigned int*)LED_T01_TIMING_CTRL_REG = *(volatile unsigned int*)LED_T01_TIMING_CTRL_REG | (0b00110u << 16);

    // config data format, default is GRB, MSB first


}
