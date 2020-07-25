/**
 * @file shell_companion.c
 * @author Letter (nevermindzzt@gmail.com)
 * @brief shell companion object support
 * @version 3.0.3
 * @date 2020-07-22
 * 
 * @copyright (c) 2020 Letter
 * 
 */
 #include "shell_companion.h"
 #include "shell.h"
 

/**
 * @brief shell添加伴生对象
 * 
 * @param shell shell对象
 * @param id 伴生对象ID
 * @param object 伴生对象
 * @return signed char 0 添加成功 -1 添加失败
 */
signed char shellCompanionAdd(struct shell_def *shell, int id, void *object)
{
    ShellCompanionObj *companions = shell->info.companions;
    ShellCompanionObj *node = SHELL_MALLOC(sizeof(ShellCompanionObj));
    SHELL_ASSERT(node, return -1);
    node->id = id;
    node->obj = object;
    node->next = companions;
    shell->info.companions = node;
    return 0;
}

/**
 * @brief shell删除伴生对象
 * 
 * @param shell shell对象
 * @param id 伴生对象ID
 * @return signed char 0 删除成功 -1 无匹配对象
 */
signed char shellCompanionDel(struct shell_def *shell, int id)
{
    ShellCompanionObj *companions = shell->info.companions;
    ShellCompanionObj *front = companions;
    while (companions)
    {
        if (companions->id == id)
        {
            if (companions == shell->info.companions && !(companions->next)) 
            {
                shell->info.companions = (void *)0;
            }
            else
            {
                front->next = companions->next;
            }
            SHELL_FREE(companions);
            return 0;
        }
        front = companions;
        companions = companions->next;
    }
    return -1;
}

/**
 * @brief shell获取伴生对象
 * 
 * @param shell shell对象
 * @param id 伴生对象ID
 * @return void* 伴生对象，无匹配对象时返回NULL
 */
void *shellCompanionGet(struct shell_def *shell, int id)
{
    SHELL_ASSERT(shell, return (void *)0);
    ShellCompanionObj *companions = shell->info.companions;
    while (companions)
    {
        if (companions->id == id)
        {
            return companions->obj;
        }
        companions = companions->next;
    }
    return (void *)0;
}
