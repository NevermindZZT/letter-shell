/**
 * @file shell_passthrough.c
 * @author Letter(nevermindzzt@gmail.com)
 * @brief shell passthrough mode
 * @version 0.1
 * @date 2021-05-31
 * 
 * @copyright (c) 2021 Letter
 * 
 */
#include "shell_passthrough.h"
#include "string.h"

/**
 * @brief shell passthrough 模式
 * 
 * @param shell shell
 * @param prompt passthrough 提示符
 * @param handler pssthrough handler
 * @param argc 命令行参数数量
 * @param argv 命令行参数
 * 
 * @return unsigned int 返回值
 */
unsigned int shellPassthrough(Shell *shell, const char *prompt, ShellPassthrough handler, int argc, char *argv[])
{
    char data;

    if (argc > 1)
    {
        handler(argv[1], strlen(argv[1]));
    }
    else
    {
        shell->status.isActive = 0;
        shellWriteString(shell, prompt);
        while (1)
        {
            if (shell->read && shell->read(&data, 1) == 1)
            {
                if (data == '\r' || data == '\n')
                {
                    shellWriteString(shell, "\r\n");
                    if (shell->parser.length != 0)
                    {
                        shell->parser.buffer[shell->parser.length] = 0;
                        handler(shell->parser.buffer, shell->parser.length);
                        shell->parser.length = 0;
                        shell->parser.cursor = 0;
                    }
                    shellWriteString(shell, prompt);
                }
                else if (data == SHELL_PASSTHROUGH_EXIT_KEY)
                {
                    shellWriteString(shell, "\r\n");
                    shell->parser.length = 0;
                    shell->parser.cursor = 0;
                    return -1;
                }
                else
                {
                    shellHandler(shell, data);
                }
            }
        }
    }
    return 0;
}
