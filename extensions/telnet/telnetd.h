/**
 * @file telnetd.h
 * @author Letter
 * @brief telnet server for letter shell
 * @version 0.1
 * @date 2021-08-07
 * 
 * @copyright (c) 2021 Letter
 * 
 */
#ifndef __TELNETD_H__
#define __TELNETD_H__

/**
 * @brief 版本
 */
#define TELNET_VERSION              "1.0.0"

/**
 * @brief telnetd shell伴生对象ID
 */
#define SHELL_COMPANION_ID_TELNETD  -3

/**
 * @brief telnet server 监听的IP
 */
#define TELNETD_SERVER_ADDRESS      "127.0.0.1"

/**
 * @brief telnet 默认监听端口
 */
#define TELNETD_DEFAULT_SERVER_PORT 23

/**
 * @brief telnet shell的数据缓冲区大小
 */
#define TELNETD_SHELL_BUFFER_SIZE   512

/**
 * @brief telnet shell的用户名，使用默认shell用户设置为NULL即可
 */
#define TELNETD_SHELL_USER          NULL

/**
 * @brief 新线程接口
 * 
 * @param void * 线程执行函数
 * @param void * 线程参数
 * 
 * @reutrn int 0 启动线程成功 -1 启动线程失败
 */
typedef int (*NewThread)(void *, void *);

/**
 * @brief telnet server初始化
 * 
 * @param newThreadInterface 新线程接口 
 * 
 * @return int 0 启动telent server成功 -1 启动失败
 */
int telentdInit(NewThread newThreadInterface);

/**
 * @brief 启动 telnet server
 * 
 * @return int 0 启动telent成功 -1 启动失败
 */
int telnetdStart();

/**
 * @brief 停止 telnet server
 * 
 */
void telnetdStop();

#endif
