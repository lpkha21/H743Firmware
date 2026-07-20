#include "uart.h"

#include "stm32h743xx.h"
#include "system_clock.h"

static uint32_t calculateBrr(uint32_t peripheralClock, uint32_t baudRate)
{
    return (peripheralClock + (baudRate / 2U)) / baudRate;
}

void uart4Init(uint32_t baudRate)
{
    /*
     * Enable GPIOA and UART4 clocks.
     */
    RCC->AHB4ENR |= RCC_AHB4ENR_GPIOAEN;
    RCC->APB1LENR |= RCC_APB1LENR_UART4EN;
    __DSB();

    /*
     * PA11 = UART4_RX
     * PA12 = UART4_TX
     *
     * Alternate function 6.
     */

    GPIOA->MODER &=
        ~((3UL << (11U * 2U)) |
          (3UL << (12U * 2U)));

    GPIOA->MODER |=
        ((2UL << (11U * 2U)) |
         (2UL << (12U * 2U)));

    GPIOA->OTYPER &=
        ~((1UL << 11U) |
          (1UL << 12U));

    GPIOA->OSPEEDR |=
        ((3UL << (11U * 2U)) |
         (3UL << (12U * 2U)));

    GPIOA->PUPDR &=
        ~((3UL << (11U * 2U)) |
          (3UL << (12U * 2U)));

    /*
     * PA11 and PA12 are in AFRH because they are pins 8–15.
     */
    GPIOA->AFR[1] &=
        ~((0xFUL << ((11U - 8U) * 4U)) |
          (0xFUL << ((12U - 8U) * 4U)));

    GPIOA->AFR[1] |=
        ((6UL << ((11U - 8U) * 4U)) |
         (6UL << ((12U - 8U) * 4U)));

    /*
     * Disable UART before changing configuration.
     */
    UART4->CR1 &= ~USART_CR1_UE;

    UART4->CR1 = 0U;
    UART4->CR2 = 0U;
    UART4->CR3 = 0U;

    /*
     * UART4 clock currently comes from PCLK1 = 50 MHz.
     */
    UART4->BRR = calculateBrr(SYSTEM_PCLK1_HZ, baudRate);

    /*
     * Enable transmitter and receiver.
     */
    UART4->CR1 =
        USART_CR1_TE |
        USART_CR1_RE;

    UART4->CR1 |= USART_CR1_UE;

    /*
     * Wait until transmitter is enabled.
     */
    while ((UART4->ISR & USART_ISR_TEACK) == 0U) {
    }

    while ((UART4->ISR & USART_ISR_REACK) == 0U) {
    }
}

void uart4WriteByte(uint8_t data)
{
    while ((UART4->ISR & USART_ISR_TXE_TXFNF) == 0U) {
    }

    UART4->TDR = data;
}

void uart4WriteString(const char *text)
{
    if (text == 0) {
        return;
    }

    while (*text != '\0') {
        if (*text == '\n') {
            uart4WriteByte('\r');
        }

        uart4WriteByte((uint8_t)*text);
        text++;
    }
}