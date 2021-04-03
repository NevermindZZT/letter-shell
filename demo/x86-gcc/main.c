/**
 * @file main.c
 * @author Letter (NevermindZZT@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2020-07-12
 * 
 * @copyright (c) 2019 Letter
 * 
 */
#include "shell_port.h"
#include <stdlib.h>

int main(void)
{
    userShellInit();
    shellTask(&shell);
    return 0;
}


SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC)|SHELL_CMD_PARAM_NUM(1),
exit, exit, exit);
SHELL_EXPORT_KEY_AGENCY(SHELL_CMD_PERMISSION(0),
0x03000000, exit, exit, 0);
