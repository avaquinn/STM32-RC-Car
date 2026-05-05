/*
 * speed.h
 *
 *  Created on: May 4, 2026
 *      Author: avais
 */

#ifndef INC_SPEED_H_
#define INC_SPEED_H_

// Set LED pins
#define SPEED_LED_PORT GPIOC
#define SPEED_LED1_PIN GPIO_PIN_0
#define SPEED_LED2_PIN GPIO_PIN_1
#define SPEED_LED3_PIN GPIO_PIN_2
#define SPEED_LED4_PIN GPIO_PIN_3
#define SPEED_LED_ALL_PINS (SPEED_LED1_PIN | SPEED_LED2_PIN | SPEED_LED3_PIN | SPEED_LED4_PIN)

// Set baselines for x acceleration and noise
#define BASELINE_X_ACCEL 16
#define NOISE_THRESHOLD 10

// Set LED threshold voltages
#define SPEED_LED1_THRESHOLD 2000
#define SPEED_LED2_THRESHOLD 5000
#define SPEED_LED3_THRESHOLD 11000
#define SPEED_LED4_THRESHOLD 15000

#define MAX_VELOCITY_SCORE 20000

// Function prototypes
void speed_leds_init(void);
void update_velocity_score(uint32_t);
void reset_velocity_score(uint32_t);
void velocity_leds_update(void);

int32_t speed_get_velocity_score(void);

#endif /* INC_SPEED_H_ */
