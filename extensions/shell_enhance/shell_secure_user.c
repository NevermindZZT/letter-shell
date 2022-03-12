/**
 * @file shell_secure_user.c
 * @author Letter (nevermindzzt@gmail.com)
 * @brief shell secure user
 * @version 0.1
 * @date 2022-03-12
 * 
 * @copyright Copyright (c) 2022 Letter
 * 
 */
#include "shell_secure_user.h"

extern void shellSetUser(Shell *shell, const ShellCommand *user);

ShellCommand secureUser;

int shellSecureUser(Shell *shell, const char *name, int attr, ShellSecureUserGetPassword handler)
{
    secureUser.attr.value = attr | SHELL_CMD_TYPE(SHELL_TYPE_USER);
    secureUser.data.user.name = name;
    secureUser.data.user.password = handler(name);
    shellSetUser(shell, &secureUser);
    return 0;
}
