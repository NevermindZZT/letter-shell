/**
 * @file game.c
 * @author Letter
 * @brief game
 * @version 0.1
 * @date 2021-07-18
 * 
 * @copyright (c) 2021 Letter
 * 
 */
#include "shell_cmd_group.h"

extern int main_2048(int argc, char *argv[]);
extern int main_pushbox(int argc, char* argv[]);


ShellCommand gameGroup[] =
{
    SHELL_CMD_GROUP_ITEM(SHELL_TYPE_CMD_MAIN, 2048, main_2048, 
        game 2048\n2048 [param]\nParam: blackwhite bluered or null),
    SHELL_CMD_GROUP_ITEM(SHELL_TYPE_CMD_MAIN,
        pushbox, main_pushbox, game pushbox),
    SHELL_CMD_GROUP_END()
};
SHELL_EXPORT_CMD_GROUP(
SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN)|SHELL_CMD_DISABLE_RETURN,
game, gameGroup, games);
