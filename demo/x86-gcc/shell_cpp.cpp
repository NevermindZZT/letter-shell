/**
 * @file shell_cpp.cpp
 * @author Letter
 * @brief shell cpp support test
 * @version 0.1
 * @date 2021-01-17
 * 
 * @copyright (c) 2021 Letter
 * 
 */

#include <shell_cpp.h>

void cppTest(void)
{
    Shell *shell = shellGetCurrent();
    shellPrint(shell, "hello world from cpp\r\n");
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),
cppTest, cppTest, cpp test);
