# shell_enhance

![version](https://img.shields.io/badge/version-1.0.0-brightgreen.svg)
![standard](https://img.shields.io/badge/standard-c99-brightgreen.svg)
![build](https://img.shields.io/badge/build-2020.10.18-brightgreen.svg)
![license](https://img.shields.io/badge/license-MIT-brightgreen.svg)

- [shell_enhance](#shell_enhance)
  - [简介](#简介)
  - [组件](#组件)
    - [shell_cmd_group](#shell_cmd_group)
    - [shell_passthrough](#shell_passthrough)
    - [shell_secure_user](#shell_secure_user)

## 简介

`shell_enhance`是`letter shell 3.0`上用于增强shell功能的组件，`shell_enhance`分离了一些不常用的shell功能，对于需要这些功能的用户，只需要将对应的文件加入到编译系统中即可

## 组件

`shell_enhance`目前包含的组件如下：

| 组件              | 描述           | 依赖文件                                |
| ----------------- | -------------- | --------------------------------------- |
| shell_cmd_group   | 提供命令组功能 | shell_cmd_group.c shell_cmd_group.h     |
| shell_passthrough | 提供透传功能   | shell_passthrough.c shell_passthrough.h |
| shell_secure_user | 安全用户功能   | shell_secure_user.c shell_secure_user.h |

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

### shell_passthrough

`shell_passthrough`提供了一个透传模式的功能，用户可以定义一个透传命令，执行透传命令后进入对应的透传模式，此后，命令行接收到的数据将以行为单位透传到定义的`handler`中，此扩展适用于一些需要进行终端数据转发的场景，常见的场景比如说通过命令行发送AT指令

- 定义hanadler

    passthrough模式需要定义一个函数，用于处理命令行传过来的数据，函数原型为`typedef int (*ShellPassthrough)(char *data, unsigned short len)`

    ```C
    void shellPassthroughTest(char *data, unsigned short len)
    {
        printf("passthrough mode test, data: %s, len: %d\r\n", data, len);
    }
    ```

- 定义passthrough

    使用宏`SHELL_EXPORT_PASSTROUGH`定义`passthrough`

    ```C
    SHELL_EXPORT_PASSTROUGH(SHELL_CMD_PERMISSION(0), passTest, passthrough>>, shellPassthroughTest, passthrough mode test);
    ```

- 调用

    直接在命令行输入定义的`passthrough`名字可进入对应的透传模式，之后在输入的数据会按照行为单位通过`handler`调用

    ```sh
    letter:/mnt/f/Github/letter shell/demo/x86-gcc$ passTest
    passthrough>>hello
    passthrough mode test, data: hello, len: 5
    ```

- 退出

    使用组合键`Ctrl + D`退出`passthrough`，键值可以在`shell_passthrough.h`文件中的`SHELL_PASSTHROUGH_EXIT_KEY`宏修改

- 单次调用

    某些情况下，使用`passthrough`模式时，我们可能只需要单次数据的透传，此时可以不进入`passthrough`命令行，直接调用命令带上透传的数据即可

    ```sh
    letter:/mnt/f/Github/letter shell/demo/x86-gcc$ passTest "hello world"
    passthrough mode test, data: hello world, len: 11
    ```

### shell_secure_user

`shell_secure_user`组件是对shell用户的一个补充，在`letter shell`中，我们可以通过定义不同的用户和分配命令权限，约束使用者可以执行的命令，可以将高权限的用户定义密码，但是，shell用户的默认实现只支持固定的密码，在编译时就已经确定，安全性不高

使用`shell_secure_user`组件，我们可以定义一个函数，使用这个函数得到用户的密码，一般的，我们可以根据芯片的ID，mac地址，甚至可以向服务器做请求以获取密码，提高安全性

- 定义获取用户密码函数(handler)

    示例函数直接返回用户名作为密码，实际使用时可以通过任何方式计算或者获取密码，注意，此处返回的密码字符串，请自行分配内存保存

    ```c
    char *shellSecureUserHandlerTest(const char *name)
    {
        return (char *)name;
    }
    ```

- 定义用户

    ```c
    SHELL_EXPORT_SECURE_USER(SHELL_CMD_PERMISSION(0xFF), secure, shellSecureUserHandlerTest, secure user test);
    ```

- 调用

    使用`shell_secure_user`定义的用户和shell默认用户调用方法完全一致，只需要在shell命令行输入用户名和密码即可
