#include "shell_all.h"

#include <stdbool.h>
#include <stdint.h>

// 将所有的源码文件加入到这一个文件中,方便Keil工程的添加

#if 1

#include "./ringbuffer/ringbuffer.c" // 环形队列

// shell核心实现包含
#include "../../src/shell.c"
#include "../../src/shell_ext.c"
#include "../../src/shell_cmd_list.c"
#include "../../src/shell_companion.c"

// shell拓展组件

// shell增强组件
#include "../../extensions/shell_enhance/shell_cmd_group.c"   // 命令组
#include "../../extensions/shell_enhance/shell_passthrough.c" // 透传
#include "../../extensions/shell_enhance/shell_secure_user.c" // 安全用户

// shell log组件
#include "../../extensions/log/log.c" // 日志

//// shell fs组件
// #include "../../extensions/fs_support/shell_fs.c" // 文件命令

//// shell telnet组件
// #include "../../extensions/telnet/telnetd.c" // telnet,需要命令组拓展(注意头文件路径包含)

#endif

//// shell game组件需要单独添加.c到工程中,不然会有一些同名函数的问题
// #include "../../extensions/game/game.c"            // 游戏,需要命令组拓展(注意头文件路径包含)
// #include "../../extensions/game/2048/2048.c"       // 2048
// #include "../../extensions/game/pushbox/pushbox.c" // 贪吃蛇

//+********************************* 环形队列中转 **********************************/
#if SHELL_SHOW_INFO == 1
#if SHELL_TX_BUFFER_SIZE < 400
#warning "建议将环形队列的大小设置为大于400,否则打印初始化信息的时候可能会出现部分打印数据丢失"
#warning "shell tx buffer size is too small, please increase the buffer size"
#endif
#endif

// 定义shell和log对象
static Shell user_shell;
static Log user_log;

// 发送和接收的中转环形缓冲队列
static ringbuffer_t rb_rx;
static ringbuffer_t rb_tx;

// 写入是否可以阻塞,默认不可以阻塞,一般判断是否在中断中就可以判断是否可以阻塞
#ifndef SHELL_WRITE_CAN_BLOCK
#define SHELL_WRITE_CAN_BLOCK() (false)
#endif // !SHELL_WRITE_CAN_BLOCK

//+********************************* shell读写对接 **********************************/

short write_to_ringbuffer(char *data, unsigned short len)
{
    short total_written = 0;
    short ret = 0;

    while (len > 0)
    {
        SHELL_ATOMIC_ENTER();

        ret = ringbuffer_write(&rb_tx, (uint8_t *)data, len);

        SHELL_ATOMIC_EXIT();

        if (ret >= len) // 只可能小于等于
        {
            return total_written; // 全部数据写入成功
        }
        else
        {
            if (!SHELL_WRITE_CAN_BLOCK())
            {
                total_written += ret;
                break; // 如果在中断中,直接退出,未写入的数据直接丢弃,避免长时间占用中断
            }

            total_written += ret;
            data += ret;
            len -= ret;
            port_tx_trigger(); // 主动尝试触发发送

            WAIT_A_MOMENT(); // 等待一段时间,避免频繁触发发送
        }
    }

    return total_written;
}

short read_from_ringbuffer(char *data, unsigned short len)
{
    short ret = 0;
    SHELL_ATOMIC_ENTER();

    ret = ringbuffer_read(&rb_rx, (uint8_t *)data, len);

    SHELL_ATOMIC_EXIT();
    return ret;
}

//+******************************** log组件对接 ***************************************/

void UserLogWrite(char *buffer, short len)
{
#if SHELL_SUPPORT_END_LINE == 1
    USER_ASSERT(user_log.shell != NULL);
    shellWriteEndLine(user_log.shell, buffer, len);
#else
    USER_ASSERT(user_log.shell != NULL);
    USER_ASSERT(user_log.shell->write != NULL);
    user_log.shell->write(buffer, len);
#endif
}

//+******************************** 对外调用接口 ***************************************/
void port_tx_trigger(void)
{
    if (ringbuffer_is_busy(&rb_tx) || ringbuffer_get_used(&rb_tx) <= 0)
    {
        return; // 没有数据或者外设还在响应上一次的发送
    }

    // 这两个栈变量可能会在极端情况下破坏原子操作性,改成static然后提到函数开头效果可能会更好
    uint16_t size;
    uint8_t *p;

    SHELL_ATOMIC_ENTER();

    // 获取ringbuffer当前的读指针和最大线性可读数据大小
    ringbuffer_mark_busy(&rb_tx); // 标记为忙,调用port_tx_end函数后再标记为闲
    p = ringbuffer_linear_read_setup(&rb_tx, &size);

    SHELL_ATOMIC_EXIT();

    // 发送数据,调用平台相关的发送函数
    PLATFORM_TX_WRAP(p, size);
}

void port_rx_trigger(void)
{

    if (ringbuffer_is_busy(&rb_rx) || ringbuffer_get_free(&rb_rx) <= 0) // 性能优化,对于接收,外设忙的情况更多先判断是否在忙
    {
        return; // 没有空间接收或者正在接收数据
    }

    // 这两个栈变量可能会在极端情况下破坏原子操作性,改成static然后提到函数开头效果可能会更好
    uint16_t size;
    uint8_t *p;

    SHELL_ATOMIC_ENTER();
    ringbuffer_mark_busy(&rb_rx); // 标记为忙,调用port_rx_end函数后再标记为闲
    // 获取ringbuffer当前的写指针和最大线性可写数据大小
    p = ringbuffer_linear_write_setup(&rb_rx, &size);

    SHELL_ATOMIC_EXIT();

    // 接收数据,调用平台相关的接收函数
    PLATFORM_RX_WRAP(p, size);
}

// 通常用于中断中调用
void port_tx_end(short truely_tx_len)
{
    SHELL_ATOMIC_ENTER();
    if (truely_tx_len < 0)
    {
        truely_tx_len = ringbuffer_get_last_series_size(&rb_tx);
    }

    ringbuffer_linear_read_done(&rb_tx, truely_tx_len);
    ringbuffer_mark_idle(&rb_tx);

    SHELL_ATOMIC_EXIT();

#if SHELL_TX_CONTINUOUSLY
    port_tx_trigger(); // 继续发送
#endif
}

void port_rx_end(short truely_rx_len)
{
    SHELL_ATOMIC_ENTER();
    if (truely_rx_len < 0)
    {
        truely_rx_len = ringbuffer_get_last_series_size(&rb_rx);
    }

    ringbuffer_linear_write_done(&rb_rx, truely_rx_len);
    ringbuffer_mark_idle(&rb_rx);
    SHELL_ATOMIC_EXIT();

#if SHELL_RX_CONTINUOUSLY
    //! 继续接收,如果这里由于队列满了导致没有触发接收,那么直到再次触发接收之前,都不会再接收数据了
    port_rx_trigger();
#endif
}

//+******************************** shell和log组件初始化,shell任务 ***************************************/

void letter_shell_init(void)
{
    PLATFORM_INIT();

    // 初始化输入输出环形队列
    static char rb_rx_buffer[SHELL_RX_BUFFER_SIZE];
    static char rb_tx_buffer[SHELL_TX_BUFFER_SIZE];
    ringbuffer_init(&rb_rx, (uint8_t *)rb_rx_buffer, sizeof(rb_rx_buffer));
    ringbuffer_init(&rb_tx, (uint8_t *)rb_tx_buffer, sizeof(rb_tx_buffer));

    // 用于存放命令之类的buffer
    static char shell_buffer[512];

    user_shell.write = write_to_ringbuffer;
    user_shell.read = read_from_ringbuffer;
#if SHELL_USING_LOCK == 1

#if !defined(SHELL_LOCK_FUNC) || !defined(SHELL_UNLOCK_FUNC)
#error "请定义shell的加锁与解锁函数或者不使用log锁(设置SHELL_USING_LOCK为0)"
#error "please define SHELL_LOCK_FUNC and SHELL_UNLOCK_FUNC"
#endif
    user_shell.lock = SHELL_LOCK_FUNC;
    user_shell.unlock = SHELL_UNLOCK_FUNC;
#endif
    shellInit(&user_shell, shell_buffer, sizeof(shell_buffer)); // 初始化shell

    user_log.active = 1; // 创建log对象,默认输出所有等级的log
    user_log.level = LOG_ALL;
    user_log.write = UserLogWrite,
#if LOG_USING_LOCK == 1

#if !defined(LOG_LOCK_FUNC) || !defined(LOG_UNLOCK_FUNC)
#error "请定义log的加锁与解锁函数或者不使用log锁(设置LOG_USING_LOCK为0)"
#error "please define LOG_LOCK_FUNC and LOG_UNLOCK_FUNC"
#endif

    user_log.lock = LOG_LOCK_FUNC;
    user_log.unlock = LOG_UNLOCK_FUNC;
#endif

    logRegister(&user_log, &user_shell); // 注册log
}

void shell_task()
{
#if SHELL_TASK_WHILE == 1
    for (;;)
    {
#endif
        port_tx_trigger();
        port_rx_trigger();
        // 读取数据进行处理,这里可以多读几个字节,提高处理效率
        static char data;
        // while (user_shell.read(&data, 1))
        if (user_shell.read(&data, 1))
        {
            shellHandler(&user_shell, data);
        }
#if SHELL_TASK_WHILE == 1
    }
#endif
}

//+********************************* 导出一些可能用到的命令或快捷键 **********************************/
static void logChangeLevel()
{
    Log *log = &user_log;
    SHELL_ASSERT(log, return);
    log->level = (LogLevel)(log->level >= LOG_ALL ? LOG_NONE : (log->level + 1));

    char logLevel[40] = {0};
    switch (log->level)
    {
    case LOG_NONE:
        sprintf(logLevel, "set log level : LOG_NONE\t(%d)", log->level);
        break;

    case LOG_ERROR:
        sprintf(logLevel, "set log level : LOG_ERROR\t(%d)", log->level);
        break;

    case LOG_WRANING:
        sprintf(logLevel, "set log level : LOG_WRANING\t(%d)", log->level);
        break;

    case LOG_INFO:
        sprintf(logLevel, "set log level : LOG_INFO\t(%d)", log->level);
        break;

    case LOG_DEBUG:
        sprintf(logLevel, "set log level : LOG_DEBUG\t(%d)", log->level);
        break;

    case LOG_VERBOSE:
        sprintf(logLevel, "set log level : LOG_VERBOSE\t(%d)", log->level);
        break;

    case LOG_ALL:
        sprintf(logLevel, "set log level : LOG_ALL\t\t(%d)", log->level);
        break;

    default:
        break;
    }

    logPrintln("%s", logLevel);
}
//> 0x04000000对应的应该是鼠标中键,但是在Xshell中,按下鼠标中间会以字符串的形式发送04000000,导致无法触发,现在修改为 ~ 的按键值
// SHELL_EXPORT_KEY(SHELL_CMD_PERMISSION(0), 0x04000000, logChangeLevel, switch log level);
SHELL_EXPORT_KEY(SHELL_CMD_PERMISSION(0), 0x60000000, logChangeLevel, '~' switch log level);