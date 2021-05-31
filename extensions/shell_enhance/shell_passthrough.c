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

/**
 * @brief shell passthrough 模式
 * 
 * @param shell shell
 * @param prompt passthrough 提示符
 * @param handler pssthrough handler
 * 
 * @return unsigned int 返回值
 */
unsigned int shellPassthrough(Shell *shell, const char *prompt, ShellPassthrough handler)
{
    char data;

    shell->status.isActive = 0;
    shellWriteString(shell, prompt);
    while (1)
    {
        if (shell->read && shell->read(&data, 1) == 1)
        {
            if (data == '\r' || data == '\n')
            {
                if (shell->parser.length == 0)
                {
                    continue;
                }
                shellWriteString(shell, "\r\n");
                shell->parser.buffer[shell->parser.length] = 0;
                handler(shell->parser.buffer, shell->parser.length);
                shellWriteString(shell, prompt);
            }
            else if (data == SHELL_PASSTHROUGH_EXIT_KEY)
            {
                shellWriteString(shell, "\r\n");
                return -1;
            }
            else
            {
                shellHandler(shell, data);
            }
        }
    }
    return 0;
}
