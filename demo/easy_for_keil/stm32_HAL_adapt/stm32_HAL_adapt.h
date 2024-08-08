/**
 * @file stm32_HAL_adapt.h
 * @author fool_dog (2696652257@qq.com)
 * @brief // 裸机的对接方式
 * @version 1.0
 * @date 2024-08-08
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "cmsis_compiler.h"
#include "usart.h"
#include "dma.h"

// 接收发送方式,不使用阻塞式发送接收,有RTOS可以考虑阻塞发送(可以但没必要),发送和接收的方式是可以组合使用的,
#define PLATFORM_MODE_DMA 0
#define PLATFORM_MODE_IT 1

// 使用DMA发送与接收
#define PLATFORM_MODE PLATFORM_MODE_DMA

struct shell_def; // shell的前置声明
void shell_lock(struct shell_def *shell);
void shell_unlock(struct shell_def *shell);
struct log_def; // log的前置声明
void log_lock(struct log_def *log);
void log_unlock(struct log_def *log);
// 加锁解锁的对应函数
#define SHELL_LOCK_FUNC shell_lock
#define SHELL_UNLOCK_FUNC shell_unlock
#define LOG_LOCK_FUNC log_lock
#define LOG_UNLOCK_FUNC log_unlock

//+********************************* 必须对接的宏 **********************************/
// 写入是否可以阻塞的判断函数,通常不在中断中就可以阻塞
#define SHELL_WRITE_CAN_BLOCK() (__get_IPSR() == 0)

#define WAIT_A_MOMENT() HAL_Delay(1) // 等待一段时间,外设跟不上内核填充的速度

// 原子操作保证数据的完整性,支持嵌套更好
#define SHELL_ATOMIC_ENTER() __disable_irq()
#define SHELL_ATOMIC_EXIT() __enable_irq()

// 平台初始化
extern void user_hardware_init();
#define PLATFORM_INIT() user_hardware_init()

#define SHELL_UART_ADDR (&huart1)
// clang-format off

#if (PLATFORM_MODE == PLATFORM_MODE_DMA)
    extern HAL_StatusTypeDef HAL_UART_Transmit_DMA(UART_HandleTypeDef *huart, const uint8_t *pData, uint16_t Size);
    extern HAL_StatusTypeDef HAL_UARTEx_ReceiveToIdle_DMA(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size);
    #define PLATFORM_TX_WRAP(buffer, len) HAL_UART_Transmit_DMA(SHELL_UART_ADDR, (uint8_t *)buffer, len)
    #define PLATFORM_RX_WRAP(buffer, len) HAL_UARTEx_ReceiveToIdle_DMA(SHELL_UART_ADDR, (uint8_t *)buffer, len)
#elif (PLATFORM_MODE == PLATFORM_MODE_IT)
    extern HAL_StatusTypeDef HAL_UART_Transmit_IT(UART_HandleTypeDef *huart, const uint8_t *pData, uint16_t Size);
    extern HAL_StatusTypeDef HAL_UART_Receive_IT(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size);
    #define PLATFORM_TX_WRAP(buffer, len) HAL_UART_Transmit_IT(SHELL_UART_ADDR, (uint8_t *)buffer, len)
    // 中断接收改成单个字节接收
    #define PLATFORM_RX_WRAP(buffer, len) HAL_UART_Receive_IT(SHELL_UART_ADDR, (uint8_t *)buffer, 1)
#endif

// clang-format on

#ifndef SHELL_GET_TICK
/**
 * @brief 获取系统时间(ms)
 *        定义此宏为获取系统Tick，如`HAL_GetTick()`
 * @note 此宏不定义时无法使用双击tab补全命令help，无法使用shell超时锁定
 */
extern uint32_t HAL_GetTick(void);
#define SHELL_GET_TICK() HAL_GetTick()
#endif /** SHELL_GET_TICK */
