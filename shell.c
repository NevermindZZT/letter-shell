/*- Coding With UTF-8 -*/


/*******************************************************************************
*   File Name：     shell.c
*   Description：   此文件提供了shell实现的相关逻辑
*   Atuhor：        Letter
*   Date:           2018/4/20
*   Version:        2018/4/20   v1.0
*                               第一版
*                   2018/4/23   v1.1
*                               加入对带参命令的支持
*                   2018/4/25   v1.2
*                               合并带参函数和不带参函数命令表
*                   2018/7/10   v1.3
*                               修复带参命令最后输入空格时传入参数数量出错的问题
*                               修复不带参数调用带参命令时程序跑死的问题，指令处
*                               理相关代码优化
*                               加入对于输入超出命令长度的情况的处理措施
*                   2018/7/11   v1.4
*                               新增对于方向键的处理，暂时未对方向键添加具体功能
*                               修复单独的空格，制表符等无效操作被识别为指令进行
*                               解析，造成程序死机的问题
*                               取消制表符作为参数分隔符的用途
*                   2018/7/12   v1.5
*                               新增历史命令的功能，使用上下方向键操作
*                               新增tab键输入最近一条命令
*                               无历史记录时，tab键输入help命令
*                               新增一条用于清屏的默认指令
*                   2018/7/18   v1.6
*                               修改tab键功能，加入自动补全
*                               无输入情况下，按下tab输入help命令
*                               有输入情况下，进行自动补全
*                   2018/7/24   v1.7
*                               增加SHELL_TypeDef结构体
*                               采用新的命令添加方式，现在可以在任意文件的函数
*                               外部采用宏SHELL_EXPORT_CMD进行命令定义
*                   2018/7/26   v1.7.1
*                               修复不使用带参函数(SHELL_USE_PARAMETER = 0)的
*                               情况下，无法匹配命令的问题
*                               修复不使用历史命令(SHELL_USE_HISTORY = 0)的情况
*                               下，无法使用命令补全的问题
*                   2018/8/9    v1.8
*                               新增左右键移动光标功能，方便对输错的命令进行修改
*                               使用宏SHELL_ALLOW_SHIFT开启
*                               优化结构体成员命名
*                               对开启移动光标功能后，输入参数过长的情况采用新的
*                               处理方式
*                   2018/8/15   v1.8.1
*                               修复不使用光标移动功能的时候，输入命令过长时无法
*                               正常删除的问题
*                   2018/10/15  v1.8.2
*                               针对不使用MDK编译，重新加入命令表定义的方式
*                   2018/11/19  v1.8.3
*                               新增对双引号的识别处理，支持带空格的参数
*******************************************************************************/

#include    "shell.h"
#include    "string.h"

static SHELL_TypeDef shell =
{
    .commandLength = 0,
#if SHELL_ALLOW_SHIFT == 1
    .commandCursor = 0,
#endif
#if SHELL_USE_HISTORY == 1
    .historyCount = 0,
    .historyFlag = 0,
    .historyOffset = 0,
#endif
};

#ifndef __CC_ARM
/**
* shell 命令表，使用 {command, function, description} 的格式添加命令
* 其中
* command   为命令，字符串格式，长度不能超过 SHELL_PARAMETER_MAX_LENGTH
*           若不使用带参命令，则长度不超过SHELL_COMMAND_MAX_LENGTH
* function  为该命令调用的函数，支持(void *)(void)类型的无参函数以及与带参主函数
*           类似的(void *)(uint32_t argc, uint8_t *argv[])类型的带参函数，其中，
*           argc 为参数个数，argv 为参数，参数皆为字符串格式，需自行进行数据转换
* description 为对命令的描述，字符串格式
*/
const SHELL_CommandTypeDef shellCommandList[] = 
{
    /*command               function                description*/
    {(uint8_t *)"letter",   shellLetter,            (uint8_t *)"letter shell"},
    {(uint8_t *)"reboot",   shellReboot,            (uint8_t *)"reboot system"},
    {(uint8_t *)"help",     shellShowCommandList,   (uint8_t *)"show command list"},
    {(uint8_t *)"clear",    shellClear,             (uint8_t *)"clear command line"},
    
#if SHELL_USE_PARAMETER == 1    /*带参函数命令*/
   {(uint8_t *)"paraTest", (void (*)())shellParaTest, (uint8_t *)"test parameter"},
#endif
    
};
#endif


/*******************************************************************************
*@function  shellReceiveByte
*@brief     shell接收一个字符
*@param     None
*@retval    None
*@author    Letter
*@note      默认使用串口，可重写此函数自定义shell输入(某些情况下(比如使用串口
*           中断调用shell)可以不实现此函数)
*******************************************************************************/
__weak uint8_t shellReceiveByte(void)
{
    uint8_t recvData;

    HAL_UART_Receive(&shellUart, &recvData, 1, HAL_MAX_DELAY);

    return recvData;
}


/*******************************************************************************
*@function  shellDisplayByte
*@brief     shell显示一个字符
*@param     data    需要显示的字符
*@retval    None
*@author    Letter
*@note      默认使用串口，可重写此函数自定义shell输出
*******************************************************************************/
__weak void shellDisplayByte(uint8_t data)
{
    HAL_UART_Transmit(&shellUart, &data, 1, HAL_MAX_DELAY);
}


/*******************************************************************************
*@function  _ShellDisplay
*@brief     shell显示字符串
*@param     *string    需要显示的字符串指针
*@retval    字符串长度
*@author    Letter
*@note      建议使用宏shellDisplay
*******************************************************************************/
uint16_t _ShellDisplay(uint8_t *string)
{
    uint16_t count = 0;
    while (*string)
    {
        shellDisplayByte(*string++);
        count++;
    }
    return count;
}


/*******************************************************************************
*@function  shellInit
*@brief     shell初始化
*@param     None
*@retval    None
*@author    Letter
*******************************************************************************/
void shellInit(void)
{
    shellDisplay("\r\n\r\n")
    shellDisplay("+=========================================================+\r\n");
    shellDisplay("|                (C) COPYRIGHT 2018 Letter                |\r\n");
    shellDisplay("|                    Letter shell "SHELL_VERSION"                    |\r\n");
    shellDisplay("|               Build: "__DATE__" "__TIME__"               |\r\n");
    shellDisplay("+=========================================================+\r\n");
    shellDisplay(SHELL_COMMAND);

#if SHELL_USE_PARAMETER == 1
    for (uint8_t i = 0; i < SHELL_PARAMETER_MAX_NUMBER; i++)
    {
        shell.commandPointer[i] = shell.commandPara[i];
    }
#endif

#ifdef __CC_ARM
    extern const uint32_t shellCommand$$Base;
    extern const uint32_t shellCommand$$Limit;

    shell.commandBase = (SHELL_CommandTypeDef *)(&shellCommand$$Base);
    shell.commandLimit = (SHELL_CommandTypeDef *)(&shellCommand$$Limit);
#else
    shell.commandBase = (SHELL_CommandTypeDef *)shellCommandList;
    shell.commandLimit = (SHELL_CommandTypeDef *)((uint32_t)shellCommandList + sizeof(shellCommandList));
#endif
}


/*******************************************************************************
*@function  shellMain
*@brief     shell主函数
*@param     None
*@retval    None
*@author    Letter
*@note      此函数为阻塞式，适用于CPU一直处于工作在shell的情况，可由shell调用
*           外部函数跳出
*******************************************************************************/
void shellMain(void)
{
    uint8_t receiveData;

    shellInit();

    while (1)
    {
        receiveData = shellReceiveByte();
        shellHandler(receiveData);
    }
}

/*******************************************************************************
*@function  shellHandler
*@brief     shell处理函数
*@param     receiveData     接收到的数据
*@retval    None
*@author    Letter
*@note      此函数被shellMain函数调用，若使用shellMain阻塞式运行shell，直接调用
*           shellMain函数即可，但不建议这样做，建议在无操作系统情况下，在shell
*           输入触发的中断中调用此函数（通常为串口中断），此时无需调用shellMain，
*           shell也为非阻塞式，操作系统情况下，通常将此函数交给shell输入设备的
*           任务处理
*******************************************************************************/
void shellHandler(uint8_t receiveData)
{
    static uint8_t runFlag;
    static CONTROL_Status controlFlag = CONTROL_FREE;

    switch (receiveData)
    {
    case '\r':
    case '\n':
        if (shell.commandLength >= SHELL_COMMAND_MAX_LENGTH - 1)
        {
            shellDisplay("\r\nError: Command is too long\r\n");
            shell.commandBuff[shell.commandLength] = 0;
            shell.commandLength = 0;
#if SHELL_ALLOW_SHIFT == 1
            shell.commandCursor = 0;
#endif
            shellDisplay(SHELL_COMMAND);
            break;
        }

        if (shell.commandLength == 0)
        {
            shellDisplay(SHELL_COMMAND);
            break;
        }
        else
        {
            shell.commandBuff[shell.commandLength++] = 0;
#if SHELL_USE_PARAMETER == 1
            shell.commandCount = 0;

            uint8_t j = 0;
            uint8_t quotes = 0;                             //引号
            for (int8_t i = 0; i < shell.commandLength; i++)
            {
                if ((quotes != 0 ||
                    (shell.commandBuff[i] != ' ' &&
                    shell.commandBuff[i] != '\t')) &&
                    shell.commandBuff[i] != 0)
                {
                    if (shell.commandBuff[i] == '\"')
                    {
                        quotes = quotes ? 0 : 1;
                    }
                    else
                    {
                        shell.commandPara[shell.commandCount][j++] = shell.commandBuff[i];
                    }
                }
                else
                {
                    if (j != 0)
                    {
                        shell.commandPara[shell.commandCount][j] = 0;
                        shell.commandCount ++;
                        j = 0;
                    }
                }
            }
            shell.commandLength = 0;
#if SHELL_ALLOW_SHIFT == 1
            shell.commandCursor = 0;
#endif

            if (shell.commandCount == 0)                      //是否为无效指令
            {
                shellDisplay(SHELL_COMMAND);
                break;
            }

#if SHELL_USE_HISTORY == 1
            shellStringCopy(shell.historyCommand[shell.historyFlag++],
                            shell.commandBuff);
            if (++shell.historyCount > SHELL_HISTORY_MAX_NUMBER)
            {
                shell.historyCount = SHELL_HISTORY_MAX_NUMBER;
            }
            if (shell.historyFlag >= SHELL_HISTORY_MAX_NUMBER)
            {
                shell.historyFlag = 0;
            }
            shell.historyOffset = 0;
#endif /* SHELL_USE_HISTORY == 1 */

            shellDisplay("\r\n");
            runFlag = 0;

            for (int8_t  i= 0;
                    i < ((uint32_t)shell.commandLimit -
                        (uint32_t)shell.commandBase) / sizeof(SHELL_CommandTypeDef);
                    i++)
            {
                if (strcmp((const char *)shell.commandPara[0],
                           (const char *)(shell.commandBase + i)->name) == 0)
                {
                    runFlag = 1;
                    (shell.commandBase + i)->function(shell.commandCount, shell.commandPointer);
                    break;
                }
            }

#else /*SHELL_USE_PARAMETER == 1*/

            shell.commandBuff[shell.commandLength] = 0;
            shell.commandLength = 0;
            shellDisplay("\r\n");
            runFlag = 0;
            for (int8_t  i= 0;
                    i < ((uint32_t)shell.commandLimit -
                        (uint32_t)shell.commandBase) / sizeof(SHELL_CommandTypeDef);
                    i++)
            {
                if (strcmp((const char *)shell.commandBuff,
                           (const char *)(shell.commandBase + i)->name) == 0)
                {
                    runFlag = 1;
                    (shell.commandBase + i)->function();
                    break;
                }
            }
#endif /*SHELL_USE_PARAMETER == 1*/

            if (runFlag == 0)
            {
                shellDisplay("Command not found");
            }
        }
        shellDisplay(SHELL_COMMAND);
        break;

    case 0x08:                                          //退格
        if (shell.commandLength != 0)
        {  
#if SHELL_ALLOW_SHIFT == 1
            if (shell.commandCursor == shell.commandLength)
            {
                shell.commandLength--;
                shell.commandCursor--;
                shell.commandBuff[shell.commandLength] = 0;
                shellBackspace(1);
            }
            else if (shell.commandCursor > 0)
            {
                for (int8_t i = 0;
                        i < shell.commandLength - shell.commandCursor; i++)
                {
                    shell.commandBuff[shell.commandCursor + i - 1] =
                        shell.commandBuff[shell.commandCursor + i];
                }
                shellClearLine();
                shell.commandBuff[shell.commandLength - 1] = 0;
                shellDisplay(shell.commandBuff);
                shell.commandLength --;
                shell.commandCursor --;
                for (int8_t i = shell.commandLength - shell.commandCursor;
                        i > 0; i--)
                {
                    shellDisplayByte('\b');
                }
            }
#else
            shell.commandLength--;
            shellBackspace(1);
#endif /* SHELL_ALLOW_SHIFT == 1 */
        }
        break;

    case '\t':                                          //制表符
        if (shell.commandLength != 0)
        {
            for (int8_t  i= 0;
                    i < ((uint32_t)shell.commandLimit -
                        (uint32_t)shell.commandBase) / sizeof(SHELL_CommandTypeDef);
                    i++)
            {
                if (strncmp((const char *)shell.commandBuff,
                            (const char *)(shell.commandBase + i)->name, shell.commandLength) == 0)
                {
#if SHELL_ALLOW_SHIFT ==1
                    shellClearLine();
#else
                    shellBackspace(shell.commandLength);
#endif /* SHELL_ALLOW_SHIFT ==1 */
                    shell.commandLength = shellStringCopy(shell.commandBuff,
                                             (shell.commandBase + i)->name);
#if SHELL_ALLOW_SHIFT == 1
                    shell.commandCursor = shell.commandLength;
#endif
                    shellDisplay(shell.commandBuff);
                }
            }
        }
        else                                            //无输入，输入help
        {
            shellBackspace(shell.commandLength);
            shell.commandLength = shellStringCopy(shell.commandBuff, (uint8_t *)"help");
#if SHELL_ALLOW_SHIFT == 1
            shell.commandCursor = shell.commandLength;
#endif
            shellDisplay(shell.commandBuff);
        }
        break;

    case 0x1B:                                          //控制键
        controlFlag = CONTROL_STEP_ONE;
        break;

    default:
        switch ((uint8_t)controlFlag)
        {
        case CONTROL_STEP_TWO:
            if (receiveData == 0x41)                //方向上键
            {
#if SHELL_USE_HISTORY == 1
#if SHELL_ALLOW_SHIFT == 1
                shellClearLine();
#else
                shellBackspace(shell.commandLength);
#endif /* SHELL_ALLOW_SHIFT == 1 */
                if (shell.historyOffset--
                        <= -((shell.historyCount > shell.historyFlag)
                             ? shell.historyCount : shell.historyFlag))
                {
                    shell.historyOffset
                        = -((shell.historyCount > shell.historyFlag)
                            ? shell.historyCount : shell.historyFlag);
                }
                shell.commandLength = shellStringCopy(shell.commandBuff,
                                         shell.historyCommand[(shell.historyFlag + SHELL_HISTORY_MAX_NUMBER
                                                 + shell.historyOffset) % SHELL_HISTORY_MAX_NUMBER]);
#if SHELL_ALLOW_SHIFT == 1
                shell.commandCursor = shell.commandLength;
#endif
                shellDisplay(shell.commandBuff);
#else
                //shellDisplay("up\r\n");
#endif /* SHELL_ALLOW_SHIFT == 1 */
            }
            else if (receiveData == 0x42)           //方向下键
            {
#if SHELL_USE_HISTORY == 1
                if (++shell.historyOffset >= 0)
                {
                    shell.historyOffset = -1;
                    break;
                }
#if SHELL_ALLOW_SHIFT == 1
                shellClearLine();
#else
                shellBackspace(shell.commandLength);
#endif
                shell.commandLength = shellStringCopy(shell.commandBuff,
                                         shell.historyCommand[(shell.historyFlag + SHELL_HISTORY_MAX_NUMBER
                                                 + shell.historyOffset) % SHELL_HISTORY_MAX_NUMBER]);
#if SHELL_ALLOW_SHIFT == 1
                shell.commandCursor = shell.commandLength;
#endif
                shellDisplay(shell.commandBuff);
#else
                //shellDisplay("down\r\n");
#endif /* SHELL_ALLOW_SHIFT == 1 */
            }
            else if (receiveData == 0x43)           //方向右键
            {
#if SHELL_ALLOW_SHIFT == 1
                if (shell.commandCursor < shell.commandLength)
                {
                    shellDisplayByte(shell.commandBuff[shell.commandCursor]);
                    shell.commandCursor++;
                }
#endif
                //shellDisplay("right\r\n");
            }
            else if (receiveData == 0x44)           //方向左键
            {
#if SHELL_ALLOW_SHIFT == 1
                if (shell.commandCursor > 0)
                {
                    shellDisplayByte('\b');
                    shell.commandCursor--;
                }
#endif
                //shellDisplay("left\r\n");
            }
            else
            {
                controlFlag = CONTROL_FREE;
                goto normal;
            }
            break;

        case CONTROL_STEP_ONE:
            if (receiveData == 0x5B)
            {
                controlFlag = CONTROL_STEP_TWO;
            }
            else
            {
                controlFlag = CONTROL_FREE;
                goto normal;
            }
            break;

        case CONTROL_FREE:                          //正常按键处理
normal:
            if (shell.commandLength < SHELL_COMMAND_MAX_LENGTH - 1)
            {
#if SHELL_ALLOW_SHIFT == 1
                if (shell.commandCursor == shell.commandLength)
                {
                    shell.commandBuff[shell.commandLength++] = receiveData;
                    shell.commandCursor++;              //光标位置加1
                    shellDisplayByte(receiveData);
                }
                else
                {
                    for (int8_t i = shell.commandLength - shell.commandCursor;
                            i > 0; i--)
                    {
                        shell.commandBuff[shell.commandCursor + i] =
                            shell.commandBuff[shell.commandCursor + i - 1];
                    }
                    shellClearLine();
                    shell.commandBuff[shell.commandCursor++] = receiveData;
                    shell.commandBuff[++shell.commandLength] = 0;
                    shellDisplay(shell.commandBuff);
                    for (int8_t i = shell.commandLength - shell.commandCursor;
                            i > 0; i--)
                    {
                        shellDisplayByte('\b');
                    }
                }
#else /* SHELL_ALLOW_SHIFT == 1 */
                shell.commandBuff[shell.commandLength++] = receiveData;
                shellDisplayByte(receiveData);
#endif /* SHELL_ALLOW_SHIFT == 1 */
            }
            else
            {
#if SHELL_ALLOW_SHIFT == 1
                shellDisplay("\r\nWarnig: Command is too long\r\n");
                shellDisplay(SHELL_COMMAND);
                shellDisplay(shell.commandBuff);
                shell.commandCursor = shell.commandLength;
#else
                shell.commandLength++;
                shellDisplayByte(receiveData);
#endif /* SHELL_ALLOW_SHIFT == 1 */
            }
            break;

        }
        break;
    }
}


/*******************************************************************************
*@function  shellStringCopy
*@brief     字符串复制
*@param     *dest       目标字符串
*           *src        源字符串
*@retval    字符串长度
*@author    Letter
*******************************************************************************/
uint8_t shellStringCopy(uint8_t *dest, uint8_t *src)
{
    uint8_t count = 0;
    while (*src)
    {
        *(dest++) = *(src++);
        count++;
    }
    *dest = 0;
    return count;
}


/*******************************************************************************
*@function  shellBackspace
*@brief     命令行退格操作
*@param     length      退格长度
*@retval    None
*@author    Letter
*******************************************************************************/
void shellBackspace(uint8_t length)
{
    while (length--)
    {
        shellDisplayByte(0x08);                             //"\b \b"
        shellDisplayByte(' ');
        shellDisplayByte(0x08);
    }
}


#if SHELL_ALLOW_SHIFT == 1
/*******************************************************************************
*@function  shellClearLine
*@brief     清除命令行
*@param     None
*@retval    None
*@author    Letter
*******************************************************************************/
void shellClearLine(void)
{
    for (int8_t i = shell.commandLength - shell.commandCursor; i > 0; i--)
    {
        shellDisplayByte(' ');
    }
    shellBackspace(shell.commandLength);
}
#endif


/*******************************************************************************
*@function  shellShowCommandList
*@brief     shell内置命令help调用函数，显示所支持的所有shell命令
*@param     None
*@retval    None
*@author    Letter
*******************************************************************************/
void shellShowCommandList(void)
{
    int32_t spaceLength;

    shellDisplay("COMMAND                 FUNCTION\r\n\r\n");

    for(uint8_t i = 0;
            i < ((uint32_t)shell.commandLimit -
                (uint32_t)shell.commandBase) / sizeof(SHELL_CommandTypeDef);
            i++)
    {
        spaceLength = 24 - (int32_t)_ShellDisplay((shell.commandBase + i)->name);
        spaceLength = (spaceLength > 0) ? spaceLength : 4;
        do {
            shellDisplay(" ");
        } while (--spaceLength);
        shellDisplay((shell.commandBase + i)->desc);
        shellDisplay("\r\n");
    }
}
SHELL_EXPORT_CMD(help, shellShowCommandList, show command list);


/*******************************************************************************
*@function  shellLetter
*@brief     shell信息
*@param     None
*@retval    None
*@author    Letter
*******************************************************************************/
void shellLetter(void)
{
    shellDisplay("Letter shell "SHELL_VERSION"\r\n");
    shellDisplay("(C) Copyright 2018 Leter, All Right Reserved\r\n");
}
SHELL_EXPORT_CMD(letter, shellLetter, letter shell);


/*******************************************************************************
*@function  shellRebot
*@brief     重启系统
*@param     None
*@retval    None
*@author    Letter
*******************************************************************************/
void shellReboot(void)
{
    shellClear();
    shellDisplay("system rebooting\r\n");
    __set_FAULTMASK(1);
    NVIC_SystemReset();
}
SHELL_EXPORT_CMD(reboot, shellReboot, reboot system);


/*******************************************************************************
*@function  shellClear
*@brief     命令行清屏
*@param     None
*@retval    None
*@author    Letter
*******************************************************************************/
void shellClear(void)
{
    shellDisplay("\033[2J\033[1H");
}
SHELL_EXPORT_CMD(clear, shellClear, clear command line);


#if SHELL_USE_PARAMETER == 1
/*******************************************************************************
*@function  shellParaTest
*@brief     带参命令示例函数
*@param     argc        参数个数
*           argv        参数
*@retval    None
*@author    Letter
*******************************************************************************/
uint32_t shellParaTest(uint32_t argc, uint8_t *argv[])
{
    shellDisplayByte((uint8_t)(argc + '0'));
    shellDisplay(" parameter(s)\r\n");
    
    if (argc == 1)
    {
        return 0;

    }
    while (--argc > 0)
    {
        shellDisplay(*++argv);
        shellDisplay("\r\n");
    }
    return 0;
}
SHELL_EXPORT_CMD(paraTest, shellParaTest, test parameter);
#endif
