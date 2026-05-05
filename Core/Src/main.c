/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "mc.h"
#include "bt.h"
#include "accel.h"
#include "speed.h"

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart2;
uint32_t last_cmd_time = 0;

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void controller(void);
void speed_timer_init(void);

static volatile uint32_t current_tick = 0;


/**
  * @brief  The application entry point.
  * @retval int
  * Initializes all connected sensors and
  * communication channels. Then, runs motor
  * control, velocity calculation, and LED update
  * in an infinite loop.
  */
int main(void)
{
	HAL_Init();
	SystemClock_Config();

	speed_leds_init();
	speed_timer_init();

	PWM_init();
	timer_init();
	motors_stop();

	UART1_init();

	I2C_init();
	ADXL345_init();
	reset_velocity_score(current_tick);

	while (1)
	{
	    controller();
	    update_velocity_score(current_tick);
	    velocity_leds_update();
	    HAL_Delay(50);
	}
}

/**
  * @brief  Configure TIM6 to count ticks
  * @retval None
  * Uses TIM6 to increment a ms counter every
  * millisecond. This is later used to integrate
  * acceleration over time to get velocity, and to
  * track Bluetooth connectivity.
  */
void speed_timer_init(void)
{
	// Enable TIM6 clock
	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM6EN;

	// Stop timer while configuring
	TIM6->CR1 &= ~TIM_CR1_CEN;

	// Configure 1ms clock
	TIM6->PSC = 7999;
	TIM6->ARR = 9;

	TIM6->SR &= ~TIM_SR_UIF; // Clear flags
	TIM6->DIER |= TIM_DIER_UIE; // Enable update interrupt

	// Enable TIM6
	NVIC_EnableIRQ(TIM6_DAC_IRQn);
	__enable_irq();

	// Start timer
	TIM6->CR1 |= TIM_CR1_CEN;
}

/**
  * @brief Use TIM6 Interrupts to count ms
  * @retval None
  * Increments the time in ms at each interrupt.
  */
void TIM6_DAC_IRQHandler(void)
{
	if (TIM6->SR & TIM_SR_UIF)
	{
		TIM6->SR &= ~TIM_SR_UIF;
		current_tick++;
	}
}

/**
  * @brief Motor controller
  * @retval None
  * Uses input from the bluetooth sensor to
  * interpret wasd key presses into driving directions.
  */
void controller(void)
{
	if (UART1_char_available())
	{
		char cmd = USART1->RDR;

		if (cmd == '\r' || cmd == '\n')
		{
			return;
		}

		UART1_write_char(cmd);
		UART1_print("\r\n");

		switch (cmd)
		{
			case 'W':
			case 'w':
				motors_forward();
				last_cmd_time = current_tick;
				UART1_print("Forward\r\n");
				break;

			case 'S':
			case 's':
				motors_backward();
				last_cmd_time = current_tick;
				UART1_print("Backward\r\n");
				break;

			case 'A':
			case 'a':
				motors_left();
				last_cmd_time = current_tick;
				UART1_print("Left\r\n");
				break;

			case 'D':
			case 'd':
				motors_right();
				last_cmd_time = current_tick;
				UART1_print("Right\r\n");
				break;

			case 'Q':
			case 'q':
				motors_stop();
				reset_velocity_score(current_tick);
				last_cmd_time = current_tick;
				UART1_print("Stop, velocity reset\r\n");
				break;

			default:
				UART1_print("Unknown ignored\r\n");
				break;
		}
	}

	// Stop if no valid command recently
	if ((current_tick - last_cmd_time) > BT_TIMEOUT_MS)
	{
		motors_stop();
	}
}




/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 10;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}


/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
