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

#if LOG_USING_LOCK == 1
/**
 * @brief   上锁log对象
 * @param   log log对象
 */
static void logLock(Log *log)
{
    if (log == LOG_ALL_OBJ)
    {
        for (short i = 0; i < LOG_MAX_NUMBER; i++)
        {
            if (logList[i] && logList[i]->active)
            {
                if (logList[i]->lock)
                {
                    LOG_LOCK(logList[i]);
                }
            }
        }
    }
    else if (log->lock)
    {
        LOG_LOCK(log);
    }
}

/**
 * @brief   解锁log对象
 * @param   log log对象
 */
static void logUnlock(Log *log)
{
    if (log == LOG_ALL_OBJ)
    {
        for (short i = 0; i < LOG_MAX_NUMBER; i++)
        {
            if (logList[i] && logList[i]->active)
            {
                if (logList[i]->unlock)
                {
                    LOG_UNLOCK(logList[i]);
                }
            }
        }
    }
    else if (log->unlock)
    {
        LOG_UNLOCK(log);
    }
}
#endif /* LOG_USING_LOCK == 1 */

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
#if SHELL_USING_COMPANION == 1
SHELL_EXPORT_CMD_AGENCY(
SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC)|SHELL_CMD_DISABLE_RETURN,
logSetLevel, logSetLevel,  set log level\r\n logSetLevel [level],
(void *)shellCompanionGet(shellGetCurrent(), SHELL_COMPANION_ID_LOG), p1);
#else
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),
logSetLevel, logSetLevel, set log level\r\n logSetLevel [log] [level]);
#endif /** SHELL_USING_COMPANION == 1 */


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
#if LOG_USING_LOCK == 1
    logLock(log);
#endif /* LOG_USING_LOCK == 1 */
    if (log == LOG_ALL_OBJ)
    {
        for (short i = 0; i < LOG_MAX_NUMBER; i++)
        {
            if (logList[i] 
                && logList[i]->active
                && logList[i]->level >= level)
            {
                logList[i]->write(logBuffer, len);
            }
        }
    }
    else if (log && log->active && log->level >= level)
    {
        log->write(logBuffer, len);
    }
#if LOG_USING_LOCK == 1
    logUnlock(log);
#endif /* LOG_USING_LOCK == 1 */
}

/**
 * @brief log格式化写入数据
 * 
 * @param log log对象
 * @param level log级别
 * @param fmt 格式
 * @param ... 参数
 */
void logWrite(Log *log, LogLevel level, const char *fmt, ...)
{
    va_list vargs;
    int len;
    
#if LOG_USING_LOCK == 1
    logLock(log);
#endif /* LOG_USING_LOCK == 1 */
    va_start(vargs, fmt);
    len = vsnprintf(logBuffer, LOG_BUFFER_SIZE - 1, fmt, vargs);
    va_end(vargs);

    if (len > LOG_BUFFER_SIZE)
    {
        len = LOG_BUFFER_SIZE;
    }

    logWriteBuffer(log, level, logBuffer, len);
#if LOG_USING_LOCK == 1
    logUnlock(log);
#endif /* LOG_USING_LOCK == 1 */
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
#if LOG_USING_LOCK == 1
    logLock(log);
#endif /* LOG_USING_LOCK == 1 */
    len = snprintf(logBuffer, LOG_BUFFER_SIZE - 1, "memory of 0x%08x, size: %d:\r\n%s",
                   (unsigned int)base, length, memPrintHead);
    logWriteBuffer(log, level, logBuffer, len);

    len = length;
    
    address = (unsigned char *)((unsigned int)base & (~0x0000000F));
    length += (unsigned int)base - (unsigned int)address;
    length = (length + 15) & (~0x0000000F);

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
        logBuffer[printLen ++] = ' ';
        logBuffer[printLen ++] = '|';
        logBuffer[printLen ++] = '\r';
        logBuffer[printLen ++] = '\n';
        logWriteBuffer(log, level, logBuffer, printLen);
        address += 16;
        length -= 16;
        printLen = 0;
    }
#if LOG_USING_LOCK == 1
    logUnlock(log);
#endif /* LOG_USING_LOCK == 1 */
}
#if SHELL_USING_COMPANION == 1
SHELL_EXPORT_CMD_AGENCY(
SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC)|SHELL_CMD_DISABLE_RETURN,
hexdump, logHexDump, hex dump\r\n hexdump [base] [len],
(void *)shellCompanionGet(shellGetCurrent(), SHELL_COMPANION_ID_LOG), LOG_NONE, (void *)p1, (unsigned int)p2);
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
