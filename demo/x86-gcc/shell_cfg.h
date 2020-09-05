/**
 * @file shell_cfg.h
 * @author Letter (nevermindzzt@gmail.com)
 * @brief shell config
 * @version 3.0.0
 * @date 2019-12-31
 * 
 * @copyright (c) 2019 Letter
 * 
 */

#ifndef __SHELL_CFG_H__
#define __SHELL_CFG_H__

#include "stdlib.h"

/**
 * @brief 是否使用默认shell任务while循环，使能宏`SHELL_USING_TASK`后此宏有意义
 *        使能此宏，则`shellTask()`函数会一直循环读取输入，一般使用操作系统建立shell
 *        任务时使能此宏，关闭此宏的情况下，一般适用于无操作系统，在主循环中调用`shellTask()`
 */
#define     SHELL_TASK_WHILE            1

/**
 * @brief 是否使用命令导出方式
 *        使能此宏后，可以使用`SHELL_EXPORT_CMD()`等导出命令
 *        定义shell命令，关闭此宏的情况下，需要使用命令表的方式
 */
#define     SHELL_USING_CMD_EXPORT      1

/**
 * @brief 是否使用shell伴生对象
 *        一些扩展的组件(文件系统支持，日志工具等)需要使用伴生对象
 */
#define     SHELL_USING_COMPANION       1

/**
 * @brief 支持shell尾行模式
 */
#define     SHELL_SUPPORT_END_LINE      1

/**
 * @brief 是否在输出命令列表中列出用户
 */
#define     SHELL_HELP_LIST_USER        0

/**
 * @brief 是否在输出命令列表中列出变量
 */
#define     SHELL_HELP_LIST_VAR         0

/**
 * @brief 是否在输出命令列表中列出按键
 */
#define     SHELL_HELP_LIST_KEY         0

/**
 * @brief 是否在输出命令列表中展示命令权限
 */
#define     SHELL_HELP_SHOW_PERMISSION  1

/**
 * @brief 使用LF作为命令行回车触发
 *        可以和SHELL_ENTER_CR同时开启
 */
#define     SHELL_ENTER_LF              1

/**
 * @brief 使用CR作为命令行回车触发
 *        可以和SHELL_ENTER_LF同时开启
 */
#define     SHELL_ENTER_CR              1

/**
 * @brief 使用CRLF作为命令行回车触发
 *        不可以和SHELL_ENTER_LF或SHELL_ENTER_CR同时开启
 */
#define     SHELL_ENTER_CRLF            0

/**
 * @brief 使用执行未导出函数的功能
 *        启用后，可以通过`exec [addr] [args]`直接执行对应地址的函数
 * @attention 如果地址错误，可能会直接引起程序崩溃
 */
#define     SHELL_EXEC_UNDEF_FUNC       1

/**
 * @brief shell命令参数最大数量
 *        包含命令名在内，超过8个参数并且使用了参数自动转换的情况下，需要修改源码
 */
#define     SHELL_PARAMETER_MAX_NUMBER  8

/**
 * @brief 历史命令记录数量
 */
#define     SHELL_HISTORY_MAX_NUMBER    5

/**
 * @brief 双击间隔(ms)
 *        使能宏`SHELL_LONG_HELP`后此宏生效，定义双击tab补全help的时间间隔
 */
#define     SHELL_DOUBLE_CLICK_TIME     200

/**
 * @brief 管理的最大shell数量
 */
#define     SHELL_MAX_NUMBER            5

/**
 * @brief shell格式化输出的缓冲大小
 *        为0时不使用shell格式化输出
 */
#define     SHELL_PRINT_BUFFER          128

/**
 * @brief 获取系统时间(ms)
 *        定义此宏为获取系统Tick，如`HAL_GetTick()`
 * @note 此宏不定义时无法使用双击tab补全命令help，无法使用shell超时锁定
 */
#define     SHELL_GET_TICK()            0

/**
 * @brief shell内存分配
 *        shell本身不需要此接口，若使用shell伴生对象，需要进行定义
 */
#define     SHELL_MALLOC(size)          malloc(size)

/**
 * @brief shell内存释放
 *        shell本身不需要此接口，若使用shell伴生对象，需要进行定义
 */
#define     SHELL_FREE(obj)             free(obj)

/**
 * @brief 是否显示shell信息
 */
#define     SHELL_SHOW_INFO             1

/**
 * @brief 是否在登录后清除命令行
 */
#define     SHELL_CLS_WHEN_LOGIN        1

/**
 * @brief shell默认用户
 */
#define     SHELL_DEFAULT_USER          "letter"

/**
 * @brief shell默认用户密码
 *        若默认用户不需要密码，设为""
 */
#define     SHELL_DEFAULT_USER_PASSWORD ""

/**
 * @brief shell自动锁定超时
 *        shell当前用户密码有效的时候生效，超时后会自动重新锁定shell
 *        设置为0时关闭自动锁定功能，时间单位为`SHELL_GET_TICK()`单位
 * @note 使用超时锁定必须保证`SHELL_GET_TICK()`有效
 */
#define     SHELL_LOCK_TIMEOUT          0 * 60 * 1000

#endif
