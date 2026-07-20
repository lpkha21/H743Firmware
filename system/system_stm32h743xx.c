#include <stdint.h>

#include "stm32h743xx.h"
#include "system_clock.h"

uint32_t SystemCoreClock = 200000000UL;

static void clockError(void)
{
    while (1) {
    }
}

void SystemInit(void)
{
    /* Enable FPU */
    SCB->CPACR |=
        (3UL << (10U * 2U)) |
        (3UL << (11U * 2U));

    /* Vector table at flash base */
    SCB->VTOR = FLASH_BASE;

    /*
     * Power configuration:
     * Voltage Scale 1 for high-frequency operation.
     */
    RCC->APB4ENR |= RCC_APB4ENR_SYSCFGEN;
    __DSB();

    PWR->D3CR &= ~PWR_D3CR_VOS;
    PWR->D3CR |= PWR_D3CR_VOS_0;

    while ((PWR->D3CR & PWR_D3CR_VOSRDY) == 0U) {
    }

    /*
     * Enable external 8 MHz HSE.
     */
    RCC->CR |= RCC_CR_HSEON;

    uint32_t timeout = 1000000U;

    while ((RCC->CR & RCC_CR_HSERDY) == 0U) {
        if (--timeout == 0U) {
            clockError();
        }
    }

    /*
     * PLL1 configuration:
     *
     * HSE      = 8 MHz
     * PLLM     = 4
     * PLL input = 2 MHz
     * PLLN     = 200
     * VCO      = 400 MHz
     * PLLP     = 2
     * PLL1_P   = 200 MHz
     *
     * SYSCLK is then configured as 200 MHz.
     *
     * We will raise this to the final 400 MHz CPU setup after
     * proving the HSE and PLL configuration works reliably.
     */

    RCC->PLLCKSELR =
        RCC_PLLCKSELR_PLLSRC_HSE |
        (4UL << RCC_PLLCKSELR_DIVM1_Pos);

    RCC->PLLCFGR =
        RCC_PLLCFGR_PLL1VCOSEL |
        RCC_PLLCFGR_PLL1RGE_1 |
        RCC_PLLCFGR_DIVP1EN;

    RCC->PLL1DIVR =
        ((200U - 1U) << RCC_PLL1DIVR_N1_Pos) |
        ((2U - 1U)   << RCC_PLL1DIVR_P1_Pos) |
        ((2U - 1U)   << RCC_PLL1DIVR_Q1_Pos) |
        ((2U - 1U)   << RCC_PLL1DIVR_R1_Pos);

    RCC->CR |= RCC_CR_PLL1ON;

    timeout = 1000000U;

    while ((RCC->CR & RCC_CR_PLL1RDY) == 0U) {
        if (--timeout == 0U) {
            clockError();
        }
    }

    /*
     * Bus clocks:
     *
     * SYSCLK = 200 MHz
     * HCLK   = 100 MHz
     * APB1   = 50 MHz
     * APB2   = 50 MHz
     * APB3   = 50 MHz
     * APB4   = 50 MHz
     */

    RCC->D1CFGR =
        RCC_D1CFGR_HPRE_DIV2 |
        RCC_D1CFGR_D1PPRE_DIV2 |
        RCC_D1CFGR_D1CPRE_DIV1;

    RCC->D2CFGR =
        RCC_D2CFGR_D2PPRE1_DIV2 |
        RCC_D2CFGR_D2PPRE2_DIV2;

    RCC->D3CFGR =
        RCC_D3CFGR_D3PPRE_DIV2;

    /*
     * Flash wait states.
     */
    FLASH->ACR =
        FLASH_ACR_LATENCY_2WS |
        FLASH_ACR_WRHIGHFREQ_1;

    /*
     * Switch SYSCLK to PLL1.
     */
    RCC->CFGR &= ~RCC_CFGR_SW;
    RCC->CFGR |= RCC_CFGR_SW_PLL1;

    timeout = 1000000U;

    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL1) {
        if (--timeout == 0U) {
            clockError();
        }
    }

    SystemCoreClock = 200000000UL;

    __DSB();
    __ISB();
}

void SystemCoreClockUpdate(void)
{
    SystemCoreClock = 200000000UL;
}