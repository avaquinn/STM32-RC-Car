/*
 * speed.c
 *
 *  Created on: May 4, 2026
 *      Author: avais
 */
#include "main.h"
#include "speed.h"
#include "accel.h"
#include <stdlib.h>

int32_t velocity_score = 0;
int16_t x_baseline = 0;
uint32_t last_vel_time = 0;


/**
  * @brief Update velocity by integrating acceleration
  * @retval None
  * This function uses the incoming accelerometer data
  * paired with the current time in ms to integrate
  * acceleration data into an approximate velocity.
  */
void update_velocity_score(uint32_t current_tick)
{
	int16_t x, y, z;
	ADXL345_read_accel(&x, &y, &z);

	uint32_t now = current_tick;
	uint32_t dt_ms = now - last_vel_time;
	last_vel_time = now;


	// Take absolute value and subtract baseline
	int16_t a_forward = (x - BASELINE_X_ACCEL);

	if (a_forward > -NOISE_THRESHOLD && a_forward < NOISE_THRESHOLD) a_forward = 0;

	// Decays data when stopped to reduce drift
	if (a_forward == 0 && velocity_score > 0)
	{
		velocity_score -= 2;
		if (velocity_score < 0)
		{
			velocity_score = 0;
		}
	}

	// integrate acceleration into rough velocity
	velocity_score += ((int32_t)a_forward * (int32_t)dt_ms);

		// cap max value
	if (velocity_score > MAX_VELOCITY_SCORE)
	{
		velocity_score = MAX_VELOCITY_SCORE;
	}
}

/**
  * @brief Reset the current velocity to 0
  * @retval None
  * Calibrates a baseline x value and resets
  * the velocity score to zero. Sets time to
  * begin the next integral at current time in ms
  */
void reset_velocity_score(uint32_t current_tick)
{
	velocity_score = 0;
	last_vel_time = current_tick;
}

/**
  * @brief  Update velocity output LEDS
  * @retval None
  * Compares the current velocity to a series of
  * threshold voltages and activates LEDs for the
  * velocity benchmarks.
  */
void velocity_leds_update(void)
{
	// Turn all speed LEDs off
	GPIOC->BRR = GPIO_BRR_BR0 | GPIO_BRR_BR1 | GPIO_BRR_BR2 | GPIO_BRR_BR3;

	// Turn LEDs on according to threshold voltages
	if (velocity_score > SPEED_LED1_THRESHOLD || velocity_score < -SPEED_LED1_THRESHOLD)
		GPIOC->BSRR = GPIO_BSRR_BS0;

	if (velocity_score > SPEED_LED2_THRESHOLD || velocity_score < -SPEED_LED2_THRESHOLD)
		GPIOC->BSRR = GPIO_BSRR_BS1;

	if (velocity_score > SPEED_LED3_THRESHOLD|| velocity_score < -SPEED_LED3_THRESHOLD)
		GPIOC->BSRR = GPIO_BSRR_BS2;

	if (velocity_score > SPEED_LED4_THRESHOLD || velocity_score < -SPEED_LED4_THRESHOLD)
		GPIOC->BSRR = GPIO_BSRR_BS3;
}



/**
  * @brief Configure the velocity output LEDs
  * @retval None
  * Configures the velocity output LEDs and GPIO
  * out at low speed.
  */
void speed_leds_init(void)
{
	// Enable GPIOC clock
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;

	// LED 0: PC0 Config
	GPIOC->MODER &= ~(GPIO_MODER_MODE0);
	GPIOC->MODER |= (GPIO_MODER_MODE0_0);       // Output
	GPIOC->OTYPER &= ~(GPIO_OTYPER_OT0);        // Push-pull
	GPIOC->PUPDR &= ~(GPIO_PUPDR_PUPD0);        // Not PUPD
	GPIOC->OSPEEDR &= ~(GPIO_OSPEEDR_OSPEED0);  // Low speed

	// LED 1: PC1 Config
	GPIOC->MODER &= ~(GPIO_MODER_MODE1);
	GPIOC->MODER |= (GPIO_MODER_MODE1_0);       // Output
	GPIOC->OTYPER &= ~(GPIO_OTYPER_OT1);        // Push-pull
	GPIOC->PUPDR &= ~(GPIO_PUPDR_PUPD1);        // Not PUPD
	GPIOC->OSPEEDR &= ~(GPIO_OSPEEDR_OSPEED1);  // Low speed

	// LED 2: PC2 Config
	GPIOC->MODER &= ~(GPIO_MODER_MODE2);
	GPIOC->MODER |= (GPIO_MODER_MODE2_0);       // Output
	GPIOC->OTYPER &= ~(GPIO_OTYPER_OT2);        // Push-pull
	GPIOC->PUPDR &= ~(GPIO_PUPDR_PUPD2);        // Not PUPD
	GPIOC->OSPEEDR &= ~(GPIO_OSPEEDR_OSPEED2);  // Low speed

	// LED 3: PC3 Config
	GPIOC->MODER &= ~(GPIO_MODER_MODE3);
	GPIOC->MODER |= (GPIO_MODER_MODE3_0);       // Output
	GPIOC->OTYPER &= ~(GPIO_OTYPER_OT3);        // Push-pull
	GPIOC->PUPDR &= ~(GPIO_PUPDR_PUPD3);        // Not PUPD
	GPIOC->OSPEEDR &= ~(GPIO_OSPEEDR_OSPEED3);  // Low speed

	// Start with all speed LEDs off
	GPIOC->BRR = GPIO_BRR_BR0 | GPIO_BRR_BR1 | GPIO_BRR_BR2 | GPIO_BRR_BR3;
}
