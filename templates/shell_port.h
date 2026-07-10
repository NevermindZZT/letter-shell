/**
 * @file shell_port.h
 * @brief Project-local port interface for letter shell.
 */

#ifndef __SHELL_PORT_H__
#define __SHELL_PORT_H__

#include "shell.h"

extern Shell g_shell;

void shell_init(void);

// 提供简单一点的函数注册(导出)
#define SHELL_FUNC_EXPORT(func, ...) SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), func, func, func, ##__VA_ARGS__)

#endif
