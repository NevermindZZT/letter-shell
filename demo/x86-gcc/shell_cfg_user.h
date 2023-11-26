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

#include "stdlib.h"
unsigned int userGetTick();

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
 * @brief 使用执行未导出函数的功能
 *        启用后，可以通过`exec [addr] [args]`直接执行对应地址的函数
 * @attention 如果地址错误，可能会直接引起程序崩溃
 */
#define     SHELL_EXEC_UNDEF_FUNC       1

/**
 * @brief 保存命令返回值
 *        开启后会默认定义一个`RETVAL`变量，会保存上一次命令执行的返回值，可以在随后的命令中进行调用
 *        如果命令的`SHELL_CMD_DISABLE_RETURN`标志被设置，则该命令不会更新`RETVAL`
 */
#define     SHELL_KEEP_RETURN_VALUE     1

/**
 * @brief shell格式化输入的缓冲大小
 *        为0时不使用shell格式化输入
 * @note shell格式化输入会阻塞shellTask, 仅适用于在有操作系统的情况下使用
 */
#define     SHELL_SCAN_BUFFER          128

/**
 * @brief 获取系统时间(ms)
 *        定义此宏为获取系统Tick，如`HAL_GetTick()`
 * @note 此宏不定义时无法使用双击tab补全命令help，无法使用shell超时锁定
 */
#define     SHELL_GET_TICK()            userGetTick()

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
 * @brief 使用函数签名
 *        使能后，可以在声明命令时，指定函数的签名，shell 会根据函数签名进行参数转换，
 *        而不是自动判断参数的类型，如果参数和函数签名不匹配，会停止执行命令
 */
#define     SHELL_USING_FUNC_SIGNATURE  1

/**
 * @brief 支持数组参数
 *        使能后，可以在命令中使用数组参数，如`cmd [1,2,3]`
 *        需要使能 `SHELL_USING_FUNC_SIGNATURE` 宏，并且配置 `SHELL_MALLOC`, `SHELL_FREE`
 */
#define     SHELL_SUPPORT_ARRAY_PARAM   1
#endif
