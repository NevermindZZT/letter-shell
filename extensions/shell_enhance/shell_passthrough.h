/**
 * @file shell_passthrough.h
 * @author Letter(nevermindzzt@gmail.com)
 * @brief shell passthrough mode
 * @version 0.1
 * @date 2021-05-31
 * 
 * @copyright (c) 2021 Letter
 * 
 */
#ifndef __SHELL_PASSTHROUGH_H__
#define __SHELL_PASSTHROUGH_H__

#include "shell.h"

/**
 * @brief 退出 passthrough 模式的按键键值，单字节
 */
#define     SHELL_PASSTHROUGH_EXIT_KEY          0x04    // Ctrl + D

/**
 * @brief passthrough 模式 handler 函数原型
 * 
 * @param data 数据
 * @param len 数据长度
 * 
 * @return int 返回值
 */
typedef int (*ShellPassthrough)(char *data, unsigned short len);

/**
 * @brief shell passthrough 模式函数名
 */
#define SHELL_PASSTROUGH_FUNC_NAME(_name)       agency##_name

/**
 * @brief shell passthrough 模式函数定义
 * 
 * @param _name 命令名
 * @param _prompt passthrough 模式提示符
 * @param _handler passthrough 模式 handler
 */
#define SHELL_PASSTROUGH_FUNC(_name, _prompt, _handler) \
        void SHELL_PASSTROUGH_FUNC_NAME(_name)(int p1, int p2) \
        { shellPassthrough(shellGetCurrent(), #_prompt, _handler, p1, (void *)p2); }

/**
 * @brief shell passthrouh 定义
 * 
 * @param _attr 属性
 * @param _name passthrough 命令名
 * @param _prompt passthrough 提示符
 * @param _handler passthrough handler
 * @param _desc passthrough 描述
 */
#define SHELL_EXPORT_PASSTROUGH(_attr, _name, _prompt, _handler, _desc) \
        SHELL_PASSTROUGH_FUNC(_name, _prompt, _handler) \
        SHELL_EXPORT_CMD(_attr|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN), _name, SHELL_PASSTROUGH_FUNC_NAME(_name), _desc)

unsigned int shellPassthrough(Shell *shell, const char *prompt, ShellPassthrough handler, int argc, char *argv[]);

#endif