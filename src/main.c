#include <stdint.h>

#include "stm32h743xx.h"
#include "target.h"

static void dwtInit(void)
{
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CYCCNT = 0U;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
}

static void delayMs(uint32_t milliseconds)
{
    const uint32_t cyclesPerMs = SystemCoreClock / 1000U;
    const uint32_t start = DWT->CYCCNT;
    const uint32_t delayCycles = cyclesPerMs * milliseconds;

    while ((uint32_t)(DWT->CYCCNT - start) < delayCycles) {
    }
}

static void ledInit(void)
{
    RCC->AHB4ENR |= RCC_AHB4ENR_GPIOEEN;
    __DSB();

    GPIOE->MODER &=
        ~(3UL << (LED0_PIN * 2U));

    GPIOE->MODER |=
        (1UL << (LED0_PIN * 2U));

    GPIOE->OTYPER &=
        ~(1UL << LED0_PIN);

    GPIOE->OSPEEDR &=
        ~(3UL << (LED0_PIN * 2U));

    GPIOE->PUPDR &=
        ~(3UL << (LED0_PIN * 2U));
}

static void buzzerInitOff(void)
{
    RCC->AHB4ENR |= RCC_AHB4ENR_GPIOBEN;
    __DSB();

    /* PB1 as general-purpose output */
    GPIOB->MODER &= ~(3UL << (BUZZER_PIN * 2U));
    GPIOB->MODER |=  (1UL << (BUZZER_PIN * 2U));

    GPIOB->OTYPER &= ~(1UL << BUZZER_PIN);
    GPIOB->OSPEEDR &= ~(3UL << (BUZZER_PIN * 2U));
    GPIOB->PUPDR &= ~(3UL << (BUZZER_PIN * 2U));

    /* Start with buzzer off */
    GPIOB->BSRR = (1UL << BUZZER_PIN);
}

int main(void)
{
    buzzerInitOff();
    ledInit();
    dwtInit();

    while (1) {
        GPIOE->ODR ^= (1UL << LED0_PIN);
        delayMs(500U);
    }
}