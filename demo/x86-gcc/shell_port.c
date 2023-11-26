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
#include "shell_ext.h"
#include "shell_fs.h"
#include "shell_passthrough.h"
#include "shell_secure_user.h"
#include "log.h"
#include "telnetd.h"
#include <stdio.h>
#include <dirent.h>
#include <unistd.h>
#include <stddef.h>
#include <string.h>
#include <sys/time.h>
#include <pthread.h>

Shell shell;
char shellBuffer[512];
ShellFs shellFs;
char shellPathBuffer[512] = "/";
Log log = {
    .active = 1,
    .level = LOG_DEBUG
};

/**
 * @brief 获取系统tick
 * 
 * @return unsigned int 系统tick
 */
unsigned int userGetTick()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (tv.tv_sec * 1000 + tv.tv_usec / 1000) & 0x7FFFFFFF;
}

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
    while (length--)
    {
        *data++ = getchar();
    }
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
 * @brief 新线程接口
 * 
 * @param handler 线程函数
 * @param param 线程参数
 * 
 * @return int 0 启动成功 -1 启动失败
 */
int userNewThread(void *handler, void *param)
{
    pthread_t tid;

    return pthread_create(&tid, NULL, handler, param) == 0 ? 0 : -1;
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

    telentdInit(userNewThread);

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

int shellRetValChange(int value)
{
    return value;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),
changeRetVal, shellRetValChange, change shell return vallue);

char *shellSecureUserHandlerTest(const char *name)
{
    return (char *)name;
}
SHELL_EXPORT_SECURE_USER(SHELL_CMD_PERMISSION(0xFF), secure, shellSecureUserHandlerTest, secure user test);

int systemPassthrough(char *data, unsigned short len)
{
    system(data);
}
SHELL_EXPORT_PASSTROUGH(SHELL_CMD_PERMISSION(0), system, system>>\x20, systemPassthrough, passthrough for system command);

#if SHELL_USING_FUNC_SIGNATURE == 1

void shellFuncSignatureTest(int a, char *b, char c)
{
    printf("a = %d, b = %s, c = %c\r\n", a, b, c);
}
SHELL_EXPORT_CMD_SIGN(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),
funcSignatureTest, shellFuncSignatureTest, test function signature, isc);

typedef struct {
    int a;
    char *b;
} TestStruct;

int testStructParser(char *string, void **param)
{
    TestStruct *data = malloc(sizeof(TestStruct));
    data->b = malloc(16);
    if (sscanf(string, "%d %s", &(data->a), data->b) == 2)
    {
        *param = (void *)data;
        return 0;
    }
    return -1;
}

int testStructClener(void *param)
{
    TestStruct *data = (TestStruct *)param;
    free(data->b);
    free(data);
    return 0;
}
SHELL_EXPORT_PARAM_PARSER(0, LTestStruct;, testStructParser, testStructClener);

void shellParamParserTest(int a, TestStruct *data, char *c)
{
    printf("a = %d, data->a = %d, data->b = %s, c = %s\r\n", a, data->a, data->b, c);
}
SHELL_EXPORT_CMD_SIGN(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),
paramParserTest, shellParamParserTest, test function signature and param parser, iLTestStruct;s);

#if SHELL_SUPPORT_ARRAY_PARAM == 1
int shellArrayTest(int a, int *b, char *c, TestStruct **datas)
{
    int i;
    printf("a = %d, b = %p, c = %p, datas = %p\r\n", a, b, c, datas);
    for (i = 0; i < shellGetArrayParamSize(b); i++)
    {
        printf("b[%d] = %d\r\n", i, b[i]);
    }
    for (i = 0; i < shellGetArrayParamSize(c); i++)
    {
        printf("c[%d] = %d\r\n", i, c[i]);
    }
    for (i = 0; i < shellGetArrayParamSize(datas); i++)
    {
        printf("datas[%d]->a = %d, datas[%d]->b = %s\r\n", i, datas[i]->a, i, datas[i]->b);
    }
    return 0;
}
SHELL_EXPORT_CMD_SIGN(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),
arrayTest, shellArrayTest, test array param parser, i[i[q[LTestStruct;);
#endif /** SHELL_SUPPORT_ARRAY_PARAM == 1 */

#endif /** SHELL_USING_FUNC_SIGNATURE == 1 */

int paramTest(int argc, char *argv[])
{
    int i;
    printf("argc = %d\r\n", argc);
    for (i = 0; i < argc; i++)
    {
        printf("argv[%d] = %s\r\n", i, argv[i]);
    }
    return 0;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN),
paramTest, paramTest, test param);
