/**
 * @file shell_ext.h
 * @author Letter (NevermindZZT@gmail.com)
 * @brief shell extensions
 * @version 3.0.0
 * @date 2019-12-31
 * 
 * @copyright (c) 2019 Letter
 * 
 */

#ifndef __SHELL_EXT_H__
#define __SHELL_EXT_H__

#include "shell.h"

/**
 * @brief 数字类型
 * 
 */
typedef enum
{
    NUM_TYPE_DEC,                                           /**< 十进制整型 */
    NUM_TYPE_BIN,                                           /**< 二进制整型 */
    NUM_TYPE_OCT,                                           /**< 八进制整型 */
    NUM_TYPE_HEX,                                           /**< 十六进制整型 */
    NUM_TYPE_FLOAT                                          /**< 浮点型 */
} ShellNumType;

#if SHELL_SUPPORT_ARRAY_PARAM == 1
typedef struct
{
    unsigned short size;
    unsigned char elementBytes;
} ShellArrayHeader;
#endif /** SHELL_SUPPORT_ARRAY_PARAM == 1 */

int shellExtParsePara(Shell *shell, char *string, char *type, size_t *result);
#if SHELL_USING_FUNC_SIGNATURE == 1
int shellExtCleanerPara(Shell *shell, char *type, size_t param);
#endif /** SHELL_USING_FUNC_SIGNATURE == 1 */
#if SHELL_SUPPORT_ARRAY_PARAM == 1
int shellGetArrayParamSize(void *param);
#endif /** SHELL_SUPPORT_ARRAY_PARAM == 1 */
int shellExtRun(Shell *shell, ShellCommand *command, int argc, char *argv[]);

#endif
