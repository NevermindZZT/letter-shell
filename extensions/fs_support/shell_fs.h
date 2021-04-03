/**
 * @file shell_fs.h
 * @author Letter (nevermindzzt@gmail.com)
 * @brief shell file system support
 * @version 0.1
 * @date 2020-07-22
 * 
 * @copyright (c) 2020 Letter
 * 
 */
#ifndef __SHELL_FS_H__
#define __SHELL_FS_H__

#include "stddef.h"
#include "shell.h"

#define     SHELL_FS_VERSION                "1.0.0"

#define     SHELL_COMPANION_ID_FS           -1

#define     SHELL_FS_LIST_FILE_BUFFER_MAX   4096

/**
 * @brief shell文件系统支持结构体
 * 
 */
typedef struct shell_fs
{
    size_t (*getcwd)(char *, size_t);
    size_t (*chdir)(char *);
    size_t (*listdir)(char *dir, char *buffer, size_t maxLen);

    struct {
        char *path;
        size_t pathLen;
    } info;
} ShellFs;

void shellFsInit(ShellFs *shellFs, char *pathBuffer, size_t pathLen);

#endif
