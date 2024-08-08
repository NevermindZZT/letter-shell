/**
 * @file ringbuffer.h
 * @author 独霸一方 (2696652257@qq.com)
 * @brief //> 部分参考cherryRB实现的环形缓冲区,完整实现参考:https://github.com/cherry-embedded/CherryRB
 * 源码直接加入shell_all.c中进行编译,无需再次添加此目录下.c文件进行编译
 * @version 1.0
 * @date 2024-08-05
 *
 * @copyright Copyright (c) 2024
 *
 */

// > 单次包含宏定义
#ifndef __RINGBUFFER_H_
#define __RINGBUFFER_H_

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

// > C/C++兼容性宏定义
#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct __ringbuffer_t ringbuffer_t;
    struct __ringbuffer_t
    {
        bool busy; // 用于指示'慢速'外设是否正在操作缓冲区,比如DMA传输
        uint16_t head;
        uint16_t tail;
        uint16_t data_count;        // 当前队列中的数据量
        uint16_t last_series_count; // 调用连续读或写时候实际设置的值
        uint16_t buffer_size;       // 缓冲区大小
        uint8_t *buffer;
    };

    void ringbuffer_init(ringbuffer_t *rb, uint8_t *buffer, uint16_t size);
    void ringbuffer_reset(ringbuffer_t *rb);

    uint16_t ringbuffer_get_free(ringbuffer_t *rb);             // 获取剩余空间
    uint16_t ringbuffer_get_used(ringbuffer_t *rb);             // 获取已使用空间
    uint16_t ringbuffer_get_size(ringbuffer_t *rb);             // 获取缓冲区大小
    uint16_t ringbuffer_get_last_series_size(ringbuffer_t *rb); // 获取上次调用线性读取或者写入时候的设置的大小

    uint16_t ringbuffer_write(ringbuffer_t *rb, const uint8_t *data, uint16_t size); // 返回实际写入的数据量
    uint16_t ringbuffer_read(ringbuffer_t *rb, uint8_t *data, uint16_t size);        // 返回实际读取的数据量

    // for linear read/write operation(such as DMA)
    extern uint8_t *ringbuffer_linear_write_setup(ringbuffer_t *rb, uint16_t *size); // 返回写入的地址并设置实际能写入的数据量
    extern uint8_t *ringbuffer_linear_read_setup(ringbuffer_t *rb, uint16_t *size);  // 返回读取的地址并设置实际能读取的数据量
    extern uint16_t ringbuffer_linear_write_done(ringbuffer_t *rb, uint16_t size);   // 返回实际写入的数据量
    extern uint16_t ringbuffer_linear_read_done(ringbuffer_t *rb, uint16_t size);    // 返回实际读取的数据量

    void ringbuffer_mark_busy(ringbuffer_t *rb);
    void ringbuffer_mark_idle(ringbuffer_t *rb);
    bool ringbuffer_is_busy(ringbuffer_t *rb);

#ifdef __cplusplus
}
#endif //\ __cplusplus

#endif //\ __RINGBUFFER_H_
