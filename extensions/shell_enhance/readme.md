# shell_enhance

![version](https://img.shields.io/badge/version-1.0.0-brightgreen.svg)
![standard](https://img.shields.io/badge/standard-c99-brightgreen.svg)
![build](https://img.shields.io/badge/build-2020.10.18-brightgreen.svg)
![license](https://img.shields.io/badge/license-MIT-brightgreen.svg)

- [shell_enhance](#shell_enhance)
  - [简介](#简介)
  - [组件](#组件)
    - [shell_cmd_group](#shell_cmd_group)

## 简介

`shell_enhance`是`letter shell 3.0`上用于增强shell功能的组件，`shell_enhance`分离了一些不常用的shell功能，对于需要这些功能的用户，只需要将对应的文件加入到编译系统中即可

## 组件

`shell_enhance`目前包含的组件如下：

| 组件            | 描述           | 依赖文件                            |
| --------------- | -------------- | ----------------------------------- |
| shell_cmd_group | 提供命令组功能 | shell_cmd_group.c shell_cmd_group.h |

### shell_cmd_group

`shell_cmd_group`提供了一个命令组的功能，用户可以将多个相关的命令打包成一个命令组，然后通过形如`cmdgroup subcmd [param]`的方式进行命令调用

- 定义命令数组

    命令数组用于将多个命令关联到一个数组，这个数组就是定义命令组需要的，你需要想正常命令一样，为每个子命令对应定义一个函数，然后将他们添加到一个数组中，可以使用宏`SHELL_CMD_GROUP_ITEM`进行定义

    ```c
    int test(int a, char *b)
    {
        printf("hello world, %d, %s\r\n", a, b);
        return 0;
    }

    int test2(int argc, char *argv[])
    {
        printf("%d parameters\r\n", argc);
        for (short i = 0; i < argc; i++)
        {
            printf("%s\t", argv[i]);
        }
        printf("\r\n");
    }

    ShellCommand testGroup[] =
    {
        SHELL_CMD_GROUP_ITEM(SHELL_TYPE_CMD_FUNC, test, test, command group test1),
        SHELL_CMD_GROUP_ITEM(SHELL_TYPE_CMD_MAIN, test2, test2, command group test2),
        SHELL_CMD_GROUP_END()
    };
    ```

    其中`SHELL_CMD_GROUP_END()`需要写在每个命令数组的最后

- 定义命令组

    使用宏`SHELL_EXPORT_CMD_GROUP`定义命令组

    ```c
    SHELL_EXPORT_CMD_GROUP(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN), gt, testGroup, command group test);
    ```

- 调用

    在命令行，使用`cmdgroup subcmd [param]`的形式调用命令

    ```sh
    letter:/$ gt test 666 Letter
    hello world, 666, Letter
    Return: 0, 0x00000000

    letter:/$ gt test2 hello world
    3 parameters
    test2   hello   world
    Return: 0, 0x00000000
    ```

- 命令帮助

    通过使用`cmdgroup -h`的方式查看完整的命令帮助信息

    ```sh
    letter:/$ gt -h
    command group help of gt
    test: command group test1
    test2: command group test2
    Return: 0, 0x00000000
    ```
