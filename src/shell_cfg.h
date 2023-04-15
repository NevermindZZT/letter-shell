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

#ifdef SHELL_CFG_USER
#include SHELL_CFG_USER
#endif

#ifndef SHELL_TASK_WHILE
/**
 * @brief 是否使用默认shell任务while循环
 *        使能此宏，则`shellTask()`函数会一直循环读取输入，一般使用操作系统建立shell
 *        任务时使能此宏，关闭此宏的情况下，一般适用于无操作系统，在主循环中调用`shellTask()`
 */
#define     SHELL_TASK_WHILE            1
#endif /** SHELL_TASK_WHILE */

#ifndef SHELL_USING_CMD_EXPORT
/**
 * @brief 是否使用命令导出方式
 *        使能此宏后，可以使用`SHELL_EXPORT_CMD()`等导出命令
 *        定义shell命令，关闭此宏的情况下，需要使用命令表的方式
 */
#define     SHELL_USING_CMD_EXPORT      1
#endif /** SHELL_USING_CMD_EXPORT */

#ifndef SHELL_USING_COMPANION
/**
 * @brief 是否使用shell伴生对象
 *        一些扩展的组件(文件系统支持，日志工具等)需要使用伴生对象
 */
#define     SHELL_USING_COMPANION       0
#endif /** SHELL_USING_COMPANION */

#ifndef SHELL_SUPPORT_END_LINE
/**
 * @brief 支持shell尾行模式
 */
#define     SHELL_SUPPORT_END_LINE      0
#endif /** SHELL_SUPPORT_END_LINE */

#ifndef SHELL_HELP_LIST_USER
/**
 * @brief 是否在输出命令列表中列出用户
 */
#define     SHELL_HELP_LIST_USER        0
#endif /** SHELL_HELP_LIST_USER */

#ifndef SHELL_HELP_LIST_VAR
/**
 * @brief 是否在输出命令列表中列出变量
 */
#define     SHELL_HELP_LIST_VAR         0
#endif /** SHELL_HELP_LIST_VAR */

#ifndef SHELL_HELP_LIST_KEY
/**
 * @brief 是否在输出命令列表中列出按键
 */
#define     SHELL_HELP_LIST_KEY         0
#endif /** SHELL_HELP_LIST_KEY */

#ifndef SHELL_HELP_SHOW_PERMISSION
/**
 * @brief 是否在输出命令列表中展示命令权限
 */
#define     SHELL_HELP_SHOW_PERMISSION  1
#endif /** SHELL_HELP_SHOW_PERMISSION */

#ifndef SHELL_ENTER_LF
/**
 * @brief 使用LF作为命令行回车触发
 *        可以和SHELL_ENTER_CR同时开启
 */
#define     SHELL_ENTER_LF              1
#endif /** SHELL_ENTER_LF */

#ifndef SHELL_ENTER_CR
/**
 * @brief 使用CR作为命令行回车触发
 *        可以和SHELL_ENTER_LF同时开启
 */
#define     SHELL_ENTER_CR              1
#endif /** SHELL_ENTER_CR */

#ifndef SHELL_ENTER_CRLF
/**
 * @brief 使用CRLF作为命令行回车触发
 *        不可以和SHELL_ENTER_LF或SHELL_ENTER_CR同时开启
 */
#define     SHELL_ENTER_CRLF            0
#endif /** SHELL_ENTER_CRLF */

#ifndef SHELL_EXEC_UNDEF_FUNC
/**
 * @brief 使用执行未导出函数的功能
 *        启用后，可以通过`exec [addr] [args]`直接执行对应地址的函数
 * @attention 如果地址错误，可能会直接引起程序崩溃
 */
#define     SHELL_EXEC_UNDEF_FUNC       0
#endif /** SHELL_EXEC_UNDEF_FUNC */

#ifndef SHELL_PARAMETER_MAX_NUMBER
/**
 * @brief shell命令参数最大数量
 *        包含命令名在内，超过16个参数并且使用了参数自动转换的情况下，需要修改源码
 */
#define     SHELL_PARAMETER_MAX_NUMBER  8
#endif /** SHELL_PARAMETER_MAX_NUMBER */

#ifndef SHELL_HISTORY_MAX_NUMBER
/**
 * @brief 历史命令记录数量
 */
#define     SHELL_HISTORY_MAX_NUMBER    5
#endif /** SHELL_HISTORY_MAX_NUMBER */

#ifndef SHELL_DOUBLE_CLICK_TIME
/**
 * @brief 双击间隔(ms)
 *        使能宏`SHELL_LONG_HELP`后此宏生效，定义双击tab补全help的时间间隔
 */
#define     SHELL_DOUBLE_CLICK_TIME     200
#endif /** SHELL_DOUBLE_CLICK_TIME */

#ifndef SHELL_QUICK_HELP
/**
 * @brief 快速帮助
 *        作用于双击tab的场景，当使能此宏时，双击tab不会对命令进行help补全，而是直接显示对应命令的帮助信息
 */
#define     SHELL_QUICK_HELP            1
#endif /** SHELL_QUICK_HELP */

#ifndef SHELL_KEEP_RETURN_VALUE
/**
 * @brief 保存命令返回值
 *        开启后会默认定义一个`RETVAL`变量，会保存上一次命令执行的返回值，可以在随后的命令中进行调用
 *        如果命令的`SHELL_CMD_DISABLE_RETURN`标志被设置，则该命令不会更新`RETVAL`
 */
#define     SHELL_KEEP_RETURN_VALUE     0
#endif /** SHELL_KEEP_RETURN_VALUE */

#ifndef SHELL_MAX_NUMBER
/**
 * @brief 管理的最大shell数量
 */
#define     SHELL_MAX_NUMBER            5
#endif /** SHELL_MAX_NUMBER */

#ifndef SHELL_PRINT_BUFFER
/**
 * @brief shell格式化输出的缓冲大小
 *        为0时不使用shell格式化输出
 */
#define     SHELL_PRINT_BUFFER          128
#endif /** SHELL_PRINT_BUFFER */

#ifndef SHELL_SCAN_BUFFER
/**
 * @brief shell格式化输入的缓冲大小
 *        为0时不使用shell格式化输入
 * @note shell格式化输入会阻塞shellTask, 仅适用于在有操作系统的情况下使用
 */
#define     SHELL_SCAN_BUFFER          0
#endif /** SHELL_SCAN_BUFFER */

#ifndef SHELL_GET_TICK
/**
 * @brief 获取系统时间(ms)
 *        定义此宏为获取系统Tick，如`HAL_GetTick()`
 * @note 此宏不定义时无法使用双击tab补全命令help，无法使用shell超时锁定
 */
#define     SHELL_GET_TICK()            0
#endif /** SHELL_GET_TICK */

#ifndef SHELL_USING_LOCK
/**
 * @brief 使用锁
 * @note 使用shell锁时，需要对加锁和解锁进行实现
 */
#define     SHELL_USING_LOCK            0
#endif /** SHELL_USING_LOCK */

#ifndef SHELL_MALLOC
/**
 * @brief shell内存分配
 *        shell本身不需要此接口，若使用shell伴生对象，需要进行定义
 */
#define     SHELL_MALLOC(size)          0
#endif /** SHELL_MALLOC */

#ifndef SHELL_FREE
/**
 * @brief shell内存释放
 *        shell本身不需要此接口，若使用shell伴生对象，需要进行定义
 */
#define     SHELL_FREE(obj)             0
#endif /** SHELL_FREE */

#ifndef SHELL_SHOW_INFO
/**
 * @brief 是否显示shell信息
 */
#define     SHELL_SHOW_INFO             1
#endif /** SHELL_SHOW_INFO */

#ifndef SHELL_CLS_WHEN_LOGIN
/**
 * @brief 是否在登录后清除命令行
 */
#define     SHELL_CLS_WHEN_LOGIN        1
#endif /** SHELL_CLS_WHEN_LOGIN */

#ifndef SHELL_DEFAULT_USER
/**
 * @brief shell默认用户
 */
#define     SHELL_DEFAULT_USER          "letter"
#endif /** SHELL_DEFAULT_USER */

#ifndef SHELL_DEFAULT_USER_PASSWORD
/**
 * @brief shell默认用户密码
 *        若默认用户不需要密码，设为""
 */
#define     SHELL_DEFAULT_USER_PASSWORD ""
#endif /** SHELL_DEFAULT_USER_PASSWORD */

#ifndef SHELL_LOCK_TIMEOUT
/**
 * @brief shell自动锁定超时
 *        shell当前用户密码有效的时候生效，超时后会自动重新锁定shell
 *        设置为0时关闭自动锁定功能，时间单位为`SHELL_GET_TICK()`单位
 * @note 使用超时锁定必须保证`SHELL_GET_TICK()`有效
 */
#define     SHELL_LOCK_TIMEOUT          0 * 60 * 1000
#endif /** SHELL_LOCK_TIMEOUT */

#ifndef SHELL_USING_FUNC_SIGNATURE
/**
 * @brief 使用函数签名
 *        使能后，可以在声明命令时，指定函数的签名，shell 会根据函数签名进行参数转换，
 *        而不是自动判断参数的类型，如果参数和函数签名不匹配，会停止执行命令
 */
#define     SHELL_USING_FUNC_SIGNATURE  0
#endif /** SHELL_USING_FUNC_SIGNATURE */

#ifndef SHELL_COMMAND_FILL_BYTES
/**
 * @brief 命令填充字节数
 *        这个选项控制对声明的命令结构体进行填充，填充的字节数为`SHELL_COMMAND_FILL_BYTES`，
 *        填充的数据位于命令结构体的末尾
 *        部分编译器会在结构体数据后面进行填充，使变量的地址对齐，这会导致 shell 中
 *        通过 sizeof 获取结构体大小，然后通过偏移进行遍历的时候，无法正确地找到命令
 *        通过填充，我们主动将结构体对齐，从而使得 shell 可以正确地遍历命令
 */
#define     SHELL_COMMAND_FILL_BYTES    0
#endif /** SHELL_COMMAND_FILL_BYTES */

#endif
