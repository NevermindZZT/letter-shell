#include "thread_all.h"

#include "log.h"
#include "shell.h"
#include "stm32h7xx_hal.h"
#include "tx_api.h"
#include "usart.h"
#include <stdio.h>


extern TX_QUEUE logQueueHandle;

static uint8_t*		pBuffer;
static TX_SEMAPHORE logSemaphoreHandle;

/**
 * @brief 日志打印线程
 * @return {*}
 * @note 系统初始化错误次数打印
 * @note 系统时钟频率打印
 */
void thread_logPrint_entry()
{
	tx_semaphore_create(&logSemaphoreHandle, "log Semaphore", 1);
	while ( 1 ) {
		if ( tx_semaphore_get(&logSemaphoreHandle, TX_WAIT_FOREVER) == TX_SUCCESS ) {
			tx_byte_release(pBuffer);
			tx_queue_receive(&logQueueHandle, &pBuffer, TX_WAIT_FOREVER);
			SCB_CleanDCache_by_Addr(pBuffer, strlen(pBuffer));
			HAL_UART_Transmit_DMA(&huart4, pBuffer, strlen(pBuffer));
		}
	}
}

void shellUartTransmitCallback()
{
	tx_semaphore_put(&logSemaphoreHandle);
}
