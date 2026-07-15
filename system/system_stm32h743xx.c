#include <stdint.h>

#include "stm32h743xx.h"

uint32_t SystemCoreClock = 64000000UL;

void SystemInit(void)
{
    /*
     * Keep the reset HSI clock for the first test.
     *
     * The STM32H743 starts from the internal HSI oscillator.
     * We will configure the 8 MHz HSE and PLL only after the
     * startup, linker, flashing and GPIO path are proven.
     */

    SCB->CPACR |=
        (3UL << (10U * 2U)) |
        (3UL << (11U * 2U));

    SCB->VTOR = FLASH_BASE;

    __DSB();
    __ISB();
}

void SystemCoreClockUpdate(void)
{
    SystemCoreClock = 64000000UL;
}