# letter shell

![version](https://img.shields.io/badge/version-2.0.0-brightgreen.svg)
![build](https://img.shields.io/badge/build-2019.2.12-brightgreen.svg)

一个体积极小的嵌入式shell

## 功能

- 命令自动补全，使用tab键补全命令
- 命令长帮助，使用help [command]显示命令长帮助
- 长帮助补全，输入命令后双击tab键补全命令长帮助指令

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
shell->read = shellRead;
shell->write = shellWrite;
shellInit(&shell);
```

4. 说明

- 对于中断方式使用shell，不用定义shell->read，但需要在中断中调用shellHandler
- 对于在无操作系统环境下，可以使用查询的方式，使能```SHELL_UISNG_TASK```，然后在循环中不断调用shellTask
- 对于使用操作系统的情况，使能```SHELL_USING_TASK```和```SHEHLL_TASK_WHILE```宏，然后创建shellTask任务

5. 其他配置

- 定义宏```SHELL_GET_TICK()```为获取系统tick函数，使能tab双击操作，用户长帮助补全

## 使用方式

### 命令定义

- letter shell 支持在使用keil编译器的时候，在函数体外部，采用定义常量的方式定义命令，例如```SHELL_EXPORT_CMD_EX(help, shellHelp, command help, help [command] --show help info of command);```，或者```SHELL_EXPORT_CMD(help, shellHelp, command help);```，此时，需要在keil的target option中增加--keep shellCommand*，防止定义的命令被优化掉

- 当使用其他编译器时，暂时不支持使用类似keil中命令导出的方式，需要在命令表中添加

```C
const SHELL_CommandTypeDef shellDefaultCommandList[] =
{
    SHELL_CMD_ITEM_EX(help, shellHelp, command help, help [command] --show help info of command),
};
```

- 其中，带有EX的命令导出宏最后一个参数为命令的长帮助信息，在shell中使用help [command]可查看帮助信息，通过shell.h中的SHELL_LONG_HELP宏可设置是否使用此功能

### 建议终端软件

- 对于基于串口移植，letter shell建议使用secureCRT软件，letter shell中的相关按键映射都是按照secureCRT进行设计的，使用其他串口软件可能会出现某些功能无法使用的情况


## 更新日志

### 2018/4/20   v1.0

- 第一版

### 2018/4/23   v1.1

- 加入对带参命令的支持

### 2018/4/25   v1.2

- 合并带参函数和不带参函数命令表

### 2018/7/10   v1.3

- 修复带参命令最后输入空格时传入参数数量出错的问题
- 修复不带参数调用带参命令时程序跑死的问题，指令处理相关代码优化
- 加入对于输入超出命令长度的情况的处理措施

### 2018/7/11   v1.4

- 新增对于方向键的处理，暂时未对方向键添加具体功能
- 修复单独的空格，制表符等无效操作被识别为指令进行解析，造成程序死机的问题
- 取消制表符作为参数分隔符的用途

### 2018/7/12   v1.5

- 新增历史命令的功能，使用上下方向键操作
- 新增tab键输入最近一条命令
- 无历史记录时，tab键输入help命令
- 新增一条用于清屏的默认指令

### 2018/7/18   v1.6

- 修改tab键功能，加入自动补全
- 无输入情况下，按下tab输入help命令
- 有输入情况下，进行自动补全

### 2018/7/24 v1.7

- 增加SHELL_TypeDef结构体
- 采用新的命令添加方式，现在可以在任意文件的函数 外部采用宏SHELL_EXPORT_CMD进行命令定义

### 2018/7/26 v1.7.1

- 修复不使用带参函数(SHELL_USE_PARAMETER = 0)的情况下，无法匹配命令的问题
- 修复不使用历史命令(SHELL_USE_HISTORY = 0)的情况下，无法使用命令补全的问题

### 2018/8/9  v1.8

- 新增左右键移动光标功能，方便对输错的命令进行修改使用宏SHELL_ALLOW_SHIFT开启
- 优化结构体成员命名
- 对开启移动光标功能后，输入参数过长的情况采用新的处理方式

### 2018/11/19 v1.8.3

- 修复不使用光标移动功能的时候，输入命令过长时无法正常删除的问题
- 针对不使用MDK编译，重新加入命令表定义的方式
- 新增对双引号的识别处理，支持带空格的参数

### 2019/01/07 2.0.0

- 重构代码，优化逻辑结构，减少内存开销
- 新增shell扩展模块，支持函数参数自动转化
- 精简shell可选项
- 新增多shell支持