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

#define     SHELL_VERSION               "2.0.0"                 /**< 版本号 */

#define     SHELL_USING_OS              0                       /**< 运行在操作系统环境中 */
#define     SHELL_USING_CMD_EXPORT      1                       /**< 是否使用命令导出方式 */
#define     SHELL_AUTO_PRASE            1                       /**< 是否使用shell参数自动解析 */
#define     SHELL_COMMAND_MAX_LENGTH    50                      /**< shell命令最大长度 */
#define     SHELL_PARAMETER_MAX_NUMBER  5                       /**< shell命令参数最大数量 */
#define     SHELL_HISTORY_MAX_NUMBER    5                       /**< 历史命令记录数量 */

#define     SHELL_COMMAND               "\r\nletter>>"          /**< shell提示符 */

/**
 * @brief shell命令导出
 * 
 * @attention 命令导出方式目前仅支持 keil 所带的编译器，使用时需要在 keil 的
 *            Option for Target 中 Linker 选项卡的 Misc controls 中添加 --keep shellCommand*
 */
#if SHELL_USING_CMD_EXPORT == 1
#define     SHELL_EXPORT_CMD(cmd, func, desc)                               \
            const SHELL_CommandTypeDef                                      \
            shellCommand##cmd __attribute__((section("shellCommand"))) =    \
            {                                                               \
                #cmd,                                                       \
                (void (*)())func,                                           \
                #desc                                                       \
            }
#else
#define     SHELL_EXPORT_CMD(cmd, func, desc)
#endif

#if SHELL_USING_CMD_EXPORT == 0
/**
 * @brief shell命令条目
 * 
 * @note 用于shell命令通过命令表的方式定义
 */
#define     SHELL_CMD_ITEM(cmd, func, desc)                                 \
            {                                                               \
                #cmd,                                                       \
                (void (*)())func,                                           \
                #desc                                                       \
            }
#endif
            
typedef char (*shellRead)(void);                                /**< shell读取数据函数原型 */
typedef void (*shellWrite)(const char);                         /**< shell写数据函数原型 */
typedef void (*shellFunction)();                                /**< shell指令执行函数原型 */


/**
 * @brief shell控制指令状态
 * 
 */
typedef enum
{
    CONTROL_FREE = 0,
    CONTROL_STEP_ONE,
    CONTROL_STEP_TWO,
}CONTROL_Status;


/**
 * @brief shell 命令定义
 * 
 */
typedef struct
{
    const char *name;                                           /**< shell命令名称 */
    shellFunction function;                                     /**< shell命令函数 */
    const char *desc;                                           /**< shell命令描述 */
}SHELL_CommandTypeDef;


/**
 * @brief shell对象定义
 * 
 */
typedef struct
{
    char buffer[SHELL_COMMAND_MAX_LENGTH];                      /**< shell命令缓冲 */
    unsigned short length;                                      /**< shell命令长度 */
    char *param[SHELL_PARAMETER_MAX_NUMBER];                    /**< shell参数 */
    unsigned short cursor;                                      /**< shell光标位置 */
    char history[SHELL_HISTORY_MAX_NUMBER][SHELL_COMMAND_MAX_LENGTH];  /**< 历史记录 */
    unsigned short historyCount;                                /**< 历史记录数量 */
    short historyFlag;                                          /**< 当前记录位置 */
    short historyOffset;                                        /**< 历史记录偏移 */
    SHELL_CommandTypeDef *commandBase;                          /**< 命令表基址 */
    unsigned short commandNumber;                               /**< 命令数量 */
    CONTROL_Status status;                                      /**< 控制键状态 */
    shellRead read;                                             /**< shell读字符 */
    shellWrite write;                                           /**< shell写字符 */
}SHELL_TypeDef;

void shellInit(SHELL_TypeDef *shell);
void shellSetCommandList(SHELL_TypeDef *shell, SHELL_CommandTypeDef *base, unsigned short size);
void shellHandler(SHELL_TypeDef *shell, char data);

#if SHELL_USING_OS == 1
void shellTask(void *param);
#endif

#endif

