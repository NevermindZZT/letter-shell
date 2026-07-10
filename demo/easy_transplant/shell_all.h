/**
 * @file shell_all.h
 * @author 独霸一方 (2696652257@qq.com)
 * @brief //> 这个文件将包含letter shell的所有头文件实现(相对路径),方便在Keil工程中使用
 * @version 1.0
 * @date 2024-08-05
 *
 * @copyright Copyright (c) 2024
 *
 */

// 此文件使用相对路径,保证在任何地方都可以使用

// > 单次包含宏定义
#ifndef __SHELL_ALL_H_
#define __SHELL_ALL_H_

//> 平台适配的头文件必须放在最前面,确保宏定义的预处理定义正确
//> 如果有自己的实现请替换为对应自己的头文件
#include "./stm32_HAL_adapt/stm32_HAL_adapt.h"

#include "shell_cfg_user.h" // 使用本文件目录下的自定义配置文件

//> 环形队列用来等待接收和发送的数据,这样就可以在中断中调用shell的读写函数
#define SHELL_RX_BUFFER_SIZE 512
#define SHELL_TX_BUFFER_SIZE 512

// 连续发送与接收宏定义,当使用阻塞式接收发送时必须关闭指定的宏定义
#define SHELL_TX_CONTINUOUSLY 1 // 使能连续发送(end中继续检测是否还有数据需要发送)
#define SHELL_RX_CONTINUOUSLY 1 // 使能连续接收(end中继续检测是否还有空间可以接收)

// shell 写入溢出的时候可以调用钩子函数进行一些处理
#ifndef WRITE_OVER_FLOW_HOOK
#define WRITE_OVER_FLOW_HOOK(data, len) (void)0
#endif

#if !defined(PLATFORM_TX_WRAP) || !defined(PLATFORM_RX_WRAP)
#error "请实现自己的接收与发送函数对接"
#error "please implement your own receive and send function docking"
#endif

// > C/C++兼容性宏定义
#ifdef __cplusplus
extern "C"
{
#endif

    //+******************************** C scope ***************************************/

#include "./ringbuffer/ringbuffer.h" // 环形队列

#include "../../src/shell.h"

// shell核心头文件
#include "../../src/shell_cfg.h"
#include "../../src/shell.h"
#include "../../src/shell_ext.h"

// shell增强组件
#include "../../extensions/shell_enhance/shell_cmd_group.h"   // 命令组
#include "../../extensions/shell_enhance/shell_passthrough.h" // 透传
#include "../../extensions/shell_enhance/shell_secure_user.h" // 安全用户

    // #undef  LOG_ENABLE
    // #define LOG_ENABLE  0  //>在对应的C文件中使用此定义即可关闭当前文件的日志输出,需要加在文件最前面,直接打开这里的注释是关闭所有文件的日志输出

// shell log组件
#include "../../extensions/log/log.h" // 日志

    //// shell fs组件
    // #include "../../extensions/fs_support/shell_fs.h" // 文件命令

    //// shell telnet组件
    // #include "../../extensions/telnet/telnetd.h" // telnet

    void port_tx_trigger(void);            // 触发发送,定期调用
    void port_tx_end(short truely_tx_len); // 实际发送完后一定要调用,发送中断中调用(如果有的话),end函数如果传入负数表示使用上次调用时候记录的值
    int port_tx_available(void);           // 获取发送缓冲区已有的数据量

    void port_rx_trigger(void);            // 触发接收,定期调用
    void port_rx_end(short truely_rx_len); // 实际接收完后一定要调用,接收中断中调用(如果有的话),end函数如果传入负数表示使用上次调用时候记录的值
    int port_rx_available(void);           // 获取接收缓冲区已有的数据量,可以根据此函数判断是否需要调用shell任务,对于RTOS可以在无数据时释放CPU

    // 初始化shell和log
    void letter_shell_init(void);

    // shell任务
    void letter_shell_task(void);

    // shell写入函数,用于向shell写入数据,返回实际写入的字节数
    int shell_write(char *data, int len);

#define USE_SHELL_PRINTF 1
#if USE_SHELL_PRINTF
    // 代替标准库的printf,shell_printf使用xprintf的格式化输出,以源码形式添加编译
    int shell_printf(const char *format, ...);
#define SHELL_PRINTF
#endif

#ifdef __cplusplus
}
//+******************************** c++ scope ***************************************/
// shell C++支持组件
#include "../../extensions/cpp_support/shell_cpp.h" // C++命令

#endif //\ __cplusplus

#endif //\ __SHELL_ALL_H_
