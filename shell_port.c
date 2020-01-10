/**
 * @file shell_port.c
 * @author EtcFly (EtcFlywy@126.com)
 * @brief Portable interface for letter shell
 * @version 1.0.0
 * @date 2020-01-10
 * 
 * @Copyright (c) 2019 Letter
 * 
 */
#include "stm32f1xx_hal.h"
#include "shell.h"



extern UART_HandleTypeDef huart2;

static SHELL_TypeDef *pDefault_shell;
static SHELL_TypeDef default_shell;


/**
 * @brief shell底层写字节函数
 * 
 * @param ch 待发送字节
 * @return None
 */
static void shell_write_byte(char ch)
{
	HAL_UART_Transmit(&huart2, (void *)&ch, 1, 1000);
}


/**
 * @brief shell底层读字节函数
 * 
 * @param ch 待读取字节指针
 * @return
 *       -1: fail    0: success
 */
static signed char shell_read_byte(char *ch)
{
	if (HAL_UART_Receive(&huart2, (void *)ch, 1, 50) != HAL_OK)
		return -1;
	return 0;
}

/**
 * @brief shell组件初始化
 * 
 * @param None
 * @return 
 *       -1: fail    0: success
 */
void shell_initAll(void)
{
    default_shell.write = shell_write_byte;
	default_shell.read = shell_read_byte;
	
	pDefault_shell = &default_shell;
	
	shellInit(pDefault_shell);
}

/**
 * @brief 设置默认使用的shell
 * 
 * @param shell 待设置的默认shell指针 
 * @return None
 */
void set_defaultShell(SHELL_TypeDef *shell)
{
	if (NULL == shell)
	{
		return ;
	}
	pDefault_shell = (SHELL_TypeDef *)shell;
}

/**
 * @brief 获取默认使用的shell指针
 * 
 * @param None
 * @return 默认使用的shell指针
 */
SHELL_TypeDef *get_default_shell(void)
{
	return (void *)pDefault_shell;
}


