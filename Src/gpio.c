/**
  ******************************************************************************
  * File Name          : gpio.c
  * Description        : This file provides code for the configuration
  *                      of all used GPIO pins.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "gpio.h"
/* USER CODE BEGIN 0 */

#include "lcd.h"

/* USER CODE END 0 */

/*----------------------------------------------------------------------------*/
/* Configure GPIO                                                             */
/*----------------------------------------------------------------------------*/
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/** Configure pins as 
        * Analog 
        * Input 
        * Output
        * EVENT_OUT
        * EXTI
*/
void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15|GPIO_PIN_8 
                          |GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3 
                          |GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5|GPIO_PIN_8|GPIO_PIN_9, GPIO_PIN_RESET);

  /*Configure GPIO pins : PC13 PC14 PC15 PC0 
                           PC1 PC2 PC3 PC4 
                           PC5 PC6 PC7 PC8 
                           PC9 PC10 PC11 PC12 */
  GPIO_InitStruct.Pin = GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15|GPIO_PIN_0 
                          |GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4 
                          |GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8 
                          |GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : PA0 */
  GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PA1 */
  GPIO_InitStruct.Pin = GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PB0 PB1 PB2 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : PA8 */
  GPIO_InitStruct.Pin = GPIO_PIN_8;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PD2 */
  GPIO_InitStruct.Pin = GPIO_PIN_2;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pins : PB5 PB8 PB9 */
  GPIO_InitStruct.Pin = GPIO_PIN_5|GPIO_PIN_8|GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI1_IRQn);

}

/* USER CODE BEGIN 2 */

/** index : LED index (1-8)
  * enable : 1-enable 0-disable 
  */
void led_set_status(uint8_t index, uint8_t enable)
{
    if (1 == enable)
    {
        HAL_GPIO_WritePin(GPIOC, 0x01 << (index + 7), GPIO_PIN_RESET);
    }
    else
    {
        HAL_GPIO_WritePin(GPIOC, 0x01 << (index + 7), GPIO_PIN_SET);
    }
}

/** enable : 1-enable 0-disable 
  */
void led_lock_enable(uint8_t enable)
{
	if (1 == enable)
    {
        HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_SET);
    }
    else
    {
        HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_RESET);
    }
}

/** key_index : KEY index (1-4)
  * key_status : return key status (0-key down, 1-key up)
  */
void key_get_status(uint8_t key_index, uint8_t *key_status)
{
    if (key_index >= 1 && key_index <= 3)
    {
        *key_status = HAL_GPIO_ReadPin(GPIOB, 0x01 << (key_index - 1));
    }
    else
    {
        *key_status = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0);
    }
}

void key_led_test(void)
{
    uint8_t i = 0;
    uint8_t key11_state = 0;
    uint8_t key21_state = 0;
    uint8_t key31_state = 0;
    uint8_t key41_state = 0;
    uint8_t key12_state = 0;
    uint8_t key22_state = 0;
    uint8_t key32_state = 0;
    uint8_t key42_state = 0;
    
    key_get_status(1, &key11_state);
    key_get_status(2, &key21_state);
    key_get_status(3, &key31_state);
    key_get_status(4, &key41_state);
    HAL_Delay(50);
    key_get_status(1, &key12_state);
    key_get_status(2, &key22_state);
    key_get_status(3, &key32_state);
    key_get_status(4, &key42_state);
      
    if (key11_state == 0 && key12_state == 0)
    {
        for (i = 1; i <= 8; i++)
        {
            led_set_status(i, 1);
        }
    }
    else if (key21_state == 0 && key22_state == 0)
    {
        for (i = 1; i <= 8; i++)
        {
            led_set_status(i, 0);
        }
    }
    else if (key31_state == 0 && key32_state == 0)
    {
        for (i = 1; i <= 8; i++)
        {
            if (i <= 4)
            {
                led_set_status(i, 1);
            }
            else
            {
                led_set_status(i, 0);
            }
        }
        
    }
    else if (key41_state == 0 && key42_state == 0)
    {
        for (i = 1; i <= 8; i++)
        {
            if (i <= 4)
            {
                led_set_status(i, 0);
            }
            else
            {
                led_set_status(i, 1);
            }
        }
    }
    
    led_lock_enable(1);
    led_lock_enable(0);
}

void key_scan(uint8_t *key_down_index)
{
    uint8_t key11_state = 0;
    uint8_t key21_state = 0;
    uint8_t key31_state = 0;
    uint8_t key41_state = 0;
    uint8_t key12_state = 0;
    uint8_t key22_state = 0;
    uint8_t key32_state = 0;
    uint8_t key42_state = 0;
    
    key_get_status(1, &key11_state);
    key_get_status(2, &key21_state);
    key_get_status(3, &key31_state);
    key_get_status(4, &key41_state);
    HAL_Delay(50);
    key_get_status(1, &key12_state);
    key_get_status(2, &key22_state);
    key_get_status(3, &key32_state);
    key_get_status(4, &key42_state);
      
    if (key11_state == 0 && key12_state == 0)
    {
        while(key12_state == 0)
        {
            key_get_status(1, &key12_state);
        }
        
        *key_down_index = 1;
    }
    else if (key21_state == 0 && key22_state == 0)
    {
        while(key22_state == 0)
        {
            key_get_status(2, &key22_state);
        }
        
        *key_down_index = 2;
    }
    else if (key31_state == 0 && key32_state == 0)
    {
        while(key32_state == 0)
        {
            key_get_status(3, &key32_state);
        }
        
        *key_down_index = 3;
    }
    else if (key41_state == 0 && key42_state == 0)
    {
        while(key42_state == 0)
        {
            key_get_status(4, &key42_state);
        }
        
        *key_down_index = 4;
    }
    else
    {
        *key_down_index = 0;
    }
}

void led_single_set_state(uint8_t index, uint8_t enable)
{
    uint8_t i = 0;
    
    for (i = 1; i <= 8; i++)
    {
        led_set_status(i, 0);
    }
    
    led_set_status(index, enable);
    
    led_lock_enable(1);
    led_lock_enable(0);
}

void led_all_on(void)
{
    uint8_t i = 0;
    
    for (i = 1; i <= 8; i++)
    {
        led_set_status(i, 1);
    }
    
    led_lock_enable(1);
    led_lock_enable(0);
}

void led_all_off(void)
{
    uint8_t i = 0;
    
    for (i = 1; i <= 8; i++)
    {
        led_set_status(i, 0);
    }

    led_lock_enable(1);
    led_lock_enable(0);
}

static uint8_t barrier_flag = 0;

static uint8_t get_barrier_flag(void)
{
    return barrier_flag;
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_PIN_1 == GPIO_Pin)
    {
        if (HAL_GPIO_ReadPin(GPIOA, GPIO_Pin) == 0)
        {
            barrier_flag = 1;
        }
        else
        {
            barrier_flag = 0;
        }
    }
}

void barrier_test(void)
{
    if (get_barrier_flag() == 1)
    {
        LCD_DisplayStringLine(Line4, (u8 *)"Barrier Detected!   ");
    }
    else
    {
        LCD_DisplayStringLine(Line4, (u8 *)"No Barrier Detected!");
    }
}

/* USER CODE END 2 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
