# letter shell 3.0

![version](https://img.shields.io/badge/version-3.0.5-brightgreen.svg)
![standard](https://img.shields.io/badge/standard-c99-brightgreen.svg)
![build](https://img.shields.io/badge/build-2020.08.23-brightgreen.svg)
![license](https://img.shields.io/badge/license-MIT-brightgreen.svg)

一个功能强大的嵌入式shell

![shell_info.png](doc/img/shell_info.png)

- [letter shell 3.0](#letter-shell-30)
  - [简介](#简介)
  - [功能](#功能)
  - [移植说明](#移植说明)
  - [使用方式](#使用方式)
    - [函数定义](#函数定义)
      - [main函数形式](#main函数形式)
      - [普通C函数形式](#普通c函数形式)
    - [变量使用](#变量使用)
    - [在函数中获取当前shell对象](#在函数中获取当前shell对象)
    - [执行未导出函数](#执行未导出函数)
  - [命令定义](#命令定义)
    - [定义方式](#定义方式)
    - [定义宏说明](#定义宏说明)
    - [命令属性字段说明](#命令属性字段说明)
  - [代理函数和代理参数解析](#代理函数和代理参数解析)
  - [权限系统说明](#权限系统说明)
  - [伴生对象](#伴生对象)
  - [尾行模式](#尾行模式)
  - [建议终端软件](#建议终端软件)
  - [命令遍历工具](#命令遍历工具)

## 简介

[letter shell 3.0](https://github.com/NevermindZZT/letter-shell/tree/shell3.0)是一个C语言编写的，可以嵌入在程序中的嵌入式shell，主要面向嵌入式设备，以C语言函数为运行单位，可以通过命令行调用，运行程序中的函数

相对2.x版本，letter shell 3.0增加了用户管理，权限管理，后续会增加对文件系统的支持

此外3.0版本修改了命令格式和定义，2.x版本的工程需要经过简单的修改才能完成迁移

若只需要使用基础功能，可以使用[letter shell 2.x](https://github.com/NevermindZZT/letter-shell/tree/shell2.x)版本

使用说明可参考[Letter shell 3.0 全新出发](https://nevermindzzt.github.io/2020/01/19/Letter%20shell%203.0%E5%85%A8%E6%96%B0%E5%87%BA%E5%8F%91/)

## 功能

- 命令自动补全
- 快捷键功能定义
- 命令权限管理
- 用户管理
- 变量支持
- 代理函数和参数代理解析

## 移植说明

1. 定义shell对象

    ```C
    Shell shell;
    ```

2. 定义shell读，写函数，函数原型如下

    ```C
    /**
     * @brief shell读取数据函数原型
     *
     * @param char shell读取的字符
     *
     * @return char 0 读取数据成功
     * @return char -1 读取数据失败
     */
    typedef signed char (*shellRead)(char *);

    /**
     * @brief shell写数据函数原型
     *
     * @param const char 需写的字符
     */
    typedef void (*shellWrite)(const char);
    ```

3. 申请一片缓冲区

    ```C
    char shellBuffer[512];
    ```

4. 调用shellInit进行初始化

    ```C
    shell.read = shellRead;
    shell.write = shellWrite;
    shellInit(&shell, shellBuffer, 512);
    ```

5. 调用(建立)shell任务

    对于运行在操作系统的情况，建立`shellTask`任务(确保sell_cfg.h中的配置无误)，任务参数为shell对象

    ```C
    OsTaskCreate(shellTask, &shell, ...);
    ```

    对于裸机环境，在主循环中调用`shellTask`，或者在接收到数据时，调用`shellHandler`

6. 说明

   - 对于中断方式使用shell，不用定义`shell->read`，但需要在中断中调用`shellHandler`
   - 对于使用操作系统的情况，使能```SHEHLL_TASK_WHILE```宏，然后创建shellTask任务

7. 其他配置

   - 定义宏`SHELL_GET_TICK()`为获取系统tick函数，使能tab双击操作，用户长帮助补全

8. 配置宏

    shell_cfg.h文件中包含了所有用于配置shell的宏，在使用前，需要根据需要进行配置

    | 宏                          | 意义                           |
    | --------------------------- | ------------------------------ |
    | SHELL_TASK_WHILE            | 是否使用默认shell任务while循环 |
    | SHELL_USING_CMD_EXPORT      | 是否使用命令导出方式           |
    | SHELL_USING_COMPANION       | 是否使用shell伴生对象功能      |
    | SHELL_SUPPORT_END_LINE      | 是否支持shell尾行模式          |
    | SHELL_HELP_LIST_USER        | 是否在输入命令列表中列出用户   |
    | SHELL_HELP_LIST_VAR         | 是否在输入命令列表中列出变量   |
    | SHELL_HELP_LIST_KEY         | 是否在输入命令列表中列出按键   |
    | SHELL_ENTER_LF              | 使用LF作为命令行回车触发       |
    | SHELL_ENTER_CR              | 使用CR作为命令行回车触发       |
    | SHELL_ENTER_CRLF            | 使用CRLF作为命令行回车触发     |
    | SHELL_EXEC_UNDEF_FUNC       | 使用执行未导出函数的功能       |
    | SHELL_COMMAND_MAX_LENGTH    | shell命令最大长度              |
    | SHELL_PARAMETER_MAX_NUMBER  | shell命令参数最大数量          |
    | SHELL_HISTORY_MAX_NUMBER    | 历史命令记录数量               |
    | SHELL_DOUBLE_CLICK_TIME     | 双击间隔(ms)                   |
    | SHELL_MAX_NUMBER            | 管理的最大shell数量            |
    | SHELL_GET_TICK()            | 获取系统时间(ms)               |
    | SHELL_MALLOC(size)          | 内存分配函数(shell本身不需要)  |
    | SHELL_FREE(obj)             | 内存释放函数(shell本身不需要)  |
    | SHELL_SHOW_INFO             | 是否显示shell信息              |
    | SHELL_CLS_WHEN_LOGIN        | 是否在登录后清除命令行         |
    | SHELL_DEFAULT_USER          | shell默认用户                  |
    | SHELL_DEFAULT_USER_PASSWORD | 默认用户密码                   |
    | SHELL_LOCK_TIMEOUT          | shell自动锁定超时              |

## 使用方式

### 函数定义

letter shell 3.0同时支持两种形式的函数定义方式，形如main函数定义的`func(int argc, char *agrv[])`以及形如普通C函数的定义`func(int i, char *str, ...)`，两种函数定义方式适用于不同的场景

#### main函数形式

使用此方式，一个函数定义的例子如下：

```C
int func(int argc, char *agrv[])
{
    printf("%dparameter(s)\r\n", argc);
    for (char i = 1; i < argc; i++)
    {
        printf("%s\r\n", argv[i]);
    }
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN), func, func, test);
```

终端调用

```sh
letter:/$ func "hello world"
2 parameter(s)
hello world
```

#### 普通C函数形式

使用此方式，shell会自动对参数进行转化处理，目前支持二进制，八进制，十进制，十六进制整形，字符，字符串的自动处理，如果需要其他类型的参数，请使用代理参数解析的方式(参考[代理函数和代理参数解析](#代理函数和代理参数解析))，或者使用字符串的方式作为参数，自行进行处理，例子如下：

```C
int func(int i, char ch, char *str)
{
    printf("input int: %d, char: %c, string: %s\r\n", i, ch, str);
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), func, func, test);
```

终端调用

```sh
letter:/$ func 666 'A' "hello world"
input int: 666, char: A, string: hello world
```

### 变量使用

letter shell 3.0支持导出变量，通过命令行查看，设置以及使用变量的值

- 导出变量

    变量导出使用`SHELL_EXPORT_VAR`宏，支持整形(char, short, int)，字符串，指针以及节点变量，变量导出需要使用引用的方式，如果不允许对变量进行修改，在属性中添加`SHELL_CMD_READ_ONLY`

    ```C
    int varInt = 0;
    SHELL_EXPORT_VAR(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_VAR_INT), varInt, &varInt, test);

    char str[] = "test string";
    SHELL_EXPORT_VAR(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_VAR_STRING), varStr, str, test);

    Log log;
    SHELL_EXPORT_VAR(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_VAR_POINT), log, &log, test);
    ```

- 查看变量

    在命令行直接输入导出的变量名即可查看变量当前的值

    ```sh
    letter:/$ varInt
    varInt = 0, 0x00000000

    letter:/$ varStr
    varStr = "test string"
    ```

- 修改变量

    使用`setVar`命令修改变量的值，对于字符串型变量，请确认字符串有分配足够的空间，指针类型的变量不可修改

    ```sh
    letter:/$ setVar varInt 45678
    varInt = 45678, 0x0000b26e

    letter:/$ setVar varStr "hello"
    varStr = "hello"
    ```

- 使用变量

    letter shell 3.0的变量可以在命令中作为参数传递，对于需要传递结构体引用到命令中的场景特别适用，使用`$`+变量名的方式传递

    ```sh
    letter:/$ shellPrint $shell "hello world\r\n"
    hello world
    ```

### 在函数中获取当前shell对象

letter shell采取一个静态数组对定义的多个shell进行管理，shell数量可以修改宏`SHELL_MAX_NUMBER`定义(为了不使用动态内存分配，此处通过数据进行管理)，从而，在shell执行的函数中，可以调用`shellGetCurrent()`获得当前活动的shell对象，从而可以实现某一个函数在不同的shell对象中发生不同的行为，也可以通过这种方式获得shell对象后，调用`shellWriteString(shell, string)`进行shell的输出

### 执行未导出函数

letter shell支持通过函数地址直接执行函数，可以方便执行那些没有导出，但是有临时需要使用的函数，使用命令`exec [addr] [args]`执行，使用此功能需要开启`SHELL_EXEC_UNDEF_FUNC`宏，注意，由于直接操作函数地址执行，如果给进的地址有误，可能引起程序崩溃

函数的地址可以通过编译生成的文件查找，比如说对于keil，可以在`.map`文件中查找到每个函数的地址，对于keil，`.map`文件中的地址需要偏移一个字节，才可以成功执行，比如说`shellClear`函数地址为`0x08028620`，则通过`exec`执行应为`exec 0x08028621`

其他编译器查找函数地址的方式和地址偏移的处理，请参考各编译器手册

## 命令定义

letter shell 3.0将可执行的函数命令定义，用户定义，按键定义以及变量定义统一归为命令定义，使用相同的结构储存，查找和执行

### 定义方式

letter shell 支持使用命令导出方式和命令表方式进行命令的添加，定义，通过宏```SHELL_USING_CMD_EXPORT```控制

命令导出方式支持keil，IAR(未测试)以及GCC

1. 命令导出方式

    letter shell 支持在函数体外部，采用定义常量的方式定义命令，例如`SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE (SHELL_TYPE_CMD_MAIN)|SHELL_CMD_DISABLE_RETURN,help, shellHelp, show command info\r\nhelp [cmd]);`

    对于使用keil进行编译，需要在keil的target option中增加--keep shellCommand*，防止定义的命令被优化掉

    使用GCC编译时，需要在ld文件中的只读数据区(建议)添加：

    ```ld
    _shell_command_start = .;
    KEEP (*(shellCommand))
    _shell_command_end = .;
    ```

2. 命令表方式

    - 当使用其他暂时不支持使用命令导出方式的编译器时，需要在`shell_cmd_list.c`文件的命令表中添加

        ```C
        const SHELL_CommandTypeDef shellDefaultCommandList[] =
        {
            SHELL_CMD_ITEM(
                       SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN)|SHELL_CMD_DISABLE_RETURN,
                       help, shellHelp, show command info\r\nhelp [cmd]),
        };
        ```

### 定义宏说明

letter shell 3.0对可执行命令，按键，用户以及变量分别提供了一个宏，用于进行命令定义

1. 可执行命令定义

    使用宏`SHELL_EXPORT_CMD`定义可执行命令，定义如下

    ```C
    /**
     * @brief shell 命令定义
     *
     * @param _attr 命令属性
     * @param _name 命令名
     * @param _func 命令函数
     * @param _desc 命令描述
     */
    #define SHELL_EXPORT_CMD(_attr, _name, _func, _desc) \
            const char shellCmd##_name[] = #_name; \
            const char shellDesc##_name[] = #_desc; \
            const ShellCommand \
            shellCommand##_name SECTION("shellCommand") =  \
            { \
                .attr.value = _attr, \
                .data.cmd.name = shellCmd##_name, \
                .data.cmd.function = (int (*)())_func, \
                .data.cmd.desc = shellDesc##_name \
            }
    ```

2. 变量定义

    使用宏`SHELL_EXPORT_VAR`定义变量，定义如下

    ```C
    /**
     * @brief shell 变量定义
     *
     * @param _attr 变量属性
     * @param _name 变量名
     * @param _value 变量值
     * @param _desc 变量描述
     */
    #define SHELL_EXPORT_VAR(_attr, _name, _value, _desc) \
            const char shellCmd##_name[] = #_name; \
            const char shellDesc##_name[] = #_desc; \
            const ShellCommand \
            shellVar##_name SECTION("shellCommand") =  \
            { \
                .attr.value = _attr, \
                .data.var.name = shellCmd##_name, \
                .data.var.value = (void *)_value, \
                .data.var.desc = shellDesc##_name \
            }
    ```

    变量定义时，`_value`应该是变量的引用，如果变量不允许修改，则需要在增加`SHELL_CMD_READ_ONLY`属性

3. 用户定义

    使用宏`SHELL_EXPORT_USER`定义用户，定义如下

    ```C
    /**
     * @brief shell 用户定义
     *
     * @param _attr 用户属性
     * @param _name 用户名
     * @param _password 用户密码
     * @param _desc 用户描述
     */
    #define SHELL_EXPORT_USER(_attr, _name, _password, _desc) \
            const char shellCmd##_name[] = #_name; \
            const char shellPassword##_name[] = #_password; \
            const char shellDesc##_name[] = #_desc; \
            const ShellCommand \
            shellUser##_name SECTION("shellCommand") =  \
            { \
                .attr.value = _attr|SHELL_CMD_TYPE(SHELL_TYPE_USER), \
                .data.user.name = shellCmd##_name, \
                .data.user.password = shellPassword##_name, \
                .data.user.desc = shellDesc##_name \
            }
    ```

4. 按键定义

    使用宏`SHELL_EXPORT_KEY`定义按键，定义如下

    ```C
    /**
     * @brief shell 按键定义
     *
     * @param _attr 按键属性
     * @param _value 按键键值
     * @param _func 按键函数
     * @param _desc 按键描述
     */
    #define SHELL_EXPORT_KEY(_attr, _value, _func, _desc) \
            const char shellDesc##_value[] = #_desc; \
            const ShellCommand \
            shellKey##_value SECTION("shellCommand") =  \
            { \
                .attr.value = _attr|SHELL_CMD_TYPE(SHELL_TYPE_KEY), \
                .data.key.value = _value, \
                .data.key.function = (void (*)(Shell *))_func, \
                .data.key.desc = shellDesc##_value \
            }
    ```

    按键键值为在终端输入按键会发送的字符串序列，以大端模式表示，比如在SecureCRT中断，按下Tab键，会发送0x0B，则这个按键的键值为0x0B000000，如果按下方向上，会依次发送0x1B, 0x5B, 0x41, 则这个键的键值为0x1B5B4100

### 命令属性字段说明

在命令定义中，有一个`attr`字段，表示该命令的属性，具体定义为

```C
union
{
    struct
    {
        unsigned char permission : 8;                       /**< command权限 */
        ShellCommandType type : 4;                          /**< command类型 */
        unsigned char enableUnchecked : 1;                  /**< 在未校验密码的情况下可用 */
        unsigned char  readOnly : 1;                        /**< 只读 */
        unsigned char reserve : 1;                          /**< 保留 */
        unsigned char paramNum : 4;                         /**< 参数数量 */
    } attrs;
    int value;
} attr;
```

在定义命令时，需要给定这些值，可以通过宏`SHELL_CMD_PERMISSION(permission)`, `SHELL_CMD_TYPE(type)`, `SHELL_CMD_ENABLE_UNCHECKED`, `SHELL_CMD_DISABLE_RETURN`, `SHELL_CMD_READ_ONLY`, `SHELL_CMD_PARAM_NUM(num)`快速声明

## 代理函数和代理参数解析

letter shell 3.0原生支持将整数，字符，字符串参数，以及在某些情况下的浮点参数直接传递给执行命令的函数，一般情况下，这几种参数类型完全可以满足调试需要，然而在某些情况下，用户确实需要传递其他类型的参数，此时，可以选择将命令定义成main函数形式，使用字符串传递参数，然后自行对参数进行解析，除此之外，letter shell还提供了代理函数的机制，可以对任意类型的参数进行自定义解析

关于代理函数的实现原理和具体使用示例，可以参考[letter-shell代理函数解析](https://nevermindzzt.github.io/2020/04/17/letter-shell%E4%BB%A3%E7%90%86%E5%87%BD%E6%95%B0%E8%A7%A3%E6%9E%90/)

使用代理函数，用户需要自定义代理参数解析器，即一个将基本参数(整数，字符，字符串参数)转换成目标类型参数的函数或者宏，letter shell默认实现了浮点类型的参数解析器`SHELL_PARAM_FLOAT(x)`

然后，使用代理函数命令导出宏定义命令，比如需要需要传递多个浮点参数的函数，如下

```C
void test(int a, float b, int c, float d)
{
    printf("%d, %f, %d, %f \r\n", a, b, c, d);
}
SHELL_EXPORT_CMD_AGENCY(SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),
test, test, test,
p1, SHELL_PARAM_FLOAT(p2), p3, SHELL_PARAM_FLOAT(p4));
```

相比常规的命令导出，代理函数命令导出前4个参数和常规形式的命令导出一致，之后的参数即传递至目标函数的参数，letter shell默认实现的代理函数定义支持最多7个参数，p1~p7，对于不需要代理参数解析的参数，只需要对应写入`px(x为1~7)`即可，比如上方示例的`p1`和`p3`，而需要代理参数解析的参数，则需要使用对应的参数解析器，比如上方示例的`p2`和`p4`

## 权限系统说明

letter shell 3.0的权限管理同用户定义紧密相关，letter shell 3.0使用8个bit位表示命令权限，当用户和命令的权限按位与为真，或者命令权限为0时，表示该用户拥有此命令的权限，可以调用改命令

## 伴生对象

letter shell 3.0.3版本引入了伴生对象的概念，通过宏`SHELL_USING_COMPANION`开启或者关闭，若使用伴生对象的功能，需要同时将shell_companion.c文件加入到工程中，伴生对象可以用于需要将某个对象同shell关联的场景，比如说，通过快捷键控制shell终端对应的日志打印对象

一般情况下，使用`shellCompanionAdd`将伴生对象同shell对象进行关联，之后，可以在shell操作中，通过`shellCompanionGet`获取相应的伴生对象，以达到在不同的shell中，操作不同对象的目的

## 尾行模式

letter shell 3.0.4版本新增了尾行模式，适用于需要在shell所使用的交互终端同时输入其他信息(比如说日志)时，防止其他信息的输出，导致shell交互体验极差的情况，使用时，使能宏`SHELL_SUPPORT_END_LINE`，然后对于其他需要使用终端输入信息的地方，调用`shellWriteEndLine`接口将信息输入，此时，调用`shellWriteEndLine`进行输入的内容将会插入到命令行上方，终端会一直保持shell命令行位于最后一行

使用letter shell尾行模式结合[log](./extensions/log/readme.md)日志输出的效果如下：

![end lin mode](doc/img/shell_end_line_mode.gif)

## 建议终端软件

- 对于基于串口移植，letter shell建议使用secureCRT软件，letter shell中的相关按键映射都是按照secureCRT进行设计的，使用其他串口软件时，可能需要修改键值

## 命令遍历工具

letter shell 3.0提供了一个用于遍历工程中命令导出的工具，位于tools/shellTools.py，需要python3环境运行，可以列出工程中，所有使用`SHELL_EXPORT_XXX`导出的命令名，以及位置，结合VS Code可以直接进行跳转

```sh
python shellTools.py project
```

注意：shellTools会遍历指定目录中所有文件，所以当工程中文件较多时，速度会比较慢，建议只用于遍历用户模块的目录
