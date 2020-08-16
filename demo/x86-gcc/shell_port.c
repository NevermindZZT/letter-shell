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
#include <stdio.h>
#include <dirent.h>
#include <unistd.h>
#include <stddef.h>
#include <string.h>

Shell shell;
char shellBuffer[512];
ShellFs shellFs;
char shellPathBuffer[512] = "/";

/**
 * @brief 用户shell写
 * 
 * @param data 数据
 */
void userShellWrite(char data)
{
    putchar(data);
}

/**
 * @brief 用户shell读
 * 
 * @param data 数据
 * @return char 状态
 */
signed char userShellRead(char *data)
{
    system("stty -echo");
    system("stty -icanon");
    *data = getchar();
    system("stty icanon");
    system("stty echo");
    return 0;
}

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
    shellSetPath(&shell, shellPathBuffer);
    shellInit(&shell, shellBuffer, 512);
    shellCompanionAdd(&shell, SHELL_COMPANION_ID_FS, &shellFs);
}
