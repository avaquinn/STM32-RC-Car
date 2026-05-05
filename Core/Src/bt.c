/*
 * bt.c
 *
 *  Created on: May 4, 2026
 *      Author: avais
 */

#include "main.h"
#include "bt.h"

/**
  * @brief Initializes UART1 for bluetooth
  * @retval None
  * Initializes TX and RX pins for UART1, and
  * configures UART1 to match bluetooth device.
  */
void UART1_init(void)
{
    // Enable GPIOA clock
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;

    // Enable USART1 clock
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;


    // Configure PA9 as TX alternate function mode
	GPIOA->MODER &= ~(GPIO_MODER_MODE9);
	GPIOA->MODER |= (GPIO_MODER_MODE9_1); // Alternate function mode
	GPIOA->OTYPER &= ~(GPIO_OTYPER_OT9);  // Push-pull
	GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPD9);  // Not PUPD
	GPIOA->AFR[1] &= ~(0xF << GPIO_AFRH_AFSEL9_Pos);
	GPIOA->AFR[1] |=  (7 << GPIO_AFRH_AFSEL9_Pos); // AF7 for USART1

	// Configure PA10 as RX alternate function mode
	GPIOA->MODER &= ~(GPIO_MODER_MODE10);
	GPIOA->MODER |= (GPIO_MODER_MODE10_1); // Alternate function mode
	GPIOA->OTYPER &= ~(GPIO_OTYPER_OT10);  // Push-pull
	GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPD10);  // Not PUPD
	GPIOA->AFR[1] &= ~(0xF << GPIO_AFRH_AFSEL10_Pos);
	GPIOA->AFR[1] |= (7 << GPIO_AFRH_AFSEL10_Pos); // AF7 for USART1


    // 9600 baud with 80 MHz clock
    USART1->BRR = 80000000  / 9600;

    // Enable transmitter and receiver
    USART1->CR1 |= USART_CR1_TE | USART_CR1_RE;

    // Enable USART1
    USART1->CR1 |= USART_CR1_UE;
}

/**
  * @brief Check if a character is available to be read
  * @retval uint8_t
  */
uint8_t UART1_char_available(void)
{
    return (USART1->ISR & USART_ISR_RXNE) != 0;
}

/**
  * @brief reads a char via UART1
  * @retval char
  */
char UART1_read_char(void)
{
    while (!(USART1->ISR & USART_ISR_RXNE));
    return USART1->RDR;
}

/**
  * @brief writes a char via UART1
  * @retval None
  */
void UART1_write_char(char c)
{
    while (!(USART1->ISR & USART_ISR_TXE));
    USART1->TDR = c;
}

/**
  * @brief Send a string via UART1
  * @retval void
  */
void UART1_print(char *str)
{
    while (*str)
    {
        UART1_write_char(*str);
        str++;
    }
}

