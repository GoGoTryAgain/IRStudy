/**
  ******************************************************************************
  * File Name          : USART.c
  * Description        : This file provides code for the configuration
  *                      of the USART instances.
  ******************************************************************************
  ** This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * COPYRIGHT(c) 2018 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "usart.h"

#include "gpio.h"
#include "dma.h"

/* USER CODE BEGIN 0 */
static uart_param_t usart1Parm;
static uart_param_t usart2Parm;


FIFO_Queue_t FIFO_uart1;
FIFO_Queue_t FIFO_uart2;


uint8_t FIFO_buf_Uart1[FIFO_SIZE] = {0};
uint8_t FIFO_buf_Uart2[FIFO_SIZE] = {0};


static uint8_t Uart1Txbuf[MAX_TX_BUF] = {0};
static uint8_t Uart1Rxbuf[MAX_RX_BUF] = {0};

static uint8_t Uart2Txbuf[MAX_TX_BUF] = {0};
static uint8_t Uart2Rxbuf[MAX_RX_BUF] = {0};

/* USER CODE END 0 */

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;
DMA_HandleTypeDef hdma_usart1_tx;
DMA_HandleTypeDef hdma_usart1_rx;
DMA_HandleTypeDef hdma_usart2_tx;
DMA_HandleTypeDef hdma_usart2_rx;




void SetUartRXCB(uart_param_t *uartparam,UartRXCallBack callbcak);
void Uart1RxHandle(void);
void Uart2RxHandle(void);

/* USART1 init function */

void MX_USART1_UART_Init(void)
{

  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}
/* USART2 init function */

void MX_USART2_UART_Init(void)
{

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
    _Error_Handler(__FILE__, __LINE__);
  }

}

void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct;
  if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspInit 0 */

  /* USER CODE END USART1_MspInit 0 */
    /* USART1 clock enable */
    __HAL_RCC_USART1_CLK_ENABLE();
  
    /**USART1 GPIO Configuration    
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX 
    */
    GPIO_InitStruct.Pin = DebugTX_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(DebugTX_GPIO_Port, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = DebugRX_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(DebugRX_GPIO_Port, &GPIO_InitStruct);

    /* USART1 DMA Init */
    /* USART1_TX Init */
    hdma_usart1_tx.Instance = DMA1_Channel4;
    hdma_usart1_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_usart1_tx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_usart1_tx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_usart1_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart1_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_usart1_tx.Init.Mode = DMA_NORMAL;
    hdma_usart1_tx.Init.Priority = DMA_PRIORITY_LOW;
    if (HAL_DMA_Init(&hdma_usart1_tx) != HAL_OK)
    {
      _Error_Handler(__FILE__, __LINE__);
    }

    __HAL_LINKDMA(uartHandle,hdmatx,hdma_usart1_tx);

    /* USART1_RX Init */
    hdma_usart1_rx.Instance = DMA1_Channel5;
    hdma_usart1_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_usart1_rx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_usart1_rx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_usart1_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart1_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_usart1_rx.Init.Mode = DMA_NORMAL;
    hdma_usart1_rx.Init.Priority = DMA_PRIORITY_LOW;
    if (HAL_DMA_Init(&hdma_usart1_rx) != HAL_OK)
    {
      _Error_Handler(__FILE__, __LINE__);
    }

    __HAL_LINKDMA(uartHandle,hdmarx,hdma_usart1_rx);

    /* USART1 interrupt Init */
    HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USART1_IRQn);
  /* USER CODE BEGIN USART1_MspInit 1 */

  /* USER CODE END USART1_MspInit 1 */
  }
  else if(uartHandle->Instance==USART2)
  {
  /* USER CODE BEGIN USART2_MspInit 0 */

  /* USER CODE END USART2_MspInit 0 */
    /* USART2 clock enable */
    __HAL_RCC_USART2_CLK_ENABLE();
  
    /**USART2 GPIO Configuration    
    PA2     ------> USART2_TX
    PA3     ------> USART2_RX 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_2;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* USART2 DMA Init */
    /* USART2_TX Init */
    hdma_usart2_tx.Instance = DMA1_Channel7;
    hdma_usart2_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_usart2_tx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_usart2_tx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_usart2_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart2_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_usart2_tx.Init.Mode = DMA_NORMAL;
    hdma_usart2_tx.Init.Priority = DMA_PRIORITY_LOW;
    if (HAL_DMA_Init(&hdma_usart2_tx) != HAL_OK)
    {
      _Error_Handler(__FILE__, __LINE__);
    }

    __HAL_LINKDMA(uartHandle,hdmatx,hdma_usart2_tx);

    /* USART2_RX Init */
    hdma_usart2_rx.Instance = DMA1_Channel6;
    hdma_usart2_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_usart2_rx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_usart2_rx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_usart2_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart2_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_usart2_rx.Init.Mode = DMA_NORMAL;
    hdma_usart2_rx.Init.Priority = DMA_PRIORITY_LOW;
    if (HAL_DMA_Init(&hdma_usart2_rx) != HAL_OK)
    {
      _Error_Handler(__FILE__, __LINE__);
    }

    __HAL_LINKDMA(uartHandle,hdmarx,hdma_usart2_rx);

    /* USART2 interrupt Init */
    HAL_NVIC_SetPriority(USART2_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USART2_IRQn);
  /* USER CODE BEGIN USART2_MspInit 1 */

  /* USER CODE END USART2_MspInit 1 */
  }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{

  if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspDeInit 0 */

  /* USER CODE END USART1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART1_CLK_DISABLE();
  
    /**USART1 GPIO Configuration    
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX 
    */
    HAL_GPIO_DeInit(GPIOA, DebugTX_Pin|DebugRX_Pin);

    /* USART1 DMA DeInit */
    HAL_DMA_DeInit(uartHandle->hdmatx);
    HAL_DMA_DeInit(uartHandle->hdmarx);

    /* USART1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USART1_IRQn);
  /* USER CODE BEGIN USART1_MspDeInit 1 */

  /* USER CODE END USART1_MspDeInit 1 */
  }
  else if(uartHandle->Instance==USART2)
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

    /* USART2 DMA DeInit */
    HAL_DMA_DeInit(uartHandle->hdmatx);
    HAL_DMA_DeInit(uartHandle->hdmarx);

    /* USART2 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USART2_IRQn);
  /* USER CODE BEGIN USART2_MspDeInit 1 */

  /* USER CODE END USART2_MspDeInit 1 */
  }
} 

/* USER CODE BEGIN 1 */

int fputc(int ch ,FILE *s)
{
	while(huart1.gState != HAL_UART_STATE_READY);
	HAL_UART_Transmit_DMA(&huart1,(uint8_t*)&ch,1);
	return 1;
}


void Uart1Init(void)
{
	usart1Parm.huart = &huart1;
	usart1Parm.Txparam.BUFFSIZE = MAX_TX_BUF;
	usart1Parm.Txparam.txbuf = Uart1Txbuf;
	usart1Parm.Txparam.TxCb = NULL;
	
	
	usart1Parm.Rxparam.BuffSize = MAX_RX_BUF;
	usart1Parm.Rxparam.rxbuf = Uart1Rxbuf;
	usart1Parm.Rxparam.RxCb = NULL;
	
	FIFO_uart1.rx.BufAddr = FIFO_buf_Uart1;
	FIFO_uart1.rx.WritePtr = FIFO_uart1.rx.BufAddr;
	FIFO_uart1.rx.ReadPtr = FIFO_uart1.rx.BufAddr;
	
	SetUartRXCB(&usart1Parm,Uart1RxHandle);
	
	__HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE); 
	HAL_UART_Receive_DMA(&huart1,usart1Parm.Rxparam.rxbuf,MAX_RX_BUF);
}

void Uart2Init(void)
{
	usart2Parm.huart = &huart2;
	usart2Parm.Txparam.BUFFSIZE = MAX_TX_BUF;
	usart2Parm.Txparam.txbuf = Uart2Txbuf;
	usart2Parm.Txparam.TxCb = NULL;
	
	
	usart2Parm.Rxparam.BuffSize = MAX_RX_BUF;
	usart2Parm.Rxparam.rxbuf = Uart2Rxbuf;
	usart2Parm.Rxparam.RxCb = NULL;
	
	FIFO_uart2.rx.BufAddr = FIFO_buf_Uart2;
	FIFO_uart2.rx.WritePtr = FIFO_uart2.rx.BufAddr;
	FIFO_uart2.rx.ReadPtr = FIFO_uart2.rx.BufAddr;
	
	SetUartRXCB(&usart2Parm,Uart2RxHandle);
	
	__HAL_UART_ENABLE_IT(&huart2, UART_IT_IDLE); 
	HAL_UART_Receive_DMA(&huart2,usart2Parm.Rxparam.rxbuf,MAX_RX_BUF);
}


void Bsp_uartInit(void)
{
	

	
}

//set rx callback function
void SetUartRXCB(uart_param_t *uartparam,UartRXCallBack callbcak)
{
	uartparam->Rxparam.RxCb = callbcak;
}


//set tx callback function
void SetUartTXCB(uart_param_t *uartparam,UartTXCallBack callbcak)
{
	uartparam->Txparam.TxCb = callbcak;
}

//rx
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	
	if(huart == &huart1)
	{
		if(usart1Parm.Rxparam.RxCb != NULL)
			usart1Parm.Rxparam.RxCb();
	}
	if(huart == &huart2)
	{
		if(usart1Parm.Rxparam.RxCb != NULL)
			usart1Parm.Rxparam.RxCb();
	}
	
	
}



void Uart1RxHandle()
{
//	uint32_t tmp_flag = 0;
	uint32_t temp;
	uint32_t Len;
//	temp = huart1.Instance->SR;  
//	temp = huart1.Instance->DR; 
	HAL_UART_DMAStop(&huart1);
	temp  = hdma_usart1_rx.Instance->CNDTR;
	
	Len =  MAX_RX_BUF - temp; 
	FIFO_uart1.rx.RxLen = Len + FIFO_uart1.rx.RxLen > FIFO_SIZE ? FIFO_SIZE : (Len + FIFO_uart1.rx.RxLen);
	FIFO_uart1.rx.WritePtr = (FIFO_uart1.rx.WritePtr - FIFO_uart1.rx.BufAddr + FIFO_uart1.rx.RxLen) %
														FIFO_SIZE + FIFO_uart1.rx.BufAddr;
	
	HAL_UART_Receive_DMA(&huart1,usart1Parm.Rxparam.rxbuf,MAX_RX_BUF);
	
}

void Uart2RxHandle(void)
{
//	uint32_t tmp_flag = 0;
	uint32_t temp;
	uint32_t Len;
//	temp = huart1.Instance->SR;  
//	temp = huart1.Instance->DR; 
	HAL_UART_DMAStop(&huart1);
	temp  = hdma_usart2_rx.Instance->CNDTR;
	
	Len =  MAX_RX_BUF - temp; 
	FIFO_uart2.rx.RxLen = Len + FIFO_uart2.rx.RxLen > FIFO_SIZE ? FIFO_SIZE : (Len + FIFO_uart2.rx.RxLen);
	FIFO_uart2.rx.WritePtr = (FIFO_uart1.rx.WritePtr - FIFO_uart1.rx.BufAddr + FIFO_uart1.rx.RxLen) %
														FIFO_SIZE + FIFO_uart1.rx.BufAddr;

	HAL_UART_Receive_DMA(&huart1,usart1Parm.Rxparam.rxbuf,MAX_RX_BUF);
	
}






/* USER CODE END 1 */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
