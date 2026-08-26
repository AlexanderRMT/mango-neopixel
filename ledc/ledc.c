//
// LEDC backend. Created by alexa on 8/22/2026.
//

#include "ledc.h"

static void config_ccu_ledc(void)
{
    *(volatile unsigned int*)LEDC_BUS_GATING_RST_REG = 1u; // pass gate
    *(volatile unsigned int*)LEDC_CLK_REG = 1u << 31; // CLK on, HOSC 24Mhz by default
    (void)*(volatile unsigned int*)LEDC_CLK_REG; // read back to ensure clk on
    *(volatile unsigned int*)LEDC_BUS_GATING_RST_REG = *(volatile unsigned int*)LEDC_BUS_GATING_RST_REG | 1u << 16; // De-assert reset
}

