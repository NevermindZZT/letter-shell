/**
 * @file log.h
 * @author Letter (nevermindzzt@gmail.com)
 * @brief log
 * @version 1.0.0
 * @date 2020-07-30
 * 
 * @copyright (c) 2020 Letter
 * 
 */
#ifndef __LOG_H__
#define __LOG_H__

#include "shell.h"

#define     LOG_VERSION        "1.0.0"

#define     SHELL_COMPANION_ID_LOG          -2

#define     LOG_BUFFER_SIZE    256              /**< log输出缓冲大小 */
#define     LOG_USING_COLOR    1                /**< 是否使用颜色 */
#define     LOG_MAX_NUMBER     5                /**< 允许注册的最大log对象数量 */
#define     LOG_AUTO_TAG       1                /**< 是否自动添加TAG */
#define     LOG_END            "\r\n"           /**< log信息结尾 */
#define     LOG_TAG            __FUNCTION__     /**< 自定添加的TAG */
#define     LOG_TIME_STAMP     0                /**< 设置获取系统时间戳 */

#ifndef     LOG_ENABLE
    #define LOG_ENABLE         1                /**< 使能log */
#endif

#define     LOG_ALL_OBJ        ((Log *)-1)      /**< 所有已注册的log对象 */

/**
 * 终端字体颜色代码
 */
#define     CSI_BLACK           30              /**< 黑色 */
#define     CSI_RED             31              /**< 红色 */
#define     CSI_GREEN           32              /**< 绿色 */
#define     CSI_YELLOW          33              /**< 黄色 */
#define     CSI_BLUE            34              /**< 蓝色 */
#define     CSI_FUCHSIN         35              /**< 品红 */
#define     CSI_CYAN            36              /**< 青色 */
#define     CSI_WHITE           37              /**< 白色 */
#define     CSI_BLACK_L         90              /**< 亮黑 */
#define     CSI_RED_L           91              /**< 亮红 */
#define     CSI_GREEN_L         92              /**< 亮绿 */
#define     CSI_YELLOW_L        93              /**< 亮黄 */
#define     CSI_BLUE_L          94              /**< 亮蓝 */
#define     CSI_FUCHSIN_L       95              /**< 亮品红 */
#define     CSI_CYAN_L          96              /**< 亮青 */
#define     CSI_WHITE_L         97              /**< 亮白 */
#define     CSI_DEFAULT         39              /**< 默认 */

#define     CSI(code)           "\033["#code"m" /**< ANSI CSI指令 */

/**
 * log级别字符(包含颜色)
 */
#if LOG_USING_COLOR == 1
#define     ERROR_TEXT          CSI(31)"E(%d) %s:"CSI(39)       /**< 错误标签 */
#define     WARNING_TEXT        CSI(33)"W(%d) %s:"CSI(39)       /**< 警告标签 */
#define     INFO_TEXT           CSI(32)"I(%d) %s:"CSI(39)       /**< 信息标签 */
#define     DEBUG_TEXT          CSI(34)"D(%d) %s:"CSI(39)       /**< 调试标签 */
#define     VERBOSE_TEXT        CSI(36)"V(%d) %s:"CSI(39)       /**< 冗余信息标签 */
#else
#define     ERROR_TEXT          "E(%d) %s:"
#define     WARNING_TEXT        "W(%d) %s:"
#define     INFO_TEXT           "I(%d) %s:"
#define     DEBUG_TEXT          "D(%d) %s:"
#define     VERBOSE_TEXT        "V(%d) %s:"
#endif


/**
 * @brief 日志级别定义
 * 
 */
typedef enum
{
    LOG_NONE = 0,                                  /**< 无级别 */
    LOG_ERROR = 1,                                 /**< 错误 */
    LOG_WRANING = 2,                               /**< 警告 */
    LOG_INFO = 3,                                  /**< 消息 */
    LOG_DEBUG = 4,                                 /**< 调试 */
    LOG_VERBOSE = 5,                               /**< 冗余 */
    LOG_ALL = 6,                                   /**< 所有日志 */
} LogLevel;


/**
 * @brief log对象定义
 * 
 */
typedef struct
{
    void (*write)(char *, short);                   /**< 写buffer */
    char active;                                    /**< 是否激活 */
    LogLevel level;                                 /**< 日志级别 */
    Shell *shell;                                   /**< 关联shell对象 */
} Log;



/**
 * @brief log打印(自动换行)
 * 
 * @param fmt 格式
 * @param ... 参数
 */
#define logPrintln(format, ...) \
        logWrite(LOG_ALL_OBJ, LOG_NONE, format"\r\n", ##__VA_ARGS__)


/**
 * @brief 日志格式化输出
 * 
 * @param text 消息文本
 * @param level 日志级别
 * @param fmt 格式
 * @param ... 参数
 */
#define logFormat(text, level, fmt, ...) \
        if (LOG_ENABLE) {\
            logWrite(LOG_ALL_OBJ, level, text" "fmt""LOG_END, \
                LOG_TIME_STAMP, LOG_TAG, ##__VA_ARGS__); }

/**
 * @brief 错误log输出
 * 
 * @param fmt 格式
 * @param ... 参数
 */
#define logError(fmt, ...) \
        logFormat(ERROR_TEXT, LOG_ERROR, fmt, ##__VA_ARGS__)

/**
 * @brief 警告log输出
 * 
 * @param fmt 格式
 * @param ... 参数
 */
#define logWarning(fmt, ...) \
        logFormat(WARNING_TEXT, LOG_WRANING, fmt, ##__VA_ARGS__)

/**
 * @brief 信息log输出
 * 
 * @param fmt 格式
 * @param ... 参数
 */
#define logInfo(fmt, ...) \
        logFormat(INFO_TEXT, LOG_INFO, fmt, ##__VA_ARGS__)

/**
 * @brief 调试log输出
 * 
 * @param fmt 格式
 * @param ... 参数
 */
#define logDebug(fmt, ...) \
        logFormat(DEBUG_TEXT, LOG_DEBUG, fmt, ##__VA_ARGS__)

/**
 * @brief 冗余log输出
 * 
 * @param fmt 格式
 * @param ... 参数
 */
#define logVerbose(fmt, ...) \
        logFormat(VERBOSE_TEXT, LOG_VERBOSE, fmt, ##__VA_ARGS__)

/**
 * @brief 断言
 * 
 * @param expr 表达式
 * @param action 断言失败操作
 */
#define logAssert(expr, action) \
        if (!(expr)) { \
            logError("\"" #expr "\" assert failed at file: %s, line: %d", __FILE__, __LINE__); \
            action; \
        }

void logRegister(Log *log, Shell *shell);
void logUnRegister(Log *log);
void logSetLevel(Log *log, LogLevel level);
void logWrite(Log *log, LogLevel level, char *fmt, ...);
void logHexDump(Log *log, void *base, unsigned int length);

#endif
