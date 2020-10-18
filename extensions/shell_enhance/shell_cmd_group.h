/**
 * @file shell_cmd_group.h
 * @author Letter(nevermindzzt@gmail.com)
 * @brief shell command group support
 * @version 0.1
 * @date 2020-10-18
 * 
 * @copyright (c) 2020 Letter
 * 
 */
#include "shell.h"

/**
 * @brief shell 命令组函数名
 */
#define     SHELL_CMD_GROUP_FUNC_NAME(_group)   agency##_group

/**
 * @brief shell命令组函数定义
 * 
 * @param _group 命令数组
 */
#define     SHELL_CMD_GROUP_FUNC(_group) \
            void SHELL_CMD_GROUP_FUNC_NAME(_group)(int p1, int p2) \
            { shellCmdGroupRun(&_group, p1, p2); }


/**
 * @brief shell 命令组定义
 * 
 * @param _attr 属性
 * @param _name 命令组名
 * @param _group 命令数组
 * @param _desc 命令组描述
 */
#define SHELL_EXPORT_CMD_GROUP(_attr, _name, _group, _desc) \
        SHELL_CMD_GROUP_FUNC(_group) \
        SHELL_EXPORT_CMD(_attr, _name, SHELL_CMD_GROUP_FUNC_NAME(_group), _desc)

/**
 * @brief shell 命令组item定义
 * 
 * @param _type 命令类型(SHELL_TYPE_CMD_MAIN or SHELL_TYPE_CMD_FUNC)
 * @param _func 命令函数
 * @param _desc 命令描述
 */
#define SHELL_CMD_GROUP_ITEM(_type, _name, _func, _desc) \
        { \
            .attr.value = SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(_type)|SHELL_CMD_DISABLE_RETURN, \
            .data.cmd.name = #_name, \
            .data.cmd.function = (int (*)())_func, \
            .data.cmd.desc = #_desc \
        }

/**
 * @brief shell 命令组定义结尾
 * 
 * @note 需要添加在每个命令数组的最后一条
 */
#define SHELL_CMD_GROUP_END()   {0}

unsigned int shellCmdGroupRun(ShellCommand *group, int argc, char *argv[]);
