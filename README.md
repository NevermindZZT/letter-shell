# letter shell

![version](https://img.shields.io/badge/version-2.0.8-brightgreen.svg)
![build](https://img.shields.io/badge/build-2020.01.14-brightgreen.svg)
![build](https://img.shields.io/badge/license-MIT-brightgreen.svg)

一个体积极小的嵌入式shell

- [letter shell](#letter-shell)
  - [功能](#%e5%8a%9f%e8%83%bd)
  - [移植说明](#%e7%a7%bb%e6%a4%8d%e8%af%b4%e6%98%8e)
  - [使用方式](#%e4%bd%bf%e7%94%a8%e6%96%b9%e5%bc%8f)
    - [函数定义](#%e5%87%bd%e6%95%b0%e5%ae%9a%e4%b9%89)
      - [main函数形式](#main%e5%87%bd%e6%95%b0%e5%bd%a2%e5%bc%8f)
      - [普通C函数形式](#%e6%99%ae%e9%80%9ac%e5%87%bd%e6%95%b0%e5%bd%a2%e5%bc%8f)
      - [在函数中获取当前shell对象](#%e5%9c%a8%e5%87%bd%e6%95%b0%e4%b8%ad%e8%8e%b7%e5%8f%96%e5%bd%93%e5%89%8dshell%e5%af%b9%e8%b1%a1)
    - [命令定义](#%e5%91%bd%e4%bb%a4%e5%ae%9a%e4%b9%89)
    - [命令导出方式](#%e5%91%bd%e4%bb%a4%e5%af%bc%e5%87%ba%e6%96%b9%e5%bc%8f)
    - [命令表方式](#%e5%91%bd%e4%bb%a4%e8%a1%a8%e6%96%b9%e5%bc%8f)
    - [组合按键](#%e7%bb%84%e5%90%88%e6%8c%89%e9%94%ae)
    - [shell变量](#shell%e5%8f%98%e9%87%8f)
      - [导出变量](#%e5%af%bc%e5%87%ba%e5%8f%98%e9%87%8f)
      - [读取变量](#%e8%af%bb%e5%8f%96%e5%8f%98%e9%87%8f)
      - [修改变量](#%e4%bf%ae%e6%94%b9%e5%8f%98%e9%87%8f)
      - [变量作为命令参数](#%e5%8f%98%e9%87%8f%e4%bd%9c%e4%b8%ba%e5%91%bd%e4%bb%a4%e5%8f%82%e6%95%b0)
    - [shell密码](#shell%e5%af%86%e7%a0%81)
    - [建议终端软件](#%e5%bb%ba%e8%ae%ae%e7%bb%88%e7%ab%af%e8%bd%af%e4%bb%b6)
  - [更新日志](#%e6%9b%b4%e6%96%b0%e6%97%a5%e5%bf%97)

## 功能

- 命令自动补全，使用tab键补全命令
- 命令长帮助，使用help [command]显示命令长帮助
- 长帮助补全，输入命令后双击tab键补全命令长帮助指令
- 快捷键，支持使用Ctrl + A~Z组合按键直接调用函数
- shell变量，支持在shell中查看和修改变量值，支持变量作为命令参数
- 登录密码，支持在shell中使用登录密码，支持超时自动锁定

## 移植说明

1. 定义shell对象

    ```C
    SHELL_TypeDef shell;
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

3. 调用shellInit进行初始化

    ```C
    shell.read = shellRead;
    shell.write = shellWrite;
    shellInit(&shell);
    ```

4. 调用(建立)shell任务

    对于运行在操作系统的情况，建立`shellTask`任务(确保sell_cfg.h中的配置无误)，任务参数为shell对象

    ```C
    OsTaskCreate(shellTask, &shell, ...);
    ```

    对于裸机环境，在主循环中调用`shellTask`，或者在接收到数据时，调用`shellInput`

5. 说明

   - 对于中断方式使用shell，不用定义`shell->read`，但需要在中断中调用`shellInput`
   - 对于在无操作系统环境下，可以使用查询的方式，使能```SHELL_UISNG_TASK```，然后在循环中不断调用shellTask
   - 对于使用操作系统的情况，使能```SHELL_USING_TASK```和```SHEHLL_TASK_WHILE```宏，然后创建shellTask任务
   - 打印函数返回值，使能```SHELL_DISPLAY_RETURN```宏，返回值均作为整型数据打印

6. 其他配置

   - 定义宏```SHELL_GET_TICK()```为获取系统tick函数，使能tab双击操作，用户长帮助补全

7. 配置宏

    shell_cfg.h文件中包含了所有用于配置shell的宏，在使用前，需要根据需要进行配置

    | 宏                         | 意义                           |
    | -------------------------- | ------------------------------ |
    | SHELL_USING_TASK           | 是否使用默认shell任务          |
    | SHELL_USING_CMD_EXPORT     | 是否使用命令导出方式           |
    | SHELL_DISPLAY_RETURN       | 是否显示命令调用函数返回值     |
    | SHELL_TASK_WHILE           | 是否使用默认shell任务while循环 |
    | SHELL_AUTO_PRASE           | 是否使用shell参数自动解析      |
    | SHELL_LONG_HELP            | 是否使用shell长帮助            |
    | SHELL_COMMAND_MAX_LENGTH   | shell命令最大长度              |
    | SHELL_PARAMETER_MAX_NUMBER | shell命令参数最大数量          |
    | SHELL_HISTORY_MAX_NUMBER   | 历史命令记录数量               |
    | SHELL_DOUBLE_CLICK_TIME    | 双击间隔(ms)                   |
    | SHELL_GET_TICK()           | 获取系统时间(ms)               |
    | SHELL_DEFAULT_COMMAND      | shell默认提示符                |
    | SHELL_MAX_NUMBER           | 管理的最大shell数量            |
    | SHELL_USING_AUTH           | 是否使用密码功能               |
    | SHELL_USER_PASSWORD        | 用户密码                       |
    | SHELL_LOCK_TIMEOUT         | shell自动锁定超时              |

## 使用方式

### 函数定义

letter shell 支持两种形式的函数定义方式，形如main函数定义的```func(int argc, char *agrv[])```以及形如普通C函数的定义```func(int i, char *str, ...)```,这两种方式目前不可共存，只能选择其中的一种，通过宏```SHELL_AUTO_PRASE```选择

#### main函数形式

使用此方式，一个函数定义的例子如下：

```C
func(int argc, char *agrv[])
{
    printf("%dparameter(s)\r\n", argc);
    for (char i = 1; i < argc; i++)
    {
        printf("%s\r\n", argv[i]);
    }
}
SHELL_EXPORT_CMD(func, func, test)
```

终端调用

```sh
letter>>func "hello world"
2 parameter(s)
hello world
```

#### 普通C函数形式

使用此方式，shell会自动对参数进行转化处理，目前支持二进制，八进制，十进制，十六进制整形，字符，字符串的自动处理，如果需要其他类型的参数，请使用字符串的方式作为参数，自行进行处理，例子如下：

```C
func(int i, char ch, char *str)
{
    printf("input int: %d, char: %c, string: %s\r\n", i, ch, str);
}
SHELL_EXPORT_CMD(func, func, test)
```

终端调用

```sh
letter>>func 666 'A' "hello world"
input int: 666, char: A, string: hello world
```

#### 在函数中获取当前shell对象

shell采取一个静态数组对定义的多个shell进行管理，shell数量可以修改宏```SHELL_MAX_NUMBER```定义(为了不使用动态内存分配，此处通过数据进行管理)，从而，在shell执行的函数中，可以调用```shellGetCurrent()```获得当前活动的shell对象，从而可以实现某一个函数在不同的shell对象中发生不同的行为，也可以通过这种方式获得shell对象后，调用```shellDisplay(shell, string)```进行shell的输出

### 命令定义

letter shell 支持使用命令导出方式和命令表方式进行命令的添加，定义，通过宏```SHELL_USING_CMD_EXPORT```控制

命令导出方式支持keil，IAR(未测试)以及GCC

### 命令导出方式

letter shell 支持在函数体外部，采用定义常量的方式定义命令，例如```SHELL_EXPORT_CMD_EX(help, shellHelp, command help, help [command] --show help info of command);```，或者```SHELL_EXPORT_CMD(help, shellHelp, command help);```

对于使用keil进行编译，需要在keil的target option中增加--keep shellCommand*，防止定义的命令被优化掉

使用GCC编译时，需要在ld文件中的只读数据区(建议)添加：

```ld
_shell_command_start = .;
KEEP (*(shellCommand))
_shell_command_end = .;
```

### 命令表方式

- 当使用其他编译器时，暂时不支持使用类似keil中命令导出的方式，需要在命令表中添加

    ```C
    const SHELL_CommandTypeDef shellDefaultCommandList[] =
    {
        SHELL_CMD_ITEM_EX(help, shellHelp, command help, help [command] --show  help info of command),
    };
    ```

- 其中，带有EX的命令导出宏最后一个参数为命令的长帮助信息，在shell中使用help [command]可查看帮助信息，通过shell.h中的SHELL_LONG_HELP宏可设置是否使用此功能

### 组合按键

letter shell支持使用Ctrl键加任意字母键的组合按键一键执行操作，使用时，在任意文件定义按键命令表

```C
SHELL_KeyFunctionDef keyFuncList[] =
{
    {SHELL_KEY_CTRL_T,      switchUlog}
};
```

然后使用`shellSetKeyFuncList`进行注册

```C
    shellSetKeyFuncList(&shell, keyFuncList, sizeof(keyFuncList) / sizeof(SHELL_KeyFunctionDef));
```

### shell变量

letter shell支持shell变量，通过导出变量，将变量进行注册，可以在shell中读取，修改变量的值，可以将变量作为参数传递给shell命令

使用时，在shell_cfg.h文件中将`SHELL_USING_VAR`修改为1

执行`vars`命令查看所有变量

#### 导出变量

使用变量导出方式时，通过宏`SHELL_EXPORT_VAR_INT`,`SHELL_EXPORT_VAR_SHORT`,`SHELL_EXPORT_VAR_CHAR`,`SHELL_EXPORT_VAR_POINTER`导出变量，例如：

```C
SHELL_EXPORT_VAR_INT(testVar1, testVar1, var for test);
```

使用变量表方式时，定义一个命令表，并调用`shellSetVariableList`进行注册，参考命令导出

#### 读取变量

shell变量使用`$[var]`的方式读取，直接在命令行输入即可，例如：

```sh
letter>>$testVar1
testVar1 = 100, 0x00000064
```

#### 修改变量

使用`setVar`命令修改变量，例如：

```sh
letter>>setVar testVar1 200
testVar1 = 200, 0x000000c8
```

#### 变量作为命令参数

直接使用`$[var]`即可将变量的值作为参数传递给shell命令，例如：

```sh
letter>>getVar $testVar1
```

### shell密码

letter shell支持shell密码，支持在一定时间shell无操作时自动锁定

使能宏`SHELL_USING_AUTH`开启shell密码功能，同时修改宏`SHELL_USER_PASSWORD`定义shell密码

在使用密码，并且宏`SHELL_GET_TICK()`有有效定义后，可以使用shell超时自动锁定，通过宏`SHELL_LOCK_TIMEOUT`设置超时时长，当此宏设为0时，禁用超时自动锁定功能

### 建议终端软件

- 对于基于串口移植，letter shell建议使用secureCRT软件，letter shell中的相关按键映射都是按照secureCRT进行设计的，使用其他串口软件可能会出现某些功能无法使用的情况

## 更新日志

- 2018/4/20   v1.0

  - 第一版

- 2018/4/23   v1.1

  - 加入对带参命令的支持

- 2018/4/25   v1.2

  - 合并带参函数和不带参函数命令表

- 2018/7/10   v1.3

  - 修复带参命令最后输入空格时传入参数数量出错的问题
  - 修复不带参数调用带参命令时程序跑死的问题，指令处理相关代码优化
  - 加入对于输入超出命令长度的情况的处理措施

- 2018/7/11   v1.4

  - 新增对于方向键的处理，暂时未对方向键添加具体功能
  - 修复单独的空格，制表符等无效操作被识别为指令进行解析，造成程序死机的问题
  - 取消制表符作为参数分隔符的用途

- 2018/7/12   v1.5

  - 新增历史命令的功能，使用上下方向键操作
  - 新增tab键输入最近一条命令
  - 无历史记录时，tab键输入help命令
  - 新增一条用于清屏的默认指令

- 2018/7/18   v1.6

  - 修改tab键功能，加入自动补全
  - 无输入情况下，按下tab输入help命令
  - 有输入情况下，进行自动补全

- 2018/7/24 v1.7

  - 增加SHELL_TypeDef结构体
  - 采用新的命令添加方式，现在可以在任意文件的函数 外部采用宏SHELL_EXPORT_CMD进行命令定义

- 2018/7/26 v1.7.1

  - 修复不使用带参函数(SHELL_USE_PARAMETER = 0)的情况下，无法匹配命令的问题
  - 修复不使用历史命令(SHELL_USE_HISTORY = 0)的情况下，无法使用命令补全的问题

- 2018/8/9  v1.8

  - 新增左右键移动光标功能，方便对输错的命令进行修改使用宏SHELL_ALLOW_SHIFT开启
  - 优化结构体成员命名
  - 对开启移动光标功能后，输入参数过长的情况采用新的处理方式

- 2018/11/19 v1.8.3

  - 修复不使用光标移动功能的时候，输入命令过长时无法正常删除的问题
  - 针对不使用MDK编译，重新加入命令表定义的方式
  - 新增对双引号的识别处理，支持带空格的参数

- 2019/01/07 2.0.0

  - 重构代码，优化逻辑结构，减少内存开销
  - 新增shell扩展模块，支持函数参数自动转化
  - 精简shell可选项
  - 新增多shell支持

- 2019/02/20 2.0.1

  - 新增命令导出方式对于IAR，GCC的支持
  - 新增命令长帮助
  - 新增二进制参数支持
  - 修复bug以及一些优化

- 2019/03/29 2.0.2

  - 新增函数返回值打印
  - 新增shell对象管理，支持在shell调用的函数中获取当前shell对象
  - bug修复

- 2019/08/16 2.0.3

  - 新增一个shell格式化输出函数

- 2019/08/20 2.0.4

  - 新增组合按键功能自定义
  - 新增shell变量

- 2019/08/21 2.0.5

  - 新增shell常量
  - 修复bug以及一些优化

- 2019/11/16 2.0.6

  - 修复历史命令概率性异常的问题
  - 新增shell密码
  - 一些细节优化

- 2019/11/30 2.0.7

  - 新增shell超时自动锁定
  - 修复未验证密码的情况下，仍能查看历史命令，使用tab查看命令表的问题
  - 修复双击tab补全，可能在使用多shell的时候有冲突的问题
  - 细节优化

- 2020/01/14 2.0.8

  - 修复命令名过长时，显示命令列表异常的问题
  - 修复命令被限制在256个的问题
