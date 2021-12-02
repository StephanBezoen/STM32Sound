/**
  ******************************************************************************
  * @file    usart.c
  * @brief   This file provides code for the configuration
  *          of the USART instances.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
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
#include "usart.h"

/* USER CODE BEGIN 0 */
char messageBuffer[PENDING_MESSAGES_BUFFER_SIZE];
uint readPointer = 0;
uint writePointer = 0;

/* USER CODE END 0 */

UART_HandleTypeDef huart2;

/* USART2 init function */

void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(uartHandle->Instance==USART2)
  {
  /* USER CODE BEGIN USART2_MspInit 0 */

  /* USER CODE END USART2_MspInit 0 */
    /* USART2 clock enable */
    __HAL_RCC_USART2_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**USART2 GPIO Configuration
    PA2     ------> USART2_TX
    PA3     ------> USART2_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* USART2 interrupt Init */
    HAL_NVIC_SetPriority(USART2_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USART2_IRQn);
  /* USER CODE BEGIN USART2_MspInit 1 */

  /* USER CODE END USART2_MspInit 1 */
  }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{

  if(uartHandle->Instance==USART2)
  {
  /* USER CODE BEGIN USART2_MspDeInit 0 */

  /* USER CODE END USART2_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART2_CLK_DISABLE();

    /**USART2 GPIO Configuration
    PA2     ------> USART2_TX
    PA3     ------> USART2_RX
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_2|GPIO_PIN_3);

    /* USART2 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USART2_IRQn);
  /* USER CODE BEGIN USART2_MspDeInit 1 */

  /* USER CODE END USART2_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */

void checkSendNextMessage()
{
  if (huart2.gState != HAL_UART_STATE_READY) // Transmission in progress
    return;

  if (readPointer == writePointer) // All messages sent
    return;

  int messageStart = (uint)messageBuffer + readPointer;
  
  int messageLength = 0;
  if (readPointer > writePointer) // Read pointer is beyond write pointer, we'll do this in 2 batches
  {
    messageLength = PENDING_MESSAGES_BUFFER_SIZE - readPointer;
    readPointer = 0;
  }
  else  // Read pointer is before write pointer, send all bytes in between
  {
    messageLength = writePointer - readPointer;
    readPointer = writePointer;
  }

  HAL_UART_Transmit_IT(&huart2, (uint8_t *)messageStart, messageLength);
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
  checkSendNextMessage();
}

void appendMessage(char *message)
{
  int messageLength = strlen(message);
  int bufferSizeLeft = (PENDING_MESSAGES_BUFFER_SIZE - writePointer);
  if (messageLength > bufferSizeLeft)
  {
    int len1 = bufferSizeLeft;
    int len2 = messageLength - bufferSizeLeft;
    memcpy(messageBuffer + writePointer, message, len1);
    memcpy(messageBuffer, message + len1, len2);
    writePointer = len2;
  }
  else
  {
    memcpy(messageBuffer + writePointer, message, messageLength);
    writePointer += messageLength;
  }
}

void writeLog(char text[])
{
  appendMessage(text);

  char buffer[32];
  snprintf(buffer, sizeof(buffer), " - %8ld", HAL_GetTick());
  appendMessage(buffer);

  appendMessage("\r\n");

  checkSendNextMessage();
}

/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/