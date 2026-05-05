/*
 * mc.c
 *
 *  Created on: May 4, 2026
 *      Author: avais
 */

#include "main.h"
#include "mc.h"

/**
  * @brief Stop both motors
  * @retval None
  * Stop motors by setting all PWM to 0
  */
void motors_stop(void)
{
    TIM2->CCR1 = 0;
    TIM2->CCR2 = 0;
    TIM3->CCR3 = 0;
    TIM3->CCR4 = 0;
}

/**
  * @brief Drive forward
  * @retval None
  * Drive forward by setting forward controls
  * to maximum
  */
void motors_forward(void)
{
    TIM2->CCR1 = PWM_MAX;
    TIM2->CCR2 = 0;

    TIM3->CCR3 = PWM_MAX;
    TIM3->CCR4 = 0;
}

/**
  * @brief Drive backward
  * @retval None
  * Drive backward by setting reverse controls
  * to a set speed
  */
void motors_backward(void)
{
    // Left motor reverse
    TIM2->CCR1 = 0;
    TIM2->CCR2 = PWM_REVERSE;

    // Right motor reverse
    TIM3->CCR3 = 0;
    TIM3->CCR4 = PWM_REVERSE;
}

/**
  * @brief Drive left
  * @retval None
  * Drive left by setting right motor faster
  * than left motor.
  */
void motors_left(void)
{
    // Left motor slower, right motor faster
    TIM2->CCR1 = PWM_INNER_TURN;
    TIM2->CCR2 = 0;

    TIM3->CCR3 = PWM_OUTER_TURN;
    TIM3->CCR4 = 0;
}

/**
  * @brief Drive right
  * @retval None
  * Drive right by setting left motor faster
  * than right motor.
  */
void motors_right(void)
{
    // Left motor faster, right motor slower
    TIM2->CCR1 = PWM_OUTER_TURN;
    TIM2->CCR2 = 0;

    TIM3->CCR3 = PWM_INNER_TURN;
    TIM3->CCR4 = 0;
}

/**
  * @brief Initializes the PWM GPIO ports
  * @retval None
  * Intializes the PWM GPIO ports A0 and A1
  * in alternate function mode. Configures them
  * to PWM on TIM2 channel 1 and channel 2.
  */
void PWM_init(void)
{
    // Enable GPIOA and GPIOB clock
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;

    // Configure PA0 as PWM1 alternate function mode
	GPIOA->MODER &= ~(GPIO_MODER_MODE0);
	GPIOA->MODER |= (GPIO_MODER_MODE0_1); // Alternate function mode
	GPIOA->OTYPER &= ~(GPIO_OTYPER_OT0);  // Push-pull
	GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPD0);  // Not PUPD
	GPIOA->AFR[0] &= ~(GPIO_AFRL_AFSEL0);
	GPIOA->AFR[0] |=  (1 << GPIO_AFRL_AFSEL0_Pos); // AF1 = TIM2_CH1

	// Configure PA1 as PWM2 alternate function mode
	GPIOA->MODER &= ~(GPIO_MODER_MODE1);
	GPIOA->MODER |= (GPIO_MODER_MODE1_1); // Alternate function mode
	GPIOA->OTYPER &= ~(GPIO_OTYPER_OT1);  // Push-pull
	GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPD1);  // Not PUPD
	GPIOA->AFR[0] &= ~(GPIO_AFRL_AFSEL1);
	GPIOA->AFR[0] |=  (1 << GPIO_AFRL_AFSEL1_Pos); // AF1 = TIM2_CH2

	// Configure PB0 as TIM3_CH3 alternate function mode
	GPIOB->MODER &= ~(GPIO_MODER_MODE0);
	GPIOB->MODER |=  (GPIO_MODER_MODE0_1); // Alternate function mode
	GPIOB->OTYPER &= ~(GPIO_OTYPER_OT0);   // Push-pull
	GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPD0);   // No pull-up/pull-down
	GPIOB->AFR[0] &= ~(GPIO_AFRL_AFSEL0);
	GPIOB->AFR[0] |=  (2 << GPIO_AFRL_AFSEL0_Pos); // AF2 = TIM3_CH3

	// Configure PB1 as TIM3_CH4 alternate function mode
	GPIOB->MODER &= ~(GPIO_MODER_MODE1);
	GPIOB->MODER |=  (GPIO_MODER_MODE1_1); // Alternate function mode
	GPIOB->OTYPER &= ~(GPIO_OTYPER_OT1);   // Push-pull
	GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPD1);   // No pull-up/pull-down
	GPIOB->AFR[0] &= ~(GPIO_AFRL_AFSEL1);
	GPIOB->AFR[0] |=  (2 << GPIO_AFRL_AFSEL1_Pos); // AF2 = TIM3_CH4
}

/**
  * @brief Initializes TIM2 and TIM3 for motor PWM control
  * @retval None
  *
  * TIM2_CH1 on PA0 controls DRV8833 IN1
  * TIM2_CH2 on PA1 controls DRV8833 IN2
  * TIM3_CH3 on PB0 controls DRV8833 IN3
  * TIM3_CH4 on PB1 controls DRV8833 IN4
  *
  * PWM frequency = 1 kHz assuming 80 MHz timer clock.
  */
void timer_init(void)
{
    // Enable clocks to TIM2 and TIM3
    RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN;
    RCC->APB1ENR1 |= RCC_APB1ENR1_TIM3EN;

    // Stop timers during configuration
    TIM2->CR1 &= ~TIM_CR1_CEN;
    TIM3->CR1 &= ~TIM_CR1_CEN;


    // 1 kHz PWM frequency
    TIM2->PSC = 79;
    TIM2->ARR = 999;

    TIM3->PSC = 79;
    TIM3->ARR = 999;

    // Start with motors stopped
    TIM2->CCR1 = 0;
    TIM2->CCR2 = 0;
    TIM3->CCR3 = 0;
    TIM3->CCR4 = 0;

    // Configure TIM2_CH1 as PWM mode 1
    TIM2->CCMR1 &= ~TIM_CCMR1_CC1S;
    TIM2->CCMR1 &= ~TIM_CCMR1_OC1M;
    TIM2->CCMR1 |=  (6 << TIM_CCMR1_OC1M_Pos);

    // Configure TIM2_CH2 as PWM mode 1
    TIM2->CCMR1 &= ~TIM_CCMR1_CC2S;
    TIM2->CCMR1 &= ~TIM_CCMR1_OC2M;
    TIM2->CCMR1 |=  (6 << TIM_CCMR1_OC2M_Pos);

    // Enable TIM2_CH1 and TIM2_CH2 outputs, active high
    TIM2->CCER &= ~(TIM_CCER_CC1P | TIM_CCER_CC2P);
    TIM2->CCER |=  (TIM_CCER_CC1E | TIM_CCER_CC2E);

    // Configure TIM3_CH3 as PWM mode 1
    TIM3->CCMR2 &= ~TIM_CCMR2_CC3S;
    TIM3->CCMR2 &= ~TIM_CCMR2_OC3M;
    TIM3->CCMR2 |=  (6 << TIM_CCMR2_OC3M_Pos);

    // Configure TIM3_CH4 as PWM mode 1
    TIM3->CCMR2 &= ~TIM_CCMR2_CC4S;
    TIM3->CCMR2 &= ~TIM_CCMR2_OC4M;
    TIM3->CCMR2 |=  (6 << TIM_CCMR2_OC4M_Pos);

    // Enable TIM3_CH3 and TIM3_CH4 outputs, active high
    TIM3->CCER &= ~(TIM_CCER_CC3P | TIM_CCER_CC4P);
    TIM3->CCER |=  (TIM_CCER_CC3E | TIM_CCER_CC4E);

    // Force updates to load prescaler/ARR values
    TIM2->EGR |= TIM_EGR_UG;
    TIM3->EGR |= TIM_EGR_UG;

    // Start timers
    TIM2->CR1 |= TIM_CR1_CEN;
    TIM3->CR1 |= TIM_CR1_CEN;
}
