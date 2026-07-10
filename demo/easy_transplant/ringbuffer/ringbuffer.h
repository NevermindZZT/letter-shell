/**
 * @file ringbuffer.h
 * @author 独霸一方 (2696652257@qq.com)
 * @brief //> 部分参考cherryRB实现的环形缓冲区,完整实现参考:https://github.com/cherry-embedded/CherryRB
 * 这里的API调用不进行加锁,请自行保证线程安全
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

#include <string.h>

#undef RINGBUFFER_ASSERT
#define RINGBUFFER_ASSERT(exp) (void)0 // 开启此宏关闭此文件用户自定义断言

// clang-format off
#ifndef RINGBUFFER_ASSERT
    #include <stdio.h>
    #define RINGBUFFER_ASSERT(exp)                                                          \
        if (!(exp))                                                                         \
        {                                                                                   \
            printf("\"" #exp "\" assert failed at file: %s, line: %d", __FILE__, __LINE__); \
            for (;;)                                                                        \
                ;                                                                           \
        }
#endif // ! RINGBUFFER_ASSERT
// clang-format on

// > C/C++兼容性宏定义
#ifdef __cplusplus
extern "C"
{
#endif

    typedef uint16_t ring_size_t;

    typedef struct __ringbuffer_t ringbuffer_t;
    struct __ringbuffer_t
    {
        bool busy; // 用于指示'慢速'外设是否正在操作缓冲区,比如DMA传输
        ring_size_t head;
        ring_size_t tail;
        ring_size_t data_count;        // 当前队列中的数据量
        ring_size_t last_series_count; // 调用连续读或写时候实际设置的值
        ring_size_t buffer_size;       // 缓冲区大小
        uint8_t *buffer;
    };

    static inline void ringbuffer_init(ringbuffer_t *rb, uint8_t *buffer, ring_size_t size);
    static inline void ringbuffer_reset(ringbuffer_t *rb);

    static inline ring_size_t ringbuffer_get_free(ringbuffer_t *rb);             // 获取剩余空间
    static inline ring_size_t ringbuffer_get_used(ringbuffer_t *rb);             // 获取已使用空间
    static inline ring_size_t ringbuffer_get_size(ringbuffer_t *rb);             // 获取缓冲区大小
    static inline ring_size_t ringbuffer_get_last_series_size(ringbuffer_t *rb); // 获取上次调用线性读取或者写入时候的设置的大小

    static inline ring_size_t ringbuffer_write(ringbuffer_t *rb, const uint8_t *data, ring_size_t size); // 返回实际写入的数据量
    static inline ring_size_t ringbuffer_read(ringbuffer_t *rb, uint8_t *data, ring_size_t size);        // 返回实际读取的数据量

    static inline bool ringbuffer_write_byte(ringbuffer_t *rb, uint8_t data); // 返回是否写入成功
    static inline bool ringbuffer_read_byte(ringbuffer_t *rb, uint8_t *data); // 返回是否读取成功

    // for linear read/write operation(such as DMA)
    static inline uint8_t *ringbuffer_linear_write_setup(ringbuffer_t *rb, ring_size_t *size);  // 返回写入的地址并设置实际能写入的数据量
    static inline uint8_t *ringbuffer_linear_read_setup(ringbuffer_t *rb, ring_size_t *size);   // 返回读取的地址并设置实际能读取的数据量
    static inline ring_size_t ringbuffer_linear_write_done(ringbuffer_t *rb, ring_size_t size); // 返回实际写入的数据量
    static inline ring_size_t ringbuffer_linear_read_done(ringbuffer_t *rb, ring_size_t size);  // 返回实际读取的数据量

    static inline void ringbuffer_mark_busy(ringbuffer_t *rb); // 标记慢速外设正在操作缓冲区
    static inline void ringbuffer_mark_idle(ringbuffer_t *rb); // 标记慢速外设操作缓冲区结束
    static inline bool ringbuffer_is_busy(ringbuffer_t *rb);   // 返回慢速外设是否正在操作缓冲区

    //+********************************* 函数实现 **********************************/

    static inline void ringbuffer_init(ringbuffer_t *rb, uint8_t *buffer, ring_size_t size)
    {
        RINGBUFFER_ASSERT(rb != NULL);

        rb->buffer = buffer;
        rb->buffer_size = size;
        rb->busy = false;

        ringbuffer_reset(rb);
    }

    static inline void ringbuffer_reset(ringbuffer_t *rb)
    {
        RINGBUFFER_ASSERT(rb != NULL);
        rb->head = 0;
        rb->tail = 0;
        rb->data_count = 0;
    }

    static inline ring_size_t ringbuffer_get_free(ringbuffer_t *rb)
    {
        RINGBUFFER_ASSERT(rb != NULL);
        return rb->buffer_size - rb->data_count;
    }

    static inline ring_size_t ringbuffer_get_used(ringbuffer_t *rb)
    {
        RINGBUFFER_ASSERT(rb != NULL);
        return rb->data_count;
    }

    static inline ring_size_t ringbuffer_get_size(ringbuffer_t *rb)
    {
        RINGBUFFER_ASSERT(rb != NULL);
        return rb->buffer_size;
    }

    static inline ring_size_t ringbuffer_get_last_series_size(ringbuffer_t *rb)
    {
        RINGBUFFER_ASSERT(rb != NULL);
        return rb->last_series_count;
    }

    static inline ring_size_t ringbuffer_write(ringbuffer_t *rb, const uint8_t *data, ring_size_t size)
    {
        RINGBUFFER_ASSERT(rb != NULL);

        ring_size_t free_size = ringbuffer_get_free(rb);

        if (size > free_size)
        {
            size = free_size; // 多余的数据不写入(丢掉)
        }

        // 判断超过尾部没有
        if (rb->tail + size > rb->buffer_size)
        {
            ring_size_t first_size = rb->buffer_size - rb->tail;
            memcpy((uint8_t *)(rb->buffer) + rb->tail, data, first_size);
            memcpy(rb->buffer, data + first_size, size - first_size);
        }
        else
        {
            memcpy((uint8_t *)(rb->buffer) + rb->tail, data, size);
        }

        // 限制尾部位置
        if (rb->tail + size >= rb->buffer_size)
        {
            rb->tail = size - (rb->buffer_size - rb->tail);
        }
        else
        {
            rb->tail += size;
        }

        rb->data_count += size;

        return size;
    }

    static inline ring_size_t ringbuffer_read(ringbuffer_t *rb, uint8_t *data, ring_size_t size)
    {
        RINGBUFFER_ASSERT(rb != NULL);

        ring_size_t used_size = ringbuffer_get_used(rb);

        if (size > used_size)
        {
            size = used_size; // 读取的数据超过了已有数据量
        }

        // 判断超过尾部没有
        if (rb->head + size > rb->buffer_size)
        {
            ring_size_t first_size = rb->buffer_size - rb->head;
            memcpy(data, (uint8_t *)(rb->buffer) + rb->head, first_size);
            memcpy(data + first_size, rb->buffer, size - first_size);
        }
        else
        {
            memcpy(data, (uint8_t *)(rb->buffer) + rb->head, size);
        }

        // 限制头部位置
        if (rb->head + size >= rb->buffer_size)
        {
            rb->head = size - (rb->buffer_size - rb->head);
        }
        else
        {
            rb->head += size;
        }

        rb->data_count -= size;

        return size;
    }

    static inline bool ringbuffer_write_byte(ringbuffer_t *rb, uint8_t data)
    {
        RINGBUFFER_ASSERT(rb != NULL);

        if (ringbuffer_get_free(rb) == 0)
        {
            return false;
        }

        *((uint8_t *)(rb->buffer) + rb->tail) = data;

        // 限制尾部位置
        if (rb->tail + 1 >= rb->buffer_size)
        {
            rb->tail = 0;
        }
        else
        {
            rb->tail++;
        }

        rb->data_count++;

        return true;
    }

    static inline bool ringbuffer_read_byte(ringbuffer_t *rb, uint8_t *data)
    {
        RINGBUFFER_ASSERT(rb != NULL);

        if (ringbuffer_get_used(rb) == 0)
        {
            return false;
        }

        *data = *((uint8_t *)(rb->buffer) + rb->head);

        // 限制头部位置
        if (rb->head + 1 >= rb->buffer_size)
        {
            rb->head = 0;
        }
        else
        {
            rb->head++;
        }

        rb->data_count--;

        return true;
    }

    static inline uint8_t *ringbuffer_linear_write_setup(ringbuffer_t *rb, ring_size_t *size)
    {
        RINGBUFFER_ASSERT(rb != NULL);
        RINGBUFFER_ASSERT(size != NULL);
        ring_size_t max_size = ringbuffer_get_free(rb);

        if (max_size + rb->tail > rb->buffer_size)
        {
            max_size = rb->buffer_size - rb->tail;
        }

        *size = max_size;
        rb->last_series_count = max_size;

        return (uint8_t *)(rb->buffer) + rb->tail;
    }

    static inline uint8_t *ringbuffer_linear_read_setup(ringbuffer_t *rb, ring_size_t *size)
    {
        RINGBUFFER_ASSERT(rb != NULL);
        RINGBUFFER_ASSERT(size != NULL);
        ring_size_t max_size = ringbuffer_get_used(rb);

        if (max_size + rb->head > rb->buffer_size)
        {
            max_size = rb->buffer_size - rb->head;
        }

        *size = max_size;
        rb->last_series_count = max_size;

        return (uint8_t *)(rb->buffer) + rb->head;
    }

    static inline ring_size_t ringbuffer_linear_write_done(ringbuffer_t *rb, ring_size_t size)
    {
        RINGBUFFER_ASSERT(rb != NULL);

        // 限制尾部位置
        if (rb->tail + size >= rb->buffer_size)
        {
            rb->tail = size - (rb->buffer_size - rb->tail);
        }
        else
        {
            rb->tail += size;
        }

        rb->data_count += size;

        return size;
    }

    static inline ring_size_t ringbuffer_linear_read_done(ringbuffer_t *rb, ring_size_t size)
    {
        RINGBUFFER_ASSERT(rb != NULL);

        // 限制头部位置
        if (rb->head + size >= rb->buffer_size)
        {
            rb->head = size - (rb->buffer_size - rb->head);
        }
        else
        {
            rb->head += size;
        }

        rb->data_count -= size;

        return size;
    }

    static inline void ringbuffer_mark_busy(ringbuffer_t *rb)
    {
        RINGBUFFER_ASSERT(rb != NULL);
        rb->busy = true;
    }

    static inline void ringbuffer_mark_idle(ringbuffer_t *rb)
    {
        RINGBUFFER_ASSERT(rb != NULL);
        rb->busy = false;
    }

    static inline bool ringbuffer_is_busy(ringbuffer_t *rb)
    {
        RINGBUFFER_ASSERT(rb != NULL);
        return rb->busy;
    }

#ifdef __cplusplus
}
#endif //\ __cplusplus

#endif //\ __RINGBUFFER_H_
