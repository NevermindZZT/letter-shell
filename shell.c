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
*                   20187/24    v1.7
*                               增加SHELL_TypeDef结构体
*                               采用新的命令添加方式，现在可以在任意文件的函数
*                               外部采用宏SHELL_EXPORT_CMD进行命令定义
*******************************************************************************/

#include    "shell.h"
#include    "string.h"

static SHELL_TypeDef shell = 
{
    .shellCommandFlag = 0,
#if SHELL_USE_HISTORY == 1
    .shellHistoryCount = 0,
    .shellHistoryFlag = 0,
    .shellHistoryOffset = 0,
#endif
};

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
//const SHELL_CommandTypeDef shellCommandList[] = 
//{
//    /*command               function                description*/
//    {(uint8_t *)"letter",   shellLetter,            (uint8_t *)"letter shell"},
//    {(uint8_t *)"reboot",   shellReboot,            (uint8_t *)"reboot system"},
//    {(uint8_t *)"help",     shellShowCommandList,   (uint8_t *)"show command list"},
//    {(uint8_t *)"clear",    shellClear,             (uint8_t *)"clear command line"},
//    
//#if SHELL_USE_PARAMETER == 1    /*带参函数命令*/
//    {(uint8_t *)"paraTest", (void (*)())shellParaTest, (uint8_t *)"test parameter"},
//#endif
//    
//};
    

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
    for (int i = 0; i < SHELL_PARAMETER_MAX_NUMBER; i++)
    {
        shell.commandPointer[i] = shell.commandPara[i];
    }
#endif
    
    extern const int shellCommand$$Base;
    extern const int shellCommand$$Limit;
    
    shell.shellCommandBase = (SHELL_CommandTypeDef *)(&shellCommand$$Base);
    shell.shellCommandLimit = (SHELL_CommandTypeDef *)(&shellCommand$$Limit);
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
            if (shell.shellCommandFlag >= SHELL_COMMAND_MAX_LENGTH - 1)
            {
                shellDisplay("\r\nError: Command is too long\r\n");
                shell.shellCommandBuff[shell.shellCommandFlag] = 0;
                shell.shellCommandFlag = 0;
                shellDisplay(SHELL_COMMAND);
                break;
            }
            
            if (shell.shellCommandFlag == 0)
            {
                shellDisplay(SHELL_COMMAND);
                break;
            }
            else
            {
                shell.shellCommandBuff[shell.shellCommandFlag++] = 0;
#if SHELL_USE_PARAMETER == 1
                shell.commandCount = 0;
                
                uint8_t j = 0;
                for (int8_t i = 0; i < shell.shellCommandFlag; i++)
                {
                    if (shell.shellCommandBuff[i] != ' ' &&
                        shell.shellCommandBuff[i] != '\t' &&
                        shell.shellCommandBuff[i] != 0)
                    {
                        shell.commandPara[shell.commandCount][j++] = shell.shellCommandBuff[i];
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
                shell.shellCommandFlag = 0;
                
                if (shell.commandCount == 0)                      //是否为无效指令
                {
                    shellDisplay(SHELL_COMMAND);
                    break;
                }
                
#if SHELL_USE_HISTORY ==1
                shellStringCopy(shell.shellHistoryCommand[shell.shellHistoryFlag++],
                                shell.shellCommandBuff);
                if (++shell.shellHistoryCount > SHELL_HISTORY_MAX_NUMBER)
                {
                    shell.shellHistoryCount = SHELL_HISTORY_MAX_NUMBER;
                }
                if (shell.shellHistoryFlag >= SHELL_HISTORY_MAX_NUMBER)
                {
                    shell.shellHistoryFlag = 0;
                }
                shell.shellHistoryOffset = 0;
#endif
                
                shellDisplay("\r\n");
                runFlag = 0;
                
                for (int8_t  i= 0;
                     i < ((uint32_t)shell.shellCommandLimit - (uint32_t)shell.shellCommandBase) / sizeof(SHELL_CommandTypeDef);
                     i++)
                {
                    if (strcmp((const char *)shell.commandPara[0],
                        (const char *)(shell.shellCommandBase + i)->name) == 0)
                    {
                        runFlag = 1;
                        (shell.shellCommandBase + i)->function(shell.commandCount, shell.commandPointer);
                        break;
                    }
                }
//                for (int8_t i = sizeof(shellCommandList) / sizeof(SHELL_CommandTypeDef) - 1;
//                     i >=  0; i--)
//                {
//                    if (strcmp((const char *)shell.commandPara[0],
//                        (const char *)shellCommandList[i].name) == 0)
//                    {
//                        runFlag = 1;
//                        shellCommandList[i].function(shell.commandCount, shell.commandPointer);
//                        break;
//                    }
//                }
                
#else /*SHELL_USE_PARAMETER == 1*/
                
                shell.shellCommandBuff[shell.shellCommandFlag] = 0;
                shell.shellCommandFlag = 0;
                shellDisplay("\r\n");
                runFlag = 0;
                for (int8_t  i= 0;
                     i < (shell.shellCommandLimit - shell.shellCommandBase) / sizeof(SHELL_CommandTypeDef);
                     i++)
                {
                    if (strcmp((const char *)shell.shellCommandBuff,
                        (const char *)(shell.shellCommandBase + i)->name) == 0)
                    {
                        runFlag = 1;
                        (shell.shellCommandBase + i)->function();
                        break;
                    }
                }
//                for (int8_t i = sizeof(shellCommandList) / sizeof(SHELL_CommandTypeDef) - 1;
//                     i >=  0; i--)
//                {
//                    if (strcmp((const char *)shell.shellCommandBuff,
//                        (const char *)shellCommandList[i].name) == 0)
//                    {
//                        runFlag = 1;
//                        shellCommandList[i].function();
//                        break;
//                    }
//                }
#endif /*SHELL_USE_PARAMETER == 1*/
                
                if (runFlag == 0)
                {
                    shellDisplay("Command not found");
                }
            }
            shellDisplay(SHELL_COMMAND);
            break;
            
        case 0x08:                                          //退格
            if (shell.shellCommandFlag != 0)
            {
                shell.shellCommandFlag--;
                shellBackspace(1);
            }
            break;
            
        case '\t':                                          //制表符
        #if SHELL_USE_HISTORY == 1
            if (shell.shellCommandFlag != 0)
            {
                for (int8_t  i= 0;
                     i < ((uint32_t)shell.shellCommandLimit - (uint32_t)shell.shellCommandBase) / sizeof(SHELL_CommandTypeDef);
                     i++)
                {
                    if (strncmp((const char *)shell.shellCommandBuff,
                        (const char *)(shell.shellCommandBase + i)->name, shell.shellCommandFlag) == 0)
                    {
                        shellBackspace(shell.shellCommandFlag);
                        shell.shellCommandFlag = shellStringCopy(shell.shellCommandBuff, 
                                           (shell.shellCommandBase + i)->name);
                        shellDisplay(shell.shellCommandBuff);
                    }
                }
//                shellBackspace(shellCommandFlag);
//                shellCommandFlag = shellStringCopy(shellCommandBuff,
//                                   shellHistoryCommand[(shellHistoryFlag + SHELL_HISTORY_MAX_NUMBER - 1)
//                                                        % SHELL_HISTORY_MAX_NUMBER]);
//                shellDisplay(shellCommandBuff);
            }
            else                                            //无历史命令，输入help
            {
                shellBackspace(shell.shellCommandFlag);
                shell.shellCommandFlag = 4;
                shellStringCopy(shell.shellCommandBuff, (uint8_t *)"help");
                shellDisplay(shell.shellCommandBuff);
            }
        #endif
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
                        shellBackspace(shell.shellCommandFlag);
                        if (shell.shellHistoryOffset-- 
                            <= -((shell.shellHistoryCount > shell.shellHistoryFlag)
                                ? shell.shellHistoryCount : shell.shellHistoryFlag))
                        {
                            shell.shellHistoryOffset 
                            = -((shell.shellHistoryCount > shell.shellHistoryFlag)
                                ? shell.shellHistoryCount : shell.shellHistoryFlag);
                        }
                        shell.shellCommandFlag = shellStringCopy(shell.shellCommandBuff,
                            shell.shellHistoryCommand[(shell.shellHistoryFlag + SHELL_HISTORY_MAX_NUMBER
                                                 + shell.shellHistoryOffset) % SHELL_HISTORY_MAX_NUMBER]);
                        shellDisplay(shell.shellCommandBuff);
                    #else
                        //shellDisplay("up\r\n");
                    #endif
                    }
                    else if (receiveData == 0x42)           //方向下键
                    {
                    #if SHELL_USE_HISTORY == 1
                        if (++shell.shellHistoryOffset >= 0)
                        {
                            shell.shellHistoryOffset = -1;
                            break;
                        }
                        shellBackspace(shell.shellCommandFlag);
                        shell.shellCommandFlag = shellStringCopy(shell.shellCommandBuff,
                            shell.shellHistoryCommand[(shell.shellHistoryFlag + SHELL_HISTORY_MAX_NUMBER
                                                 + shell.shellHistoryOffset) % SHELL_HISTORY_MAX_NUMBER]);
                        shellDisplay(shell.shellCommandBuff);
                    #else
                        //shellDisplay("down\r\n");
                    #endif
                    }
                    else if (receiveData == 0x43)           //方向右键
                    {
                        //shellDisplay("right\r\n");
                    }
                    else if (receiveData == 0x44)           //方向左键
                    {
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
normal:             if (shell.shellCommandFlag < SHELL_COMMAND_MAX_LENGTH - 1)
                    {
                        shell.shellCommandBuff[shell.shellCommandFlag++] = receiveData;
                        shellDisplayByte(receiveData);
                    }
                    else
                    {
                        shell.shellCommandFlag++;
                        shellDisplayByte(receiveData);
                    }
                    break;
                
            }
            break;
    }
}


#if SHELL_USE_HISTORY == 1
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
#endif


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
        i < ((uint32_t)shell.shellCommandLimit - (uint32_t)shell.shellCommandBase) / sizeof(SHELL_CommandTypeDef);
        i++)
//    for (uint8_t i = 0; i < sizeof(shellCommandList) / sizeof(SHELL_CommandTypeDef); i++)
    {
        spaceLength = 24 - (int32_t)_ShellDisplay((shell.shellCommandBase + i)->name);
        spaceLength = (spaceLength > 0) ? spaceLength : 4;
        do {
            shellDisplay(" ");
        } while (--spaceLength);
        shellDisplay((shell.shellCommandBase + i)->desc);
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
    printf("%d parameter(s)\r\n", argc);
    
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
