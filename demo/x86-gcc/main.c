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
#include <signal.h>

static int demoExit(int value)
{
    system("stty icanon");
    system("stty echo");
    exit(value);
    return value;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC)|SHELL_CMD_PARAM_NUM(1),
exit, demoExit, exit);
SHELL_EXPORT_KEY_AGENCY(SHELL_CMD_PERMISSION(0),
0x03000000, exit, demoExit, 0);

static void signalHandler(int signal)
{
    demoExit(0);
}

int main(void)
{
    signal(SIGINT, signalHandler);
    system("stty -echo");
    system("stty -icanon");
    userShellInit();
    shellTask(&shell);
    return 0;
}
