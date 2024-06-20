/**
 * @file shell_port.c
 * @author Shellever (shellever@163.com)
 * @brief
 * @version 0.1
 * @date 2024-06-20
 *
 * @copyright (c) 2019 Letter
 *
 */

#include "main.h"
#include "usart.h"

#include "shell.h"
#include "shell_port.h"


Shell shell;
char shellBuffer[512];


/**
 * @brief 用户shell写
 *
 * @param data 数据
 * @param len 数据长度
 *
 * @return short 实际写入的数据长度
 */
short userShellWrite(char *data, unsigned short len)
{
	  HAL_UART_Transmit(&huart1, (uint8_t *)data, len, 200);		// 太小的话会导致超时，只有前面部分显示，显示不全

    return len;
}


/**
 * @brief 用户shell读
 *
 * @param data 数据
 * @param len 数据长度
 *
 * @return short 实际读取到
 */
short userShellRead(char *data, unsigned short len)
{
	  if (HAL_UART_Receive(&huart1, (uint8_t *)data, len, 200) == HAL_OK) {
			return len;
		}

    return 0;
}


/**
 * @brief 用户shell初始化
 *
 */
void userShellInit(void)
{
    shell.write = userShellWrite;
    shell.read = userShellRead;

    shellInit(&shell, shellBuffer, sizeof(shellBuffer));
}


