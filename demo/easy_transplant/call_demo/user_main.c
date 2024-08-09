/**
 * @file user_main.c
 * @author fool_dog (2696652257@qq.com)
 * @brief //> 此文件仅用于展示如何调用shell的逻辑,不要添加此文件进行编译
 * @version 1.0
 * @date 2024-08-09
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "../shell_all.h"

int user_main(void)
{
    letter_shell_init(); // 初始化shell

    for (;;)
    {
        letter_shell_task(); // 处理shell任务

        // 建议隔一定时间调用这两个函数,可以在中断中调用,也可以在主循环中调用
        {
            port_tx_trigger();
            port_rx_trigger();
        }
    }
}

//+********************************* 通常下面是接收和发送中断 **********************************/
void rx_end_callback()
{
    // 在接收函数回调中或者实际接收到数据后调用
    port_rx_end(-1); // 如果知道实际接收了多少个字节就传入实际的字节数,不然传入负数让其自动根据上次调用是传入的大小进行处理
}

void tx_end_callback()
{
    // 在发送函数回调中,或者实际发送数据后调用
    port_tx_end(-1); // 如果知道实际发送了多少个字节就传入实际的字节数,不然传入负数让其自动根据上次调用是传入的大小进行处理
}