/**
 * @file shell_cfg_user.h
 * @author Letter (nevermindzzt@gmail.com)
 * @brief shell config
 * @version 3.0.0
 * @date 2019-12-31
 *
 * @copyright (c) 2019 Letter
 *
 */

#ifndef __SHELL_CFG_USER_H__
#define __SHELL_CFG_USER_H__

#include <stdint.h>

// clang-format off

/**
 * @brief shell默认用户
 */
#define     SHELL_DEFAULT_USER          "letter_shell"

// /**
//  * @brief 是否使用shell伴生对象,开启后需要支持malloc和free接口
//  *        一些扩展的组件(文件系统支持，日志工具等)需要使用伴生对象
//  */
// #define     SHELL_USING_COMPANION       0

// /**
//  * @brief shell内存分配
//  *        shell本身不需要此接口，若使用shell伴生对象，需要进行定义
//  */
// extern void *pvPortMalloc(size_t xSize);
// #define     SHELL_MALLOC(size)          pvPortMalloc(size)

// /**
//  * @brief shell内存释放
//  *        shell本身不需要此接口，若使用shell伴生对象，需要进行定义
//  */
// extern void vPortFree(void *pv);
// #define     SHELL_FREE(obj)             vPortFree(obj)

/**
 * @brief 使用锁
 * @note 使用shell锁时，需要对加锁和解锁进行实现
 */
#define     SHELL_USING_LOCK            0

#ifndef SHELL_TASK_WHILE
/**
 * @brief 是否使用默认shell任务while循环
 *        使能此宏，则`shell_task()`函数会一直循环读取输入，一般使用操作系统建立shell
 *        任务时使能此宏，关闭此宏的情况下，一般适用于无操作系统，在主循环中调用`shell_task()`
 */
#define     SHELL_TASK_WHILE            0
#endif /** SHELL_TASK_WHILE */

/**
 * @brief 是否显示shell信息
 */
#define     SHELL_SHOW_INFO             1

/**
 * @brief 支持shell尾行模式
 */
#define     SHELL_SUPPORT_END_LINE      1


/**
 * @brief 使用LF作为命令行回车触发
 *        可以和SHELL_ENTER_CR同时开启
 */
#define     SHELL_ENTER_LF              0

/**
 * @brief 使用CR作为命令行回车触发
 *        可以和SHELL_ENTER_LF同时开启
 */
#define     SHELL_ENTER_CR              0

/**
 * @brief 使用CRLF作为命令行回车触发
 *        不可以和SHELL_ENTER_LF或SHELL_ENTER_CR同时开启
 */
#define     SHELL_ENTER_CRLF            1

#ifndef SHELL_PRINT_BUFFER
/**
 * @brief shell格式化输出的缓冲大小
 *        为0时不使用shell格式化输出
 */
#define     SHELL_PRINT_BUFFER          256
#endif /** SHELL_PRINT_BUFFER */

/**
 * @brief shell格式化输入的缓冲大小
 *        为0时不使用shell格式化输入
 * @note shell格式化输入会阻塞shellTask, 仅适用于在有操作系统的情况下使用
 */
#define     SHELL_SCAN_BUFFER          256

/**
 * @brief 使用函数签名
 *        使能后，可以在声明命令时，指定函数的签名，shell 会根据函数签名进行参数转换，
 *        而不是自动判断参数的类型，如果参数和函数签名不匹配，会停止执行命令
 */
#define     SHELL_USING_FUNC_SIGNATURE  1

//+******************************** log组件配置 ***************************************/
//>log组件的配置在log.h中配置

#endif
