/**
 * @file log.c
 * @author Letter (nevermindzzt@gmail.com)
 * @brief log
 * @version 1.0.0
 * @date 2020-07-30
 * 
 * @copyright (c) 2020 Letter
 * 
 */
#include "log.h"
#include "stdio.h"
#include "stdarg.h"
#include "shell.h"

#if LOG_USING_COLOR == 1
#define memPrintHead CSI(31) \
    "    Offset: 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F" \
    CSI(39) \
    "\r\n"
#define memPrintAddr CSI(31)"0x%08x: "CSI(39)
#else
#define memPrintHead "    Offset: 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\r\n"
#define memPrintAddr "0x%08x: "
#endif

Log *logList[LOG_MAX_NUMBER] = {0};
static char logBuffer[LOG_BUFFER_SIZE];

/**
 * @brief 注册log对象
 * 
 * @param log log对象
 */
void logRegister(Log *log, Shell *shell)
{
    if (shell)
    {
        log->shell = shell;
    #if SHELL_USING_COMPANION == 1
        shellCompanionAdd(shell, SHELL_COMPANION_ID_LOG, log);
    #endif
    }
    for (short i = 0; i < LOG_MAX_NUMBER; i++)
    {
        if (logList[i] == 0)
        {
            logList[i] = log;
            return;
        }
    }
}


/**
 * @brief 注销log对象
 * 
 * @param log log对象
 */
void logUnRegister(Log *log)
{
    for (short i = 0; i < LOG_MAX_NUMBER; i++)
    {
        if (logList[i] == log)
        {
            logList[i] = 0;
            return;
        }
    }
}


/**
 * @brief 设置log日志级别
 * 
 * @param log log对象
 * @param level 日志级别
 */
void logSetLevel(Log *log, LogLevel level)
{
    logAssert(log, return);
    log->level = level;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),
logSetLevel, logSetLevel, set log level);


/**
 * @brief log写buffer
 * 
 * @param log log对象
 * @param level 日志级别
 * @param buffer buffer
 * @param len buffer长度
 */
static void logWriteBuffer(Log *log, LogLevel level, char *buffer, short len)
{
    if (log == LOG_ALL_OBJ)
    {
        for (short i = 0; i < LOG_MAX_NUMBER; i++)
        {
            if (logList[i] 
                && logList[i]->active
                && logList[i]->level >= level)
            {
#if SHELL_SUPPORT_END_LINE == 1
                shellWriteEndLine(logList[i]->shell, logBuffer, len);
#else
                logList[i]->write(logBuffer, len);
#endif
            }
        }
    }
    else if (log && log->active && log->level >= level)
    {
#if SHELL_SUPPORT_END_LINE == 1
        shellWriteEndLine(log->shell, logBuffer, len);
#else
        log->write(logBuffer, len);
#endif
    }
}

/**
 * @brief log格式化写入数据
 * 
 * @param log log对象
 * @param level log级别
 * @param fmt 格式
 * @param ... 参数
 */
void logWrite(Log *log, LogLevel level, char *fmt, ...)
{
    va_list vargs;
    short len;

    va_start(vargs, fmt);
    len = vsnprintf(logBuffer, LOG_BUFFER_SIZE - 1, fmt, vargs);
    va_end(vargs);

    logWriteBuffer(log, level, logBuffer, len);
}


/**
 * @brief 16进制输出
 * 
 * @param log log对象
 * @param level 日志级别
 * @param base 内存基址
 * @param length 长度
 */
void logHexDump(Log *log, LogLevel level, void *base, unsigned int length)
{
    unsigned char *address;
    unsigned int len;
    unsigned int printLen = 0;

    if (length == 0 || (log != LOG_ALL_OBJ && log->level < level))
    {
        return;
    }

    len = snprintf(logBuffer, LOG_BUFFER_SIZE - 1, "memory of 0x%08x, size: %d:\r\n%s",
                   (unsigned int)base, length, memPrintHead);
    logWriteBuffer(log, level, logBuffer, len);
    
    address = (unsigned char *)((unsigned int)base & (~0x0000000F));
    length += (unsigned int)base - (unsigned int)address;
    length = (length + 15) & (~0x0000000F);

    len = length;

    while (length)
    {
        printLen += sprintf(logBuffer + printLen, memPrintAddr, (unsigned int)address);
        for (int i = 0; i < 16; i++)
        {
            if ((unsigned int)(address + i) < (unsigned int)base
                || (unsigned int)(address + i) >= (unsigned int)base + len)
            {
                logBuffer[printLen ++] = ' ';
                logBuffer[printLen ++] = ' ';
                logBuffer[printLen ++] = ' ';
            }
            else
            {
                printLen += sprintf(logBuffer + printLen, "%02x ", *(address + i));
            }
        }
        logBuffer[printLen ++] = '|';
        logBuffer[printLen ++] = ' ';
        for (int i = 0; i < 16; i++)
        {
            if ((unsigned int)(address + i) < (unsigned int)base
                || (unsigned int)(address + i) >= (unsigned int)base + len)
            {
                logBuffer[printLen ++] = ' ';
            }
            else
            {
                if (*(address + i) >= 32 && *(address + i) <= 126)
                {
                    printLen += sprintf(logBuffer + printLen, "%c", *(address + i));
                }
                else
                {
                    logBuffer[printLen ++] = '.';
                }
            }
        }
        logBuffer[printLen ++] = '|';
        logBuffer[printLen ++] = '\r';
        logBuffer[printLen ++] = '\n';
        logWriteBuffer(log, level, logBuffer, printLen);
        address += 16;
        length -= 16;
        printLen = 0;
    }
}
#if SHELL_USING_COMPANION == 1
SHELL_EXPORT_CMD_AGENCY(
SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC)|SHELL_CMD_DISABLE_RETURN,
hexdump, logHexDump, hex dump\r\n hexdump [base] [len],
(void *)shellCompanionGet(shellGetCurrent(), SHELL_COMPANION_ID_LOG), LOG_NONE, p1, p2);
#else
SHELL_EXPORT_CMD(
SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC)|SHELL_CMD_DISABLE_RETURN,
hexdump, logHexDump, hex dump\r\n hexdump [log] [level] [base] [len]);
#endif /** SHELL_USING_COMPANION == 1 */

#if SHELL_USING_COMPANION == 1
void logSwitchLevel(Shell *shell)
{
    Log *log = shellCompanionGet(shell, SHELL_COMPANION_ID_LOG);
    SHELL_ASSERT(log, return);
    log->level = (LogLevel)(log->level >= LOG_ALL ? LOG_NONE : (log->level + 1));
    logPrintln("set log level: %d", log->level);
}
SHELL_EXPORT_KEY(SHELL_CMD_PERMISSION(0), 0x04000000, logSwitchLevel, switch log level);
#endif /** SHELL_USING_COMPANION == 1 */
