/**
 * @file shell_fs.c
 * @author Letter (nevermindzzt@gmail.com)
 * @brief shell file system support
 * @version 0.1
 * @date 2020-07-22
 * 
 * @copyright (c) 2020 Letter
 * 
 */
#include "shell_fs.h"
#include "shell.h"
#include "stdio.h"

/**
 * @brief 改变当前路径(shell调用)
 * 
 * @param dir 路径
 */
void shellCD(char *dir)
{
    Shell *shell = shellGetCurrent();
    ShellFs *shellFs = shellCompanionGet(shell, SHELL_COMPANION_ID_FS);
    SHELL_ASSERT(shellFs, return);
    if (shellFs->chdir(dir) != 0)
    {
        shellWriteString(shell, "error: ");
        shellWriteString(shell, dir);
        shellWriteString(shell, " is not a directory\r\n");
    }
    shellFs->getcwd(shellFs->info.path, shellFs->info.pathLen);
}
SHELL_EXPORT_CMD(
SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC)|SHELL_CMD_DISABLE_RETURN,
cd, shellCD, change dir);

/**
 * @brief 列出文件(shell调用)
 * 
 */
void shellLS(void)
{
    size_t count;
    char *buffer;

    Shell *shell = shellGetCurrent();
    ShellFs *shellFs = shellCompanionGet(shell, SHELL_COMPANION_ID_FS);
    SHELL_ASSERT(shellFs, return);

    buffer = SHELL_MALLOC(SHELL_FS_LIST_FILE_BUFFER_MAX);
    SHELL_ASSERT(buffer, return);
    count = shellFs->listdir(shellGetPath(shell), buffer, SHELL_FS_LIST_FILE_BUFFER_MAX);

    shellWriteString(shell, buffer);

    SHELL_FREE(buffer);
}
SHELL_EXPORT_CMD(
SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC)|SHELL_CMD_DISABLE_RETURN,
ls, shellLS, list all files);

/**
 * @brief 初始化shell文件系统支持
 * 
 * @param shellFs shell文件系统对象
 * @param pathBuffer shell路径缓冲
 * @param pathLen 路径缓冲区大小
 */
void shellFsInit(ShellFs *shellFs, char *pathBuffer, size_t pathLen)
{
    shellFs->info.path = pathBuffer;
    shellFs->info.pathLen = pathLen;
    shellFs->getcwd(shellFs->info.path, pathLen);
}
