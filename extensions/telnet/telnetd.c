/**
 * @file telnetd.c
 * @author Letter
 * @brief telnet server for letter shell
 * @version 0.1
 * @date 2021-08-07
 * 
 * @copyright (c) 2021 Letter
 * 
 */
#include "telnetd.h"

#include "sys/socket.h"
#include "arpa/inet.h"
#include "netinet/in.h"

#include "shell.h"
#include "shell_cmd_group.h"

#if SHELL_USING_COMPANION != 1
#error telent for letter shell can not be used while shell companion is diabled
#endif

/**
 * @brief 新线程接口实例
 */
static NewThread newThread;

/**
 * @brief telnet server socket
 */
static int telnetdSocket;

/**
 * @brief telent server shell
 */
static Shell *telnetdShell;

/**
 * @brief telnet server 监听端口
 */
static int telnetdPort = TELNETD_DEFAULT_SERVER_PORT;

static void telnetdServer(void);
static void telnetdConnection(int client);
static void telentdWrite(char *data, short len);

/**
 * @brief telnet 协议命令
 */
static char telnetCmd[] = {0xff, 0xfb, 0x01, 0xff, 0xfb, 0x03, 0xff, 0xfc, 0x1f};

/**
 * @brief telnet server初始化
 * 
 * @param newThreadInterface 新线程接口 
 * 
 * @return int 0 启动telent成功 -1 启动失败
 */
int telentdInit(NewThread newThreadInterface)
{
    newThread = newThreadInterface;
    return newThread ? 0 : -1;
}

/**
 * @brief 启动 telnet server
 * 
 * @return int 0 启动telent成功 -1 启动失败
 */
int telnetdStart()
{
    return newThread ? newThread(telnetdServer, NULL) : -1;
}

/**
 * @brief 停止 telnet server
 * 
 */
void telnetdStop()
{
    close(telnetdSocket);
    if (telnetdShell != NULL)
    {
        close((int) shellCompanionGet(telnetdShell, SHELL_COMPANION_ID_TELNETD));
    }
}

/**
 * @brief 修改telnet server 监听端口
 * 
 * @param port 端口
 * 
 */
void telnetdSetPort(int port)
{
    telnetdPort = port;
}

/**
 * @brief telent 服务
 * 
 * 
 */
static void telnetdServer(void)
{
    struct sockaddr_in telnetdAddr;
    int client;
    struct sockaddr_in clientAddr;
    int addrLen = sizeof(clientAddr);

    telnetdSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    memset(&telnetdAddr, 0, sizeof(telnetdAddr));
    telnetdAddr.sin_family = AF_INET,
    telnetdAddr.sin_addr.s_addr = inet_addr(TELNETD_SERVER_ADDRESS);
    telnetdAddr.sin_port  = htons(telnetdPort);
    if (bind(telnetdSocket, (struct sockaddr *)&telnetdAddr, sizeof(telnetdAddr)) == 0 &&
        listen(telnetdSocket, 5) == 0)
    {
        while (1)
        {
            client = accept(telnetdSocket, (struct sockaddr *) &clientAddr, (socklen_t *) &addrLen);
            if (client > 0)
            {
                newThread(telnetdConnection, (void *) client);
            }
            else if (client <= 0)
            {
                break;
            }
        }
    }
    close(telnetdSocket);
}

/**
 * @brief telnet server 连接处理
 * 
 * @param client 客户端连接socket
 * 
 */
static void telnetdConnection(int client)
{
    int len = 0;
    char *data = SHELL_MALLOC(16);
    char *shellBuffer = SHELL_MALLOC(TELNETD_SHELL_BUFFER_SIZE);
    telnetdShell = SHELL_MALLOC(sizeof(Shell));

    /** 处理 telent 协议 */
    send(client, telnetCmd, 9, 0);
    recv(client, data, 6, 0);

    telnetdShell->write = telentdWrite;
    shellCompanionAdd(telnetdShell, SHELL_COMPANION_ID_TELNETD, (void *)client);
    shellInit(telnetdShell, shellBuffer, TELNETD_SHELL_BUFFER_SIZE);

    if (TELNETD_SHELL_USER)
    {
        shellRun(telnetdShell, TELNETD_SHELL_USER);
    }

    while (1)
    {
        len = recv(client, data, 1, 0);
        if (len == 0)
        {
            break;
        }
        else if (len == 1)
        {
            shellHandler(telnetdShell, data[0]);
        }
    }
    shellDeInit(telnetdShell);
    SHELL_FREE(data);
    SHELL_FREE(shellBuffer);
    SHELL_FREE(telnetdShell);
    telnetdShell = NULL;
    close(client);
}

/**
 * @brief telnet server数据写
 * 
 * @param data 写入的数据
 * @param len 数据长度
 * 
 */
static void telentdWrite(char *data, short len)
{
    int client = (int) shellCompanionGet(telnetdShell, SHELL_COMPANION_ID_TELNETD);
    if (client != 0)
    {
        send(client, data, len, 0);
    }
}


ShellCommand telnetdGroup[] =
{
    SHELL_CMD_GROUP_ITEM(SHELL_TYPE_CMD_FUNC, start, telnetdStart, start telnet server),
    SHELL_CMD_GROUP_ITEM(SHELL_TYPE_CMD_FUNC, stop, telnetdStop, stop telnet server),
    SHELL_CMD_GROUP_ITEM(SHELL_TYPE_CMD_FUNC, setPort, telnetdSetPort, set telnet server port),
    SHELL_CMD_GROUP_END()
};
SHELL_EXPORT_CMD_GROUP(
SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN)|SHELL_CMD_DISABLE_RETURN,
telnetd, telnetdGroup, telnet server\ninput telent -h for more help);
