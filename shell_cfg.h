/**
 * @file shell_cfg.h
 * @author Letter (NevermindZZT@gmail.com)
 * @brief shell config
 * @version 0.1
 * @date 2019-04-11
 * 
 * @copyright (c) 2019 Letter
 * 
 */

#ifndef __SHELL_CFG_H__
#define __SHELL_CFG_H__

/**
 * @brief 是否使用默认shell任务
 *        使能之后可以使用`shellTask()`建立shell任务，或者使用`shellTask()`进行轮询
 */
#define     SHELL_USING_TASK            0

/**
 * @brief 是否使用默认shell任务while循环，使能宏`SHELL_USING_TASK`后此宏有意义
 *        使能此宏，则`shellTask()`函数会一直循环读取输入，一般使用操作系统建立shell
 *        任务时使能此宏，关闭此宏的情况下，一般适用于无操作系统，在主循环中调用`shellTask()`
 */
#define     SHELL_TASK_WHILE            1

/**
 * @brief 是否使用命令导出方式
 *        使能此宏后，可以使用`SHELL_EXPORT_CMD()`或者`SHELL_EXPORT_CMD_EX()`
 *        定义shell命令，关闭此宏的情况下，需要使用命令表的方式
 */
#define     SHELL_USING_CMD_EXPORT      1

/**
 * @brief 是否使用变量功能
 * 
 */
#define     SHELL_USING_VAR             0

/**
 * @brief 是否显示命令调用函数返回值
 *        使能此宏，则每次调用shell命令之后会以整形和十六进制的方式打印函数的返回值
 */
#define     SHELL_DISPLAY_RETURN        1

/**
 * @brief 是否使用shell参数自动解析
 *        使能此宏以支持常规C函数形式的命令，shell会自动转换参数
 *        关闭此宏则支持main函数形式的命令，需要自行在函数中处理参数
 */
#define     SHELL_AUTO_PRASE            1

/**
 * @brief 是否使用shell长帮助
 *        使能此宏以支持命令的长帮助信息
 */
#define     SHELL_LONG_HELP             1

/**
 * @brief shell命令最大长度
 *        命令行可输入的最大字符长度
 */
#define     SHELL_COMMAND_MAX_LENGTH    50

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
 * @brief shell默认提示符
 */
#define     SHELL_DEFAULT_COMMAND       "\r\nletter>>"

/**
 * @brief 是否使用密码功能
 */
#define     SHELL_USING_AUTH             0

/**
 * @brief shell用户密码
 */
#define     SHELL_USER_PASSWORD         "letter"

/**
 * @brief shell自动锁定超时
 *        使能`SHELL_USING_AUTH`的情况下生效，超时后会自动重新锁定shell
 *        设置为0时关闭自动锁定功能，时间单位为`SHELL_GET_TICK()`单位
 * @note 使用超时锁定必须保证`SHELL_GET_TICK()`有效
 */
#define     SHELL_LOCK_TIMEOUT          5 * 60 * 1000

#endif
