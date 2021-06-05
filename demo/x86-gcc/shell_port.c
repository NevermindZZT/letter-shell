/**
 * @file shell_port.c
 * @author Letter (NevermindZZT@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2019-02-22
 * 
 * @copyright (c) 2019 Letter
 * 
 */

#include "shell.h"
#include "shell_fs.h"
#include "shell_passthrough.h"
#include "log.h"
#include <stdio.h>
#include <dirent.h>
#include <unistd.h>
#include <stddef.h>
#include <string.h>

Shell shell;
char shellBuffer[512];
ShellFs shellFs;
char shellPathBuffer[512] = "/";
Log log = {
    .active = 1,
    .level = LOG_DEBUG
};

/**
 * @brief 日志写函数实现
 * 
 * @param buffer 数据
 * @param len 数据长度
 * 
 */
void terminalLogWrite(char *buffer, short len)
{
    if (log.shell)
    {
        shellWriteEndLine(log.shell, buffer, len);
    }
}

/**
 * @brief 用户shell写
 * 
 * @param data 数据
 */
unsigned short userShellWrite(char *data, unsigned short len)
{
    unsigned short length = len;
    while (length--)
    {
        putchar(*data++);
    }
    return len;
}

/**
 * @brief 用户shell读
 * 
 * @param data 数据
 * @return char 状态
 */
unsigned short userShellRead(char *data, unsigned short len)
{
    unsigned short length = len;
    system("stty -echo");
    system("stty -icanon");
    while (length--)
    {
        *data++ = getchar();
    }
    // system("stty icanon");
    // system("stty echo");
    return len;
}

#if SHELL_USING_LOCK == 1
static int lockCount = 0;
int userShellLock(struct shell_def *shell)
{
    printf("lock: %d\r\n", lockCount);
    return 0;
}


int userShellUnlock(struct shell_def *shell)
{
    printf("unlock: %d\r\n", lockCount);
    return 0;
}
#endif

/**
 * @brief 列出文件
 * 
 * @param path 路径
 * @param buffer 结果缓冲
 * @param maxLen 最大缓冲长度
 * @return size_t 0
 */
size_t userShellListDir(char *path, char *buffer, size_t maxLen)
{
    DIR *dir;
    struct dirent *ptr;
    int i;
    dir = opendir(path);
    memset(buffer, 0, maxLen);
    while((ptr = readdir(dir)) != NULL)
    {
        strcat(buffer, ptr->d_name);
        strcat(buffer, "\t");
    }
    closedir(dir);
    return 0;
}

/**
 * @brief 用户shell初始化
 * 
 */
void userShellInit(void)
{
    shellFs.getcwd = getcwd;
    shellFs.chdir = chdir;
    shellFs.listdir = userShellListDir;
    shellFsInit(&shellFs, shellPathBuffer, 512);

    shell.write = userShellWrite;
    shell.read = userShellRead;
#if SHELL_USING_LOCK == 1
    shell.lock = userShellLock;
    shell.unlock = userShellUnlock;
#endif
    shellSetPath(&shell, shellPathBuffer);
    shellInit(&shell, shellBuffer, 512);
    shellCompanionAdd(&shell, SHELL_COMPANION_ID_FS, &shellFs);

    log.write = terminalLogWrite;
    logRegister(&log, &shell);

    // logDebug("hello world");
    // logHexDump(LOG_ALL_OBJ, LOG_DEBUG, (void *)&shell, sizeof(shell));
}


int varInt = 0;
SHELL_EXPORT_VAR(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_VAR_INT), varInt, &varInt, int var test);

char str[] = "test string";
SHELL_EXPORT_VAR(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_VAR_STRING), varStr, str, string var test);

SHELL_EXPORT_VAR(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_VAR_POINT), shell, &shell, pointer var test);

int dumpInfo(void)
{
    printf("hello world\r\n");
    return 0;
}

int sysInfoSet(int value)
{
    printf("sys info: %d\r\n", value);
    return value;
}

ShellNodeVarAttr sysInfo = {
    .get = dumpInfo,
    .set = sysInfoSet,
};
SHELL_EXPORT_VAR(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_VAR_NODE), sysInfo, &sysInfo, node var test);

int shellInfoGet(Shell *shell)
{
    printf("user name: %s\r\n", shell->info.user->data.user.name);
    printf("path: %s\r\n", shell->info.path);
    return (int)shell;
}

ShellNodeVarAttr shellInfo = {
    .var = &shell,
    .get = shellInfoGet,
};
SHELL_EXPORT_VAR(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_VAR_NODE)|SHELL_CMD_READ_ONLY,
shellInfo, &shellInfo, node var test);

void shellKeyTest(void)
{
    char data;
    Shell *shell = shellGetCurrent();
    SHELL_ASSERT(shell && shell->read, return);
    while (1)
    {
        if (shell->read(&data, 1) == 1)
        {
            if (data == '\n' || data == '\r')
            {
                return;
            }
            shellPrint(shell, "%02x ", data);
        }
    }
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),
keyTest, shellKeyTest, key test);

void shellScanTest(void)
{
    int a;
    char b[12];
    shellScan(shellGetCurrent(), "%x %s\n", &a, b);
    shellPrint(shellGetCurrent(), "result: a = %x, b = %s\r\n", a, b);
}
SHELL_EXPORT_CMD(
SHELL_CMD_PERMISSION(0x00)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC)|SHELL_CMD_DISABLE_RETURN,
scanTest, shellScanTest, test scan);


void shellPassthroughTest(char *data, unsigned short len)
{
    printf("passthrough mode test, data: %s, len: %d\r\n", data, len);
}
SHELL_EXPORT_PASSTROUGH(SHELL_CMD_PERMISSION(0), passTest, passthrough>>, shellPassthroughTest, passthrough mode test);
