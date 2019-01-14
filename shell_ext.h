/**
 * @file shell_ext.h
 * @author Letter (NevermindZZT@gmail.com)
 * @brief shell extensions
 * @version 1.0.0
 * @date 2019-01-05
 * 
 * @Copyright (c) 2019 Letter
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
    NUM_TYPE_INT,                                           /**< 十进制整型 */
    NUM_TYPE_BIN,                                           /**< 二进制整型 */
    NUM_TYPE_OCT,                                           /**< 八进制整型 */
    NUM_TYPE_HEX,                                           /**< 十六进制整型 */
    NUM_TYPE_FLOAT                                          /**< 浮点型 */
} NUM_Type;

unsigned int shellExtParsePara(char *string);
int shellExtRun(shellFunction function, int argc, char *argv[]);

#endif
