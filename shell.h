/**
 * @file shell.h
 * @author Letter (NevermindZZT@gmail.cn)
 * @brief letter shell
 * @version 2.0.0
 * @date 2018-12-29
 * 
 * @Copyright (c) 2018 Letter
 * 
 */

#ifndef     __SHELL_H__
#define     __SHELL_H__

#include "shell_cfg.h"

#define     SHELL_VERSION               "2.0.4"                 /**< 版本号 */

#define     SHELL_KEY_LF                0x0A
#define     SHELL_KEY_CR                0x0D
#define     SHELL_KEY_TAB               0x09
#define     SHELL_KEY_BACKSPACE         0x08
#define     SHELL_KEY_DELETE            0x7F
#define     SHELL_KEY_ESC               0x1B

#define     SHELL_KEY_CTRL_T            0x14
#define     SHELL_KEY_CTRL_A            0x01
#define     SHELL_KEY_CTRL_B            0x02
#define     SHELL_KEY_CTRL_C            0x03
#define     SHELL_KEY_CTRL_D            0x04
#define     SHELL_KEY_CTRL_E            0x05
#define     SHELL_KEY_CTRL_F            0x06
#define     SHELL_KEY_CTRL_G            0x07
#define     SHELL_KEY_CTRL_H            0x08
#define     SHELL_KEY_CTRL_I            0x09
#define     SHELL_KEY_CTRL_J            0x0A
#define     SHELL_KEY_CTRL_K            0x0B
#define     SHELL_KEY_CTRL_L            0x0C
#define     SHELL_KEY_CTRL_M            0x0D
#define     SHELL_KEY_CTRL_N            0x0E
#define     SHELL_KEY_CTRL_O            0x0F
#define     SHELL_KEY_CTRL_P            0x10
#define     SHELL_KEY_CTRL_Q            0x11
#define     SHELL_KEY_CTRL_R            0x12
#define     SHELL_KEY_CTRL_S            0x13
#define     SHELL_KEY_CTRL_T            0x14
#define     SHELL_KEY_CTRL_U            0x15
#define     SHELL_KEY_CTRL_V            0x16
#define     SHELL_KEY_CTRL_W            0x17
#define     SHELL_KEY_CTRL_X            0x18
#define     SHELL_KEY_CTRL_Y            0x19
#define     SHELL_KEY_CTRL_Z            0x1A

#define     SHELL_VAR_INT               0
#define     SHELL_VAR_SHORT             1
#define     SHELL_VAR_CHAR              2
#define     SHELL_VAR_POINTER           3

#if defined(__CC_ARM) || (defined(__ARMCC_VERSION) && __ARMCC_VERSION >= 6000000)
    #define SECTION(x)                  __attribute__((section(x)))
#elif defined(__ICCARM__)
    #define SECTION(x)                  @ x
#elif defined(__GNUC__)
    #define SECTION(x)                  __attribute__((section(x)))
#else
    #define SECTION(x)
#endif

/**
 * @brief shell命令导出
 * 
 * @attention 命令导出方式目前仅支持 keil 所带的编译器，使用时需要在 keil 的
 *            Option for Target 中 Linker 选项卡的 Misc controls 中添加 --keep shellCommand*
 */
#if SHELL_USING_CMD_EXPORT == 1
#if SHELL_LONG_HELP == 1
#define     SHELL_EXPORT_CMD(cmd, func, desc)                               \
            const char shellCmd##cmd[] = #cmd;                              \
            const char shellDesc##cmd[] = #desc;                            \
            const SHELL_CommandTypeDef                                      \
            shellCommand##cmd SECTION("shellCommand") =                     \
            {                                                               \
                shellCmd##cmd,                                              \
                (int (*)())func,                                            \
                shellDesc##cmd,                                             \
                (void *)0                                                   \
            }
#define     SHELL_EXPORT_CMD_EX(cmd, func, desc, help)                      \
            const char shellCmd##cmd[] = #cmd;                              \
            const char shellDesc##cmd[] = #desc;                            \
            const char shellHelp##cmd[] = #help;                            \
            const SHELL_CommandTypeDef                                      \
            shellCommand##cmd SECTION("shellCommand") =                     \
            {                                                               \
                shellCmd##cmd,                                              \
                (int (*)())func,                                            \
                shellDesc##cmd,                                             \
                shellHelp##cmd                                              \
            }
#else /** SHELL_LONG_HELP == 1 */
#define     SHELL_EXPORT_CMD(cmd, func, desc)                               \
            const char shellCmd##cmd[] = #cmd;                              \
            const char shellDesc##cmd[] = #desc;                            \
            const SHELL_CommandTypeDef                                      \
            shellCommand##cmd SECTION("shellCommand") =                     \
            {                                                               \
                shellCmd##cmd                                               \
                (int (*)())func,                                            \
                shellDesc##cmd                                              \
            }
#define     SHELL_EXPORT_CMD_EX(cmd, func, desc, help)                      \
            const char shellCmd##cmd[] = #cmd;                              \
            const char shellDesc##cmd[] = #desc;                            \
            const SHELL_CommandTypeDef                                      \
            shellCommand##cmd SECTION("shellCommand") =                     \
            {                                                               \
                shellCmd##cmd,                                              \
                (int (*)())func,                                            \
                shellDesc##cmd                                              \
            }
#endif /** SHELL_LONG_HELP == 1 */

#if SHELL_USING_VAR == 1
    #define SHELL_EXPORT_VAR(var, variable, desc, type)                     \
            const char shellVar##var[] = #var;                              \
            const char shellDesc##var[] = #desc;                            \
            const SHELL_VaribaleTypeDef                                     \
            shellVariable##var SECTION("shellVariable") =                   \
            {                                                               \
                shellVar##var,                                              \
                (int)(variable),                                           \
                shellDesc##var,                                             \
                type                                                        \
            }
#else 
    #define SHELL_EXPORT_VAR(var, variable, desc, type) 
#endif /** SHELL_USING_VAR == 1 */

#else
#define     SHELL_EXPORT_CMD(cmd, func, desc)
#define     SHELL_EXPORT_CMD_EX(cmd, func, desc, help)
#define     SHELL_EXPORT_VAR(var, variable, desc, type) 
#endif /** SHELL_USING_CMD_EXPORT == 1 */

#define     SHELL_EXPORT_VAR_INT(var, variable, desc)                       \
            SHELL_EXPORT_VAR(var, &variable, desc, SHELL_VAR_INT)
#define     SHELL_EXPORT_VAR_SHORT(var, variable, desc)                     \
            SHELL_EXPORT_VAR(var, &variable, desc, SHELL_VAR_SHORT)
#define     SHELL_EXPORT_VAR_CHAR(var, variable, desc)                      \
            SHELL_EXPORT_VAR(var, &variable, desc, SHELL_VAR_CHAR)
#define     SHELL_EXPORT_VAR_POINTER(var, variable, desc)                   \
            SHELL_EXPORT_VAR(var, variable, desc, SHELL_VAR_POINTER)


/**
 * @brief shell命令条目
 * 
 * @note 用于shell命令通过命令表的方式定义
 */
#if SHELL_USING_CMD_EXPORT == 0
#if SHELL_LONG_HELP == 1
#define     SHELL_CMD_ITEM(cmd, func, desc)                                 \
            {                                                               \
                #cmd,                                                       \
                (int (*)())func,                                            \
                #desc,                                                      \
                (void *)0                                                   \
            }
#define     SHELL_CMD_ITEM_EX(cmd, func, desc, help)                        \
            {                                                               \
                #cmd,                                                       \
                (int (*)())func,                                            \
                #desc,                                                      \
                #help                                                       \
            }   
#else /** SHELL_LONG_HELP == 1 */
#define     SHELL_CMD_ITEM(cmd, func, desc)                                 \
            {                                                               \
                #cmd,                                                       \
                (int (*)())func,                                            \
                #desc                                                       \
            }
#define     SHELL_CMD_ITEM_EX(cmd, func, desc, help)                        \
            {                                                               \
                #cmd,                                                       \
                (int (*)())func,                                            \
                #desc,                                                      \
            }  
#endif /** SHELL_LONG_HELP == 1 */

#define     SHELL_VAR_ITEM(var, variable, desc, type)                       \
            {                                                               \
                #var,                                                       \
                varialbe,                                                   \
                #desc,                                                      \
                type,                                                       \
            }
#define     SHELL_VAR_ITEM_INT(var, variable, desc)                         \
            SHELL_VAR_ITEM(var, &variable, desc, SHELL_VAR_INT)
#define     SHELL_VAR_ITEM_SHORT(var, variable, desc)                       \
            SHELL_VAR_ITEM(var, &variable, desc, SHELL_VAR_SHORT)
#define     SHELL_VAR_ITEM_CHAR(var, variable, desc)                        \
            SHELL_VAR_ITEM(var, &variable, desc, SHELL_VAR_CHAR)
#define     SHELL_VAR_ITEM_POINTER(var, variable, desc)                     \
            SHELL_VAR_ITEM(var, variable, desc, SHELL_VAR_POINTER)

#endif /** SHELL_USING_CMD_EXPORT == 0 */

/**
 * @brief shell读取数据函数原型
 * 
 * @param char shell读取的字符
 * 
 * @return char 0 读取数据成功
 * @return char -1 读取数据失败
 */
typedef signed char (*shellRead)(char *);

/**
 * @brief shell写数据函数原型
 * 
 * @param const char 需写的字符
 */
typedef void (*shellWrite)(const char);

/**
 * @brief shell指令执行函数原型
 * 
 */
typedef int (*shellFunction)();


/**
 * @brief shell输入状态
 * 
 */
typedef enum
{
    SHELL_IN_NORMAL = 0,
    SHELL_ANSI_ESC,
    SHELL_ANSI_CSI,
}SHELL_InputMode;


/**
 * @brief shell 命令定义
 * 
 */
typedef struct
{
    const char *name;                                           /**< shell命令名称 */
    shellFunction function;                                     /**< shell命令函数 */
    const char *desc;                                           /**< shell命令描述 */
#if SHELL_LONG_HELP == 1
    const char *help;                                           /**< shell长帮助信息 */
#endif
}SHELL_CommandTypeDef;


#if SHELL_USING_VAR == 1
typedef struct
{
    const char *name;
    const int value;
    const char *desc;
    const int type;
} SHELL_VaribaleTypeDef;
#endif /** SHELL_USING_VAR == 1 */


/**
 * @brief shell对象定义
 * 
 */
typedef struct
{
    char *command;                                              /**< shell命令提示符 */
    char buffer[SHELL_COMMAND_MAX_LENGTH];                      /**< shell命令缓冲 */
    unsigned short length;                                      /**< shell命令长度 */
    unsigned short cursor;                                      /**< shell光标位置 */
    char *param[SHELL_PARAMETER_MAX_NUMBER];                    /**< shell参数 */
    char history[SHELL_HISTORY_MAX_NUMBER][SHELL_COMMAND_MAX_LENGTH];  /**< 历史记录 */
    unsigned short historyCount;                                /**< 历史记录数量 */
    short historyFlag;                                          /**< 当前记录位置 */
    short historyOffset;                                        /**< 历史记录偏移 */
    SHELL_CommandTypeDef *commandBase;                          /**< 命令表基址 */
    unsigned short commandNumber;                               /**< 命令数量 */
#if SHELL_USING_VAR == 1
    SHELL_VaribaleTypeDef *variableBase;                        /**< 变量表基址 */
    unsigned short variableNumber;                              /**< 变量数量 */
#endif
    int keyFuncBase;                                            /**< 按键响应表基址 */
    unsigned short keyFuncNumber;                               /**< 按键响应数量 */
    SHELL_InputMode status;                                     /**< 输入状态 */
    unsigned char isActive;                                     /**< 是否是当前活动shell */
    shellRead read;                                             /**< shell读字符 */
    shellWrite write;                                           /**< shell写字符 */
}SHELL_TypeDef;


/**
 * @brief shell按键功能定义
 * 
 */
typedef struct
{
    unsigned char keyCode;                                      /**< shell按键键值 */
    void (*keyFunction)(SHELL_TypeDef *);                       /**< 按键响应函数 */
} SHELL_KeyFunctionDef;


void shellInit(SHELL_TypeDef *shell);
void shellSetCommandList(SHELL_TypeDef *shell, SHELL_CommandTypeDef *base, unsigned short size);

#if SHELL_USING_VAR == 1
void shellSetVariableList(SHELL_TypeDef *shell, SHELL_VaribaleTypeDef *base, unsigned short size);
int shellGetVariable(SHELL_TypeDef *shell, char *var);
#endif /** SHELL_USING_VAR == 1 */

void shellSetKeyFuncList(SHELL_TypeDef *shell, SHELL_KeyFunctionDef *base, unsigned short size);
SHELL_TypeDef *shellGetCurrent(void);
void shellPrint(SHELL_TypeDef *shell, char *fmt, ...);
unsigned short shellDisplay(SHELL_TypeDef *shell, const char *string);
void shellHandler(SHELL_TypeDef *shell, char data);


void shellHelp(int argc, char *argv[]);
void shellClear(void);

#if SHELL_USING_TASK == 1
void shellTask(void *param);
#endif

#endif

