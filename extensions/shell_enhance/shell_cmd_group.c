/**
 * @file shell_cmd_group.c
 * @author Letter(nevermindzzt@gmail.com)
 * @brief shell command group support
 * @version 0.1
 * @date 2020-10-18
 * 
 * @copyright (c) 2020 Letter
 * 
 */
#include "shell_cmd_group.h"
#include "string.h"
#include "stdio.h"

unsigned int shellRunCommand(Shell *shell, ShellCommand *command);

static void shellCmdGroupHelp(Shell *shell, char *name, ShellCommand *group);

/**
 * @brief shell命令组运行
 * 
 * @param group 命令数组
 * @param argc 参数数量
 * @param argv 参数
 * 
 * @return unsigned int 执行命令返回值
 */
unsigned int shellCmdGroupRun(ShellCommand *group, int argc, char *argv[])
{
    ShellCommand *item = group;
    Shell *shell = shellGetCurrent();

    if (argc < 2 || !shell)
    {
        return -1;
    }
    if (strcmp("-h", argv[1]) == 0)
    {
        shellCmdGroupHelp(shell, argv[0], group);
        return 0;
    }
    while (item->data.cmd.name)
    {
        if (strcmp(item->data.cmd.name, argv[1]) == 0)
        {
            for (short i = 0; i < argc - 1; i++)
            {
                argv[i] = argv[i + 1];
            }
            shell->parser.paramCount--;
            return shellRunCommand(shell, item);
            break;
        }
        item++;
    }
    shellWriteString(shell, "sub command not found\r\n");
    return -1;
}

/**
 * @brief shell 命令组帮助
 * 
 * @param shell shell对象
 * @param name 命令组名
 * @param group 命令数组
 */
static void shellCmdGroupHelp(Shell *shell, char *name, ShellCommand *group)
{
    ShellCommand *item = group;

    shellWriteString(shell, "command group help of ");
    shellWriteString(shell, name);
    shellWriteString(shell, "\r\n");

    while (item->data.cmd.name)
    {
        shellWriteString(shell, item->data.cmd.name);
        shellWriteString(shell, ": ");
        if (item->data.cmd.desc) {
            shellWriteString(shell, item->data.cmd.desc);
        }
        shellWriteString(shell, "\r\n");
        item++;
    }
}
