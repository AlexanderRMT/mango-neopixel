# NeoPixel

This project provides bare-metal WS2812/NeoPixel drivers for the Allwinner D1
on the MangoPi MQ-Pro.

The `regular` implementation generates the LED signal through GPIO using a
timing-sensitive RISC-V assembly routine. The `ledc` implementation is being
developed to move signal generation to the D1's LED controller and DMA hardware,
allowing transmission to run without blocking the CPU.
