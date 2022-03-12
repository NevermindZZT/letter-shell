/**
 * @file shell_secure_user.h
 * @author Letter (nevermindzzt@gmail.com)
 * @brief shell secure user
 * @version 0.1
 * @date 2022-03-12
 * 
 * @copyright Copyright (c) 2022 Letter
 * 
 */
#ifndef __SHELL_SECURE_USER_H__
#define __SHELL_SECURE_USER_H__

#include "shell.h"

/**
 * @brief shell secure user 获取用户密码函数原型
 * 
 * @param name 用户名
 * 
 * @return char* 用户密码
 */
typedef char* (*ShellSecureUserGetPassword)(const char *name);

/**
 * @brief shell secure user 代理函数名
 * 
 */
#define SHELL_SECURE_USER_FUNC_NAME(_name)      agency##_name

/**
 * @brief shell secure user 代理函数定义
 * 
 * @param _name 用户名
 * @param _attr 用户命令属性
 * @param _handler 获取用户密码函数
 */
#define SHELL_SECURE_USER_FUNC(_name, _attr, _handler) \
        void SHELL_SECURE_USER_FUNC_NAME(_name)(int p1, int p2) \
        { shellSecureUser(shellGetCurrent(), #_name, _attr, _handler); }

/**
 * @brief shell secure user 定义
 * 
 * @param _attr 属性
 * @param _name 用户名
 * @param _handler 获取用户密码函数
 * @param _desc 命令描述
 */
#define SHELL_EXPORT_SECURE_USER(_attr, _name, _handler, _desc) \
        SHELL_SECURE_USER_FUNC(_name, _attr, _handler) \
        SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN)|SHELL_CMD_DISABLE_RETURN, \
                         _name, SHELL_SECURE_USER_FUNC_NAME(_name), _desc)


int shellSecureUser(Shell *shell, const char *name, int attr, ShellSecureUserGetPassword handler);

#endif
