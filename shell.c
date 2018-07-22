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
*******************************************************************************/

#include    "shell.h"
#include    "string.h"

uint8_t shellCommandBuff[SHELL_COMMAND_MAX_LENGTH];
uint8_t shellCommandFlag = 0;

#if SHELL_USE_PARAMETER == 1
    static uint8_t commandPara[SHELL_PARAMETER_MAX_NUMBER][SHELL_PARAMETER_MAX_LENGTH];
    static uint8_t commandCount;
    static uint8_t *commandPointer[SHELL_PARAMETER_MAX_NUMBER];
#endif

#if SHELL_USE_HISTORY == 1
    static uint8_t shellHistoryCommand[SHELL_HISTORY_MAX_NUMBER][SHELL_COMMAND_MAX_LENGTH];
    static uint8_t shellHistoryCount = 0;                       //已记录的历史命令数量
    static int8_t shellHistoryFlag = 0;                         //当前记录位置
    static int8_t shellHistoryOffset = 0;
#endif

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
        commandPointer[i] = commandPara[i];
    }
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
            if (shellCommandFlag >= SHELL_COMMAND_MAX_LENGTH - 1)
            {
                shellDisplay("\r\nError: Command is too long\r\n");
                shellCommandBuff[shellCommandFlag] = 0;
                shellCommandFlag = 0;
                shellDisplay(SHELL_COMMAND);
                break;
            }
            
            if (shellCommandFlag == 0)
            {
                shellDisplay(SHELL_COMMAND);
                break;
            }
            else
            {
                shellCommandBuff[shellCommandFlag++] = 0;
#if SHELL_USE_PARAMETER == 1
                commandCount = 0;
                
                uint8_t j = 0;
                for (int8_t i = 0; i < shellCommandFlag; i++)
                {
                    if (shellCommandBuff[i] != ' ' &&
                        shellCommandBuff[i] != '\t' &&
                        shellCommandBuff[i] != 0)
                    {
                        commandPara[commandCount][j++] = shellCommandBuff[i];
                    }
                    else
                    {
                        if (j != 0)
                        {
                            commandPara[commandCount][j] = 0;
                            commandCount ++;
                            j = 0;
                        }
                    }
                }
                shellCommandFlag = 0;
                
                if (commandCount == 0)                      //是否为无效指令
                {
                    shellDisplay(SHELL_COMMAND);
                    break;
                }
                
#if SHELL_USE_HISTORY ==1
                shellStringCopy(shellHistoryCommand[shellHistoryFlag++], shellCommandBuff);
                if (++shellHistoryCount > SHELL_HISTORY_MAX_NUMBER)
                {
                    shellHistoryCount = SHELL_HISTORY_MAX_NUMBER;
                }
                if (shellHistoryFlag >= SHELL_HISTORY_MAX_NUMBER)
                {
                    shellHistoryFlag = 0;
                }
                shellHistoryOffset = 0;
#endif
                
                shellDisplay("\r\n");
                runFlag = 0;

                for (int8_t i = sizeof(shellCommandList) / sizeof(SHELL_CommandTypeDef) - 1;
                     i >=  0; i--)
                {
                    if (strcmp((const char *)commandPara[0],
                        (const char *)shellCommandList[i].name) == 0)
                    {
                        runFlag = 1;
                        shellCommandList[i].function(commandCount, commandPointer);
                        break;
                    }
                }
                
#else /*SHELL_USE_PARAMETER == 1*/
                
                shellCommandBuff[shellCommandFlag] = 0;
                shellCommandFlag = 0;
                shellDisplay("\r\n");
                runFlag = 0;
                for (int8_t i = sizeof(shellCommandList) / sizeof(SHELL_CommandTypeDef) - 1;
                     i >=  0; i--)
                {
                    if (strcmp((const char *)shellCommandBuff,
                        (const char *)shellCommandList[i].name) == 0)
                    {
                        runFlag = 1;
                        shellCommandList[i].function();
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
            if (shellCommandFlag != 0)
            {
                shellCommandFlag--;
                shellBackspace(1);
            }
            break;
            
        case '\t':                                          //制表符
        #if SHELL_USE_HISTORY == 1
            if (shellCommandFlag != 0)
            {
                for (int8_t i = sizeof(shellCommandList) / sizeof(SHELL_CommandTypeDef) - 1;
                     i >=  0; i--)
                {
                    if (strncmp((const char *)shellCommandBuff,
                        (const char *)shellCommandList[i].name, shellCommandFlag) == 0)
                    {
                        shellBackspace(shellCommandFlag);
                        shellCommandFlag = shellStringCopy(shellCommandBuff, 
                                           shellCommandList[i].name);
                        shellDisplay(shellCommandBuff);
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
                shellBackspace(shellCommandFlag);
                shellCommandFlag = 4;
                shellStringCopy(shellCommandBuff, (uint8_t *)"help");
                shellDisplay(shellCommandBuff);
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
                        shellBackspace(shellCommandFlag);
                        if (shellHistoryOffset-- 
                            <= -((shellHistoryCount > shellHistoryFlag)
                                ? shellHistoryCount : shellHistoryFlag))
                        {
                            shellHistoryOffset 
                            = -((shellHistoryCount > shellHistoryFlag)
                                ? shellHistoryCount : shellHistoryFlag);
                        }
                        shellCommandFlag = shellStringCopy(shellCommandBuff,
                            shellHistoryCommand[(shellHistoryFlag + SHELL_HISTORY_MAX_NUMBER
                                                 + shellHistoryOffset) % SHELL_HISTORY_MAX_NUMBER]);
                        shellDisplay(shellCommandBuff);
                    #else
                        //shellDisplay("up\r\n");
                    #endif
                    }
                    else if (receiveData == 0x42)           //方向下键
                    {
                    #if SHELL_USE_HISTORY == 1
                        if (++shellHistoryOffset >= 0)
                        {
                            shellHistoryOffset = -1;
                            break;
                        }
                        shellBackspace(shellCommandFlag);
                        shellCommandFlag = shellStringCopy(shellCommandBuff,
                            shellHistoryCommand[(shellHistoryFlag + SHELL_HISTORY_MAX_NUMBER
                                                 + shellHistoryOffset) % SHELL_HISTORY_MAX_NUMBER]);
                        shellDisplay(shellCommandBuff);
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
normal:             if (shellCommandFlag < SHELL_COMMAND_MAX_LENGTH - 1)
                    {
                        shellCommandBuff[shellCommandFlag++] = receiveData;
                        shellDisplayByte(receiveData);
                    }
                    else
                    {
                        shellCommandFlag++;
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
    
    for (uint8_t i = 0; i < sizeof(shellCommandList) / sizeof(SHELL_CommandTypeDef); i++)
    {
        spaceLength = 24 - (int32_t)_ShellDisplay(shellCommandList[i].name);
        spaceLength = (spaceLength > 0) ? spaceLength : 4;
        do {
            shellDisplay(" ");
        } while (--spaceLength);
        shellDisplay(shellCommandList[i].desc);
        shellDisplay("\r\n");
    }
}


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
#endif
