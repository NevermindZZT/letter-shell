/**
 * @file shell_cpp.h
 * @author Letter (nevermindzzt@gmail.com)
 * @brief shell cpp support
 * @version 1.0.0
 * @date 2021-01-09
 * 
 * @copyright (c) 2021 Letter
 * 
 */
#ifndef __SHELL_CPP_H__
#define __SHELL_CPP_H__

#ifdef __cplusplus
extern "C" {

#include "shell.h"

/**
 * @brief shell command cpp 支持 cmd 定义
 */
typedef struct shell_command_cpp_cmd
{
    int attr;                                                   /**< 属性 */
    const char *name;                                           /**< 命令名 */
    int (*function)();                                          /**< 命令执行函数 */
    const char *desc;                                           /**< 命令描述 */
#if SHELL_USING_FUNC_SIGNATURE == 1
    const char *signature;                                      /**< 函数签名 */
#endif
} ShellCommandCppCmd;

/**
 * @brief shell command cpp 支持 var 定义
 */
typedef struct shell_command_cpp_var
{
    int attr;                                                   /**< 属性 */
    const char *name;                                           /**< 变量名 */
    void *value;                                                /**< 变量值 */
    const char *desc;                                           /**< 变量描述 */
#if SHELL_USING_FUNC_SIGNATURE == 1
    void *unused;                                               /**< 未使用成员，需要保持和 ShellCommandCppCmd 大小一致 */
#endif
} ShellCommandCppVar;

/**
 * @brief shell command cpp 支持 user 定义
 */
typedef struct shell_command_cpp_user
{
    int attr;                                                   /**< 属性 */
    const char *name;                                           /**< 用户名 */
    const char *password;                                       /**< 用户密码 */
    const char *desc;                                           /**< 用户描述 */
#if SHELL_USING_FUNC_SIGNATURE == 1
    void *unused;                                               /**< 未使用成员，需要保持和 ShellCommandCppCmd 大小一致 */
#endif
} ShellCommandCppUser;

/**
 * @brief shell command cpp 支持 key 定义
 */
typedef struct shell_command_cpp_key
{
    int attr;                                                   /**< 属性 */
    int value;                                                  /**< 按键键值 */
    void (*function)(Shell *);                                  /**< 按键执行函数 */
    const char *desc;                                           /**< 按键描述 */
#if SHELL_USING_FUNC_SIGNATURE == 1
    void *unused;                                               /**< 未使用成员，需要保持和 ShellCommandCppCmd 大小一致 */
#endif
} ShellCommandCppKey;

#if SHELL_USING_FUNC_SIGNATURE == 1
typedef struct shell_command_cpp_param_parser
{
    int attr;                                                   /**< 属性 */
    const char *type;                                           /**< 参数类型 */
    int (*parser)(char *, void **);;                            /**< 解析函数 */
    int (*cleaner)(void *);                                     /**< 清理函数 */
    void *unsed;                                                /**< 未使用成员，需要保持和 ShellCommandCppCmd 大小一致 */
} ShellCommandCppParamParser;
#endif

#if SHELL_USING_CMD_EXPORT == 1

    #undef SHELL_EXPORT_CMD
    /**
     * @brief shell 命令定义
     * 
     * @param _attr 命令属性
     * @param _name 命令名
     * @param _func 命令函数
     * @param _desc 命令描述
     * @param ... 其他参数
     */
    #define SHELL_EXPORT_CMD(_attr, _name, _func, _desc, ...) \
            const char shellCmd##_name[] = #_name; \
            const char shellDesc##_name[] = #_desc; \
            extern "C" SHELL_USED const ShellCommandCppCmd \
            shellCommand##_name SHELL_SECTION("shellCommand") =  \
            { \
                _attr, \
                shellCmd##_name, \
                (int (*)())_func, \
                shellDesc##_name, \
                ##__VA_ARGS__ \
            }

    #undef SHELL_EXPORT_VAR
    /**
     * @brief shell 变量定义
     * 
     * @param _attr 变量属性
     * @param _name 变量名
     * @param _value 变量值
     * @param _desc 变量描述
     */
    #define SHELL_EXPORT_VAR(_attr, _name, _value, _desc) \
            const char shellCmd##_name[] = #_name; \
            const char shellDesc##_name[] = #_desc; \
            extern "C" SHELL_USED const ShellCommandCppVar \
            shellVar##_name SHELL_SECTION("shellCommand") =  \
            { \
                _attr, \
                shellCmd##_name, \
                (void *)_value, \
                shellDesc##_name \
            }

    #undef SHELL_EXPORT_USER
    /**
     * @brief shell 用户定义
     * 
     * @param _attr 用户属性
     * @param _name 用户名
     * @param _password 用户密码
     * @param _desc 用户描述
     */
    #define SHELL_EXPORT_USER(_attr, _name, _password, _desc) \
            const char shellCmd##_name[] = #_name; \
            const char shellPassword##_name[] = #_password; \
            const char shellDesc##_name[] = #_desc; \
            extern "C" SHELL_USED const ShellCommandCppUser \
            shellUser##_name SHELL_SECTION("shellCommand") =  \
            { \
                _attr|SHELL_CMD_TYPE(SHELL_TYPE_USER), \
                shellCmd##_name, \
                shellPassword##_name, \
                shellDesc##_name \
            }

    #undef SHELL_EXPORT_KEY
    /**
     * @brief shell 按键定义
     * 
     * @param _attr 按键属性
     * @param _value 按键键值
     * @param _func 按键函数
     * @param _desc 按键描述
     */
    #define SHELL_EXPORT_KEY(_attr, _value, _func, _desc) \
            const char shellDesc##_value[] = #_desc; \
            extern "C" SHELL_USED const ShellCommandCppKey \
            shellKey##_value SHELL_SECTION("shellCommand") =  \
            { \
                _attr|SHELL_CMD_TYPE(SHELL_TYPE_KEY), \
                _value, \
                (void (*)(Shell *))_func, \
                shellDesc##_value \
            }

#if SHELL_USING_FUNC_SIGNATURE == 1
    #undef SHELL_EXPORT_PARAM_PARSER
    /**
     * @brief shell 参数解析器定义
     * 
     * @param _attr 参数解析器属性
     * @param _type 参数解析器类型
     * @param _parser 参数解析器函数
     * @param _cleaner 参数清理函数
     */
    #define SHELL_EXPORT_PARAM_PARSER(_attr, _type, _parser, _cleaner) \
            const char shellDesc##_parser[] = #_type; \
            extern "C" SHELL_USED const ShellCommandCppParamParser \
            shellCommand##_parser SHELL_SECTION("shellCommand") = \
            { \
                _attr|SHELL_CMD_TYPE(SHELL_TYPE_PARAM_PARSER), \
                shellDesc##_parser, \
                (int (*)(char *, void **))_parser, \
                (int (*)(void *))_cleaner \
            }
#endif
#endif /** SHELL_USING_CMD_EXPORT == 1 */

}
#endif /**< defined __cplusplus */

#endif /**< __SHELL_CPP_H__ */
