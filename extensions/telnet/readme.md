# telnet

![version](https://img.shields.io/badge/version-1.0.0-brightgreen.svg)
![standard](https://img.shields.io/badge/standard-c99-brightgreen.svg)
![build](https://img.shields.io/badge/build-2021.08.07-brightgreen.svg)
![license](https://img.shields.io/badge/license-MIT-brightgreen.svg)

基于letter shell运行的telnet

- [telnet](#telnet)
  - [简介](#简介)
  - [使用](#使用)
  - [其他](#其他)

## 简介

telnet是基于letter shell运行的一个telnet服务端，可以在一些具有网络能力的嵌入式环境中使用，特别适用于现场不方便接线条件下的调试工作，telnet默认运行在23端口，可以在`telnetd.h`文件中进行配置，也可以在启动服务前，调用`telnetdSetPort`进行修改，注意23端口可能会被系统占用

## 使用

1. 修改包含的头文件

    telnet使用标准的socket接口实现，运行此telnet实现需要你的运行环境具有socket接口，使用时，需要首先确认你的运行环境中，提供标准socket的头文件，然后替换`telnetd.c`文件中包含的头文件

    ```c
    #include "sys/socket.h"
    #include "arpa/inet.h"
    #include "netinet/in.h"
    ```

2. 实现线程接口

    telnet的运行需要多任务支持，你选哟提供一个用于创建新线程的接口，下面是在linux环境下的示例

    ```c
    /**
     * @brief 新线程接口
     * 
     * @param handler 线程函数
     * @param param 线程参数
     * 
     * @return int 0 启动成功 -1 启动失败
     */
    int userNewThread(void *handler, void *param)
    {
        pthread_t tid;

        return pthread_create(&tid, NULL, handler, param) == 0 ? 0 : -1;
    }
    ```

3. 初始化

    ```c
    telnetdInit(userNewThread);
    ```

4. 启动

    直接调用`telnetdStart`函数，或者在shell执行`telnetd start`命令，启动telent server

## 其他

- 只支持一个客户端连接

  做telnet这个功能的时候发现，letter shell目前提供的接口，不好在telnet上做多客户端支持，遂暂时只支持一个客户端连接

- 部分shell功能不可用

  目前的telent实现没有实现letter shell的`read`接口，所以部分依赖于此接口的功能，比如说`shellScan`都是不可用的

- 使用telent需要开启letter shell的伴生对象功能

- 性能优化

  对于资源紧张的单片机，telnet频繁的网络操作可能会对系统性能产生压力，此时建议优化`telentd.c`中的`telnetdWrite`函数，可以使用环形缓冲区，每次此接口写入的数据不直接调用`send`函数，而是写入缓冲区，然后定时(100ms左右)调用`send`函数清空缓冲区

- x86 demo

  x86 demo中实现了telnet的接口，可以直接运行试用

  ```sh
  letter:/$ telnetd setPort 4545
  letter:/$ telnetd start
  ```
