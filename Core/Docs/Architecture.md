# Architecture

## IO
* USART2: connected to terminal on Mac
* I2S2: connected to DAC

## Render flow
1. Reset timer
2. Read input from USART
3. Render buffer
4. Read & stop timer
5. Write load percentage to USART
6. Wait for DMA interrupt
7. Swap buffers

## Object definitions
