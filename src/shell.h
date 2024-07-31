/**
 * @file shell.h
 * @author Letter (NevermindZZT@gmail.com)
 * @brief letter shell
 * @version 3.0.0
 * @date 2019-12-30
 * 
 * @copyright (c) 2020 Letter
 * 
 */

#ifndef     __SHELL_H__
#define     __SHELL_H__

#include "shell_cfg.h"

#define     SHELL_VERSION               "3.2.4"                 /**< 版本号 */


/**
 * @brief shell 断言
 * 
 * @param expr 表达式
 * @param action 断言失败操作
 */
#define     SHELL_ASSERT(expr, action) \
            if (!(expr)) { \
                action; \
            }

#if SHELL_USING_LOCK == 1
#define     SHELL_LOCK(shell)           shell->lock(shell)
#define     SHELL_UNLOCK(shell)         shell->unlock(shell)
#else
#define     SHELL_LOCK(shell)
#define     SHELL_UNLOCK(shell)
#endif /** SHELL_USING_LOCK == 1 */
/**
 * @brief shell 命令权限
 * 
 * @param permission 权限级别
 */
#define     SHELL_CMD_PERMISSION(permission) \
            (permission & 0x000000FF)

/**
 * @brief shell 命令类型
 * 
 * @param type 类型
 */
#define     SHELL_CMD_TYPE(type) \
            ((type & 0x0000000F) << 8)

/**
 * @brief 使能命令在未校验密码的情况下使用
 */
#define     SHELL_CMD_ENABLE_UNCHECKED \
            (1 << 12)

/**
 * @brief 禁用返回值打印
 */
#define     SHELL_CMD_DISABLE_RETURN \
            (1 << 13)

/**
 * @brief 只读属性(仅对变量生效)
 */
#define     SHELL_CMD_READ_ONLY \
            (1 << 14)

/**
 * @brief 命令参数数量
 */
#define     SHELL_CMD_PARAM_NUM(num) \
            ((num & 0x0000000F)) << 16

#ifndef SHELL_SECTION
    #if defined(__CC_ARM) || defined(__CLANG_ARM)
        #define SHELL_SECTION(x)                __attribute__((section(x), aligned(1)))
    #elif defined (__IAR_SYSTEMS_ICC__)
        #define SHELL_SECTION(x)                @ x
    #elif defined(__GNUC__)
        #define SHELL_SECTION(x)                __attribute__((section(x), aligned(1)))
    #else
        #define SHELL_SECTION(x)
    #endif
#endif

#ifndef SHELL_USED
    #if defined(__CC_ARM) || defined(__CLANG_ARM)
        #define SHELL_USED                      __attribute__((used))
    #elif defined (__IAR_SYSTEMS_ICC__)
        #define SHELL_USED                      __root
    #elif defined(__GNUC__)
        #define SHELL_USED                      __attribute__((used))
    #else
        #define SHELL_USED
    #endif
#endif

/**
 * @brief shell float型参数转换
 */
#define     SHELL_PARAM_FLOAT(x)            (*(float *)(&x))

/**
 * @brief shell 代理函数名
 */
#define     SHELL_AGENCY_FUNC_NAME(_func)   agency##_func

/**
 * @brief shell代理函数定义
 * 
 * @param _func 被代理的函数
 * @param ... 代理参数
 */
#define     SHELL_AGENCY_FUNC(_func, ...) \
            void SHELL_AGENCY_FUNC_NAME(_func)(int p1, int p2, int p3, int p4, int p5, int p6, int p7) \
            { _func(__VA_ARGS__); }

#if SHELL_USING_CMD_EXPORT == 1

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
            SHELL_USED const ShellCommand \
            shellCommand##_name SHELL_SECTION("shellCommand") =  \
            { \
                .attr.value = _attr, \
                .data.cmd.name = shellCmd##_name, \
                .data.cmd.function = (int (*)())_func, \
                .data.cmd.desc = shellDesc##_name, \
                ##__VA_ARGS__ \
            }

#if SHELL_USING_FUNC_SIGNATURE == 1
    /**
     * @brief shell 命令定义
     * 
     * @param _attr 命令属性
     * @param _name 命令名
     * @param _func 命令函数
     * @param _desc 命令描述
     * @param _sign 命令签名
     */
    #define SHELL_EXPORT_CMD_SIGN(_attr, _name, _func, _desc, _sign) \
            const char shellCmd##_name[] = #_name; \
            const char shellDesc##_name[] = #_desc; \
            const char shellSign##_name[] = #_sign; \
            SHELL_USED const ShellCommand \
            shellCommand##_name SHELL_SECTION("shellCommand") =  \
            { \
                .attr.value = _attr, \
                .data.cmd.name = shellCmd##_name, \
                .data.cmd.function = (int (*)())_func, \
                .data.cmd.desc = shellDesc##_name, \
                .data.cmd.signature = shellSign##_name \
            }
#endif /** SHELL_USING_FUNC_SIGNATURE == 1 */

    /**
     * @brief shell 代理命令定义
     * 
     * @param _attr 命令属性
     * @param _name 命令名
     * @param _func 命令函数
     * @param _desc 命令描述
     * @param ... 代理参数
     */
    #define SHELL_EXPORT_CMD_AGENCY(_attr, _name, _func, _desc, ...) \
            SHELL_AGENCY_FUNC(_func, ##__VA_ARGS__) \
            SHELL_EXPORT_CMD(_attr, _name, SHELL_AGENCY_FUNC_NAME(_func), _desc)

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
            SHELL_USED const ShellCommand \
            shellVar##_name SHELL_SECTION("shellCommand") =  \
            { \
                .attr.value = _attr, \
                .data.var.name = shellCmd##_name, \
                .data.var.value = (void *)_value, \
                .data.var.desc = shellDesc##_name \
            }

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
            SHELL_USED const ShellCommand \
            shellUser##_name SHELL_SECTION("shellCommand") =  \
            { \
                .attr.value = _attr|SHELL_CMD_TYPE(SHELL_TYPE_USER), \
                .data.user.name = shellCmd##_name, \
                .data.user.password = shellPassword##_name, \
                .data.user.desc = shellDesc##_name \
            }

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
            SHELL_USED const ShellCommand \
            shellKey##_value SHELL_SECTION("shellCommand") =  \
            { \
                .attr.value = _attr|SHELL_CMD_TYPE(SHELL_TYPE_KEY), \
                .data.key.value = _value, \
                .data.key.function = (void (*)(Shell *))_func, \
                .data.key.desc = shellDesc##_value \
            }

    /**
     * @brief shell 代理按键定义
     * 
     * @param _attr 按键属性
     * @param _value 按键键值
     * @param _func 按键函数
     * @param _desc 按键描述
     * @param ... 代理参数
     */
    #define SHELL_EXPORT_KEY_AGENCY(_attr, _value, _func, _desc, ...) \
            SHELL_AGENCY_FUNC(_func, ##__VA_ARGS__) \
            SHELL_EXPORT_KEY(_attr, _value, SHELL_AGENCY_FUNC_NAME(_func), _desc)

#if SHELL_USING_FUNC_SIGNATURE == 1
    /**
     * @brief shell 参数解析器定义
     * 
     * @param _attr 参数解析器属性
     * @param _type 参数解析器类型
     * @param _parser 参数解析器函数
     * @param _cleaner 参数清理器
     */
    #define SHELL_EXPORT_PARAM_PARSER(_attr, _type, _parser, _cleaner) \
            const char shellDesc##_parser[] = #_type; \
            SHELL_USED const ShellCommand \
            shellCommand##_parser SHELL_SECTION("shellCommand") = \
            { \
                .attr.value = _attr|SHELL_CMD_TYPE(SHELL_TYPE_PARAM_PARSER), \
                .data.paramParser.type = shellDesc##_parser, \
                .data.paramParser.parser = (int (*)(char *, void **))_parser, \
                .data.paramParser.cleaner = (int (*)(void *))_cleaner \
            }
#endif

#else
    /**
     * @brief shell 命令item定义
     * 
     * @param _attr 命令属性
     * @param _name 命令名
     * @param _func 命令函数
     * @param _desc 命令描述
     */
    #define SHELL_CMD_ITEM(_attr, _name, _func, _desc) \
            { \
                .attr.value = _attr, \
                .data.cmd.name = #_name, \
                .data.cmd.function = (int (*)())_func, \
                .data.cmd.desc = #_desc \
            }

    /**
     * @brief shell 变量item定义
     * 
     * @param _attr 变量属性
     * @param _name 变量名
     * @param _value 变量值
     * @param _desc 变量描述
     */
    #define SHELL_VAR_ITEM(_attr, _name, _value, _desc) \
            { \
                .attr.value = _attr, \
                .data.var.name = #_name, \
                .data.var.value = (void *)_value, \
                .data.var.desc = #_desc \
            }

    /**
     * @brief shell 用户item定义
     * 
     * @param _attr 用户属性
     * @param _name 用户名
     * @param _password 用户密码
     * @param _desc 用户描述
     */
    #define SHELL_USER_ITEM(_attr, _name, _password, _desc) \
            { \
                .attr.value = _attr|SHELL_CMD_TYPE(SHELL_TYPE_USER), \
                .data.user.name = #_name, \
                .data.user.password = #_password, \
                .data.user.desc = #_desc \
            }

    /**
     * @brief shell 按键item定义
     * 
     * @param _attr 按键属性
     * @param _value 按键键值
     * @param _func 按键函数
     * @param _desc 按键描述
     */
    #define SHELL_KEY_ITEM(_attr, _value, _func, _desc) \
            { \
                .attr.value = _attr|SHELL_CMD_TYPE(SHELL_TYPE_KEY), \
                .data.key.value = _value, \
                .data.key.function = (void (*)(Shell *))_func, \
                .data.key.desc = #_desc \
            }

#if SHELL_USING_FUNC_SIGNATURE == 1
    /**
     * @brief shell 参数解析器item定义
     * 
     * @param _attr 参数解析器属性
     * @param _type 参数解析器类型
     * @param _parser 参数解析器函数
     * @param _cleaner 参数清理器
     */
    #define SHELL_PARAM_PARSER_ITEM(_attr, _type, _parser, _cleaner) \
            { \
                .attr.value = _attr|SHELL_CMD_TYPE(SHELL_TYPE_PARAM_PARSER), \
                .data.paramParser.type = #_type, \
                .data.paramParser.parser = (int (*)(char *, void **))_parser, \
                .data.paramParser.cleaner = (int (*)(void *))_cleaner \
            }
#endif /** SHELL_USING_FUNC_SIGNATURE == 1 */

    #define SHELL_EXPORT_CMD(_attr, _name, _func, _desc)
#if SHELL_USING_FUNC_SIGNATURE == 1
    #define SHELL_EXPORT_CMD_SIGN(_attr, _name, _func, _desc, _sign)
#endif /** SHELL_USING_FUNC_SIGNATURE == 1 */
    #define SHELL_EXPORT_CMD_AGENCY(_attr, _name, _func, _desc, ...)
    #define SHELL_EXPORT_VAR(_attr, _name, _value, _desc)
    #define SHELL_EXPORT_USER(_attr, _name, _password, _desc)
    #define SHELL_EXPORT_KEY(_attr, _value, _func, _desc)
    #define SHELL_EXPORT_KEY_AGENCY(_attr, _name, _func, _desc, ...)
#if SHELL_USING_FUNC_SIGNATURE == 1
    #define SHELL_EXPORT_PARAM_PARSER(_attr, _type, _parser, _cleaner)
#endif /** SHELL_USING_FUNC_SIGNATURE == 1 */
#endif /** SHELL_USING_CMD_EXPORT == 1 */

/**
 * @brief shell command类型
 */
typedef enum
{
    SHELL_TYPE_CMD_MAIN = 0,                                    /**< main形式命令 */
    SHELL_TYPE_CMD_FUNC,                                        /**< C函数形式命令 */
    SHELL_TYPE_VAR_INT,                                         /**< int型变量 */
    SHELL_TYPE_VAR_SHORT,                                       /**< short型变量 */
    SHELL_TYPE_VAR_CHAR,                                        /**< char型变量 */
    SHELL_TYPE_VAR_STRING,                                      /**< string型变量 */
    SHELL_TYPE_VAR_POINT,                                       /**< 指针型变量 */
    SHELL_TYPE_VAR_NODE,                                        /**< 节点变量 */
    SHELL_TYPE_USER,                                            /**< 用户 */
    SHELL_TYPE_KEY,                                             /**< 按键 */
#if SHELL_USING_FUNC_SIGNATURE == 1
    SHELL_TYPE_PARAM_PARSER,                                    /**< 参数解析器 */
#endif
} ShellCommandType;


/**
 * @brief Shell定义
 */
typedef struct shell_def
{
    struct
    {
        const struct shell_command *user;                       /**< 当前用户 */
        int activeTime;                                         /**< shell激活时间 */
        char *path;                                             /**< 当前shell路径 */
    #if SHELL_USING_COMPANION == 1
        struct shell_companion_object *companions;              /**< 伴生对象 */
    #endif
    #if SHELL_KEEP_RETURN_VALUE == 1
        int retVal;                                             /**< 返回值 */
    #endif
    } info;
    struct
    {
        unsigned short length;                                  /**< 输入数据长度 */
        unsigned short cursor;                                  /**< 当前光标位置 */
        char *buffer;                                           /**< 输入缓冲 */
        char *param[SHELL_PARAMETER_MAX_NUMBER];                /**< 参数 */
        unsigned short bufferSize;                              /**< 输入缓冲大小 */
        unsigned short paramCount;                              /**< 参数数量 */
        int keyValue;                                           /**< 输入按键键值 */
    } parser;
#if SHELL_HISTORY_MAX_NUMBER > 0
    struct
    {
        char *item[SHELL_HISTORY_MAX_NUMBER];                   /**< 历史记录 */
        unsigned short number;                                  /**< 历史记录数 */
        unsigned short record;                                  /**< 当前记录位置 */
        signed short offset;                                    /**< 当前历史记录偏移 */
    } history;
#endif /** SHELL_HISTORY_MAX_NUMBER > 0 */
    struct
    {
        void *base;                                             /**< 命令表基址 */
        unsigned short count;                                   /**< 命令数量 */
    } commandList;
    struct
    {
        unsigned char isChecked : 1;                            /**< 密码校验通过 */
        unsigned char isActive : 1;                             /**< 当前活动Shell */
        unsigned char tabFlag : 1;                              /**< tab标志 */
    } status;
    signed short (*read)(char *, unsigned short);               /**< shell读函数 */
    signed short (*write)(char *, unsigned short);              /**< shell写函数 */
#if SHELL_USING_LOCK == 1
    int (*lock)(struct shell_def *);                              /**< shell 加锁 */
    int (*unlock)(struct shell_def *);                            /**< shell 解锁 */
#endif
} Shell;


/**
 * @brief shell command定义
 */
typedef struct shell_command
{
    union
    {
        struct
        {
            unsigned char permission : 8;                       /**< command权限 */
            ShellCommandType type : 4;                          /**< command类型 */
            unsigned char enableUnchecked : 1;                  /**< 在未校验密码的情况下可用 */
            unsigned char disableReturn : 1;                    /**< 禁用返回值输出 */
            unsigned char readOnly : 1;                         /**< 只读 */
            unsigned char reserve : 1;                          /**< 保留 */
            unsigned char paramNum : 4;                         /**< 参数数量 */
        } attrs;
        int value;
    } attr;                                                     /**< 属性 */
    union
    {
        struct
        {
            const char *name;                                   /**< 命令名 */
            int (*function)();                                  /**< 命令执行函数 */
            const char *desc;                                   /**< 命令描述 */
        #if SHELL_USING_FUNC_SIGNATURE == 1
            const char *signature;                              /**< 函数签名 */
        #endif
        } cmd;                                                  /**< 命令定义 */
        struct
        {
            const char *name;                                   /**< 变量名 */
            void *value;                                        /**< 变量值 */
            const char *desc;                                   /**< 变量描述 */
        } var;                                                  /**< 变量定义 */
        struct
        {
            const char *name;                                   /**< 用户名 */
            const char *password;                               /**< 用户密码 */
            const char *desc;                                   /**< 用户描述 */
        } user;                                                 /**< 用户定义 */
        struct
        {
            int value;                                          /**< 按键键值 */
            void (*function)(Shell *);                          /**< 按键执行函数 */
            const char *desc;                                   /**< 按键描述 */
        } key;                                                  /**< 按键定义 */
#if SHELL_USING_FUNC_SIGNATURE == 1
        struct
        {
            const char *type;                                   /**< 参数类型 */
            int (*parser)(char *, void **);                     /**< 解析函数 */
            int (*cleaner)(void *);                             /**< 清理器 */
        } paramParser;                                          /**< 参数解析器 */
#endif
    } data;
} ShellCommand;

/**
 * @brief shell节点变量属性
 */
typedef struct
{
    void *var;                                                  /**< 变量引用 */
    int (*get)();                                               /**< 变量get方法 */
    int (*set)();                                               /**< 变量set方法 */
} ShellNodeVarAttr;


#define shellSetPath(_shell, _path)     (_shell)->info.path = _path
#define shellGetPath(_shell)            ((_shell)->info.path)

#define shellDeInit(shell)              shellRemove(shell)

void shellInit(Shell *shell, char *buffer, unsigned short size);
void shellRemove(Shell *shell);
unsigned short shellWriteString(Shell *shell, const char *string);
void shellPrint(Shell *shell, const char *fmt, ...);
void shellScan(Shell *shell, char *fmt, ...);
Shell* shellGetCurrent(void);
void shellHandler(Shell *shell, char data);
void shellWriteEndLine(Shell *shell, char *buffer, int len);
void shellTask(void *param);
int shellRun(Shell *shell, const char *cmd);



#if SHELL_USING_COMPANION == 1
/**
 * @brief shell伴生对象定义
 */
typedef struct shell_companion_object
{
    int id;                                                     /**< 伴生对象ID */
    void *obj;                                                  /**< 伴生对象 */
    struct shell_companion_object *next;                        /**< 下一个伴生对象 */
} ShellCompanionObj;


signed char shellCompanionAdd(Shell *shell, int id, void *object);
signed char shellCompanionDel(Shell *shell, int id);
void *shellCompanionGet(Shell *shell, int id);
#endif

#endif
