/**
 * @file stm32_HAL_adapt.h
 * @author fool_dog (2696652257@qq.com)
 * @brief // STM32HAL库 裸机与FreeRTOS的对接方式
 * @version 1.0
 * @date 2024-08-08
 *
 * @copyright Copyright (c) 2024
 *
 */

// > 单次包含宏定义
#ifndef _STM32_HAL_ADAPT_H_
#define _STM32_HAL_ADAPT_H_

// clang-format off
/**
 * 此文件主要是必须对接的宏定义,包括:
 * #define PLATFORM_TX_WRAP(buffer, len) ...   //> 发送包装宏
 * #define PLATFORM_RX_WRAP(buffer, len) ...   //> 接收包装宏
 * #define SHELL_GET_TICK() ...                //> 获取系统时间(ms)
 * 可选宏:
 * #define SHELL_WRITE_CAN_BLOCK() ...         //> 判定写入是否可以阻塞
 * #define WAIT_A_MOMENT() ...                 //> 等待一段时间(可选,默认等待1ms)
 * #define SHELL_ATOMIC_ENTER() ...            //> 进入原子操作
 * #define SHELL_ATOMIC_EXIT() ...             //> 退出原子操作
 * #define SHELL_UART_ADDR ...                 //> 取决于接收和发送宏的实现(可以不用串口)
 */

// clang-format on

#include "cmsis_compiler.h"
#include "usart.h"

// 接收发送方式,不使用阻塞式发送接收,有RTOS可以考虑阻塞发送(可以但没必要),发送和接收的方式是可以组合使用的需自行更改
#define PLATFORM_MODE_DMA 0  // DMA发送与接收
#define PLATFORM_MODE_IT 1   // 单字节中断发送与接收
#define PLATFORM_MODE_POLL 2 // 轮询发送与接收

// 使用DMA发送与接收
#define PLATFORM_MODE PLATFORM_MODE_DMA

#define RTOS_NONE 0     // 无操作系统
#define RTOS_FREERTOS 1 // FreeRTOS
// ... 其他操作系统

// 定义使用的操作系统
#define RTOS_MODE RTOS_NONE

//+********************************* 必须对接的宏 **********************************/
// 写入是否可以阻塞的判断函数,通常不在中断中就可以阻塞
#define SHELL_WRITE_CAN_BLOCK() (__get_IPSR() == 0)

#if (RTOS_MODE == RTOS_NONE)
#define WAIT_A_MOMENT() HAL_Delay(1) // 等待一段时间,外设跟不上内核填充的速度
#elif (RTOS_MODE == RTOS_FREERTOS)
#include "FreeRTOS.h"
#include "task.h"
#define WAIT_A_MOMENT() vTaskDelay(1)
#else
#warning "请实现自己的等待函数"
#warning "please implement your own wait function"
#endif

// 原子操作保证数据的完整性,支持嵌套更好
#define SHELL_ATOMIC_ENTER() __disable_irq()
#define SHELL_ATOMIC_EXIT() __enable_irq()

//> 默认使用串口1
#define SHELL_UART_ADDR (&huart1)
// clang-format off

#if (PLATFORM_MODE == PLATFORM_MODE_DMA)
#include "dma.h"
    extern HAL_StatusTypeDef HAL_UART_Transmit_DMA(UART_HandleTypeDef *huart, const uint8_t *pData, uint16_t Size);
    extern HAL_StatusTypeDef HAL_UARTEx_ReceiveToIdle_DMA(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size);
    #define PLATFORM_TX_WRAP(buffer, len) HAL_UART_Transmit_DMA(SHELL_UART_ADDR, (uint8_t *)buffer, len)
    #define PLATFORM_RX_WRAP(buffer, len) HAL_UARTEx_ReceiveToIdle_DMA(SHELL_UART_ADDR, (uint8_t *)buffer, len)
#elif (PLATFORM_MODE == PLATFORM_MODE_IT)
    extern HAL_StatusTypeDef HAL_UART_Transmit_IT(UART_HandleTypeDef *huart, const uint8_t *pData, uint16_t Size);
    extern HAL_StatusTypeDef HAL_UART_Receive_IT(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size);
    #define PLATFORM_TX_WRAP(buffer, len) HAL_UART_Transmit_IT(SHELL_UART_ADDR, (uint8_t *)buffer, len)
    // 中断接收改成单个字节接收,接收回调调用port_rx_end(1);表示接收到一个字节
    #define PLATFORM_RX_WRAP(buffer, len) HAL_UART_Receive_IT(SHELL_UART_ADDR, (uint8_t *)buffer, 1)
#elif (PLATFORM_MODE == PLATFORM_MODE_POLL)
#warning "强烈不建议使用阻塞式API发送与接收,建议使用DMA或者中断方式"
#warning "It is not recommended to use blocking API for sending and receiving. It is recommended to use DMA or interrupt mode"
    extern HAL_StatusTypeDef HAL_UART_Transmit(UART_HandleTypeDef *huart, const uint8_t *pData, uint16_t Size, uint32_t Timeout);
    extern HAL_StatusTypeDef HAL_UART_Receive(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size, uint32_t Timeout);
    #define PLATFORM_TX_WRAP(buffer, len) HAL_UART_Transmit(SHELL_UART_ADDR, (uint8_t *)buffer, len, 1000)
    #define PLATFORM_RX_WRAP(buffer, len) HAL_UART_Receive(SHELL_UART_ADDR, (uint8_t *)buffer, len, 1000)
#else
#error "请实现自己的接收与发送函数对接"
#error "please implement your own receive and send function docking"
#endif

// clang-format on

/**
 * @brief 获取系统时间(ms)
 *        定义此宏为获取系统Tick，如`HAL_GetTick()`
 * @note 此宏不定义时无法使用双击tab补全命令help，无法使用shell超时锁定
 */
extern uint32_t HAL_GetTick(void);
#define SHELL_GET_TICK() HAL_GetTick()

#endif //\ _STM32_HAL_ADAPT_H_
