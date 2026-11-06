/**
 * @file telnetd_stub.c
 * @author Letter (NevermindZZT@gmail.com)
 * @brief telnet server 空实现，替代 extensions/telnet/telnetd.c
 *        该文件依赖 POSIX socket 接口（<sys/socket.h>），MinGW 下不可用
 */

#include "telnetd.h"

/**
 * @brief telnet server初始化
 *
 * @param newThreadInterface 新线程接口
 * @return int -1 未实现
 */
int telentdInit(NewThread newThreadInterface)
{
    (void)newThreadInterface;
    return -1;
}

/**
 * @brief 启动 telnet server
 *
 * @return int -1 未实现
 */
int telnetdStart()
{
    return -1;
}

/**
 * @brief 停止 telnet server
 *
 */
void telnetdStop()
{
}