/*- Coding With UTF-8 -*/


/*******************************************************************************
*   File Name：     shell.h
*   Description：   此文件提供了shell的相关配置，声明
*   Atuhor：        Letter
*   Date:           2018/4/20
*******************************************************************************/

#ifndef     __SHELL_H__
#define     __SHELL_H__

#include    "config.h"

/*------------------------------宏定义----------------------------------------*/
#define     SHELL_VERSION               "v1.7"                  //版本

#define     SHELL_USE_PARAMETER         1                       //是否使用带参函数
#define     SHELL_USE_HISTORY           1                       //是否使用历史命令

#define     shellUart                   huart3                  //shell使用的串口

#define     SHELL_COMMAND_MAX_LENGTH    50                      //shell命令最大长度
#define     SHELL_PARAMETER_MAX_LENGTH  10                      //shell命令参数最大长度
#define     SHELL_PARAMETER_MAX_NUMBER  5                       //shell命令参数最大数量

#define     SHELL_HISTORY_MAX_NUMBER    5                       //历史命令记录数量

#define     SHELL_COMMAND               "\r\n\r\nletter>>"

#define     shellDisplay(x)             _ShellDisplay((uint8_t *) (x));

#define     SHELL_EXPORT_CMD(cmd, func, desc)                               \
            const SHELL_CommandTypeDef                                      \
            shellCommand##cmd __attribute__((section("shellCommand"))) =    \
            {                                                               \
                (uint8_t *)#cmd,                                            \
                (void (*)())func,                                           \
                (uint8_t *)#desc                                            \
            }
            

/*---------------------------函数指针定义-------------------------------------*/
typedef void (*shellFunction)();


/*----------------------------结构体定义--------------------------------------*/
typedef struct
{
    uint8_t *name;                                              //shell命令名称
    shellFunction function;                                     //shell命令函数
    uint8_t *desc;                                              //shell命令描述
}SHELL_CommandTypeDef;                                          //shell命令定义


typedef struct
{
    uint8_t shellCommandBuff[SHELL_COMMAND_MAX_LENGTH];
    uint8_t shellCommandFlag;

#if SHELL_USE_PARAMETER == 1
    uint8_t commandPara[SHELL_PARAMETER_MAX_NUMBER][SHELL_PARAMETER_MAX_LENGTH];
    uint8_t commandCount;
    uint8_t *commandPointer[SHELL_PARAMETER_MAX_NUMBER];
#endif

#if SHELL_USE_HISTORY == 1
    uint8_t shellHistoryCommand[SHELL_HISTORY_MAX_NUMBER][SHELL_COMMAND_MAX_LENGTH];
    uint8_t shellHistoryCount;                       //已记录的历史命令数量
    int8_t shellHistoryFlag;                         //当前记录位置
    int8_t shellHistoryOffset;
#endif
    
    SHELL_CommandTypeDef *shellCommandBase;
    SHELL_CommandTypeDef *shellCommandLimit;
}SHELL_TypeDef;


typedef enum
{
    CONTROL_FREE = 0,
    CONTROL_STEP_ONE,
    CONTROL_STEP_TWO,
}CONTROL_Status;

/*-----------------------------函数声明---------------------------------------*/

uint8_t shellReceiveByte(void);                                 //shell接收一字节数据

void shellDisplayByte(uint8_t data);                            //shell显示一字节数据

void shellInit(void);                                           //shell初始化

void shellMain(void);                                           //shell主函数（阻塞式）

void shellHandler(uint8_t receiveData);                         //shell处理函数

#if SHELL_USE_HISTORY == 1
uint8_t shellStringCopy(uint8_t *dest, uint8_t *src);           //字符串复制
#endif

void shellBackspace(uint8_t length);                            //shell退格

void shellShowCommandList(void);                                //显示所有shell命令

void shellLetter(void);                                         //显示shell信息

void shellReboot(void);                                         //重启系统

void shellClear(void);                                          //shell清屏

#if SHELL_USE_PARAMETER == 1
uint32_t shellParaTest(uint32_t argc, uint8_t *argv[]);         //带参函数示例
#endif

#endif

