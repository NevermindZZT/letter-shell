#include "app_all.h"
#include "thread_all.h"
#include <nx_api.h>
#include <tx_api.h>

#include "log.h"
#include "shell.h"

#define SHELL_UART_HANDLE	  huart4
#define LOG_QUEUE_BUFFER_SIZE (256 * 4)
#define MAX_COMMAND_LENGTH	  32
#define LETTERSHELL_BUF_SIZE  (MAX_COMMAND_LENGTH * (SHELL_HISTORY_MAX_NUMBER + 1))

extern SHELL_UART_HANDLE;

static uint8_t	 uartBuffer[1];									  // shell串口中断接收缓冲区
static TX_THREAD shellThreadHandle;								  // shell线程控制块句柄
static TX_QUEUE	 shellQueueHandle;								  // shell接收消息队列
static uint8_t	 shellQueueBuffer[16];							  // shell发送同步信号量缓冲区
static TX_MUTEX	 shellMutexHandle;								  // shell互斥锁
static uint8_t	 shellHistoryCommandBuffer[LETTERSHELL_BUF_SIZE]; // shell命令历史记录缓冲区

static TX_THREAD	logThreadHandle;															   // 日志打印线程句柄
TX_QUEUE			logQueueHandle;																   // log打印队列句柄
static uint8_t*		log_queue_buffer[LOG_QUEUE_BUFFER_SIZE];									   // log打印队列缓冲区
static TX_MUTEX		logMutexHandle;																   // log互斥锁
static TX_BYTE_POOL bpool_log_stack;															   // log内存控制块
static uint8_t		log_stack_memory[HEAP_ASYNC_LOG_SIZE] __attribute__((section(".DmaSection"))); // log内存缓冲区

static uint32_t userShellLock(struct shell_def* shell)
{
	while ( tx_mutex_get(&shellMutexHandle, TX_WAIT_FOREVER) != TX_SUCCESS ) {
		__asm("nop");
	}
	return 0;
}

static uint32_t userShellUnlock(struct shell_def* shell)
{
	tx_mutex_put(&shellMutexHandle);
	return 0;
}

static uint8_t userShellRead(char* data, unsigned short len)
{
	uint32_t tempBuffer[1];
	for ( int32_t i = 0; i < len; i++ ) {
		tx_queue_receive(&shellQueueHandle, tempBuffer, TX_WAIT_FOREVER);
		data[i] = tempBuffer[0];
	}

	return len;
}

static uint8_t userShellWrite(char* data, unsigned short len)
{
	uint8_t* pointer = TX_NULL;
	if ( tx_byte_allocate(&bpool_log_stack, (VOID**)&pointer, len + 1, TX_NO_WAIT) == TX_SUCCESS ) {
		memcpy(pointer, data, len);
		pointer[len] = '\0';
		if ( tx_queue_send(&logQueueHandle, &pointer, TX_NO_WAIT) == TX_SUCCESS ) {
			return len;
		} else {
			tx_byte_release(pointer);
			return 0;
		}
	} else {
		return 0;
	}
}

static Shell lx_shell = {
	.read  = userShellRead,
	.write = userShellWrite,
#if SHELL_USING_LOCK == 1
	.lock	= userShellLock,
	.unlock = userShellUnlock //

#endif /* SHELL_USING_LOCK */
};

static uint32_t userLogLock(struct log_def* log)
{
	while ( tx_mutex_get(&logMutexHandle, TX_WAIT_FOREVER) != TX_SUCCESS ) {
		__asm("nop");
	}
	return 0;
}

static uint32_t userLogUnlock(struct log_def* log)
{
	tx_mutex_put(&logMutexHandle);
	return 0;
}

static void userLogOutput(char* buffer, short len)
{
	shellWriteEndLine(&lx_shell, buffer, len);
}

static Log sLog = {
	.active = 1,
	.write	= userLogOutput,
#if LOG_USING_LOCK == 1
	.lock	= userLogLock,
	.unlock = userLogUnlock,
#endif /* LOG_USING_LOCK */

	.level = LOG_ALL //
};

static void openLog(void)
{
	tx_byte_pool_create(&bpool_log_stack, "log Stack", log_stack_memory, HEAP_ASYNC_LOG_SIZE);
	tx_queue_create(&logQueueHandle, "Log Queue", 1, (VOID*)log_queue_buffer, sizeof(log_queue_buffer));
	tx_mutex_create(&logMutexHandle, "log mutex", TX_INHERIT);
	logRegister(&sLog, &lx_shell);
}

static void openShell()
{
	tx_queue_create(&shellQueueHandle, "shell Queue", 1, shellQueueBuffer, 16);
	tx_mutex_create(&shellMutexHandle, "shell mutex", TX_INHERIT);
	shellInit(&lx_shell, shellHistoryCommandBuffer, LETTERSHELL_BUF_SIZE);
}

/**
 * @brief 启用shell(letter-shell)
 * @note 开启日志打印功能
 * @note 开启串口接收中断
 * @note 创建shell主线程
 * @note 创建日志打印线程
 */
uint32_t shell_application_define(TX_BYTE_POOL* pBpool_thread_stack)
{
	uint32_t status	 = TX_SUCCESS;
	uint8_t* pointer = TX_NULL;

	openShell();
	openLog();
	HAL_UART_Receive_IT(&SHELL_UART_HANDLE, uartBuffer, 1);

	/* shell task任务 */
	status += tx_byte_allocate(pBpool_thread_stack, (VOID**)&pointer, THREAD_SHELL_STACK_SIZE, TX_NO_WAIT);
	status += tx_thread_create(&shellThreadHandle,
							   THREAD_SHELL_NAME,
							   shellTask,
							   (ULONG)&lx_shell,
							   pointer,
							   THREAD_SHELL_STACK_SIZE,
							   THREAD_SHELL_PRIORITY,
							   THREAD_SHELL_PRIORITY,
							   TX_NO_TIME_SLICE,
							   TX_AUTO_START //
	);

	/* 初始化日志线程 */
	status += tx_byte_allocate(pBpool_thread_stack, (VOID**)&pointer, THREAD_LOG_STACK_SIZE, TX_NO_WAIT);
	status += tx_thread_create(&logThreadHandle,
							   THREAD_LOG_NAME,
							   thread_logPrint_entry,
							   NULL,
							   pointer,
							   THREAD_LOG_STACK_SIZE,
							   THREAD_LOG_PRIORITY,
							   THREAD_LOG_PRIORITY,
							   TX_NO_TIME_SLICE,
							   TX_AUTO_START //
	);

	return status;
}

void shellUartReceiveCallback()
{
	uint32_t tempBuffer[1] = { uartBuffer[0] };
	tx_queue_send(&shellQueueHandle, tempBuffer, TX_NO_WAIT);
	HAL_UART_Receive_IT(&SHELL_UART_HANDLE, uartBuffer, 1);
}

static void logSwitchLevel()
{
	sLog.level = (LogLevel)((sLog.level + 1) % (LOG_ALL + 1));
	logPrintln("set log level: %02d", sLog.level);
}
SHELL_EXPORT_KEY(SHELL_CMD_PERMISSION(0),
				 0x04000000,
				 logSwitchLevel,
				 switch log level //
);
