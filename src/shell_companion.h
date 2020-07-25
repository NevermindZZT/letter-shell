/**
 * @file shell_companion.h
 * @author Letter (nevermindzzt@gmail.com)
 * @brief shell companion object support
 * @version 3.0.3
 * @date 2020-07-22
 * 
 * @copyright (c) 2020 Letter
 * 
 */
#ifndef __SHELL_COMPANION_H__
#define __SHELL_COMPANION_H__

#include "shell_cfg.h"
#include "shell.h"

/**
 * @brief shell伴生对象定义
 */
typedef struct shell_companion_object
{
    int id;                                                     /**< 伴生对象ID */
    void *obj;                                                  /**< 伴生对象 */
    struct shell_companion_object *next;                        /**< 下一个伴生对象 */
} ShellCompanionObj;


signed char shellCompanionAdd(struct shell_def *shell, int id, void *object);
signed char shellCompanionDel(struct shell_def *shell, int id);
void *shellCompanionGet(struct shell_def *shell, int id);


#endif
